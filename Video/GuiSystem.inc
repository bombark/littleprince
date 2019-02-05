;-----------------------------------------------------------------------------------------

; GUI System

;-----------------------------------------------------------------------------------------


;======================== C O M E N T A R I O ============================================

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

Obj_Ass	equ 'OBJ '

; Tipos de Objetos
ObjType_Text	equ 1
ObjType_Image	equ 2
ObjType_InputText	equ 3
ObjType_Button	equ 4
ObjType_Radio	equ 5
ObjType_CheckBox	equ 6
ObjType_Select	equ 7

; Estrutura de um objeto
   Obj_Ass	equ 0		; dd - Assinatura
   Obj_Prox	equ 4  		; dd - Proximo objeto
   Obj_PosX	equ 8		; dw - Posicao X
   Obj_PosY	equ 0xa 		; dw - Posicao Y
   Obj_SizeX	equ 0xc		; dw - Tamanho X
   Obj_SizeY	equ 0xe		; dw - Tamanho Y
   Obj_Type	equ 0x10		; dw - Tipo do objeto
   Obj_Reserved	equ 0x12		; dw - free
   Obj_Color	equ 0x14		; dd - Cor do fundo
Obj_Size  equ 0x18		; Tamanho de um objeto


; Estrutura de um texto comum (tah mais pra input do que text)
   Text_Font	equ Obj_Size+0	; dd - Fonte do texto
   Text_FontColor	equ Obj_Size+4	; dd - Cor do texto
   Text_FontSize	equ Obj_Size+8	; db - Tamanho da font do texto
   Text_Attr	equ Obj_Size+9	; db - Atributos (Com Roller, negrito, sublinhado,...)
   Text_SizeX	equ Obj_Size+0xa	; dw - tamanho maximo de letras na horizontal
   Text_SizeY	equ Obj_Size+0xc	; dw - tamanho maximo de linhas
   Text_CursorX	equ Obj_Size+0xe	; dw - posicao X do cursor
   Text_CursorY	equ Obj_Size+0x10	; dw - posicao Y do cursor
   Text_Pointer	equ Obj_Size+0x12	; dd - Ponteiro do texto
Text_ItemSize equ Obj_Size+0x16


; Estrutura de um formulario de texto


; Estrutura de um botao



; Estrutura de uma imagem (é a mesma utilizada no descritor de imagem)
  Image_Ass           equ Obj_Size+0x0	  ; dw - assinatura
  Image_Type      	  equ Obj_Size+0x2    ; db - Tipo
  Image_Bpp	  equ Obj_Size+0x3    ; db - Profundidade da cor
  Image_Height    	  equ Obj_Size+0x4	  ; dw - tamanho X da imagem
  Image_Width     	  equ Obj_Size+0x6	  ; dw - tamanho Y da imagem
  Image_BytesPerLine  equ Obj_Size+0x8    ; dd - quantidade de bytes por linha
  Image_Pointer 	  equ Obj_Size+0xc	  ; dd - Ponteiro da memoria da imagem
Obj_ImageSize	equ Obj_Size+0x10	; Tamanho do objeto imagem


; Estrutura de cada item da lista HTMP_List
   HTMPList_Prox	 equ 0x0		; dd - proximo item
   HTMPList_Function equ 0x4		; dd - deslocamento do item no objeto
   HTMPList_Name	 equ 0x8		; rb - nome


;HTMPList - Lista de atributos generica (serve para todas as tags)
GuiGeneric_List:
	dd @f
	dd HtmpGer_putWidth
	db "width"

@@:	dd @f
	dd HtmpGer_putHeight
	db "height"

@@:	dd @f
	dd HtmpGer_putPosX
	db "posX"

@@:	dd @f
	dd HtmpGer_putPosY
	db "posY"

@@:	dd @f
	dd HtmpGer_putCode
	db "code"

@@:	dd 0
	db 5
	db 2
	db "color"

;HTMPList - Lista de atributos da tag <input>
GuiInput_List:
	dd @f
	dd 0
	db "type"

@@:	dd 0
	dd 0
	db "value"

;HTMPList - Lista de atributos da tag <input type=???>
GuiInputType_List:
	dd @f
	dd 0
	db "text"

@@:	dd @f
	dd 0
	db "password"


