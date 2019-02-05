;-----------------------------------------------------------------------------------------

; GUI System

;-----------------------------------------------------------------------------------------  
  

;======================== V A R I A B L E S ==============================================
  
  ; Tabela de Acoes ==========
    RE_Action_Table:
      dd RunEvent.end2
      dd RE_Object_Redraw		; 1- Redesenha o objeto e seus sub-objetos 	
      dd RE_Object_PartRedraw		; 2- Redesenha parte de um objeto
      dd RE_Object_Redraw2		; 3- Redesenha só o objeto	

  ;---------------------------

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Executa eventos pré-definidos
; IN:
;	nothing
; OUT:
;	nothing
; Executa 3 eventos, nao quis fazer um loop pra nao ficar muito preso na rotina

RunEvent:	
	pushad

; TESTE (edit) ====

	;call ObtemTeclaCar
	;jc @f
	;call ObjEdit_AddChar
@@:	;call ObtemTeclaCom
	;jc @f	

;------------------
	
@@:	xor cl, cl
	mov esi, [Task_Current]
	
  	xor cl, cl 	
	xor edx, edx
	call Task_ReceiveDados		
	jc .end	
	cmp dl, 3
	ja .end	

	call dword [RE_Action_Table+edx*4]		;chama a rotina
	
.end:	popad
.end2:	ret

;-----------------------------------------------------------------------------------------


;===================== S U B F U N C T I O N  1 ==========================================	
; Comando 1 = Redesenha o objeto e seus sub-objetos
; IN:
;	esi= Programa
; OUT:
;	nothing

RE_Object_Redraw:
	mov cl, 2	
	call Task_ReceiveDados	
	jc .end
	mov edi, edx
	call Object_AllDraw
.end:	ret


;-----------------------------------------------------------------------------------------


;===================== S U B F U N C T I O N  2 ==========================================
; Comando 2 = Redesenha parte do objeto e de seus sub-objetos
; IN:
;	esi= Programa
; OUT:
;	nothing

RE_Object_PartRedraw:
  ; Pega os dados necessarios	
    ; Ponteiro do objeto	
	mov cl, 2
	call Task_ReceiveDados
	jc .end
	mov edi, edx
    ; PosXY (coordenada absoluta)	
	call Task_ReceiveDados
	jc .end
	mov eax, edx
    ; TamXY	
	call Task_ReceiveDados
	jc .end
	mov ebx, edx
	
  ; Transforma coordenada absoluta em relativa	
	

  ; Redesenha o objeto	
	;call Object_PartDraw
.end:	ret


;-----------------------------------------------------------------------------------------


;===================== S U B F U N C T I O N  1 ==========================================	
; Comando 3 = Redesenha apenas o objeto 
; IN:
;	esi= Programa
; OUT:
;	nothing

RE_Object_Redraw2:
	mov cl, 2	
	call Task_ReceiveDados	
	jc .end
	mov edi, edx
	call Object_Draw
.end:	ret


;-----------------------------------------------------------------------------------------