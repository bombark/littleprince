;-----------------------------------------------------------------------------------------

; View Memory

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

Ex_Path	db "c:\",0
	rb 0x50
Ex_Teste	db "d:\felipe\",0

Ex_List	dd 0
Ex_Bitmap	rb 0x10


; Estrutura dos itens devolvidos da rotina list_folder (int 0x45)
	ExFL_Prox	   equ 0x0	; dd - ponteiro para proximo item
	ExFL_Ant	   equ 0x4	; dd - ponteiro para item anterior
	ExFL_Ass	   equ 0x8	; dw - assinatura
	ExFL_Data	   equ 0xa	; dw - data de criacao
	ExFL_Hora	   equ 0xc	; dw - hora de criacao
	ExFL_Data2   equ 0xe	; dw - data de modificacao
	ExFL_Hora2   equ 0x10	; dw - hora de modificacao
	ExFL_SubTipo equ 0x12	; db - subtipo do item (jpeg,bmp,mp3,mpeg,...)
	ExFL_Tipo	   equ 0x13	; db - tipo do item (pasta,texto,musica,...)
	ExFL_Rs1	   equ 0x14	; dw - free
	ExFl_Rs2	   equ 0x16	; db - free
	ExFL_Color   equ 0x17	; db - cor do nome
	ExFL_Comment equ 0x18	; dd - ponteiro para o comentario
	ExFL_Name	   equ 0x1c	; dd - ponteiro para o nome

FL_ItemSize equ 0x20
FL_Assinatura equ "FL"


Ex_Nome 	 db "Nome",0
Ex_Tamanho db "Tamanho",0
Ex_Tipo	 db "Tipo",0
Ex_Data	 db "Data",0


Ex_Button1 db "posX=530 posY=600 height=32 width=32 code=1>",0

Ex_Imagem	 db "d:\img2.bmp",0

Ex_Pasta	equ 0x1
Ex_Exec	equ 0x2
Ex_Image	equ 0x3
Ex_Audio	equ 0x4
Ex_Texto	equ 0x5
Ex_Video	equ 0x6
Ex_Vetor	equ 0x7

Script2	db "OBJ1"
	dd 0xff000003,0x02580212,0x00200020,0x00010003
	;dd 0xff000005,0x00000000,0x00000000,0x00020005,Ex_Imagem


;-----------------------------------------------------------------------------------------


;======================== P R I N C I P A L ==============================================

use32
Explorer_Main:
	call Explorer_InitWindow

.ini:
	mov esi, Ex_Path
	mov ebp, 0x14
	int 0x45

	mov [Ex_List], esi
	mov eax, 0x00210020
	mov ebx, 0x001000d0
	mov ebp, 0xffffff
	mov ecx, 0x10
	mov dl, 1

.a:	or esi, esi
	jz .loop
	push ax
	push ecx
	push esi
	mov ecx, 0xff0000
	mov esi, [esi+ExFL_Name]
	call draw_String
	pop esi
	pop ecx
	pop ax
	;call SWN_CreateEvent
	inc ecx
	mov esi, [esi]
	add eax, 0x00100000
	jmp .a

.loop:	mov ebp, 5
	int 0x42
	or cx, cx
	jz .loop

	cmp cx, 0x1
	je .back

	sub cx, 0x10
	mov esi, [Ex_List]
.b:	or cx,cx
	jz .file
	dec cx
	mov esi, [esi]
	jmp .b

.file:	;cmp byte [esi+ExFL_Tipo], Ex_Pasta
	;jne @f
	call Explorer_Pasta_Down
	jmp .ini

@@:	cmp byte [esi+ExFL_Tipo], Ex_Image
	jne .loop
	call Explorer_Load_Image
	jmp .loop

.back:	call Explorer_Pasta_Up
	jmp .ini






;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Desenha janela
; IN:
; OUT:

Explorer_InitWindow:
	mov eax, 0x00200020		;pos
	mov ebx, 0x02a00280		;size
	mov ebp, 1
	int 0x42
	mov edi, 0x80000001

	mov esi, Script2
	mov dl, 2
	call GUI_Run

	mov eax, 0x00200000
	mov ebx, 0x0230023f
	mov ecx, 0xffffff
	call SDW_DrawRectFill

	mov eax, 0x00200000
	mov ebx, 0x02300240
	mov ecx, 0x00
	call SDW_DrawRect


	mov eax, 0x00100000
	mov ebx, 0x00100240
	mov ecx, 0x666666
	call SDW_DrawRectFill

	mov eax, 0x00100030
	mov ecx, 0x02ffffff
	mov esi, Ex_Nome
	call draw_String

	mov eax, 0x001000d0
	mov esi, Ex_Tipo
	call draw_String

	mov eax, 0x00100140
	mov esi, Ex_Tamanho
	call draw_String

	mov eax, 0x001001a0
	mov esi, Ex_Data
	call draw_String

	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Desenha janela
; IN:
; OUT:

Explorer_Pasta_Down:
	mov edi, [esi+ExFL_Name]
	mov esi, Ex_Path
	call strlen
	add esi, ecx
	xchg esi, edi
	call cpystr_Z
	add edi, ecx
	mov byte [edi], '\'
	mov byte [edi+1], 0

	mov eax, 0x00200000
	mov ebx, 0x0230023f
	mov ecx, 0xffffff
	mov edi, 0x80000001
	call SDW_DrawRectFill
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Desenha janela
; IN:
; OUT:

Explorer_Pasta_Up:
	mov esi, Ex_Path
	call strlen
	add esi, ecx
	sub esi, 2
.c:	mov al, [esi]
	cmp al, '/'
	je .d
	cmp al, '\'
	je .d
	cmp al, ':'
	je .e
	dec esi
	cmp esi, Ex_Path
	jb .d
	jmp .c
.d:	mov byte [esi+1], 0

.e:	mov eax, 0x00200000
	mov ebx, 0x0230023f
	mov ecx, 0xffffff
	mov edi, 0x80000001
	call SDW_DrawRectFill
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Desenha janela
; IN:
; OUT:

Explorer_Open_File:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Desenha janela
; IN:
; OUT:

Explorer_Load_Image:
	mov eax, 0x00800080		;pos
	mov ebx, 0x03000300		;size
	mov ebp, 1
	int 0x42

	mov edi, [esi+ExFL_Name]
	mov esi, Ex_Path
	call strlen
	add esi, ecx
	xchg esi, edi
	call cpystr_Z
	add edi, ecx
	mov byte [edi], 0

	mov esi, Ex_Path
	mov edi, Ex_Bitmap
	call load_Image

	mov eax, 0
	mov esi, edi
	mov edi, 0x80000002
	call SDW_DrawImage

	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Desenha janela
; IN:
; OUT:

Explorer_Load_Texto:
	ret

;-----------------------------------------------------------------------------------------