;HTMPList - Lista de atributos da tag Image
GuiImage_List:
	dd 0
	dd HtmpImage_putSource
	db "src"


;-----------------------------------------------------------------------------------------



;======================== F U N C T I O N  1 =============================================
; Inicia o sistema GUI
; IN:
; OUT:

GUI_Init:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Inicia o sistema GUI
; IN:
; OUT:

GUI_Deinit:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Cria um texto fixo
; IN:
;	 dl= numero da janela
;	esi= String
; OUT:
;	nothing
; esi= "<text posX=100 posY=100 sizeX=200 sizeY=200> Textoaooooo </text>"

GUI_CreateText:


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Cria um botao
; IN:
;	 dl= numero da janela
;	esi= String
; OUT:
;	nothing
; esi= "width=10 height=10 color=#ffaacc code=1 value=' Botao 1'>"

GUI_CreateButton:
	pushad

; Aloca um bloco para o objeto
	push esi
	mov eax, Obj_Size
	call aloca_HB
	mov edi, esi
	pop esi
	jc .error

; Interpreta a tag
.loop:	call Word_Length
	jc .end
	mov ebx, GuiGeneric_List
	call Word_Compara
	jc .loop
	call ebx		; chama a rotina adequada para cada atributo (posX,posY,...)
	jmp .loop

; Cria o botao com usando as informacoes do objeto
.end:	call Create_Button
	popad
	ret

.error:	stc
	popad
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Cria um texto fixo
; IN:
;	 dl= numero da janela
;	esi= String
; OUT:
;	nothing
; esi= "src=c:\foto02.bmp>"

GUI_CreateImage:
	pushad

; Aloca um bloco para o objeto
	push esi
	mov eax, Obj_ImageSize
	call aloca_HB
	mov edi, esi
	pop esi
	jc .error

; Interpreta a tag
.loop:	call Word_Length
	jc .end
	mov ebx, GuiImage_List
	call Word_Compara
	jc .loop
	call ebx		; chama a rotina adequada para cada atributo (posX,posY,...)
	jmp .loop

; Cria o botao com usando as informacoes do objeto
.end:	call Create_Image
	popad
	ret

.error:	stc
	popad
	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  1 =======================================
; Cria um botao
; IN:
;	esi= String
; OUT:
;	ecx= tamanho da palavra
; edx= "width=10 height=10 color=#ffaacc code=1 value=' Botao 1'>"

Word_Length:
	push eax
	xor ecx, ecx

.a:	mov al, [esi]
	cmp al, ' '
	jne .b
	inc esi
	jmp .a

.b:	push esi

; Pega a extensao da string
.loop:	mov al, [esi]
	cmp al, '='		; terminou a palavra
	je .end
	cmp al, ' '		; terminou a palavra
	je .end
	cmp al, '>'		; terminou a string
	je .error
	cmp al, 0
	je .error			; terminou a string
	inc esi
	inc ecx
	jmp .loop

.end:	pop esi
	pop eax
	ret

.error:	stc
	pop esi
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  2 =======================================
; Cria um botao
; IN:
;	esi= String
;	ecx= tamanho da palavra
;	ebx= lista de palavras (HTMP_List)
; OUT:
;	ebx= ponteiro da rotina
;	esi= string atualizada (esi = esi + ecx)

Word_Compara:
	push edi
	mov edi, ebx

; Procura na lista fornecida em ebx
.loop:	or edi, edi
	jz .findGer
	push edi
	add edi, HTMPList_Name
	call cmpstr
	pop edi
	je .found
	mov edi, [edi]
	jmp .loop

; O item foi encontrado
.found:	add esi, ecx
	mov ebx, [edi+HTMPList_Function]
	clc
	pop edi
	ret

; Procura na lista de palavras genericas (para todas as tags)
.findGer: cmp ebx, GuiGeneric_List
	je .error
	mov edi,GuiGeneric_List
.loop2:	or edi, edi
	jz .error
	push edi
	add edi, HTMPList_Name
	call cmpstr
	pop edi
	je .found
	mov edi, [edi]
	jmp .loop2

; O item nao foi encontrado
.error:	add esi, ecx
	stc
	pop edi
	ret


