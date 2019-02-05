;-----------------------------------------------------------------------------------------

; Object System Interface

;-----------------------------------------------------------------------------------------


;========================  C O M E N T A R I O  ==========================================
; Apenas um modo do usuario conseguir pegar e colocar variaveis dentro do objeto, tambem
; modo de executar funcoes especificas dos objetos de tipos diferentes.

;-----------------------------------------------------------------------------------------	
	
	
;========================  V A R I A V E I S =============================================
; É resevado 0x10 rotinas para as rotinas genericas, ou seja, que todo objeto tem.	

  ; GET ============================================	
ObjInt_Obj_GetTable:	
	dd 0
	dd ObjInt_GetCode
	dd ObjInt_GetPosSize
	dd ObjInt_GetTypeAttr
	dd ObjInt_GetType
	dd ObjInt_GetColor
	dd ObjInt_GetComment
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0

  ; Tabela 
    ; Funcoes Especifica de cada objetos
    ObjInt_Function_GetTable:
	dd ObjInt_Obj_GetTable			; Objeto puro
	dd 0					; Janela
    	dd ObjInt_Button_GetTable		; Botao
    	dd ObjInt_Label_GetTable		; Label
    	dd ObjInt_Image_GetTable		; Imagem
  
  
    ; Objeto Botao
    ObjInt_Button_GetTable:
  	dd 0			; qtde de funcoes 
;  	dd ObjButton_GetText

    ; Objeto Label
    ObjInt_Label_GetTable:
	dd 0			; qtde de funcoes 
 ; 	dd ObjText_GetText
  ;	dd ObjText_GetFont
  ;	dd ObjText_GetAttr
  ;	dd ObjText_GetSize

    ; Objeto Imagem
    ObjInt_Image_GetTable:
  	dd 1			; qtde de funcoes 
 	dd ObjImage_GetImage
 ; 	dd ObjImage_GetAttr
  ;	dd ObjImage_GetSizeVirtual

  ;-------------------------------------------------
	
	
  ; SET ============================================
;	dd 0
;	dd Obj_SetCode
;	dd Obj_SetPos
;	dd Obj_SetSize
;	dd Obj_SetAttr
;	dd Obj_SetColor
;	dd Obj_SetComment
;	
;; Funcoes Especifica de cada objetos
;  ; Objeto Botao
;  	dd ObjButton_SetText
;
;  ; Objeto Texto
;  	dd ObjText_SetText
;  	dd ObjText_SetFont
;  	dd ObjText_SetAttr
;  	dd ObjText_SetSize
;
;  ; Objeto Imagem
;  	dd ObjImage_Load
;  	dd ObjImage_SetImage
;  	dd ObjImage_SetAttr
;  	dd ObjImage_SetSizeVirtual	
;		
  ;-------------------------------------------------


  ; Codigos de Erros ===============================

	ObjInt_ObjInvalid	equ 1	; objeto invalido
	ObjInt_FunctionInvalid	equ 2	; funcao invalida
	ObjInt_ParamInvalid	equ 3	; parametro invalido

  ;-----------------------------------------------

;-----------------------------------------------------------------------------------------		


;========================  F U N C T I O N   G E T  ======================================
; Retorna alguma variavel do objeto
; IN:
;	edx: Funcao
;	edi: Objeto
; OUT:
;	ebp: codigo de erro
;	depende da rotina

ObjInt_Get:
	push edx
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .erro1
	
	cmp edx, 0x10			
	ja .ext		
	
  ; Rotina Geral	
	mov edx, [ObjInt_Obj_GetTable+edx*4]
	or edx, edx
	jz .end
	call edx
	jmp .end
	
  ; Rotina Especifica	
.ext:	
int 1
	sub edx, 0x10
	movzx ebp, word [edi+Obj_Type]
	mov ebp, [ObjInt_Function_GetTable+ebp*4]
	cmp edx, [ebp]		; verifica se existe a funcao
	ja .erro2
	shl edx, 2
	mov edx, [ebp+edx]
	call dword edx
.end:	xor ebp, ebp
	pop edx
	ret

.erro1:	mov ebp, ObjInt_ObjInvalid
	stc
	pop edx
	ret

.erro2:	mov ebp, ObjInt_FunctionInvalid
	stc
	pop edx
	ret 
	
;-----------------------------------------------------------------------------------------


;========================  F U N C T I O N   S E T  ======================================
; Seta uma variavel do objeto
; IN:
;	ebp: Funcao
;	edi: Objeto
; OUT:
;	depende da rotina

ObjInt_Set:	
	push edx
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .erro1
	
	cmp edx, 0x10			
	ja .ext		
	
  ; Rotina Geral	
	mov edx, [ObjInt_Obj_GetTable+edx*4]
	or edx, edx
	jz .end
	call edx
	jmp .end
	
  ; Rotina Especifica	
.ext:	sub edx, 0x10
	movzx ebp, word [edi+Obj_Type]
	mov ebp, [ObjInt_Function_GetTable+ebp*4]
	cmp edx, [ebp]		; verifica se existe a funcao
	ja .erro2
	shl edx, 2
	add edx, [ObjInt_Function_GetTable+ebp*4]
	call dword edx
.end:	xor ebp, ebp
	pop edx
	ret

.erro1:	mov ebp, ObjInt_ObjInvalid
	stc
	pop edx
	ret

.erro2:	mov ebp, ObjInt_FunctionInvalid
	stc
	pop edx
	ret 

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Retorna a posicao relativa e tamanho do objeto
; IN:
;	edi: Objeto
; OUT:
;	ecx: codigo do objeto

ObjInt_GetCode:
	movzx ecx, word [edi+Obj_Code]
	ret 

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Retorna a posicao relativa e tamanho do objeto
; IN:
;	edi: Objeto
; OUT:
;	eax: Posicao
;	ebx: Tamanho

ObjInt_GetPosSize:
	mov eax, [edi+Obj_PosX]
	mov ebx, [edi+Obj_SizeX]
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Retorna a posicao relativa e tamanho do objeto
; IN:
;	edi: Objeto
; OUT:
;	eax: 

ObjInt_GetTypeAttr:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Retorna a posicao relativa e tamanho do objeto
; IN:
;	edi: Objeto
; OUT:
;	al: Tipo

ObjInt_GetType:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Retorna a posicao relativa e tamanho do objeto
; IN:
;	edi: Objeto
; OUT:
;	ecx: Cor

ObjInt_GetColor:
	mov ecx, [edi+Obj_Color]
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Retorna a posicao relativa e tamanho do objeto
; IN:
;	edi: Objeto
; OUT:
;	esi: Comentario

ObjInt_GetComment:
	mov esi, [edi+Obj_Comentario]
	ret
	
;-----------------------------------------------------------------------------------------