;-----------------------------------------------------------------------------------------

; Draw Service

;-----------------------------------------------------------------------------------------


;======================== C O M E N T A R I O ============================================
;


;-----------------------------------------------------------------------------------------


;======================== T A B E L A ====================================================

SDW_Table:
	dd SDW_Enter		; 0- Retorna informacoes sobre o servico
	dd SDW_GetPixel		; 1- Pega pixel de uma imagem
	dd SDW_PutPixel		; 2- Coloca pixel em uma imagem
	dd SDW_DrawLine		; 3- Desenha uma linha
	dd SDW_DrawVLine		; 4- Desenha uma linha na vertical
	dd SDW_DrawHLine		; 5- Desenha uma linha na horizontal
	dd SDW_DrawTriangulo	; 6- Desenha um triangulo
	dd 0;SDW_DrawTrianguloFill	; 7- Desenha um triangulo preenchido
	dd SDW_DrawRect		; 8- Desenha um retangulo
	dd SDW_DrawRectFill		; 9- Desenha um retangulo preenchido
	dd 0;SDW_DrawRectRend	; 0xa- Desenha um retangulo com lados arrendondados
	dd 0;SDW_DrawRectRendFill	; 0xb- Desenha um retangulo com lados preenchidos
	dd 0;SDW_DrawCircle		; 0xc- Desenha um circulo
	dd 0;SDW_DrawCircleFill	; 0xd- Desenha um circulo preenchido
	dd 0;SDW_DrawElipse		; 0xe- Desenha uma elipse
	dd 0;SDW_DrawElipseFill	; 0xf- Desenha uma elipse preenchido
	dd 0;SDW_DrawPolygon	; 0x10- Desenha um poligono

	dd SDW_DrawChar		; 0x11- Desenha um caracter
	dd SDW_DrawString		; 0x12- Desenha uma string
	dd 0;SDW_DrawInt		; 0x13- Desenha um numero

	dd SDW_CreateBitmap		; 0x14- Cria um bitmap
	dd SDW_FreeBitmap		; 0x15- Libera um bitmap
	dd SDW_LoadImage		; 0x16- Carrega uma imagem
	dd SDW_FreeImage		; 0x17- Libera uma imagem
	dd SDW_DrawImage		; 0x18- Desenha uma imagem
	dd SDW_DrawMaskedImage	; 0x19- Desenha uma imagem mascarada
	dd SDW_BlitImage		; 0x1a- Desenha parte de uma imagem
	dd SDW_BlitMaskedImage	; 0x1b- Desenha parte de uma imagem mascarada

SDW_Table_Qtde	equ 0x1b

; OBS: Imagem mascarada nao desenha a cor rosa (255,0,255)(RGB)
;-----------------------------------------------------------------------------------------


;======================== E N T R A D A ==================================================
; Entrada do servico de desenho
; IN:
;	ebp: numero da funcao
;	int 0x43
; OUT:

SDW_Enter:
	cmp ebp, 0x1d
	ja .error
	shl ebp, 2
	call dword [cs:SDW_Table+ebp]
	jc .error
	xor ebp, ebp
	iretd

.error:	;or [esp+IFlags], Carry
	iretd

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  0 =============================================
; Devolve informacoes sobre o servico
; IN:
; OUT:

SDW_Reserved:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Entrada do servico de desenho
; IN:
;	eax= [Y|X] coordenate
;	esi= bitmap
; OUT:
;	ecx= cor

SDW_GetPixel:
  ; Se (edi>0x80000000)  -> Desenha numa imagem do programa	
	cmp edi, 0x80000000
	ja .obj
	cmp dword [edi+IAss], 'BM'
	jne .error
	jmp getPixel	

  ; Senao  -> Desenha num objeto
    ; Verifica se é um objeto
.obj:	cmp dword [edi], 'OBJ1'
	jne .error
    
    ; Desenha 
	push eax
	push edx
	push edi
	call Object_GetPos
	mov edx, edi
	mov edi, Screen
	call getPixel
	pop edi
	pop edx
	pop eax
	ret

.error:	stc
	ret
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Entrada do servico de desenho
; IN:
;	eax= [Y|X] coordenate
;	ecx= cor
;	edi= bitmap
;	  Se (edi>0x80000000) -> Escreve no objeto
; OUT:
;	nothing
; OK

