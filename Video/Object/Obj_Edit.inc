;-----------------------------------------------------------------------------------------

; Object_System - Edit

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

  ; Estrutura de Edicao ==========================

    Edit_ScrollV 	equ Obj_Size		; dd - Ponteiro para o obj Scroll vertical	
    Edit_ScrollH 	equ Obj_Size+4		; dd - Ponteiro para o obj Scroll horizontal

    Edit_CursorX	equ Obj_Size+8		; dw - Posicao X do cursor de escrita
    Edit_CursorY	equ Obj_Size+0xa	; dw - Posicao Y do cursor de escrita  
    Edit_CursorPointer	equ Obj_Size+0xc	; dd - Ponteiro do texto no local do cursor
    
    Edit_QtdeChar	equ Obj_Size+0x10 	; dw - Quantidade de letras
    Edit_Free		equ Obj_Size+0x12	; dw - Free
    Edit_MaxLenght	equ Obj_Size+0x14	; dw - Quantidade maxima de caracteres
    Edit_MaxHorizontal	equ Obj_Size+0x16	; dw - Qtde de letras que sao visiveis
    Edit_MaxVertical	equ Obj_Size+0x18	; dw - Qtde de linhas que sao visiveis
    
    Edit_FonteType	equ Obj_Size+0x1a	; db - Fonte do texto
    Edit_FonteSize	equ Obj_Size+0x1b	; db - Tamanho da fonte			
    Edit_FonteColor	equ Obj_Size+0x1c	; dd - Cor da fonte
    
    Edit_Pointer	equ Obj_Size+0x20	; dd - Ponteiro do texto
    	
  ;----------------------------------------------- 
  
  Edit_Size equ Obj_Size+0x24	
  
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto texto
; OUT:
;	nothing

ObjEdit_Create:
	push eax

	mov byte [edi+Obj_Mouse], Obj_Mouse_Texto
	mov dword [edi+Obj_Color], 0xffffff

	mov ax, [edi+Obj_SizeX]
    	shr ax, 0x3		; divide por 8 (tamanho da letra)
    	mov [edi+Edit_MaxHorizontal], ax
		
	mov ax, [edi+Obj_SizeY]
	shr ax, 0x3		; divide por 8 (tamanho da letra)
	mov [edi+Edit_MaxVertical], ax
	
	xor eax, eax
	mov [edi+Edit_ScrollV], eax
	mov [edi+Edit_ScrollH], eax

	mov [edi+Edit_QtdeChar], ax
	mov [edi+Edit_Free], ax
	mov [edi+Edit_CursorX], ax
	mov [edi+Edit_CursorY], ax
	mov [edi+Edit_CursorPointer], eax
	mov word [edi+Edit_MaxLenght], 0x50
	
	mov [edi+Edit_FonteType], al
	mov [edi+Edit_FonteSize], al
	mov [edi+Edit_FonteColor], eax
	mov [edi+Edit_Pointer], eax
		
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjEdit_Destroy:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Desenha o objeto
; IN:
;	eax: [Y|X] coordenate ????
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjEdit_Draw:
	pushad

  ; Desenha a borda
  	mov edx, edi
  	add eax, [edx+Obj_PosX]
  	mov ebx, [edx+Obj_SizeX]
  	mov ecx, 0x333333
  	mov edi, Screen
  	call draw_Rect
  	
  ; Desenha o fundo
  	add eax, 0x00010001
  	sub ebx, 0x00020002
  	mov ecx, 0xffffff
  	call draw_RectFill	 	
  		
  ; Desenha as letras	
	push eax
	mov esi, [edx+Edit_Pointer]
  	or esi, esi
  	jz .end
  	xor ecx, ecx  
.loop: 	call ObjEdit_DrawLine	
  	jc .end
  	add eax, 0x00100000
  	inc ecx
  	cmp cx, [edx+Edit_MaxVertical]
  	jb .loop
  		
  ; Desenha o cursor
