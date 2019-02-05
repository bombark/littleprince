;-----------------------------------------------------------------------------------------

; Object_System - Text

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

  ; Estrutura da Botao ==========================
    Text_Align	equ Obj_Size	; db - Alinhamento do texto
    Text_Fonte	equ Obj_Size+1	; db - Fonte do texto
    Text_MaxX	equ Obj_Size+2	; db - Maximo de letras na horizontal
    Text_MaxY	equ Obj_Size+3	; db - Maximo de letras na vertical
    Text_QtdeY	equ Obj_Size+4	; db - Quantidade de linhas do texto
    Text_Size	equ Obj_Size+5	; db - Tamanho da fonte
    Text_Color	equ Obj_Size+8	; dd - Cor da fonte
    Text_Pointer	equ Obj_Size+0xc	; dd - Ponteiro do texto
    	
  ;----------------------------------------------- 
     
     ; Atributos do Text_Align ====
       Text_Left	equ 0x01		; Texto alinhado na esquerda
       Text_Right	equ 0x02		; Texto alinhado na direita
       Text_Center	equ 0x03		; Texto alinhado no centro
       Text_Inverso	equ 0x10	 	; Texto escrito invertido
     ;-----------------------------
  
  Text_Size equ Obj_Size+0x10		
  
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
    	mov byte [edi+Text_MaxX], al
    	mov ax, [edi+Obj_SizeY]
    	shr ax, 0x4		; divide por 16 - tamanho da letra
    	mov [edi+Text_MaxY], al
	
	xor eax, eax
	mov byte [edi+Text_Align], 0x1
    	mov [edi+Text_Fonte], al	; Colocar fonte padrao - fazer
    	mov [edi+Text_Size], al	; Colocar tamanho padrao - fazer
    	mov dword [edi+Obj_Color], 0xffffff
    	mov [edi+Text_Color], eax	; Colocar cor padrao - fazer
    	mov [edi+Text_Fonte], al
    	mov dword [edi+Text_Pointer], eax	
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
		
	xor ecx, ecx
	add eax, [edi+Obj_PosX]
	mov ebx, [edi+Obj_SizeX]
	mov ecx, [edi+Obj_Color]
	mov edx, edi
	mov edi, Screen
	call draw_RectFill

	mov esi, [edx+Text_Pointer]
	or esi, esi
	jz .end
		
	mov cl, [edx+Text_Align]
	cmp cl, Text_Left
	je .left
	cmp cl, Text_Right
	je .right
	cmp cl, Text_Center
	je .center
	jmp .end

  ; Texto alinhado na esquerda 	
.left:	call ObjText_DrawLine
	jc .end
	add eax, 0x00100000		; Adiciona o tamanho da letra
	jmp .left

  ; Texto alinhado na direita
    ; PosX = (posX+sizeX)-(tamanho da string*size_Fonte)
.right:	call ObjText_GetSizeLine	; ecx= tamanho da string
	add ax, [edx+Obj_SizeX]	; posX+sizeX
	
       ; ajusta posicao e esi se string for maior que Text_MaxX
	and ecx, 0xff
	cmp cl, [edx+Text_MaxX]
	jbe .ok1
	sub cl, [edx+Text_MaxX]
	add esi, ecx
	movzx cx, byte [edx+Text_MaxX]

    ; PosX = ... 
.ok1:	shl cx, 3			; string * tamanho da fonte
	sub ax, cx

    ; Desenha	
	call ObjText_DrawLine
	jc .end
	add eax, 0x00100000		; Adiciona o tamanho da letra
	jmp .right

  ; Texto alinhado no centro
    ; PosX = SizeX/2 - (Tamanho da String*size_Fonte)/2  
.center:	call ObjText_GetSizeLine	; ecx= tamanho da string			
	mov bx, [edx+Obj_SizeX]	
	add ax, bx
	shr bx, 1			; sizeX/2
	
      ; ajusta posicao e esi se string for maior que Text_MaxX
	and ecx, 0xff
	cmp cl, [edx+Text_MaxX]
	jbe .ok2
	sub cl, [edx+Text_MaxX]
	shr cx, 1
	add esi, ecx
	movzx cx, byte [edx+Text_MaxX]

    ; PosX = ...
.ok2:	shl cx, 2			; (string * tamanho da fonte)/2
	add bx, cx
	sub ax, bx
	
    ; Desenha		
	call ObjText_DrawLine
	jc .end
	add eax, 0x00100000		; Adiciona o tamanho da letra
	jmp .center

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


;======================== F U N C T I O N  1 =============================================
; PRIVADO - Desenha uma linha do texto do objeto
; IN:
;	esi: ponteiro do texto
; OUT:
;	ecx: proxima linha do texto

ObjText_GetSizeLine:
	push eax
	xor ecx, ecx
	xor eax, eax
	
.loop:	mov al, [esi+ecx]
	or al, al
	jz .end2
	cmp ax, 0x0d0a
	je .end1
	mov ah, al
	inc ecx
	jmp .loop
	
.end1:	dec ecx	
.end2:	clc
	pop eax
	ret
	
;-----------------------------------------------------------------------------------------	