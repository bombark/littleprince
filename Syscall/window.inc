;-----------------------------------------------------------------------------------------

; GUI Service

;-----------------------------------------------------------------------------------------


;======================== T A B E L A ====================================================

SWN_Table:
	dd SWN_Reserved			; 0- Retorna informacoes sobre o servico
	dd SWN_Window_Create		; 1- Cria uma janela
	dd SWN_Window_Destroy		; 2- Destroi uma janela
	dd SWN_Event_Get		; 3- Pega evento do buffer
	dd SWN_Event_Run		; 4- Executa eventos padroes
	dd SWN_Object_Create		; 5- Cria um objeto
	dd SWN_Object_Destroy		; 6- Destroi um objeto
	dd SWN_Object_Draw		; 7- Desenha um objeto
	dd SWN_Object_Get		; 8- Pega informacao de um objeto
	dd SWN_Object_Set		; 9- Coloca informacao num objeto
	dd 0;SWN_Object_Get2		; 0xa- Pega informacao de um objeto usando o codigo
	dd 0;SWN_Object_Set2		; 0xb- Coloca informacao num objeto usando o codigo
	dd SWN_Object_Move		; 0xc- Movimenta um objeto (janela, botao, texto)
	dd SWN_Object_Resize		; 0xd- Redimensiona um objeto (janela, botao, texto)
	dd SWN_Script1_Run		; 0xe- Executa script HTML
	dd SWN_Script2_Run		; 0xf- Executa script pré-compilado

	dd SPG_Program_Create		; 0x10- Executa um programa
	dd SPG_Program_Destroy		; 0x11- Destroi um progama
	dd SWN_ObjectDown_Destroy	; 0x8 - Destroi todos os sub-objetos de um objeto	

	;dd SWN_Object_GetPos		; - Retorna a posicao do objeto
	;dd SWN_Object_SetPos		; - Muda a posicao do objeto
	;dd SWN_Object_GetSize		; - Retorna o tamanho do objeto
	;dd SWN_Object_SetSize		; - Muda o tamanho do objeto
	;dd SWN_Object_GetCursor		; - Retorna o cursor do objeto
	;dd SWN_Object_SetCursor		; - Muda o cursor do objeto
	;dd SWN_Object_GetAttr		; - Retorna os atributos do objeto
	;dd SWN_Object_SetAttr		; - Muda os atributos do objeto
	;dd SWN_Object_GetStatus		; - Retorna o status do objeto
	;dd SWN_Object_GetType		; - Retorna o tipo do objeto
	;dd SWN_Object_GetCode		; - Retorna o codigo do objeto
	;dd SWN_Object_SetCode		; - Muda o codigo do objeto
	;dd SWN_Object_GetComentario	; - Retorna o comentario do objeto
	;dd SWN_Object_SetComentario	; - Muda o comentario do objeto
	;dd SWN_R1			; - Reservado para retornar o objeto pai
	;dd SWN_R2			; - Reservado para trocar o objeto pai

SWN_Table_Qtde	equ 0x10

;-----------------------------------------------------------------------------------------


;======================== E N T R A D A ==================================================
; Entrada do servico de desenho
; IN:
;	ebp= numero do servico
;	interrupcao 0x42
; OUT:

SWN_Enter:
	cmp ebp, 0x12
	ja .error
	shl ebp, 2
	call dword [cs:SWN_Table+ebp]
	iretd

.error:	stc
	iretd

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  0 =============================================
; Devolve informacoes sobre o servico
; IN:
; OUT:

SWN_Reserved:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Cria uma janela
; IN:
;	eax= [Y|X] coordenate
;	ebx= [Y|X] size
; OUT:
;	edi= Objeto
; Tirar o objeto Draw

SWN_Window_Create:
	push ecx
	push edx
	mov ecx, 0x00010001
	xor edx, edx
	call Object_Create
	call Object_Draw
	pop edx
	pop ecx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Destroy uma janela
; IN:
;	edi= Objeto
; OUT:
;	nothing

SWN_Window_Destroy:
	push esi
	cmp [edi+Obj_Program], esi
	jne .end
	call Object_Destroy
