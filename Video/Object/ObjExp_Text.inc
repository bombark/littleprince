;-----------------------------------------------------------------------------------------

; Object_System - Text Expand

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

  ; Estrutura da Botao ==========================
    Text_Align	equ 0	; db - Alinhamento do texto
    Text_Fonte	equ 1	; db - Fonte do texto
    Text_MaxX	equ 2	; db - Maximo de letras na horizontal
    Text_MaxY	equ 3	; db - Maximo de letras na vertical
    Text_QtdeY	equ 4	; db - Quantidade de linhas do texto
    Text_Size	equ 5	; db - Tamanho da fonte
    Text_Color	equ 8	; dd - Cor da fonte
    Text_Pointer	equ 0xc	; dd - Ponteiro do texto

  ;-----------------------------------------------

     ; Atributos do Text_Align ====
       Text_Left	equ 0x01		; Texto alinhado na esquerda
       Text_Right	equ 0x02		; Texto alinhado na direita
       Text_Center	equ 0x03		; Texto alinhado no centro
       Text_Inverso	equ 0x10	 	; Texto escrito invertido
     ;-----------------------------

  ;Text_Size equ Obj_Size+0x10  - Tah com o mesmo nome do tamanho da fonte -> corrompe o heap

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Desenha o objeto
; IN:
;	eax: [Y|X] coordenate ????
;	edx: Objeto
;	edi: ObjExp de texto
; OUT:
;	nothing

ObjExpText_Draw:
	pushad

	mov esi, [edi+Text_Pointer]
	or esi, esi
	jz .end

	mov cl, [edi+Text_Align]
	cmp cl, Text_Left
	je .left
	cmp cl, Text_Right
	je .right
	cmp cl, Text_Center
	je .center
	jmp .end

  ; Texto alinhado na esquerda
.left:	call ObjExpText_DrawLine
	jc .end
	add eax, 0x00100000		; Adiciona o tamanho da letra
	jmp .left

  ; Texto alinhado na direita
    ; PosX = (posX+sizeX)-(tamanho da string*size_Fonte)
.right:	call String_GetLineSize	; ecx= tamanho da string
	add ax, [edx+Obj_SizeX]	; posX+sizeX

       ; ajusta posicao e esi se string for maior que Text_MaxX
	and ecx, 0xff
	cmp cl, [edi+Text_MaxX]
	jbe .ok1
	sub cl, [edi+Text_MaxX]
	add esi, ecx
	movzx cx, byte [edi+Text_MaxX]

    ; PosX = ...
.ok1:	shl cx, 3			; string * tamanho da fonte
	sub ax, cx

    ; Desenha
	call ObjExpText_DrawLine
	jc .end
	add eax, 0x00100000		; Adiciona o tamanho da letra
	jmp .right

  ; Texto alinhado no centro
    ; PosX = SizeX/2 - (Tamanho da String*size_Fonte)/2
.center:	
	call String_GetLineSize	; ecx= tamanho da string
	mov bx, [edx+Obj_SizeX]
	add ax, bx
	shr bx, 1			; sizeX/2

      ; ajusta posicao e esi se string for maior que Text_MaxX
	and ecx, 0xff
	cmp cl, [edi+Text_MaxX]
	jbe .ok2
	sub cl, [edi+Text_MaxX]
	shr cx, 1
	add esi, ecx
	movzx cx, byte [edi+Text_MaxX]

    ; PosX = ...
.ok2:	shl cx, 2			; (string * tamanho da fonte)/2
	add bx, cx
	sub ax, bx

    ; Desenha
	call ObjExpText_DrawLine
	jc .end
	add eax, 0x00100000		; Adiciona o tamanho da letra
	jmp .center

  ; Fim
.end:	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; PRIVADO - Desenha uma linha do texto do objeto
; IN:
;	eax: [Y|X] coordenada para desenho
;	edx: Objeto
;	esi: ponteiro do texto
;	edi: ObjExp do texto
; OUT:
;	esi: proxima linha do texto
;	cy = 0 ->  Tem outra linha
;	cy = 1 ->  Acabou o texto

   ;==== variaveis =========

     cont	equ esp	; dd - contador de letras

   ;========================

ObjExpText_DrawLine:
	pushad
	sub esp, 4
	mov ebp, [edx+Obj_Color]
	mov dword [cont], 0

  ; Pega o novo caracter
.loop:	mov ecx, [cont]
	cmp cl, [edi+Text_MaxX]
	ja .end1
	mov bl, [esi+ecx]
	or bl, bl
	jz .end2
	cmp bx, 0x0d0a
	je .end1
	mov bh, bl

  ; Desenha o caracter
	mov ecx, [edi+Text_Color]
	push edi
	mov edi, Screen
	call draw_Char23
	pop edi
	inc dword [cont]
	jmp .loop

.end1:	add esp, 4
	popad
	ret

.end2:	add esp, 4
	stc
	popad
	ret

;-----------------------------------------------------------------------------------------