SDW_PutPixel:
  ; Se (edi>0x80000000)  -> Desenha numa imagem do programa	
	cmp edi, 0x80000000
	ja .obj
	cmp dword [edi+IAss], 'BM'
	jne .error
	jmp putPixel	

  ; Senao  -> Desenha num objeto
    ; Verifica se é um objeto
.obj:	cmp dword [edi], 'OBJ1'
	jne .error
    
    ; Desenha 
	push eax
	push edx
	push edi
	call Object_GetPos
	mov edx, edi
	mov edi, Screen
	call putPixel
	pop edi
	pop edx
	pop eax
	ret
  
.error:	stc
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Desenha uma linha
; IN:
;	eax= [Y|X] coordenate
;	ebx= [Y|X] coordenate
;	ecx= color
;	edi= descritor da imagem
;	  Se (edi>0x80000000) -> Escreve no objeto
; OUT:
;	nothing

SDW_DrawLine:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Entrada do servico de desenho
; IN:
;	eax= [Y|X] coordenate
;	ebx= Y size
;	ecx= color
;	edi= descritor de imagem
;	  Se (edi>0x80000000) -> Escreve no objeto
; OUT:
;	nothing
; OK

SDW_DrawVLine:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Entrada do servico de desenho
; IN:
;	eax= [Y|X] coordenate
;	ebx= X size
;	ecx= color
;	edi= descritor da imagem
;	  Se (edi>0x80000000) -> Escreve no objeto
; OUT:
;	nothing
; OK

SDW_DrawHLine:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Entrada do servico de desenho
; IN:

; OUT:
;	nothing

SDW_DrawTriangulo:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Entrada do servico de desenho
; IN:
;	eax= [Y|X] coordenate
;	ebx= [Y|X] size
;	ecx= color
;	edi= descritor da imagem
;	  Se (edi>0x80000000) -> Escreve no objeto
; OUT:
;	nothing
; OK

SDW_DrawRect:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Entrada do servico de desenho
; IN:
;	eax= [Y|X] coordenate
;	ebx= [Y|X] size
;	ecx= color
;	edi= descritor da imagem
;	  Se (edi>0x80000000) -> Escreve no objeto
; OUT:
;	nothing
; OK

SDW_DrawRectFill:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Entrada do servico de desenho
; IN:
;	eax= [Y|X] coordenate
;	ebx= [Y|X] size
;	ecx= color
;	 dh= intensidade do degrade
;	edi= descritor da imagem
;	  Se (edi>0x80000000) -> Desenha no objeto
; OUT:
;	nothing
; OK - (movimentacao da janela - trava)

SDW_DrawRectDegrade:
	cmp edi, 0x80000000
	jae .Screen

; Imagem
	cmp dword [edi+IAss], 'BM'
	jne .error
	push eax
	push edx
	push edi
	jmp .draw

.Screen:	push eax
	push edx
	push edi

; Pega o numero da janela
	;push edx		; salvar o conteudo de dh
	;mov dx, di
	;and edx, 0xff
	;mov edi, [current_Task]
	;cmp dl, [edi+Task_WindowQtde]	; Verifica se esta dentro da quantidade de janela
	;ja .error2
	;mov edi, [edi+Task_WindowTable]
	;add edi, edx
	;pop edx
	;mov dl, [edi]		; pega o numero da janela
	;or dl, dl
	;jz .error2

; Ajusta a coordenada de acordo com a posicao da janela
	;movzx edi, dl
	;shl edi, 2
	;mov edi, [window_Table+edi]
	;add eax, [edi+Win_PosX]
	;mov edi, Screen
	;jmp .draw

; Degrade UP
.draw:	or dh, dh
	js .DegDown
	call draw_RectDegradeUp
	jmp .end

.DegDown:
	not dh
	call draw_RectDegradeDown

.end:	pop edi
	pop edx
	pop eax
	ret

.error:	stc
	ret

.error2:	stc
	pop edi
	pop edx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 1 ===========================================
