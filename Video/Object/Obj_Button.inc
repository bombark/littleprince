;-----------------------------------------------------------------------------------------

; Object_System - Botao1

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

  ; Estrutura da Botao ==========================
    Button_TextDesc	  equ Obj_Size	; rb 0x10 - Descritor de texto

  ;-----------------------------------------------

  Button_Size equ Obj_Size+8

  ; Cores do Botao ===============================
    Button_Color_Table:
    	dd 0x777777	; Botao normal
    	dd 0xaaaaaa	; Mouse está sobre o Botao
    	dd 0x555555	; Botao pressionado	

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjButton_Create:
	push eax

	or word [edi+Obj_Attr], Obj_Attr_Clicavel

	mov ax, [edi+Obj_SizeX]
    	shr ax, 0x3		; divide por 8 - tamanho da letra
    	mov byte [edi+Button_TextDesc+Text_MaxX], al
    	mov ax, [edi+Obj_SizeY]
    	shr ax, 0x4		; divide por 16 - tamanho da letra
    	mov [edi+Button_TextDesc+Text_MaxY], al

	xor eax, eax
	mov byte [edi+Button_TextDesc+Text_Align], 0x3
    	mov [edi+Button_TextDesc+Text_Fonte], al	
    	mov [edi+Button_TextDesc+Text_Size], al
    	mov dword [edi+Obj_Color], 0xffffff
    	mov [edi+Button_TextDesc+Text_Color], eax	
    	mov [edi+Button_TextDesc+Text_Fonte], al
    	mov dword [edi+Button_TextDesc+Text_Pointer], eax

	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjButton_Destroy:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Desenha o objeto
; IN:
;	eax: [Y|X] coordenate
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjButton_Draw:
	pushad

  ; Desenha o botao
	mov edx, edi
	push edx
	add eax, [edx+Obj_PosX]
	mov ebx, [edx+Obj_SizeX]
	mov ecx, 0x333333
	mov edi, Screen
	call draw_Rect
	
	add eax, 0x00010001
	sub ebx, 0x00020002
	
	mov cl, [edx+Obj_Status] 
	or cl, cl
	jnz .onhover
	mov ecx, [Button_Color_Table]
	jmp .draw
	
.onhover:	cmp cl, Obj_Status_Onhover
	jne .pressed
	mov ecx, [Button_Color_Table+4]
	jmp .draw
	
.pressed:	cmp cl, Obj_Status_Pressed
	jne .error	
	mov ecx, [Button_Color_Table+8]
	
.draw:	mov esi, edx
	mov dh, 0x2
	call draw_RectDegradeDown
	pop edx

  ; Desenha o texto
  	mov edi, edx
	add edi, Button_TextDesc
  	call ObjExpText_Draw

.end:	popad
	ret
        
.error:	pop edx
	popad
	ret        

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Quando o mouse está sobre o botao
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjButton_Onhover:
	mov dl, 0x3
	jmp Object_SendDraw


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing
; Altera: 
;	edx

ObjButton_Saiu:
	mov dl, 0x3
	jmp Object_SendDraw


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Quando o botao foi pressionado
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjButton_Pressed:
	mov dl, 0x3
	jmp Object_SendDraw


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjButton_Soltou:
	ret


;-----------------------------------------------------------------------------------------