;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  3 =======================================
; Pega o valor e devolve como string
; IN:
;	esi= String
; OUT:
;	esi= onde comeca o valor
;	ecx= tamanho da string

Word_GetChar:
	push eax
	xor ecx, ecx
	xor ah, ah

; Elimina parte desnecessaria: '=' e ' '
.a:	inc esi
	mov al, [esi]
	cmp al, '='
	je .a
	cmp al, ' '
	je .a
	cmp al, '"'
	je .setflag
	jmp .c

   ; Seta a flag que está dentro ""
.setflag: or ah, 1
	inc esi
	jmp .a

; Pega o tamanho da string
.c:	push esi
.loop:	mov al, [esi]
	test ah, 1
	jnz .b

   ; Verifica em situacao normal
	cmp al, ' '
	je .end
	inc esi
	inc ecx
	jmp .loop

   ; Verifica quando está dentro de aspas. Ex: " string "
.b:	or al, al
	je .end
	cmp al, '"'
	je .end
	inc esi
	inc ecx
	jmp .loop

.end:	pop esi
	pop eax
	ret



;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  4 =======================================
; Pega
; IN:
;	esi= String
; OUT:
; 	esi= posicao apos do numero
;	eax= numero

Word_GetInt:
	push ebx
	push ecx

; Retira os espacos da frente
.a:	inc esi
	mov al, [esi]
	cmp al, 0x20
	je .a
	cmp al, '='
	je .a

; transforma char em decimal
	xor eax, eax
	xor ebx, ebx
.b:	mov bl, [esi]
	or bl, bl
	jz .end
	cmp bl, 0x20
	je .end
	cmp bl, '>'
	je .end

   ; eax= eax*10
	mov ecx, eax
	shl ecx, 1
	shl eax, 3
	add eax, ecx

	sub bl, 0x30		; transforma de ascii para numero
	add eax, ebx
	inc esi
	jmp .b

.end:	pop ecx
	pop ebx
	ret

;-----------------------------------------------------------------------------------------












;======================== S U B  F U N C T I O N  X ======================================
; Desenha o roller de um objeto
; IN:
;	esi= objeto
; OUT:
;	nothing

Gui_DrawRoller:
; Desenha o roller X
	mov eax, [esi+Obj_PosX]
	mov ebx, [esi+Obj_SizeX]
	add ax, bx
	mov bx, 0x10		; tamanho do roller
	mov ecx, 0x55aa55
	call SDW_DrawRect
	add eax, 0x00010001
	sub ebx, 0x00010001
	mov ecx, 0xbbbbbb
	call SDW_DrawRectFill

; Desenha o roller Y
	mov ax, [esi+Obj_PosY]
	mov bx, [esi+Obj_SizeY]
	add ax, bx
	shl eax, 0x10
	mov ebx, 0x00100000		; tamanho do roller
	mov ax, [esi+Obj_PosX]
	mov bx, [esi+Obj_SizeX]
	mov ecx, 0x55aa55
	call SDW_DrawRect
	add eax, 0x00010001
	sub ebx, 0x00010001
	mov ecx, 0xbbbbbb
	call SDW_DrawRectFill

; Desenha o quadrado no canto
	mov eax, [esi+Obj_PosX]
	add eax, [esi+Obj_SizeX]
	add eax, 0x00010001
	mov ebx, 0x000f000f
	call SDW_DrawRectFill

	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  X =======================================
; Desenha uma string conforme o objeto
; IN:
;	esi= objeto
; OUT:
;	nothing

Gui_DrawString:
	pushad
	xor edx, edx
	xor ebx, ebx
	mov eax, [esi+Obj_PosX]
	add eax, 0x00010001		;<- Nao desenha na borda
	mov ecx, [esi+Text_FontColor]
	mov ebp, [esi+Obj_Color]
	mov edi, Screen

.a:	push edx
	push ebx
	push esi
	cmp bx, [esi+Text_SizeX]
	jae .end
	mov ebx, [esi+Text_Font]
	mov esi, [esi+Text_Pointer]
	mov dl, [esi+edx]
	or dl, dl
	jz .end
	cmp dl, 0x10
	je .incLinha
	call draw_Char2
	pop esi
	pop ebx
	pop edx
	inc edx
	inc bx
	jmp .a

