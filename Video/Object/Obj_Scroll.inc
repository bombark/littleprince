;-----------------------------------------------------------------------------------------

; Object_System - Scroll

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

  ; Estrutura do Scroll ==========================
    Scroll_Pos		equ Obj_Size+0	; dw - Posicao visual
    Scroll_Size		equ Obj_Size+2	; dw - Tamanho total do objeto
    Scroll_Direcao	equ Obj_Size+4	; db - Direcao do scroll 	

  ;-----------------------------------------------
  
  ; Atributos do Scroll_Direcao ==================
    Scroll_Direcao_Ver	equ 0 	; sentido vertical
    Scroll_Direcao_Hor	equ 1	; sentido horizontal	
  ;-----------------------------------------------

; OBS: o Obj_SizeX do objeto scroll eh fixo em 0x14 (20)

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Cria o scroll
; IN:
;	edi: ponteiro do objeto scroll
; OUT:
;	nothing

ObjScroll_Create:
	pushad
	
	xor eax, eax
	mov word [edi+Scroll_Pos], 0
	mov word [edi+Scroll_Size], ax
	
  ; Se altura > largura ->  o scroll fica na vertical	
	mov ax, [edi+Obj_SizeX]
	mov bx, [edi+Obj_SizeY]
	cmp bx, ax
	jb .hor
	mov byte [edi+Scroll_Direcao], Scroll_Direcao_Ver
	
    ; Cria o botao de subir	o Object_Create está retornando erro
	xor eax, eax
	mov ebx, 0x00140000
	mov bx, [edi+Obj_SizeX]
	mov ecx, 0x00020001
	mov edx, edi
	call Object_Create
	or word [edi+Obj_Attr], Obj_Attr_Dependente1 or Obj_Attr_Dependente2 or Obj_Attr_Imediato
	and word [edi+Obj_Attr], not Obj_Attr_Clicavel
	
    ; Cria o botao de descer 
	mov eax, [edx+Obj_SizeX]
	sub eax, 0x00140000
	xor ax, ax
	inc ecx
	call Object_Create
	or word [edi+Obj_Attr], Obj_Attr_Dependente1 or Obj_Attr_Dependente2 or Obj_Attr_Imediato
	and word [edi+Obj_Attr], not Obj_Attr_Clicavel
	jmp .end
	
	
  ; Senao -> o scroll fica na horizontal
.hor:	mov byte [edi+Scroll_Direcao], Scroll_Direcao_Hor
    
    ; Cria o botao de subir	o Object_Create está retornando erro
	xor eax, eax
	mov ebx, [edi+Obj_SizeX]
	mov bx, 0x0014
	mov ecx, 0x00020001
	mov edx, edi
	call Object_Create
	or word [edi+Obj_Attr], Obj_Attr_Dependente1 or Obj_Attr_Dependente2 or Obj_Attr_Imediato
	and word [edi+Obj_Attr], not Obj_Attr_Clicavel
	
    ; Cria o botao de descer
	xor eax, eax 
	mov ax, [edx+Obj_SizeX]
	sub ax, 0x0014
	inc ecx			; cx = 0x00020002
	call Object_Create
	or word [edi+Obj_Attr], Obj_Attr_Dependente1 or Obj_Attr_Dependente2 or Obj_Attr_Imediato
	and word [edi+Obj_Attr], not Obj_Attr_Clicavel
	
.end:	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Destroy o Scroll 
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjScroll_Destroy:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Desenha o objeto
; IN:
;	eax: [Y|X] coordenate
;	edi: ponteiro do objeto scroll
; OUT:
;	nothing

