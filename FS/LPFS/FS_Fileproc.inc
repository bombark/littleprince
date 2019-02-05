;-----------------------------------------------------------------------------------------

; ROTINAS DE LEITURA E ESCRITA EM ARQUIVOS

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Lê um setor relativo ao arquivo
; Entrada:
;	eax: posicao do setor no arquivo
;	ebx: base do diretorio
;	esi: item de arquivo
;	ecx: quantidade de setores
;	edi: buffer
; Funcionando - nao verificado pra varios setores e casos de parametros errados
align 4
leiaA_n1 :
	push edi
	push eax
	push edx
	push esi

	or ecx, ecx
	jz .fim

.a:	mov esi, [ebx+esi+I_continue]	; Pega o item fragmentado que está a base
	or esi, esi		; Verifica se terminou a lista
	jz .fim
	mov edx, [ebx+esi+F_ext]	; Pega a extensão do item
	cmp eax, edx		; se o ax for menor que o dx, entao a base fica nesse item
	jbe .b			; verificar se eh jb ou jbe
	sub eax, edx		; senao subtrai e busca o outro item
	jmp .a

.b:	sub edx, eax		; extensão-base = extensao apos a base
	cmp ecx, edx		; terminar as observaçoes
	jbe .c
	sub ecx, edx
	jmp .d

.c:	mov edx, ecx
	and ecx, 0

.d:	push  eax ecx edx ebx
	mov ecx, edx
	add eax, [ebx+esi+F_lba1]
	adc edx, [ebx+esi+F_lba2]
	mov ebx, [ebx+D_part]
	call ReadSegEdiUp
	pop ebx edx ecx eax
	jb .error

	or ecx, ecx
	jz .fim
	xor eax, eax
	jmp .a

.fim:	clc
	pop esi
	pop edx
	pop eax
	pop edi
	ret

.erro:	stc
.error:	pop esi
	pop edx
	pop eax
	pop edi
	ret


;----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; grava dados de um arquivo
; Entrada:
;	eax: posicao do setor no arquivo - 0 para o inicio
;	ebx: diretorio
;	ecx: quantidade de setores
;	esi: ponteiro do item
;	edi: buffer
; Saida:
;	cy= 0 - Ok ->
;	cy= 1 - Erro na gravação
; Testar
align 4
graveA_n1:

	push edi
	push eax
	push edx
	push esi

	or ecx, ecx
	jz .fim

.a:	mov esi, [ebx+esi+I_continue]	; Pega o item fragmentado que está a base
	or esi, esi		; Verifica se terminou a lista
	jz .fim
	mov edx, [ebx+esi+F_ext]	; Pega a extensão do item
	cmp eax, edx		; se o eax for menor que edx, entao a base fica nesse item
	jbe .b			; verificar se eh jb ou jbe
	sub eax, edx		; senao subtrai e busca o outro item
	jmp .a

.b:	sub edx, eax		; extensão-base = extensao apos a base
	cmp ecx, edx		; terminar as observaçoes
	jbe .c
	sub ecx, edx
	jmp .d

.c:	mov edx, ecx
	and ecx, 0

.d:	push eax
	push ecx
	mov ecx, edx
	mov eax, [ebx+esi+F_lba1]	; endereço LBA
;int 1
	push ebx
	movzx ebx, byte [ebx+D_part]	; importante (busca numero da partiçao)

	call WriteSegEdiUp
          pop ebx

;int 1
	pop ecx
	pop eax
	jb .error1

	or ecx, ecx
	jz .fim
	and eax, 0
	jmp .a

.fim:	clc
	pop esi
	pop edx
	pop eax
	pop edi
	ret

.error:	stc		; error code in ebp
.error1:	pop esi
	pop edx
	pop eax
	pop edi
	ret



;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Abre o arquivo -> diretorio/diretorio/arquivo.ext = fopen
; devolve em esi= ponteiro do item e ebx= diretorio
; Entrada:
; 	ax: tipo de arquivo desejado
;	ebx: diretorio inicial
;	esi: string (diretorio/diretorio/arquivo.ext)
;	cx: ext da string
; Saida:
;	ebx: o diretorio onde está o arquivo
;	esi: aponta o arquivo
;;;;;;;;;;;;;;;;;;;;;;;;	cx: tipo de arquivo encontrado
; testar
; nao deve ter a unidade, pois admite-se aqui que esta já tenha sido escolhida em função
; do tipo da partição. O nome deve estar normalizado...isto é nao deve ter espaços em
; excesso, pois estes contam na extensao do nome.

align 4
busca_arquivoTipoAX:
	push eax
	push ecx
	push edx

