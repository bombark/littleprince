;-----------------------------------------------------------------------------------------
;
;	   sequencia de operaçoes:
;
;	1. formatar a unidade.
;	2. instalar o sistema -> instala pasta do sistema (pode-se combinar com formatDisk)
;	3. instalar aplicativos na pasta de sistema
;
;-----------------------------------------------------------------------------------------


LPFStest:
;int 1
;-----------------------------------------------------------------------------------------
; teste das rotinas de escrita e leitura
;int 1			; OK - mar/2008
;	mov bl, 3
;	mov edi, 0x30000
;	mov cx, 0
;	call ReadSeg0
;	ret

;int 1			; OK - mar/2008
;	mov bl, 3
;	mov edi, 0x30000
;	call ReadSector0

;int 1			; OK - mar/2008
	;mov bl, 3
	;mov edi, 0x30000
	;call ReadSegEdiUp

;-----------------------------------------------------------------------------------------

; formata unidade (exemplo)	; OK - mar/2008

;int 1
	;mov al, 0	; 1a partiçao
	;mov bh, 0x80	; unidade c:
	;call FormatDisk	; see FS_FormatDisk.inc

          ;
	;mov al, 0	; partição
	;mov bh, 0x81	; unidade
	;call FormatDisk
	;ret

;-----------------------------------------------------------------------------------------
; instala sistema
;int 1
	;mov bl, 3
	;mov ecx, ((sistema_fim and 0xfffffff)-sistema_ini)/0x200
	;mov edi, 0x2000
	;call InstalaSistema
	;ret

;-----------------------------------------------------------------------------------------
;
; salva aplicativo (temporario)na pasta de executaveis do sistema

;int 1	; OK
;	call Save_exe	; see FS_Fileproc.inc
;	ret
; apenas salva o arquivo na pasta sistema, mas nao serve para regravação

;-----------------------------------------------------------------------------------------
; busca serviço 9 - Mostra tamanho do arquivo
;int 1
	;mov al, (NP1-NP0)
	;mov esi, NPSys_nome
	;mov ebp, 9
	;call SFS_Enter
	;ret

;NP0:
;NPSys_nome	db 'c:/ MostraMemoria'
;NP1:

; observaçoes:


;-----------------------------------------------------------------------------------------

; busca serviço 18 - File_Open - tem problemas com nomes longos...nao carrega dados

;int 1
;	mov al, (N1-N0)
;	mov esi, NPSys_nome
;	;mov ebp, 0x18
;	;call SFS_Enter
;int 1
;	call File_Open
;int 1
;	mov ecx, 0xff
;	mov edi, 0x30000
;int 1
;	call File_Read
;int 1
;	ret

; observaçoes:

;N0:
;NPSys_nome	db 'c:/MM.exx'
;N1:


;-----------------------------------------------------------------------------------------
; busca arquivo funciona bem...
;int 1
;	mov ebx, [SystemFolderBase]	; busca no diretorio do sistema
;	mov cx, (N1-N0)
;	mov esi, NPSys_nome
;	call busca_arquivo		; retorna esi=base do item
;	ret

;N0:
;NPSys_nome	db 'MostraMemoria'
;N1:

; observações: Nao deve existir a unidade no nome (c: ou d:)
; a presençca de / ou \ indica a existencia de pasta
; o nome nao deve ser do tipo asciiz
; foi retirado a conferencia sobre o tipo de item.

;-----------------------------------------------------------------------------------------
; create file... cria item vazio
; esi...nome do arquivo a ser copiado
;
;int 1
;	mov ax, 0x10	; tipo arquivo
;	mov esi, Name	; ponteiro do nome
;	mov cx, N3-N2	; extensao do nome
;	call CreateFile	;
;	ret

;N2:
;Name	db 'Imagens/Teste'
;N3:

; Observaçoes:...Ao gravar o nome o item é sempre arredondado para um multiplo de 2,

;-----------------------------------------------------------------------------------------
; save file... busca arquivo..salva se encontrar usando o mesmo

;int 1
;	mov cx, N3-N2
;	mov esi, Name
;	mov edi, 0x2000
;	mov eax, 0x1000
;	call SaveFile
;	ret

