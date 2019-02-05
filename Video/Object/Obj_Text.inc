;-----------------------------------------------------------------------------------------

; Object_System - Text

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

  ; Estrutura da Botao ==========================
    Text_Desc	equ Obj_Size	; rb 0x10	- Objeto expandido do texto

  ;-----------------------------------------------

  ;Text_Size equ Obj_Size+0x10  - Tah com o mesmo nome do tamanho da fonte -> corrompe o heap

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto texto
; OUT:
;	nothing

ObjText_Create:
	push eax

	mov ax, [edi+Obj_SizeX]
    	shr ax, 0x3		; divide por 8 - tamanho da letra
    	mov byte [edi+Text_Desc+Text_MaxX], al
    	mov ax, [edi+Obj_SizeY]
    	shr ax, 0x4		; divide por 16 - tamanho da letra
    	mov [edi+Text_Desc+Text_MaxY], al

	xor eax, eax
	mov byte [edi+Text_Desc+Text_Align], 0x1
    	mov [edi+Text_Desc+Text_Fonte], al	; Colocar fonte padrao - fazer
    	mov [edi+Text_Desc+Text_Size], al	; Colocar tamanho padrao - fazer
    	mov dword [edi+Obj_Color], 0xffffff
    	mov [edi+Text_Desc+Text_Color], eax	; Colocar cor padrao - fazer
    	mov [edi+Text_Desc+Text_Fonte], al
    	mov dword [edi+Text_Desc+Text_Pointer], eax
  		
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjText_Destroy:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Desenha o objeto
; IN:
;	eax: [Y|X] coordenate ????
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjText_Draw:
	pushad

	mov edx, edi
	add eax, [edx+Obj_PosX]
	mov ebx, [edx+Obj_SizeX]
	mov ecx, 0x333333
	mov edi, Screen
	call draw_Rect
	
	;add eax, 0x00010001
	;sub ebx, 0x00020002
	mov ecx, [edx+Obj_Color]
	call draw_RectFill
	
	mov edi, edx
	add edi, Text_Desc
	call ObjExpText_Draw
	
  ; Fim
.end:	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Quando o mouse está sobre o botao
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjText_Onhover:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjText_Saiu:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Quando o botao foi pressionado
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjText_Pressed:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjText_Soltou:
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

ObjText_DrawLine:
	pushad
	add esp, 4
	mov ebp, [edx+Obj_Color]
	mov dword [cont], 0

  ; Pega o novo caracter
.loop:	mov ecx, [cont]
	cmp cl, [edx+Text_MaxX]
	ja .end1
	mov bl, [esi+ecx]
	or bl, bl
	jz .end2
	cmp bx, 0x0d0a
	je .end1
	mov bh, bl

  ; Desenha o caracter
	mov ecx, [edx+Text_Color]
	call draw_Char23
	inc dword [cont]
	jmp .loop

.end1:	sub esp, 4
	popad
	ret

.end2:	sub esp, 4
	stc
	popad
	ret

;-----------------------------------------------------------------------------------------