.end:	pop esi
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Devolve um evento se houve
; IN:
;	nothing
; OUT:
;	ecx= Sub-Objeto / Objeto-Pai

SWN_Event_Get:
	push ebx
	push esi

	call SWN_Event_Run

; Ajusta ponteiros de leitura
	mov esi, [Task_Current]
	movzx ebx, byte [esi+Task_EventoPR]
	cmp bl, [esi+Task_EventoPW]
	je .empty			; Buffer full
	add bl, 4
	cmp bl, Size_CicleBuffer
	jb .a
	xor bl, bl
.a:	mov [esi+Task_EventoPR], bl
	add ebx, [esi+Task_EventoBuffer]

; Lê a palavra do evento ( Codigo | Acao )
	mov cx, [ebx+WordEv_Code]
	mov bl, [ebx+WordEv_Acao]
	cmp bl, Mouse_Pressed
	jne .end
	test byte [mouse_Button], 0x1
	jz .empty

.end:	pop esi
	pop ebx
	ret

.empty:	test byte [esi+Task_Attr], Task_Attr_Pausavel
	jz .end2
	mov byte [esi+Task_Status], Task_Paused
.end2:	xor ecx, ecx
	pop esi
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Executa eventos pré-definidos
; IN:
;	nothing
; OUT:
;	nothing


SWN_Event_Run:
	jmp RunEvent

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Cria um objeto
; IN:
;	eax= [Y|X] coordenate
;	ebx= [Y|X] size
;	ecx= Tipo/Codigo
;	edx= Objeto-Pai
; OUT:
;	edi= Objeto

SWN_Object_Create:
	jmp Object_Create

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Cria um objeto
; IN:
;	edi= Objeto
; OUT:
;	nothing

SWN_Object_Destroy:
	jmp Object_Destroy

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Cria um objeto
; IN:
;	edi= objeto
; OUT:
;	nothing

SWN_Object_Draw:
	jmp Object_AllDraw

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Pega uma informacao do objeto
; IN:
;	 ax: campo (Veja no inicio deste arquivo)
;	edi: objeto
; OUT:
;	edx: informacao

SWN_Object_Get:
	jmp ObjInt_Get

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Coloca uma informacao num campo do objeto
; IN:
;	 ah: tipo do objeto
;	 al: campo (Veja no inicio deste arquivo)
;	edx: informacao
;	edi: objeto
; OUT:
;	nothing

SWN_Object_Set:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Move a janela
; IN:
;	eax= [Y|X] coordenate
;	edi= Objeto
; OUT:

SWN_Object_Move:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Redimensiona a janela
; IN:
;	ebx= [Y|X] size
;	edi= Objeto
; OUT:
;	nothing

SWN_Object_Resize:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  A =============================================
; Executa um script em HTML para montar a janela
; IN:
;	 dl= numero da janela
;	esi= script
; OUT:
;	nothing

SWN_Script1_Run:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  B =============================================
; Executa um script pré-compilado para montar a janela
; IN:
;	edx= Objeto
;	esi= script
; OUT:
;	nothing

SWN_Script2_Run:
	push edx
	push edi

; Verifica se edx é um objeto e esi é um script
	cmp dword [edx], 'OBJ1'
	jne .error
	cmp dword [esi], 'SCP1'
	jne .error
	add esi, 4

; Interpreta o script
	call GUI_Run
	jc .error

; Fim
	pop edi
	pop edx
	ret

; Error - Assinatura invalida ou erro no GUI_Run
.error:	stc
	pop edi
	pop edx
	ret

;-----------------------------------------------------------------------------------------


;=========================================================================================
SPG_Program_Create:
	ret

SPG_Program_Destroy:
	mov esi, [Task_Current]
	test byte [esi+Task_Attr], Task_Attr_Destroy
	jnz .end
	mov byte [esi+Task_Status], Task_Condenada
.end:	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================
; Destroi todos os sub-objetos 
; IN:
;	edi= objeto
; OUT:
;	nothing