; Desenha um caracter
; IN:
;	eax= [Y|X] position
;	ebx= fonte
;	ecx= cor do caracter
;	  0x0FRRGGBB	- se F=1, ele nao desenha o fundo e ebp é desprezado
;	 dl= caracter
;	esi= cor do fundo
;	edi= descritor de imagem
;	  Se (edi>0x80000000) -> Desenha no objeto
; OUT:
;	eax= [Y|X] new position

SDW_DrawChar:
	push edx
	push ebp
	push esi
	push edi
	
  ; Pega a posicao do objeto	
	mov ebp, esi
	push eax
	xor eax, eax
	call Object_GetPos	
	mov esi, eax
	pop eax
	add eax, esi
	
  ; Desenho a letra	
  	mov bl, dl
  	mov edx, edi
	mov edi, Screen
	call draw_Char23	
	sub eax, esi

  ; Fim
.end:	pop edi
	pop esi
	pop ebp
	pop edx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 1 ===========================================
; Desenha um caracter
; IN:
;	eax= [Y|X] position
;	ebx= fonte
;	ecx= cor do caracter
;	  0x0FRRGGBB	- se F=1, ele nao desenha o fundo e ebp é desprezado
;	edx= cor do fundo
;	esi= string
;	edi= descritor de imagem
;	  Se (edi>0x80000000) -> Desenha no objeto
; OUT:
;	nothing

SDW_DrawString:
	push edx
	push esi
	push ebp
	mov ebp, edx

; Pega a janela
	;and edi, 0xff
	;mov edx, [current_Task]
	;mov edx, [edx+Task_WindowTable]
	;mov dl, [edx+edi]
	;or dl, dl
	;jz .end

; Desenha o caracter
	mov edi, Screen
	call draw_String23

; Fim
.end:	pop ebp
	pop esi
	pop edx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; Entrada do servico de desenho
; IN:
;	eax= [Y|X] size
;	ebx= descritor do Heap
; OUT:
;	edi= descritor da imagem
;	cy= 0	- OK
;	cy= 1	- error

SDW_CreateBitmap:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  9 =============================================
; Entrada do servico de desenho
; IN:
;	edi= descritor da imagem
; OUT:
;	nothing

SDW_FreeBitmap:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  A =============================================
; Entrada do servico de desenho
; IN:
;	ebx= descritor do Heap
;	esi= nome do arquivo
; OUT:
;	edi= descritor da imagem
;	cy= 0	- OK
;	cy= 1	- Error

SDW_LoadImage:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  B =============================================
; Entrada do servico de desenho
; IN:
;	edi= descritor da imagem
; OUT:
;	nothing

SDW_FreeImage:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  C =============================================
; Entrada do servico de desenho
; IN:
;	eax= [Y|X] coordenate
;	esi= descritor da imagem fonte
;	  Se (edi>0x80000000) -> Desenha no objeto
;	edi= descritor da imagem destino
;	  Se (edi>0x80000000) -> Desenha no objeto
; OUT:
;	nothing

SDW_DrawImage:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  D =============================================
; Entrada do servico de desenho
; IN:
;	eax= [Y|X] coordenate
;	esi= descritor da imagem fonte
;	  Se (edi>0x80000000) -> Desenha no objeto
;	edi= descritor da imagem destino
;	  Se (edi>0x80000000) -> Desenha no objeto
; OUT:
;	nothing

SDW_DrawMaskedImage:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  E =============================================
; Entrada do servico de desenho
; IN:
;	eax= [Y|X] coordenate
;	ebx= [Y|X] size
;	esi= descritor da imagem fonte
;	  Se (edi>0x80000000) -> Desenha no objeto
;	edi= descritor da imagem destino
;	  Se (edi>0x80000000) -> Desenha no objeto
; OUT:
;	nothing

SDW_BlitImage:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  F =============================================
; Entrada do servico de desenho
; IN:
;	eax= [Y|X] coordenate
;	ebx= [Y|X] size
;	esi= descritor da imagem fonte
;	  Se (edi>0x80000000) -> Desenha no objeto
;	edi= descritor da imagem destino
;	  Se (edi>0x80000000) -> Desenha no objeto
; OUT:
;	nothing

SDW_BlitMaskedImage:
	ret

;-----------------------------------------------------------------------------------------