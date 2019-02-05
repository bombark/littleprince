;-----------------------------------------------------------------------------------------

; Object_System - Barra de Progresso

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

  ; Estrutura da Botao ==========================
    Progress_Max	 equ Obj_Size	; dw - Tamanho maximo
    Progress_Pos 	 equ Obj_Size+2	; dw - Porcentagem	

  ;-----------------------------------------------

  Button_Size equ Obj_Size+4	

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjProgress_Create:
	mov word [edi+Progress_Max], 100
	mov word [edi+Progress_Pos], 0
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjProgress_Destroy:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Desenha o objeto
; IN:
;	eax: [Y|X] coordenate
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjProgress_Draw:
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
  	mov ecx, 0xcccccc
  	call draw_RectFill
  
  ; Desenha a barra
  	add eax, 0x00010001
  	sub ebx, 0x00020002
  	
    ; Calcula o tamanho correto da barra 
    	cmp word [edx+Progress_Pos], 0x00
	jz .end
    	cmp word [edx+Progress_Max], 0x00
    	jz .end
    	push eax
    	push ebx
    	push edx	
  	mov eax, ebx
  	shl eax, 0x10			; eax = Parte inteira (16 bits) , Parte fracionada (16bits) 
  	movzx ecx, word [edx+Progress_Pos]
  	movzx ebx, word [edx+Progress_Max]
  	xor edx, edx
  	div ebx				; cx = (SizeX/Progress_Max)*Progress_Pos + 1
  	mul ecx				; bx = cx = quantidade de pixel a ser preenchido pela barra
  	shr eax, 0x10
  	mov cx, ax
  	pop edx
  	pop ebx
  	pop eax
  
  	mov bx, cx
	mov ecx, 0x0000aa
	call draw_RectFill

.end:	popad
	ret
        
.error:	pop edx
	popad
	ret        

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Quando o mouse está sobre o botao
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjProgress_Onhover:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing
; Altera: 
;	edx

ObjProgress_Saiu:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Quando o botao foi pressionado
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjProgress_Pressed:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao está pressionado e é solto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjProgress_Soltou:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Define o 'tamanho' da barra (100 eh o padrao)
; IN:
;	 ax: tamanho maximo da barra (100 = default)
;	edi: ponteiro do objeto botao
; OUT:
;	nothing
; Funcionando

ObjProgress_SetMax:
	mov [edi+Progress_Max], ax
  
  ; Se pos != 0  -> redesenha o objeto	
	cmp word [edi+Progress_Pos], 0
	jz .end
	jmp Object_Draw 
.end:	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  9 =============================================
; Define a 'porcentagem' da barra
; IN:
;	 ax: posicao
;	edi: ponteiro do objeto botao
; OUT:
;	nothing
; OTIMIZADA - Funcionando aparentemente

ObjProgress_SetPos:	
	pushad
	and eax, 0xffff
	mov cx, [edi+Progress_Max]
	cmp ax, cx
	jb .put
	mov ax, cx
.put:	movzx ecx, word [edi+Progress_Pos]
	mov [edi+Progress_Pos], ax	

  ; Se nova pos == pos  -> sai
	cmp eax, ecx
	je .end

  ; Senao -> calcula o tamanho da barra
@@:	mov ebp, eax			; salva o tamanho que falta a preencher
    	cmp word [edi+Progress_Max], 0x00
    	jz .end
  	mov eax, [edi+Obj_SizeX]
  	sub eax, 0x00040004		; retira a borda
  	shl eax, 0x10			; eax = Parte inteira (16 bits) , Parte fracionada (16bits) 

    ; Calcula o Min = Obj_Size / Progress_Max
  	movzx ebx, word [edi+Progress_Max]
  	xor edx, edx
  	div ebx		
  	mov ebx, eax 
  	
    ; Calcula o tamanho da barra desenhada = Min * Progress_Pos	
  	mul ecx				
  	shr eax, 0x10
  	mov ecx, eax
  	
    ; Calcula o tamanho que precisa adicionar a velha barra = Min * eax (Nova pos - pos)
  	mov eax, ebx
  	mul ebp
  	shr eax, 0x10
	mov ebx, eax
	
    ; Calula a posicao do objeto	
	xor eax, eax
	call Object_GetPos
	add eax, [edi+Obj_PosX]
	add eax, 0x00020002		; retira a borda
	add eax, ecx
	
    ; Se nova pos < pos  -> apaga parte da barra
	mov ebp, 0x0000ff		; cor da barra 
	sub ebx, ecx
	jns .draw
	mov ebp, 0xcccccc		; cor do fundo da barra	
	not ebx				; deixa positivo ebx
	inc ebx		
	sub eax, ebx
			
    ; Apaga ou desenha a barra	
.draw:	push bx				; pega o tamanho Y da barra
	mov ebx, [edi+Obj_SizeX]
	sub ebx, 0x00040000		; retira a borda
	pop bx
	mov ecx, ebp
	mov edx, edi
	mov edi, Screen
	call draw_RectFill

.end:	popad
	ret


;-----------------------------------------------------------------------------------------