.end:	
	pop eax

  	add eax, [edx+Edit_CursorX]
  	mov ebx, 0x0010
  	mov ecx, 0x00ff0000
  	call draw_VLine
  		
  ; Fim
	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Quando o mouse está sobre o botao
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjEdit_Onhover:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjEdit_Saiu:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Quando o botao foi pressionado
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjEdit_Pressed:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjEdit_Soltou:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Coloca ponteiro do texto no objeto 
; IN:
;	esi: ponteiro do texto
;	edi: ponteiro do objeto edit
; OUT:
;	nothing

ObjEdit_SetText:
	push ecx
	mov [edi+Edit_Pointer], esi
	mov [edi+Edit_CursorPointer], esi 
	call strlen 
	mov [edi+Edit_QtdeChar], cx
	pop ecx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; Coloca ponteiro do texto no objeto 
; IN:
;	????
; OUT:
;	nothing

ObjEdit_GetText:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  9 =============================================
; Adiciona letra no cursor do objeto
; IN:
;	 al: letra
;	edi: ponteiro do objeto edit
; OUT:
;	nothing

ObjEdit_AddChar:
	push eax
	push ecx
	push esi
	
  ; Verifica se chegou no maxlenght	
	mov cx, [edi+Edit_QtdeChar]
	inc cx
	cmp cx, [edi+Edit_MaxLenght]
	jae .end
	mov [edi+Edit_QtdeChar], cx
	
  ; Coloca o caracter
	mov esi, [edi+Edit_CursorPointer]
	inc esi
	mov [edi+Edit_CursorPointer], esi	; Incrementa o ponteiro do cursor
	add word [edi+Edit_CursorX], 0x8
	dec esi	
	mov cx, [esi]				; Salva os caracteres
	mov [esi], al				; Coloca o caracter
	
  ; Desloca os caractes 1 byte para frente	
	inc esi
.a:	or cl, cl
	jz .draw
	mov [esi], cl
	inc esi
	shr cx, 8
	mov ch, [esi]
	jmp .a
	
  ; Coloca o caracter	
.draw:	mov byte [esi], 0
	mov esi, [edi+Edit_CursorPointer]
	call Object_Draw
	
.end:	pop esi
	pop ecx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 0 ===========================================
; Adiciona letra no cursor do objeto
; IN:
;	esi: ponteiro do texto
;	edi: ponteiro do objeto edit
; OUT:
;	nothing

ObjEdit_AddText:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; PRIVADO - Desenha uma linha do texto do objeto
; IN:
;	eax: [Y|X] coordenada para desenho
;	edx: Objeto
;	esi: ponteiro do texto
; OUT:
;	esi: proxima linha do texto
;	cy = 0 ->  Tem outra linha
;	cy = 1 ->  Acabou o texto

   ;==== variaveis =========
	
     cont	equ esp	; dd - contador de letras
     
   ;========================

ObjEdit_DrawLine:
	push eax
	push ebx
	push ecx
	push edi
	push ebp
	sub esp, 4
	mov ebp, [edx+Obj_Color]
	mov dword [cont], 0

  ; Pega o novo caracter
.loop:	mov ecx, [cont]
	cmp cl, [edx+Edit_MaxHorizontal]
	ja .end2
	mov bl, [esi+ecx]
	or bl, bl
	jz .end3
	cmp bx, 0x0d0a
	je .end1
	mov bh, bl

  ; Desenha o caracter
	mov ecx, [edx+Edit_FonteColor]
	or ecx, 0x02000000
	mov edi, Screen	
	call draw_Char23	
	
	inc dword [cont]
	jmp .loop

.end1:	inc ecx
	add esi, ecx
	add esp, 4
	pop ebp
	pop edi
	pop ecx
	pop ebx
	pop eax
	ret

  ; vai ateh o final da linha
.end2:	mov bl, [esi+ecx]
	or bl, bl
	jz .end1
	cmp bx, 0x0d0a
	je .end1
	mov bh, bl
	inc ecx
	jmp .end2

.end3:	add esi, ecx
	add esp, 4
	stc
	pop ebp
	pop edi
	pop ecx
	pop ebx
	pop eax
	ret

	
;-----------------------------------------------------------------------------------------