;N2:
;Name	db 'Teste'
;N3:


;-----------------------------------------------------------------------------------------
;int 1
;	mov cx, N3-N2
;	mov esi, Name
;	mov edi, NewName
;	mov eax, N4-N3
;	call RenameFile
;	ret
;
;N2:
;Name	db 'Minhas imagens'
;N3:
;NewName	db 'Imagens'
;N4:

;-----------------------------------------------------------------------------------------



; edi...nome do arquivo na nova posiçao (asciiz)
; as extensões sao irrelevantes...devem ser obtidas no item
; copiar arquivo de uma unidade lpfs para a mesma unidade...
; carregar arquivo inteiro na memoria se for menor que 64K, senao fracionar em blocos de
; 64K.
; carregar na outra posiçao.
; Usar LoadFile e InstalaArquivo...
; Na primeira fase fazer um GetFileInfo...verificar se o tamanho excede 64K.
; guardar o ponteiro ebx e esi...apontam o diretório e o item...(GetItemInfo)
; a nova locaçao deve ser uma pasta diferente, apontada por edi.


;-----------------------------------------------------------------------------------------

;	mov esi, nome
;	mov ecx, N3-N2
;	call CreateFolder
;	ret
;
;N2:
;nome db 'Musicas'
;N3:

;-----------------------------------------------------------------------------------------
; deleta arquivo

;int 1
;	mov ax, 0x10
;	mov cx, N3-N2
;	mov esi, Nome
;	call DeleteFile
;	ret

;N2:
;Nome	db 'Teste.txt'
;N3:
; Observaçoes:...Ao gravar o nome o item é sempre arredondado para um multiplo de 2,
; assim ao colocar a extensao deve-se antes tornar esta tambem multiplo de 2

;-----------------------------------------------------------------------------------------
; executa file
; lança arquivo executavel na memoria nivel 3


;-----------------------------------------------------------------------------------------
; save file com diretório... busca arquivo..salva se encontrar usando o mesmo

;int 1
;	mov ax, 0x10	; tipo
;	mov cx, N3-N2	; ext nome
;	mov esi, Name	; pont. nome
;	mov edi, 0x3000	; buffer memoria
;	mov edx, 0x350	; ext. arquivo
;	call SaveFile
;	ret


;N2:
;Name	db 'Imagens/Cargos'  ; (funcionou) Os nomes devem ser do tipo ascii mais extensao
;N3:

; falta testar rgravação com mesmo tamanho e regravação com tamanho modificado (duas situações)

;-----------------------------------------------------------------------------------------

;save Mmexx.bin in LPFS unit
; é necessário ler todo o arquivo e gravar na unidade LP...pode ser feito em etapas
;	ax: tipo
;	ebx: diretorio inicial
;	esi: string (diretorio/diretorio/arquivo.ext)
;	cx: string extension
;	edi: data memory base
; 	edx: quantidade de bytes
;int 1
;	mov ax, 0x10	; tipo
;	mov ebx, [SystemFolderBase]
;	mov cx, N5-N4	; ext nome
;	mov esi, MName	; pont. nome
;	mov edi, Mmi	; buffer memoria
;	mov edx, [Pext]	; ext. arquivo
;	call SaveFile
;	ret
;
;N4:
;MName	db "MMexx"
;N5:
;Pext	dd (Mm_fim - Mmi)

;ok- funcionou bem, a rotina save exe nao é mais necessária porque pode ser substituida
;    por essa

;-----------------------------------------------------------------------------------------


	;mov ax, 0x10
	;mov ebx, [SystemFolderBase]
	;;mov ebx,[BootAdmFolderBase]
	;mov esi, arquivo
	;mov cx, 9
	;mov edi, [0x80000600+IPointer]
	;sub edi, 0x36
	;mov edx, 0x240036

	;call SaveFile
	;ret
;

;arquivo	db "WallPaper",0

;-----------------------------------------------------------------------------------------
; transferido para load_fonts.inc
;	mov esi, arq_exe
;	call File_Execute
	ret

;arq_exe	db "MMexx", 0


;-----------------------------------------------------------------------------------------