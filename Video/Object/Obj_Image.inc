;-----------------------------------------------------------------------------------------

; Object_System - Imagem

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

  ; Estrutura de Image ===========================
    ObjImage_Desc	equ Obj_Size		; dd - Ponteiro para o Descritor de imagem
    ObjImage_VScroll	equ Obj_Size+4		; dd - Ponteiro do objeto scroll vertical
    ObjImage_HScroll	equ Obj_Size+8 		; dd - Ponteiro do objeto scroll horizontal
    ObjImage_Attr	equ Obj_Size+0xc	; db - Atributos
    
  
  ;----------------------------------------------- 
  
  
  ; Atributos de ObjImage_Attr ===================
    ObjImage_Attr_Original	equ 0x00	; Se desenha a imagem do tamanho original
    ObjImage_Attr_Stretch	equ 0x01	; Se desenha a imagem do tamanho do objeto
    ObjImage_Attr_Tiled		equ 0x02	; Se repete a imagem até fechar o objeto
    ObjImage_Attr_Fundo		equ 0x04	; Se limpa onde está a imagem (cor do objeto)
    ObjImage_Attr_Scroll	equ 0x08	; Se permite scroll
    ObjImage_Attr_Left		equ 0x10  	; Se é alinhado na esquerda
    ObjImage_Attr_Right		equ 0x20	; Se é alinhado na direita
    ObjImage_Attr_Top		equ 0x40	; Se é alinhado no topo
    ObjImage_Attr_Bottom	equ 0x80	; Se é alinhado embaixo
    ObjImage_Attr_Center	equ 0xf0	; Se é alinhado no centro
  ;-----------------------------------------------
  
  ObjImage_Size equ Obj_Size+0x6
  
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto imagem
; OUT:
;	nothing

ObjImage_Create:
	push eax
	xor eax, eax
	mov [edi+ObjImage_Desc], eax
	mov [edi+ObjImage_VScroll], eax
	mov [edi+ObjImage_HScroll], eax
	mov byte [edi+ObjImage_Attr], 0x1d	; esquerda, original, permite scroll e limpa	
	pop eax
	ret
	

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjImage_Destroy:
  ; Verifica se tem um bitmap carregado
  ; Libera o bitmap 
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Desenha a janela
; IN:
;	eax: [Y|X] coordenate
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjImage_Draw:
	pushad
	mov edx, edi
	add eax, [edx+Obj_PosX]
	mov esi, [edx+ObjImage_Desc]
	mov edi, Screen
	or esi, esi
	jz .end

  ; Se (edi->ObjImage_Attr-Strect==1)  -> Desenha a imagem em tamanho reduzido ou expandido	
	mov cl, [edx+ObjImage_Attr]
	and cl, 0x3
	
    ; Se (cl==0)  -> Tamanho original da imagem	
	or cl, cl
	jnz .Stretch
	
      ; Se (tamanho da foto < tamanho do objeto)	
	mov bp, [edx+Obj_SizeX]
	cmp [esi+IWidth], bp
	ja .blit
	mov bp, [edx+Obj_SizeY]
	cmp [esi+IHeight], bp
	ja .blit
	
        ; Colocar o alinhamento da imagem aqui	
;	mov cl, [edx+ObjImage_Attr]
;	test cl, ObjImage_Attr_Center
;	jz @f
;	
;@@:	test cl, ObjImage_Attr_Left
;	jz @f
;	
;@@:	test cl, ObjImage_Attr_Right
;	jz @f
;	
;@@:	test cl, ObjImage_Attr_Top
;	jz @f
;	
;@@:	test cl, ObjImage_Attr_Bottom
;	jz @f
	
@@:	call draw_Image
	jmp .end
	
      ; Senao  -> Pega a coordenada fonte e desenha parte da imagem
.blit:	
	mov ecx, [edx+ObjImage_VScroll]
	or ecx, ecx
	jz .hor
	mov ebx, [ecx+Scroll_Pos-2]	; para o Scroll_Pos cair na parte alta de ebx
	xor bx, bx
	
.hor:	mov ecx, [edx+ObjImage_HScroll]
	or ecx, ecx
	jz @f
	mov bx, [ecx+Scroll_Pos]	; para o Scroll_Pos cair na parte alta de ebx
	
@@:	mov ecx, [edx+Obj_SizeX]
	sub ecx, 0x00140014		; retira um quadrado entre os 2 scrolls
	call blit_Image 	
	jmp .end		

	
    ; Senao 
      ; Se (cl==1)  -> Redimensiona a imagem para o tamanho do objeto	
.Stretch:		
	cmp cl, ObjImage_Attr_Stretch
	jne .Tiled
	mov ecx, [edx+Obj_SizeX]
	call Stretch_Image
	jmp .end

      ; Senao 
        ; Se (cl==2)  -> repete a imagem até completar o objeto
.Tiled:	cmp cl, ObjImage_Attr_Tiled
	jne .end	
	mov ecx, eax 
	mov ebp, eax		; salva o eax
	add ecx, [edx+Obj_SizeX]
.draw:	call draw_Image	
	add ax, [esi+IWidth]
	cmp ax, cx
	jae .incline
	jmp .draw
	
	; Incrementa linha - Ajustar melhor
.incline:	
	push ecx
	shr eax, 0x10
	shr ecx, 0x10
	add ax, [esi+IHeight]
	cmp ax, cx
	pop ecx
	jae .end
	shl eax, 0x10
	mov ax, bp
	jmp .draw
	
.end:	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Quando o mouse está sobre o imagem
; IN:
;	edi: ponteiro do objeto imagem
; OUT:
;	nothing

