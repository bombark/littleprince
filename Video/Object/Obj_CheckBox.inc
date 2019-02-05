;-----------------------------------------------------------------------------------------

; Object_System - ObjCheckBox

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

  ; Estrutura de ObjCheckBox =====================
	CheckBox_Pointer equ Obj_Size		; Ponteiro do flag (1- marcado, 0- desmarcado)
	CheckBox_Text	 equ Obj_Size+4		; Ponteiro do texto
    	
  ;----------------------------------------------- 
  
  ObjCheckBox_Size equ Obj_Size+0x24	
  
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto texto
; OUT:
;	nothing

ObjCheckBox_Create:
	mov dword [edi+CheckBox_Pointer], 0
	mov dword [edi+CheckBox_Text], 0
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjCheckBox_Destroy:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Desenha o objeto
; IN:
;	eax: [Y|X] coordenate ????
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjCheckBox_Draw:
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
  
  ; Se marcado -> Desenha o X
  		
  ; Desenha o texto
  
	
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

ObjCheckBox_Onhover:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjCheckBox_Saiu:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Quando o botao foi pressionado
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjCheckBox_Pressed:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjCheckBox_Soltou:
	push esi
	mov esi, [edi+CheckBox_Pointer]
	or esi, esi
	jz .end 
	
  ; Muda o flag	
	pushad
	mov al, [esi]
	not al
	and al, 0x1
	mov [esi], al
	
    ; Se estava desmarcado -> desenha o fundo e sai	
	or al, al
	jnz .marca
	
	; desenha	
		
	popad
	pop esi
	ret
	
    ; Senao -> Desenha o X e sai 	
.marca: 	
 	; desenha	
 	
	popad
.end:	pop esi
	ret


;-----------------------------------------------------------------------------------------