SWN_ObjectDown_Destroy:
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .end
	push edi
	mov edi, [edi+Obj_Down]
	
  ; Destroi todos os sub-objetos	
.a:	or edi, edi
	jz .redraw
	mov esi, [edi+Obj_Prox]
	call Object_Destroy		; Destroi
	mov edi, esi
	jmp .a 
	
  ; Redesenha	
.redraw:	
	pop edi
	call Object_Draw
 	
.end:	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================
; Retorna a posicao do objeto
; IN:
;	edi: Objeto
; OUT:
;	eax: Posicao

SWN_Object_GetPos:
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .error
	mov eax, [edi+Obj_PosX]
	clc
	ret
.error:	stc
	ret	
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================	
; Muda a posicao do objeto
; IN:
;	eax: [Y|X] new position
;	edi: Objeto
; OUT:
;	nothing

SWN_Object_SetPos:
	jmp Object_SetPos

	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================
; Retorna o tamanho do objeto
; IN:
;	edi: Objeto
; OUT:
;	eax: [Y|X] Size

SWN_Object_GetSize:	
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .error 
	mov eax, [edi+Obj_SizeX]
	clc
	ret
.error:	stc
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================
; Muda o tamanho do objeto
; IN:
;	eax: [Y|X] new size
;	edi: Objeto
; OUT:
;	nothing


SWN_Object_SetSize:
	ret
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================
; Retorna o cursor do objeto
; IN:
;	edi: Objeto
; OUT:
;	 al: Cursor

SWN_Object_GetCursor:
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .error
	mov al, [edi+Obj_Mouse]
	clc
	ret
.error:	stc
	ret
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================
; Muda o cursor do objeto
; IN:
;	 al: Cursor
;	edi: Objeto
; OUT:
;	nothing

SWN_Object_SetCursor:	
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .error
	mov [edi+Obj_Mouse], al
	clc
	ret
.error:	stc
	ret
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================
; Retorna o atributo do objeto
; IN:
;	edi: Objeto
; OUT:
;	 ax: Atributo

SWN_Object_GetAttr:
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .error
	mov ax, [edi+Obj_Attr]
	clc
	ret
.error:	stc
	ret
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================
; Muda o atributo do objeto
; IN:
;	 ax: Atributo
;	edi: Objeto
; OUT:
;	nothing

SWN_Object_SetAttr:
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .error
	mov [edi+Obj_Attr], ax
	clc
	ret
.error:	stc
	ret
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================
; Retorna o status do objeto
; IN:
;	edi: Objeto
; OUT:
;	 al: Status

SWN_Object_GetStatus:
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .error
	mov al, [edi+Obj_Status] 
	clc
	ret
.error:	stc
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================	
; Retorna o tipo do objeto
; IN:
;	edi: Objeto
; OUT:
;	 al: Tipo do objeto

SWN_Object_GetType:
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .error
	mov al, [edi+Obj_Type] 
	clc
	ret
.error:	stc
	ret
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================
; Retorna o codigo do objeto
; IN:
;	edi: Objeto
; OUT:
;	 cx: Codigo

SWN_Object_GetCode:
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .error
	mov cx, [edi+Obj_Code] 
	clc
	ret
.error:	stc
	ret
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================
; Muda o codigo do objeto
; IN:
;	 cx: Codigo
;	edi: Objeto
; OUT:
;	nothing

SWN_Object_SetCode:
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .error
	mov [edi+Obj_Code], cx 
	clc
	ret
.error:	stc
	ret
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================
; Retorna o ponteiro do comentario
; IN:
;	edi: Objeto
; OUT:
;	esi: Posicao

SWN_Object_GetComentario:
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .error
	mov esi, [edi+Obj_Comentario] 
	clc
	ret
.error:	stc
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N    =============================================
; Muda o comentario do objeto
; IN:
;	esi: Ponteiro do Comentario
;	edi: Objeto
; OUT:
;	nothing

SWN_Object_SetComentario:
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .error
	cmp esi, OS_BASE
	jae .error
	mov [edi+Obj_Comentario], esi
	clc
	ret
.error:	stc
	ret

;-----------------------------------------------------------------------------------------