; Abre o diretorio
          call LPFS_Folder_Open_n0
          movzx edx, cx

; Busca item tipo AX  -> eax= ext nome, ebx=dir, esi=pont nome, edx=ext nome
.c:	or edx, edx
	jz .fim
	mov eax, edx
	push edi
	mov cx, word [esp+0xc]
	call procura_tipoI		; procura por tipo igual a AX
	jc .error1
	mov esi, edi
	pop edi

; Fim
.fim:	clc
	pop edx
	pop ecx
	pop eax
	ret

.error1:	pop edi
	mov ecx, edx
.error:	pop edx
	add esp, 4	;pop ecx		; mantem residual em ecx
	stc
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; instala arquivo na pasta ebx
; IN:
;	ebx=base da pasta
;	edi= base do arquivo na memoria
;	ecx= extensao em bytes
;	esi= base do nome
;	al= extensao do nome (max 255)
; OUT:
;	edi= base da pasta
;	esi= base do item.....mudado para soma de bytes
;	ebp = codigo de erro se cy=1, 0 se cy=0
align 4
InstalaArquivo:
	push edx
.2:	push edi		; edi = base arquivo na memoria
	push ecx		; ecx = ext arquivo em bytes
.ini:	mov ecx,eax	; esi= base do nome
	mov al, 4		; tipo de arquivo- 4 executavel binario simples sem cabeçalho-ponto de entrada no 1o byte

.c:	call cria_item	; edi= base do item, ebx=base diretorio alocado
	pop ecx		; ecx= ext OS,
	jb .error1

.a:	mov edx, ecx		; edx= size in bytes
	add ecx, 0x1ff		; round to sector size
	and ecx, 0xfffffe00
	shr ecx, 9		; ecx= sectors size
.d:
	push edi			; edi = base do item
	call aloca_espaco_item	; retorna edi= base do fragmento
	pop edi
	jb .error1
	mov esi, edi		; esi = edi = base do item
.f:	pop edi

.b:
;int 1
	push ebx			; ebx= base da pasta alocada
	xor eax, eax		; deve ser zero
	call graveA_n1		; OK, modifica ebx (bl=part), nao modifica edi
	pop ebx
	jb .error
; save info
	mov [ebx+esi+I_SizeSectores], edx	; extensao
;Calcula a soma dos bytes
	push ebx
	xor eax, eax
	xor ebx, ebx
	mov ecx, edx
.s:	mov bl, byte [edi]
	add eax, ebx
	inc edi
	loop .s
	pop ebx
	mov [ebx+esi+I_soma], eax	; eax= soma de bytes
; save folder
	push dword [ebx+esi+I_diretorio]
	and [ebx+esi+I_diretorio], dword 0
;int 1
	call salva_diretorio		; salva diretorio atua e DM
	jb .error
	pop dword [ebx+esi+I_diretorio]
	pop edx
	mov edi, eax		; edi=
	mov esi, eax		; esi= soma de bytes
	xor ebp,ebp
	ret

.error1:	pop edi
.error:	pop edx
	xor esi,esi		; edi=
	xor edi,edi		; esi= soma de bytes
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; salva arquivo executável- so pode ser usado na primeira vez. nao serve para regravação
; IN:
;	ebx= base da pasta
;	edi= base do arquivo na memoria
;	ecx= extensao em bytes
;	esi= base do nome
;	al= extensao do nome (max 255)
; OUT:
;	edi= base da pasta
;	esi= base do item
;	bl = codigo de erro se cy=1, 0 se cy=0
;	# incluir um setor no inicio para definir o cabeçalho do programa onde devem
; 	ser definidos o ponto de entrada, tamanho de pilha, data/hora da criaçao, data/
;	hora da ultima modificaçao, versão, dados do criador, atributos, senhas...etc
;
;align 4
;Save_exe:
;;int 1
;	push ebx
;	mov ebx, [SystemFolderBase]		; deve ser a pasta do sistema
;	or ebx, ebx
;	jz .error
;
;; header.... to build
;	;mov ecx, [P_ext]			; extensao em bytes
;	;mov esi, P_nome			; ponteiro do nome
;	;mov al, 13			; extensao do nome
;	;mov edi, MM_g			; posição na memoria
;	;add ecx, 0x200			; soma header
;
;
;	mov al, 5
;	mov edi, Mm
;	mov esi, P_nome
;	mov ecx, [P_ext]
;	call InstalaArquivo
;.fim:	pop ebx
;	ret
;.error:	stc
;	jmp .fim
;
;P_nome	db 'MMexx'
;P_ext	dd (Mm_fim - Mmi)

