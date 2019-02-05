;-----------------------------------------------------------------------------------------

; Rotinas de Manipulacao de Diretorio

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S  =============================================

; include "FS_constant.inc"
;DM_tam 	equ 0x400		; Tamanho padrao dos diretorios
;assDM	equ 0xd0d0	; assinatura para DM
;assD	equ 0xd1d1	; assinatura para Diretorios normais

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Carrega um diretorio na memoria
; Entrada:
;	ebx= base do diretorio (buscar este valor no diretório ativo)
;	esi= o item a ser carregado
; Saida:
;	ebx= ponteiro do novo diretorio
;	al = erro, clc= OK, stc= falta de espaco
; Testada

align 4
carrega_diretorio :

	push eax
	push ecx
	push edx
	push edi

	push esi
	mov esi, [ebx+esi+I_continue]	;
	mov ecx, [ebx+esi+F_lba1]	;
	mov eax, [ebx+esi+F_ext]	; o bloco nao pode estar fragmentado

	shl eax, 0x9		; converte setores em bytes
.a:	call aloca_HB		; aloca bloco (multiplo de 4kb)
	jc .error1
	mov edi, esi
	shr eax, 0x9
	pop esi

	push ebx
	mov [ebx+esi+I_diretorio], edi
	mov bx, [ebx+D_part]

	xchg eax, ecx
	call ReadSeg0		; transfere para memoria
	mov [edi+D_part], bx	; Coloca a particao
	pop ebx
	jc .error2

	mov [edi+D_anterior], ebx    	; Coloca o ponteiro do diretorio anterior
	;mov eax, [ebx+D_mestre]
	;mov [edi+D_mestre], eax	; Coloca o ponteiro do diretorio mestre
	mov ebx, edi
	mov al, 0x0		; sem erros
	clc

.saida:	pop edi
	pop edx
	pop ecx
	pop eax
	ret

.error1: 	mov al, 1			; erro de alocação no heap
	jmp .erro

.error2:	mov al, 0x2		; erro na leitura do HD
.erro:	stc
	pop edi
	pop edx
	pop ecx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Salva diretorio no HD - parece OK
; Entrada:
;	ebx: base do diretório
; Saida:
;	nada

