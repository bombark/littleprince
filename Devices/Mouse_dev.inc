;-----------------------------------------------------------------------------------------

; DRIVER DO MOUSE

;-----------------------------------------------------------------------------------------


;======================== C O M E N T A R I O ============================================
; OBS:
; 	Por enquanto só aceita PS/2 mouse.
;-----------------------------------------------------------------------------------------


;======================== V A R I A B L E S ==============================================
; Depois colocar nas variaveis do sistema

Mouse_MovingX	dw 0	; Movimento do mouse X
Mouse_MovingY	dw 0	; Movimento do mouse Y
Mouse_Obj	dd 0	; Ponteiro do objeto que está sendo movimentado
Mouse_ObjX	dw 0	; Movimento do objeto no eixo X
Mouse_ObjY	dw 0	; Movimento do objeto no eixo Y
Mouse_Status	db 0	; Marca o status do mouse

  ; Atributos do Mouse_Status ====================
    Mouse_SPressed	equ 0x01 	; #1 - Mouse estava pressionado
    Mouse_Move  	equ 0x08 	; #3 - Mouse se moveu
    Mouse_Moving	equ 0x10	; #7 - Mouse arrastando um objeto	
    	
  ;-----------------------------------------------
    
  ; Constantes =================================== 
    Mouse_Onhover	equ 1
    Mouse_Pressed	equ 2
    Mouse_Solta	equ 3
  ;-----------------------------------------------  
    

Mouse_Obj_Pointer	dd 0	; Ultimo objeto que esteve em baixo do mouse  
Mouse_Obj_Status	db 0	; Status do ultimo objeto
  
  
Mouse_CursorFile	db 'Mousecursor_01',0
MousePointer_Desc 	rb 0x20		; Imagem do ponteiro do mouse
MouseBuffer_Desc  	rb 0x20		; Buffer de imagem para salvar o fundo do mouse


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Rotina que liga o mouse
; COPYRIGHT do Menuetos
; Entrada:
;	NADA
; Saida:
;	Liga o mouse

init_Mouse:
	push eax
	push ebx
			
	cli						
	call PS2_Mouse_Init

; Inicia as variaveis
	mov ax, [Screen+IWidth]
	shr ax, 1
	mov word [mouse_X], ax
	mov ax, [Screen+IHeight]
	shr ax, 1
	mov word [mouse_Y], ax
	mov byte [mouse_Z], 0
	mov byte [mouse_Button], 0x00
	mov byte [mouse_Type], 0	; PS/2
	mov word [Mouse_ObjX], 0
	mov word [Mouse_ObjY], 0
	mov byte [Mouse_Status], 0
	sti

; Cria o buffer do ponteiro do mouse (salva o fundo do mouse)
	mov eax, 12
	mov ebx, 18
	mov cl, 24
	xor edx, edx
	mov edi, MouseBuffer_Desc
	call create_Bitmap

; Carrega o ponteiro do Mouse
	mov ebx, Video_Heap
	mov esi, Mouse_CursorFile
	mov edi, MousePointer_Desc
	call load_Image

	nop
	pop ebx
	pop eax	
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Rotina para mouse de PS/2 padrao, 3 bytes de resposta do mouse. (IDT)
; Entrada: nada
; Saida: nada

Mouse_Int:
	pushad

  ; Pega a nova coordenada do mouse
	call PS2_Mouse_GetData
	jc .fim
	mov [Mouse_MovingY], ax
	mov [Mouse_MovingX], bx
	mov [mouse_Button], dl
	or [Mouse_Status], Mouse_Move	; Marca que o mouse se mexeu

  ; Desenha o fundo anterior
	push eax
	push edx
	mov ax, [mouse_Y]
	shl eax, 0x10
	mov ax, [mouse_X]
	xor edx, edx
	mov esi, MouseBuffer_Desc
	mov edi, Screen
	call draw_Image
	pop edx
	pop eax

  ; Atualiza coordenada do mouse 
	mov cx, [mouse_Y]
	sub cx, ax
	add bx, [mouse_X]

    ; Se (Mouse_Y> 0) && (Mouse_X<Screen_Y) -> Atualiza coordenada Y
	or cx, cx
	js .a
   	cmp cx, [Screen+IHeight]
   	jge .a
	mov [mouse_Y], cx

    ; Se (Mouse_X>0) && (Mouse_X<Screen_X)  -> Atualiza coordenada X
.a:	or bx, bx
	js .b
	cmp bx, [Screen+IWidth]
	jge .b
	mov [mouse_X], bx

  ; Se (bit 0 == 1)  ->  Desenha retangulos da movimentacao da janela
.b:	test byte [Mouse_Status], Mouse_Moving
	jz .pula2

    ; Retira o ultimo retangulo
	mov eax, dword [Mouse_ObjX]
	mov edi, [Mouse_Obj]
	call Object_DrawFantasma

    ; Desenha um novo retangulo
	mov ax, word [Mouse_ObjY]
	sub ax, word [Mouse_MovingY]
	shl eax, 0x10
	mov ax, word [Mouse_ObjX]
	add ax, word [Mouse_MovingX]
	mov dword [Mouse_ObjX], eax
	call Object_DrawFantasma

  ; Salva o fundo
.pula2:	push edx
	xor eax, eax
	xor edx, edx
	mov bx, [mouse_Y]
	shl ebx, 0x10
	mov bx, [mouse_X]
	mov ecx, 0x0012000c
	mov esi, Screen
	mov edi, MouseBuffer_Desc
	call blit_Image
	pop edx

  ; Desenha o Cursor
	push edx
	mov ax, [mouse_Y]
	shl eax, 0x10
	mov ax, [mouse_X]
	xor edx, edx
	mov esi, MousePointer_Desc
	mov edi, Screen
	call draw_MaskedImage
	pop edx

  ; Fim
.fim:	mov al, 0x20
	out 0x20, al
	out 0xa0, al
	popad
	iretd


;-----------------------------------------------------------------------------------------
         
          
;======================== F U N C T I O N 4 ==============================================
; Envia codigo do objeto quando o botao do mouse é pressionado
; IN:     
;	nothing
; OUT:    
;	nothing
; Altera: 
;	eax, dl
         
Mouse_Click:
	mov eax, [mouse_X]
	
  ; Se mouse está arrastando objeto  -> Sai da Rotina
  	test byte [Mouse_Status], Mouse_Moving
  	jz .a	
	test byte [mouse_Button], 1
	jnz .end
	mov edi, [Mouse_Obj]
	mov eax, dword [Mouse_ObjX]
	call Object_DrawFantasma
	call Object_SetPos
	and [Mouse_Status], not Mouse_Moving
	jmp .end

  ; Senao	
    ; Se Botao está pressionado  -> chama a rotina de Object_Pressed 	
.a:	test byte [mouse_Button], 1
	jz .b
	call Object_Pressed
	or [Mouse_Status], Mouse_SPressed
	jmp .end
	
    ; Senao
      ; Se antes estava pressionado  -> chama a rotina Object_Soltou
.b:   	test byte [Mouse_Status], Mouse_SPressed
    	jz .onhover  	   	
    	call Object_Soltou
 	and byte [Mouse_Status], not Mouse_SPressed   
 	jmp .end
 
      ; Senao
        ; Se (mouse se movimentou) ->  chama a rotina Object_Onhover 	 	
.onhover:	test [Mouse_Status], Mouse_Move
  	jz .end
	call Object_Onhover 
	and [Mouse_Status], not Mouse_Move		
     	
.end:	ret         


;-----------------------------------------------------------------------------------------



