
iniciomemoria	equ 0x80250000


VideoTest:
;int 1

	;call colorprecalc		; montar a tabela de conversao

; abrir arquivo jpeg
;int 1
	mov dword [iniciomemoria+4], 0xc		; proximo bloco
	mov dword [iniciomemoria+8], 0x80250014		; 1o bloco livre heap
	mov dword [iniciomemoria+0xc], 0x80260000	; fim heap
	mov dword [iniciomemoria+0x10], 0x100000-0x14	; extensao do proximo bloco
	ret
;int 1
	mov esi, FName
        	call File_Open
;int 1
	call File_GetSize
;int 1
	call aloca_HB
	mov edi, esi
;



;int 1
	mov ecx, 0x200
.read:	call File_Read
;int 1
	jc .end
	add edi, 0x200
	jmp .read

.end:	;call File_Close		; eax= tamanho em setores, esi=base jpg, edi- final jpg
;int 1
	;cmp dword [esi],0xe0ffd8ff;	; assinatura jpg
	nop
	mov eax,esi		; eax= ponteiro do arquivo
;
;int 1
	;mov eax, 0x80210000
	call jpeg_info
;int 1
	nop
int 1

;	call jpeg_info		; pega informaçoes

;	call jpeg_display		; apresenta

	ret


;-----------------------------------------------------------------------------------------

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
load_figure:

	mov ax, 0x4	; tipo
	mov ebx, [SystemFolderBase]
	mov cx, Ff-Fi	; ext nome
	mov esi, FName	; pont. nome
	call LoadFileInSysMemory		; carrega arquivo no heap
; validaçao do arquivo de fonte:

	cmp dword [edi], 'LP02'
	jnz .error
	add edi, [edi+0x4]
	mov [CharTable], edi
	ret
.error:	stc
	ret

Fi:
FName	db "d:\cantinf.jpg"
Ff:



;-----------------------------------------------------------------------------------------
V2:	mov al, 1
	mov cx, 0x100
.a:	inc al
	loop .a
	ret