align 4
salva_diretorio:

	push eax ebx ecx edi
	push dword [ebx+D_anterior]
	push dword [ebx+D_itemDA]

	xor eax, eax
	and [ebx+D_anterior], eax
	and [ebx+D_itemDA], eax

	mov eax, [ebx+D_lba1]	; endereço LBA do diretório (so usa #32)
	mov ecx, [ebx+D_ext]	; extensao
	mov edi, ebx

	push ebx
	mov bl, [ebx+D_part]	; partiçao (1 a 32)
	call WriteSeg0		; sai apontando a base do item na tabela de partições
	jb .a
	call SaveDM
.a:	pop ebx

	pop dword [ebx+D_itemDA]
	pop dword [ebx+D_anterior]
	pop edi ecx ebx eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
;
; IN:
; OUT:

align 4
SaveActualFolder:

	push eax ebx ecx edi

	push dword [ebx+D_anterior]
	push dword [ebx+D_itemDA]

	xor eax, eax
	and [ebx+D_anterior], eax
	and [ebx+D_itemDA], eax
	;and [ebx+D_mestre], eax

	mov eax, [ebx+D_lba1]	; endereço LBA do diretório
	mov ecx, [ebx+D_ext]	; extensao
	mov edi, ebx
	mov bl, [ebx+D_part]	; partiçao (1 a 32)

	call WriteSeg0		; sai apontando a base do item na tabela de partições

	;pop dword [ebx+D_mestre]
	pop dword [ebx+D_itemDA]
	pop dword [ebx+D_anterior]

	pop edi ecx ebx eax
	ret



; deve-se zerar a posiçao I_diretorio, D_anterior, D_itemDA e D_mestre antes da gravação
; reconstituir depois
; não é necessário fazer isso com DM

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; cria diretorio filho
; Entrada:								OK
;         ebx= base do diretorio
;	esi= item do diretorio a ser criado
; Saida:
;	edi= ponteiro do novo diretorio
;	cy= 0 - OK
;	cy= 1 - Pode ser por falta de espaço no disco ou por falta de espaço na memoria
; Funcionando
align 4
cria_diretorio_n0 :

	mov eax, 1
	mov edi, esi
	mov ecx, DM_tam/0x200		; tamanho básico das pastas
	call aloca_espaco_item
	jc .erro
	mov edx, edi
	push esi
	mov eax, DM_tam			; tamanho do diretorio
	call aloca_HB			; verificar a saida
	mov edi, esi
	pop esi
	jc .erro
	mov [ebx+esi+I_diretorio], edi	;*************************************ver...define na memoria e grava assim
	mov dword [edi+D_ass], assD
	cmp word [ebx+DM_ass], assDM
	and [edi+D_anterior], dword 0
	jz .a
	mov [edi+D_anterior], ebx	; problema: o diretorio mestre fica modificando na memoria
.a:	;mov eax, [ebx+D_mestre]	;
	;mov [edi+D_mestre], eax	; modificado (nao é mais usado)

	mov ax, [ebx+DM_part]
	mov word [edi+D_part], ax

	mov dword [edi+D_ini_heap], 0x3c
	mov dword [edi+D_topo_heap], 0x3c
	mov dword [edi+0x3c], DM_tam-0x40	; carrega o heap
	mov dword [edi+D_tam_heap], DM_tam-0x3c
	mov dword [edi+D_ini_item], 0		; define pasta vazia
	mov dword [edi+D_atual_item], 0
	mov dword [edi+D_topo_item], 0
	mov byte [edi+D_cor_nome], 0x7	; cor_padrao ; pegar as variavel que indica a cor padrao de uma pasta

	mov eax, [ebx+edx+F_lba1]
	mov [edi+D_lba1], eax
	;mov ax, [ebx+edx+Flba2]
	;mov [edi+Dlba2], ax
	mov eax, [ebx+edx+F_ext]
	mov [edi+D_ext], eax

	push edi
	push esi
	and ecx, 0x0
	mov cl, [ebx+esi+I_ext_nome]
	mov [edi+D_ext_nome], cl
	mov esi ,[ebx+esi+I_nome]
	add esi, ebx
	mov ebx, edi			; altera ebx
	push esi
	mov eax, ecx
	call aloca_item
	jc .erro2
	mov edi, esi
	pop esi
	add edi, ebx
	rep movsb
.erro2:	pop esi
	pop edi

	clc
.erro:	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; cria diretorio
;
; in: 	esi= ponteiro do nome
;	al= extensao do nome
;	ebx= base do diretório
; out:
;	ebx=ponteiro do diretório pai
;	edi=ponteiro do novo diretorio
align 4
cria_diretorio:
	push eax
	push ebx
	push ecx

	mov cx, 1			; tipo pasta = tipo 1			pasta= 1, atalho pasta=2, atalho arquivo=3, arquivos=4 em diante
	call procura_tipoI		; procura pela pasta com nome indicado por esi
	jc .fim

	mov cl,al			; esi= base do nome, cl = ext nome
	mov al, 1			; tipo: pasta
	call cria_item		;						OK
	jb .fim

	mov esi, edi
	call cria_diretorio_n0	; ebx= aponta DM (altera DM )
.erro:
.fim:	pop ecx
	pop ebx
	pop eax
	ret

; observações: pesquisa o item desejado, se não acha cria uma pasta com o nome fornecido
; se achar, sai com cy=1 indicando que a pasta já existe. a base retorna em edi
; ebx= aponta o diretório anterior

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
;
; navegação entre diretórios: busca Diretorio mestre (nao usar busca pelo diretorio atual)-pode ter mudado de local
; Se um diretorio superior for realocado na memoria todos os diretorios abaixo deste que
; estiverem abertos devem ser atualizados ...cada item aberto deve ter sua posiçao de
; memoria fixada no item
; uma forma de resolver esse problema é fechar o diretorio sendo deixado
;
;
; busca DM - vai para o inicio da arvore de diretorios
; busca superior- vai para o diretorio acima
;
; sobe e fecha o diretorio sendo deixado.
;
; IN:
;	ebx= diretorio atual
; OUT:
;	ebx= diretorio destino
align 4
diretorio_superior:

	push esi
	mov esi, ebx
	mov ebx, [ebx+D_anterior]
	or ebx, ebx		; sai se for o topo
	jz .fim
	call lib_HB		; libera memoria
	;jb .erro			- avisar!!!!!
.fim:	pop esi
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Abre uma pasta apenas (diretorio)
; Entrada:
;	ebx: diretorio
;	esi: string
;	ax: extensao da string (irrelevante)xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
; Saida:
;	ebx: diretorio do item (novo diretório carregado)
; testar
align 4
pesquisa_diretorio :
	push eax
	push ecx
	push edi

	mov cx, 0x1		; tipo pasta = tipo 1			pasta= 1, atalho pasta=2, atalho arquivo=3, arquivos=4 em diante
	call procura_tipoI		;
	jc .fim

	mov eax, [ebx+edi+I_diretorio]; <> 0 se estiver na memoria
	or eax, eax
	jz .fim

	mov esi, edi
	call carrega_diretorio	; transfere o diretório para a memoria

	pop edi
	pop ecx
	pop eax
	ret

.fim:	stc
	pop edi
	pop ecx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; Abre a pasta de um path (diretorio/diretorio/diretorio/)
; Entrada:
;	ebx: diretorio inicial da pesquisa
;	esi: string (diretorio/diretorio/arquivo)
;	cx: ext da string
; Saida:
;	ebx: o diretorio onde está o arquivo
;	esi: aponta para o arquivo
;	 cx: ext da string restante (sem os diretorio, apenas o arquivo)
; OBS: em esi nao deve conter a unidade (c:\)

align 4
LPFS_Folder_Open_n0:
	push eax
	push edx
	xor edx, edx
	and ecx, 0xffff

   ; corre o nome atras de / ou \
.a:	cmp edx, ecx
	jae .end		; busca arquivo se chegou ao final se achar / ou \
	mov al, [esi+edx]
	cmp al, '/'
	je .b
	cmp al, '\'
	je .b
	inc edx
	jmp .a		; repete até o final do nome

   ; busca diretório se achar / ou \. repete a busca por arquivo
.b:	push ecx
	dec ecx
	mov eax, edx
	push esi
	call pesquisa_diretorio	; estraga esi
;int 1
	pop esi
	pop ecx
	jc .end
	inc edx
	sub ecx, edx
	add esi, edx
	and edx, 0
	jmp .a

.end:	pop edx
	pop eax
	ret


;-----------------------------------------------------------------------------------------