;# nao precisa da partiçao pois é feita sempre na partiçao do sistema (boot)
; SUBSTITUIDA POR SAVE_FILE

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Despreza espaços, / ou \
; IN:
;	cl= extensao do nome
;	esi= base do nome
;
; OUT:	altera esi, ebx, eax
align 4
VerifyName:

.a0:	movzx ebx, byte [esi]
	cmp byte [esi+1], ':'	; verifica se existe a unidade (c:, d:, etc)
	jz .a
	cmp bl, ' '		; despreza espaços
	inc esi
	dec cl
	jmp .a0

.a:	add esi, 2	; procura por \ ou /, despreza espaços:
	sub cl, 2		; se acha passa para o nome...

	mov bl, [esi]
	inc esi
	dec cl
	cmp bl, '/'
	jz .b
	cmp bl, '\'	; despreza / ou \... o proximo caractere deve ser letra
	jz .b		; ou numero
	cmp bl, ' '
	jz .a
; procurar desde diretorio raiz (Admin ou PP) ou na pasta aberta
.b:	mov bl, [esi]
	cmp bl, ' '
	jnz .d
	inc esi
	dec cl
	jmp .b
.d:	stc		; cy=1 if there is not / or \ -> esi updated
.c:	ret		; cy=0 if \ or / is found -> esi updated


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Transfer all blocks to memory point by EDI
; IN:
;	ebx= folder base
;	esi= item base
;	edi= memory point to data
; OUT:
;	alter eax. edi=updated,
align 4
TrasferConSeg:; transfere todos os segmentos (1 a cada vez)
	push edi
	push ebx
	push esi
	push ecx
	push edx
	mov edx, ebx
	mov bl, [edx+D_part]
; 1o block and continue
.c:	mov esi, [edx+esi+I_continue]
	or esi,esi
	jz .saidaOK
	mov eax, [edx+esi+F_lba1]
	mov ecx, [edx+esi+F_ext]
	call ReadSegEdiUp		; transfere para memoria
	jb .error
	jmp .c
.saidaOK:
	xor ebp, ebp		; sem erros
	clc
.error:	pop edx
	pop ecx
	pop esi
	pop ebx
	pop edi
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; get file size
; IN:
;	ebx= folder base
;	esi= item base
; OUT:
;	ecx= file size in sectors
align 4
GetSize:

	push esi
	xor ecx, ecx
.b:	mov esi, [ebx+esi+I_continue]	; corre todos os itens de fragmento - tem defeito:
	add ecx, [ebx+esi+F_ext]	; soma todas as extensoes fragmentadas
	mov esi, [ebx+esi+I_continue]
	or esi,esi
	jz .a
	jmp .b
.a:	pop esi
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  9 =============================================
; Load file to memory point by EDI...in the System heap
; Entrada:
; 	ax: TIPO
;	ebx: diretorio inicial
;	esi: string (diretorio/diretorio/arquivo.ext)
;	cx: string extension
; Saida:
;	ebx: o diretorio onde está o arquivo
;	esi: aponta o arquivo
;	edi:
; Altera:	eax
align 4
LoadFileInSysMemory:

	call busca_arquivoTipoAX	; esi= item achado se cy=0
	jb .error
	call GetSize		; ecx= file size (bytes????)
	mov eax, ecx

; allocate block
.a:	shl eax, 0x9		; converte setores em bytes
	push esi
	call aloca_HB		; aloca bloco (multiplo de 4kb)
	mov edi, esi
	pop esi
	jc .error
	call TrasferConSeg
.error:	ret


; obs: traz todo o arquivo para a memoria do sistema num bloco alocado no heap
; para ser usado pelo sistema para carregar fontes, icones, e outros arquivos


;-----------------------------------------------------------------------------------------





;-----------------------------------------------------------------------------------------
; executar -
; carrega o arquivo: busca o tamanho do arquivo, aloca memoria multiplo de 4kb e transfere
; posiciona a tabela de paginas para fornecer os endereços correspondentes à memoria alocada
; coloca o ponteiro de entrada na lista de execução do timer
;
; tipos de arquivos :
;	4- binario flat (kernel)
;	5- executavel para instalar em interrupções
;	6- executavel nivel usuario
;	7- executavel
;	8- executavel
;	9- outros
;	a- outros até 15 nivel sistema
; 	16 em diante... arquivos de dados nivel usuario
;
;-----------------------------------------------------------------------------------------

; criar atalho - duplica o conteudo de um item em outra posiçao da mesma ou outra pasta
;	tipo de atalho: 2-atalho de pasta, 3- atalho de arquivo - rotina de item