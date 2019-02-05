;-----------------------------------------------------------------------------------------

; Object_System - Icone

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

  
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjIcone_Create:
	or word [edi+Obj_Attr], Obj_Attr_SobreEscrito or Obj_Attr_Clicavel
	
	mov eax, 0x00080008
	mov ebx, 0x00400040
	mov ecx, 0x00050000
	mov edx, edi
	call Object_Create
	or word [edi+Obj_Attr], Obj_Attr_Dependente2	; Dependente funcional
	mov dword [edi+Quadro_Desc], Icone_Desc
	call Object_Draw
	
	mov eax, 0x00500000
	mov ebx, 0x00300050
	mov ecx, 0x00030000
	call Object_Create
	or word [edi+Obj_Attr], Obj_Attr_Dependente2	; Dependente funcional
	mov dword [edi+Text_Desc+Text_Pointer], texto03
	call Object_Draw
	 
	ret
	

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjIcone_Destroy:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Desenha a janela
; IN:
;	eax: [Y|X] coordenate
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjIcone_Draw:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Quando o mouse está sobre o botao
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjIcone_Onhover:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjIcone_Saiu:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Quando o botao foi pressionado
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjIcone_Pressed:
  ; Se Icone nao está selecionado  -> seleciona
	;cmp 
	;je .arrasta
	;call 
	;jmp .end

  ; Senao  -> arrasta a janela
.arrasta:	push eax	
  	mov [Mouse_Obj], edi
	xor eax, eax  	
  	call Object_GetPos	
  	mov dword [Mouse_ObjX], eax
  	or [Mouse_Status], Mouse_Moving
  	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjIcone_Soltou:
	ret


;-----------------------------------------------------------------------------------------