.incLinha:
	pop esi
	pop ebx
	shr ebx, 0x10
	inc bx
	cmp bx, [esi+Text_SizeY]
	ja .end2
	pop edx
	shl ebx, 0x10
	mov ax, [esi+Obj_PosX]
	add eax, 0x00100001
	inc edx
 	jmp .a

.end:	pop esi
.end2:	pop ebx
	pop edx
	popad
	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  X =======================================
; Desenha uma string conforme o objeto
; IN:
;	 dl= numero da janela
;	edi= objeto
; OUT:
;	nothing

Create_Button:
	pushad

; Pega as variaveis
	mov eax, [edi+Obj_PosX]		; eax= [Y|X] coordenate
	mov ebx, [edi+Obj_SizeX]		; ebx= [Y|X] size
	mov cx, [edi+Obj_Reserved]		; cx= code

; Cria um evento
	and edx, 0xff
	push edx
	mov edi, [current_Task]
	mov edi, [edi+Task_WindowTable]
	mov dl, [edi+edx]
	or dl, dl
	jz .error
	xor esi, esi
	call Evento_Create
	pop edx

; Desenha o botao
	push eax
	push ebx
	mov ecx, 0x555555
	mov edi, 0x80000000
	add edi, edx
	call SDW_DrawRect
	add eax, 0x00010001
	sub ebx, 0x00010001
	mov ecx, 0x777777
	mov dh, 0x2
	call SDW_DrawRectDegrade
	pop ebx
	pop eax

.end:	popad
	ret

.error:	stc
	pop edx
	popad
	ret

;-----------------------------------------------------------------------------------------



;=============================== R O T I N A S ===========================================


;======================== S U B F U N C T I O N  X =======================================
; Coloca posicao X no lugar adequado no objeto
; IN:
;	esi= string
;	edi= objeto
; OUT:
;	nothing

HtmpGer_putPosX:
	call Word_GetInt
	mov [edi+Obj_PosX], ax
	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  X =======================================
; Coloca posicao X no lugar adequado no objeto
; IN:
;	esi= string
;	edi= objeto
; OUT:
;	nothing

HtmpGer_putPosY:
	call Word_GetInt
	mov [edi+Obj_PosY], ax
	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  X =======================================
; Coloca height no lugar adequado no objeto
; IN:
;	edi= objeto
; OUT:
;	nothing

HtmpGer_putHeight:
	call Word_GetInt
	mov [edi+Obj_SizeX], ax
	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  X =======================================
; Coloca width no lugar adequado no objeto
; IN:
;	edi= objeto
; OUT:
;	nothing

HtmpGer_putWidth:
	call Word_GetInt
	mov [edi+Obj_SizeY], ax
	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  X =======================================
; Coloca width no lugar adequado no objeto
; IN:
;	edi= objeto
; OUT:
;	nothing

HtmpGer_putCode:
	call Word_GetInt
	mov [edi+Obj_Reserved], ax
	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  X =======================================
; Coloca o valor no lugar adequado no objeto tipo texto
; IN:
;	edi= objeto
; OUT:
;	nothing

HtmpText_putValue:
	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  X =======================================
; Coloca a imagem no lugar adequado no objeto tipo texto
; IN:
;	edi= objeto
; OUT:
;	esi= esi atualizado
; Altera:
;	ecx

HtmpImage_putSource:
	call Word_GetChar
	mov byte [esi+ecx], 0	; Transforma numa string z

; Carrega a imagem
	push edi
	add edi, Image_Ass
	call load_Image
	pop edi
	jc .error

	add esi, ecx
	mov byte [esi], 0x20	; restaura o valor
	ret

.error:	stc
	add esi, ecx
	mov byte [esi], 0x20	; restaura o valor
	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  X =======================================
; Coloca a imagem no lugar adequado no objeto tipo texto
; IN:
;	 dl= numero da janela
;	edi= objeto
; OUT:
;	nothing
; Altera:
;	eax, esi, edi
; OBS: colocar aqui algumas frescuradas depois. (rotacao,transparencia,resize,...)

Create_Image:
; Desenha a imagem
	mov eax, [edi+Obj_PosX]
	mov esi, edi
	add esi, Image_Ass
	mov edi, 0x80000000
	add di, dx
	call SDW_DrawImage
	ret

;-----------------------------------------------------------------------------------------