ObjScroll_Draw:
	pushad
	
	mov edx, edi
	add eax, [edx+Obj_PosX]
	mov ebx, [edx+Obj_SizeX]
	mov ecx, 0xcccccc
	mov edi, Screen
	call draw_RectFill
	
	
  ; Se direcao == Vertical  -> Desenha para scroll na Vertical
	cmp byte [edx+Scroll_Direcao], Scroll_Direcao_Ver
	jne .hor
	
    ; Calcula a posicao da barra do scroll	
	mov ebp, eax  	
    	push edx	
	mov eax, [edx+Obj_SizeX]	; eax = Parte inteira | Parte fracionaria
	sub eax, 0x00280000		; retira a parte dos botoes
	xor ax, ax
  	movzx ecx, word [edx+Scroll_Pos]
  	movzx ebx, word [edx+Scroll_Size]
  	xor edx, edx
  	div ebx				; cx = (SizeX/Progress_Max)*Progress_Pos + 1
  	mul ecx				; bx = cx = quantidade de pixel a ser preenchido pela barra
  	xor ax, ax			; elimina a parte fracionaria
  	add eax, 0x00140000		; retira o botao
  	add eax, ebp			; ajusta para coordenada absoluta
  	pop edx
	
    ; Desenha a barra do scroll	
	movzx ebx, word [edx+Obj_SizeX]
	mov ecx, 0xff0000
	call draw_HLine
	popad
	ret
	
  ; Senao  -> Desenha para scroll na horizontal	
    ; Calcula a posicao da barra do scroll	
.hor:	mov ebp, eax  	
    	push edx	
	mov eax, [edx+Obj_SizeX]	
	sub ax, 0x28			; retira a parte dos botoes
	shl eax, 0x10			; eax = Parte inteira | Parte fracionaria
  	movzx ecx, word [edx+Scroll_Pos]
  	movzx ebx, word [edx+Scroll_Size]
  	xor edx, edx
  	div ebx				; cx = (SizeX/Progress_Max)*Progress_Pos + 1
  	mul ecx				; bx = cx = quantidade de pixel a ser preenchido pela barra
  	shr eax, 0x10			; ax = parte inteira
  	add ax, 0x14			; retira o botao
  	add eax, ebp			; ajusta para coordenada absoluta
  	pop edx
	
    ; Desenha a barra do scroll	
	movzx ebx, word [edx+Obj_SizeY]
	mov ecx, 0xff0000
	call draw_VLine
	popad
	ret        

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Quando o mouse está sobre o Scroll 
; IN:
;	edi: ponteiro do objeto scroll
; OUT:
;	nothing

ObjScroll_Onhover:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o Scroll está pressionado e é solto
; IN:
;	edi: ponteiro do objeto scroll
; OUT:
;	nothing
; Altera: 
;	edx

ObjScroll_Saiu:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Quando o Scroll foi pressionado
; IN:
;	edi: objeto pai do objeto clicado
; OUT:
;	nothing

ObjScroll_Pressed:
	push eax
	mov eax, [Obj_Last]
	mov ax, [eax+Obj_Code]
	cmp ax, 1
	je .up
	cmp ax, 2
	je .down
	pop eax
	ret
	
.up:	mov ax, [edi+Scroll_Pos]
	sub ax, 4	
	jc .end
	mov [edi+Scroll_Pos], ax
	call Object_Draw		; redesenha o scroll
	push edi
	mov edi, [edi+Obj_Up]
	call Object_Draw		; redesenha o objeto pai 
	pop edi
	
	pop eax
	ret

.down:	mov ax, [edi+Scroll_Pos]
	add ax, 4
	cmp ax, [edi+Scroll_Size]
	ja .end
	mov [edi+Scroll_Pos], ax
	call Object_Draw		; redesenha o scroll
	push edi
	mov edi, [edi+Obj_Up]
	call Object_Draw		; redesenha o objeto pai 
	pop edi
	
.end:	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o Scroll está pressionado e é solto
; IN:
;	edi: ponteiro do objeto scroll
; OUT:
;	nothing

ObjScroll_Soltou:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Muda a direcao do Scroll
; IN:
;	 al: Direcao (0= Vertical, 1= Horizontal) 	
;	edi: ponteiro do objeto scroll
; OUT:
;	nothing

ObjScroll_SetDirecao:
;	cmp [edi+Scroll_Direcao], al
;	je .end
;	mov [edi+Scroll_Direcao], al
;	
;  ; Se al == 0  -> Scroll na vertical -> Muda 2° botao	
;	or al, al
;	jnz .hor
;	mov 
;	
;	push esi
;	mov esi, [edi+Obj_Down]
;	mov esi, [esi+Obj_Prox]
;	cmp [esi+Obj_Type], botao
;	jne @f
;@@:	pop esi
;	
;  ; Senao -> scroll na horizontal -> Muda 2° botao 
;
.end:	ret


;-----------------------------------------------------------------------------------------