ObjImage_Onhover:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando a imagem está pressionado e é solto
; IN:
;	edi: ponteiro do objeto imagem
; OUT:
;	nothing

ObjImage_Saiu:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Quando a imagem foi pressionado
; IN:
;	edi: ponteiro do objeto imagem
; OUT:
;	nothing

ObjImage_Pressed:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando a imagem está pressionado e é solto
; IN:
;	edi: ponteiro do objeto imagem
; OUT:
;	nothing

ObjImage_Soltou:
	ret


;-----------------------------------------------------------------------------------------



;=============================  G E T  =================================================== 
;======================== F U N C T I O N  1 =============================================
; GET 0x11 - Retorna o pointeiro da imagem
; IN:
;	edi: Objeto Imagem
; OUT:
;	esi: Descritor de Imagem

ObjImage_GetImage:
	mov esi, [edi+ObjImage_Desc]
	ret
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; GET 0x12 - Retorna o atributo da imagem
; IN:
;	edi: Objeto Imagem
; OUT:
;	al: Atributo

ObjImage_GetAttr:
	mov al, [edi+ObjImage_Attr]
	ret
	
;-----------------------------------------------------------------------------------------	


;======================== F U N C T I O N  3 =============================================
; GET 0x14 - Retorna o tamanho virtual da imagem
; IN:
;	edx: Objeto Imagem
; OUT:
;	A definir

ObjImage_GetSizeVirtual:
	ret
	
;-----------------------------------------------------------------------------------------
;-----------------------------------------------------------------------------------------



;=============================  S E T  =================================================== 
;======================== F U N C T I O N  1 =============================================
; SET 0x11 - Abre uma imagem no objeto
; IN:
;	 al: atributo da imagem (original, strecth, tiled)
;	ebx: Descritor de Imagem
;	esi: Arquivo
;	edi: Objeto Imagem
; OUT:
;	cy = 0  -> OK
;	cy = 1  -> Error - ebp = codigo do erro

ObjImage_LoadImage:
	push ebx
	
	xor ebx, ebx
	call load_Image
	jc .end
	mov [edx+ObjImage_Desc], edi
	call ObjImage_SetAttr
	
.end:	pop ebx
	ret
	
;-----------------------------------------------------------------------------------------	


;======================== F U N C T I O N  2 =============================================
; SET 0x12 - Muda o ponteiro da imagem do objeto
; IN:
;	 al: atributo da imagem (original, strecth, tiled)
;	esi: Descritor de Imagem
;	edi: Objeto Imagem
; OUT:
;	cy = 0  -> OK
;	cy = 1  -> Error - ebp = codigo do erro

ObjImage_SetImage:
	mov [edx+ObjImage_Desc], esi
	jmp ObjImage_SetAttr
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; SET 0x13 - Muda o atributo do objeto
; IN:
;	 al: atributo da imagem (original, strecth, tiled)
;	edi: Objeto Imagem
; OUT:
;	cy = 0  -> OK
;	cy = 1  -> Error - ebp = codigo do erro

ObjImage_SetAttr:
   	pushad
   
    ; Cria o scroll horizontal
	mov edx, edi
	mov eax, [edx+Obj_SizeX]	; ajusta a posicao do scroll
	and eax, 0x0000ffff
	sub ax, 0x14
	mov ebx, [edx+Obj_SizeX]	; ajusta o tamanho do scroll
	sub ebx, 0x00140000		
	mov bx, 0x14
	mov ecx, 0x00080000
	call Object_Create
	mov [edx+ObjImage_VScroll], edi
	mov esi, [edx+ObjImage_Desc]
	xor ebx, ebx			; pega o tamanho da imagem e coloca no scroll
	or esi, esi
	jz @f
	mov bx, [esi+IHeight]
	sub bx, [edx+Obj_SizeY]		; Retira a parte ja visivel
	add bx, 0x0014			; Coloca a parte que scroll esconde
@@:	mov [edi+Scroll_Size], bx
	or word [edi+Obj_Attr], Obj_Attr_Dependente1 or Obj_Attr_Dependente2
	
    ; Cria o scroll vertical
	mov eax, [edx+Obj_SizeX]	; ajusta a posicao do scroll
	and eax, 0xffff0000
	sub eax, 0x00140000
	mov ebx, 0x00140000		; ajusta o tamanho do scroll
	mov bx, [edx+Obj_SizeX]
	sub bx, 0x14
	mov ecx, 0x00080000
	call Object_Create	
	mov [edx+ObjImage_HScroll], edi
	mov esi, [edx+ObjImage_Desc]
	xor ebx, ebx			; pega o tamanho da imagem e coloca no scroll
	or esi, esi
	jz @f
	mov bx, [esi+IWidth]
	sub bx, [edx+Obj_SizeX]		; Retira a parte ja visivel
	add bx, 0x0014			; Coloca a parte que scroll esconde
@@:	mov [edi+Scroll_Size], bx
	or word [edi+Obj_Attr], Obj_Attr_Dependente1 or Obj_Attr_Dependente2
	popad

  ; Se Atributo = Original com Scroll
  ;   Se necessario
  ;     Cria o Scroll
  ; Senao 
  ;   Se já contem o Scroll
  ;     Elimina o Scroll
	
  ; Se Atributo = Strecht  -> elimina scroll se tiver	
	
  ; Se Atributo = tiled    -> elimina scroll se tiver
	ret
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; SET 0x14 - Muda o tamanho virtual da imagem (scroll)
; IN:
;	A definir
; OUT:
;	A definir

ObjImage_SetSizeVirtual:

	ret
	
;-----------------------------------------------------------------------------------------
;-----------------------------------------------------------------------------------------