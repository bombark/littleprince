;-----------------------------------------------------------------------------------------

; Object_System - Imagem

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

  ; Estrutura de Quadro ===========================
    Quadro_NumX	equ Obj_Size	; dw - Numero X do quadro
    Quadro_NumY	equ Obj_Size+2	; dw - Numero Y do quadro
    Quadro_Qtde	equ Obj_Size+4	; db - Quantidade de quadro da animacao
    Quadro_Attr	equ Obj_Size+5	; db - Atributos
    Quadro_Cont	equ Obj_Size+6	; dw - Tempo pra trocar de quadro
    Quadro_Desc	equ Obj_Size+8	; dd - Ponteiro para o Descritor de imagem
  
  ;----------------------------------------------- 
  
  
  ; Atributos de Quadro_Attr =====================
    Quadro_Attr_NaoMuda	equ 0x0	; Nao muda de quadro (funcionando)
    Quadro_Attr_Mouse	equ 0x1	; O mouse muda (Funciona como botao)(funcionando) 
    Quadro_Attr_Timer	equ 0x2	; O timer muda
    
  ;-----------------------------------------------  
  
  ; Para botao, o quadro 0 -> botao normal, quadro 1 -> botao onhover 
  ; e quadro 2 -> botao pressionado 
  
  
  ObjQuadro_Size equ Obj_Size+0xc
  
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjQuadro_Create:
	push eax
	xor eax, eax
	mov [edi+Quadro_NumX], eax
	mov [edi+Quadro_Qtde], al
	mov [edi+Quadro_Attr], al	; Nao muda de imagem
	mov [edi+Quadro_Cont], ax
	mov [edi+Quadro_Desc], eax
	pop eax
	ret
	

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjQuadro_Destroy:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Desenha a janela
; IN:
;	eax: [Y|X] coordenate
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjQuadro_Draw:
	pushad
		
	mov edx, edi
	add eax, [edx+Obj_PosX]
	mov ecx, [edx+Quadro_NumX]
	mov esi, [edx+Quadro_Desc]
	or esi, esi
	jz .end
	mov edi, Screen
	call ImagemQuadro_Draw
	
.end:	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Quando o mouse está sobre o botao
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjQuadro_Onhover:
	cmp byte [edi+Quadro_Attr], Quadro_Attr_Mouse
	jne .end	
	mov word [edi+Quadro_NumX], 1
	jmp Object_Draw
.end:	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjQuadro_Saiu:
	cmp byte [edi+Quadro_Attr], Quadro_Attr_Mouse
	jne .end
	mov word [edi+Quadro_NumX], 0
	jmp Object_Draw
.end:	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Quando o botao foi pressionado
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjQuadro_Pressed:
	cmp byte [edi+Quadro_Attr], Quadro_Attr_Mouse
	jne .end
	mov word [edi+Quadro_NumX], 2
	jmp Object_Draw
.end:	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjQuadro_Soltou:
	ret


;-----------------------------------------------------------------------------------------