;-----------------------------------------------------------------------------------------

; Object_System

;-----------------------------------------------------------------------------------------

;* AOO -> Assembly Orientado a Objeto

;======================== V A R I A V E I S ==============================================

  ; Estrutura do objeto pai =====================
	   Obj_Ass		equ 0		; dd - Assinatura (-)
	   Obj_Prox		equ 4  		; dd - Proximo objeto (-)
	   Obj_Ant		equ 8		; dd - Objeto Anterior (-)
	   Obj_Up		equ 0xc		; dd - Objeto-Pai (-)
	   Obj_Down		equ 0x10	; dd - Ponteiro para o primeiro sub-objeto (-)
	   Obj_Program		equ 0x14	; dd - Ponteiro para o ultimo sub-objecto (-)
	   Obj_Comentario	equ 0x18	; dd - Ponteiro para o comentario do objeto (+)
	   Obj_PosX		equ 0x1c	; dw - Posicao X (-)
	   Obj_PosY		equ 0x1e	; dw - Posicao Y (-)
	   Obj_SizeX		equ 0x20	; dw - Tamanho X (-)
	   Obj_SizeY		equ 0x22	; dw - Tamanho Y (-)
	   Obj_Mouse		equ 0x24	; db - Ponteiro do Mouse (+)
	   Obj_Type		equ 0x25	; db - Tipo do objeto (-)
	   Obj_Free		equ 0x26	; db - free
	   Obj_Status		equ 0x27	; db - Status do objeto (-)
	   Obj_Code		equ 0x28	; dw - Codigo do objeto (+)
	   Obj_Attr		equ 0x2a	; dw - Atributo do objeto (+)
	   Obj_Color		equ 0x2c	; dd - Cor do fundo (-)
	Obj_Size  		equ 0x30	; Tamanho do objeto

     ; Atributos das variaveis do Objeto =========
       ; Estados do atributo Obj_Mouse
         Obj_Mouse_Seta 		equ 0x1
         Obj_Mouse_Texto 		equ 0x2
         Obj_Mouse_Link 		equ 0x3
         Obj_Mouse_Precisao 		equ 0x4
         Obj_Mouse_Ocupado1 		equ 0x5
         Obj_Mouse_Ocupado2 		equ 0x6
         Obj_Mouse_Ajuda 		equ 0x7
         Obj_Mouse_Naodisponivel	equ 0x8
         Obj_Mouse_Movimentacao 	equ 0x9
         Obj_Mouse_ResizeHor		equ 0xa	; Horizontal
         Obj_Mouse_ResizeVer		equ 0xb	; Vertical
         Obj_Mouse_ResizeDMa		equ 0xc	; Diagonal maior
         Obj_Mouse_ResizeDMe 		equ 0xd	; Diagonal menor
       ;------------------------------

       ; Estados do atributo Obj_Type
         Obj_Type_Puro		equ 0x0
         Obj_Type_Janela	equ 0x1
         Obj_Type_Button	equ 0x2
         Obj_Type_Label		equ 0x3
	 Obj_Type_Image		equ 0x4
	 Obj_Type_Quadro	equ 0x5
	 Obj_Type_Edit		equ 0x6
	 Obj_Type_Icone		equ 0x7
	 Obj_Type_Scroll	equ 0x8
	 Obj_Type_Frame		equ 0x9
	 Obj_Type_Progress	equ 0xa
	 Obj_Type_CheckBox	equ 0xb
	 Obj_Tyoe_Radio		equ 0xc 
       ;------------------------------

       ; Estados do atributo Obj_Type
         Obj_Attr_Clicavel	equ 0x0001	; Devolve evento quando clicado
         Obj_Attr_Imediato	equ 0x0002	; Devolve evento quando se mantem clicado
         Obj_Attr_Visivel	equ 0x0004	; Se é desenhado
         Obj_Attr_Borda		equ 0x0008	; Se tem borda
         Obj_Attr_SobreEscrito	equ 0x0010	; Se pode ser desenhado em cima
         Obj_Attr_Dependente1	equ 0x0020	; Se tem relacao de posicao com o objeto pai
         Obj_Attr_Dependente2	equ 0x0040	; Se tem dependencia funcional
         Obj_Attr_System	equ 0x0080	; Se é do sistema
         Obj_Attr_Free1		equ 0x0100
         Obj_Attr_Free2		equ 0x0200
         Obj_Attr_Free3		equ 0x0400
         Obj_Attr_Free4		equ 0x0800
         Obj_Attr_Free5		equ 0x1000
         Obj_Attr_Free6		equ 0x2000
         Obj_Attr_Free7		equ 0x4000
         Obj_Attr_Free8		equ 0x8000
       ;------------------------------

       ; Estados do atributo Obj_Status
         Obj_Status_Onhover		equ 0x01	; Se o mouse está sobre o objeto
         Obj_Status_Pressed		equ 0x02	; Se o mouse clicou nele
         Obj_Status_Duplo		equ 0x04	; Se o mouse deu um duplo click
         ;Obj_Status_Desativo		equ 0x10	; Se o objeto está Desativo
       ;------------------------------


     ;--------------------------------------------

  ; Tabela de Tamanho ==========================
    Obj_Table_Size:
    	db Obj_Size		; 0 - Objeto Puro
    	db Obj_Size+6		; 1 - Janela
    	db Obj_Size+0x10	; 2 - Botao
    	db Obj_Size+0x10	; 3 - Texto
    	db Obj_Size+0xe		; 4 - Imagem
    	db Obj_Size+0xc		; 5 - Quadro
    	db Obj_Size+0x24	; 6 - Edit
	db Obj_Size		; 7 - Icone
	db Obj_Size+6		; 8 - Scroll
	db Obj_Size		; 9 - Frame
	db Obj_Size+4		; 0xa- Progress bar
	db Obj_Size+8		; 0xb- CheckBox
	db 0			; 0xc- Radio
  ;-------------------------------------


  ; Tabelas de Rotinas ===========================

    ; Create =================
	Obj_Table_Create:
		dd Object_Create.end2
		dd ObjWin_Create
		dd ObjButton_Create
		dd ObjText_Create
		dd ObjImage_Create
		dd ObjQuadro_Create
		dd ObjEdit_Create
		dd ObjIcone_Create
		dd ObjScroll_Create
		dd 0
		dd ObjProgress_Create
    ;-------------------------

    ; Destroy ================
	Obj_Table_Destroy:
		dd Object_Destroy.end2
		dd ObjWin_Destroy
		dd ObjButton_Destroy
		dd ObjText_Destroy
		dd ObjImage_Destroy
		dd ObjQuadro_Destroy
		dd ObjEdit_Destroy
		dd ObjIcone_Destroy
		dd ObjScroll_Destroy
		dd 0
		dd ObjProgress_Destroy
    ;-------------------------

    ; Draw ================
	Obj_Table_Draw:
		dd Obj_Draw
		dd ObjWin_Draw
		dd ObjButton_Draw
		dd ObjText_Draw
		dd ObjImage_Draw
		dd ObjQuadro_Draw
		dd ObjEdit_Draw
		dd ObjIcone_Draw
		dd ObjScroll_Draw
		dd 0
		dd ObjProgress_Draw
    ;-------------------------

    ; Onhover ==============
	Obj_Table_Onhover:
		dd Object_Onhover.end2
		dd ObjWin_Onhover
		dd ObjButton_Onhover
		dd ObjText_Onhover
		dd ObjImage_Onhover
		dd ObjQuadro_Onhover
		dd ObjEdit_Onhover
		dd ObjIcone_Onhover
		dd ObjScroll_Onhover
		dd 0
		dd ObjProgress_Onhover
    ;-------------------------

    ; Saiu ==================
	Obj_Table_Saiu:
		dd Object_Onhover.end2
		dd ObjWin_Saiu
		dd ObjButton_Saiu
		dd ObjText_Saiu
		dd ObjImage_Saiu
		dd ObjQuadro_Saiu
		dd ObjEdit_Saiu
		dd ObjIcone_Saiu
		dd ObjScroll_Saiu
		dd 0
		dd ObjProgress_Saiu
    ;-------------------------

    ; Pressed ================
	Obj_Table_Pressed:
		dd Object_Pressed.end2
		dd ObjWin_Pressed
		dd ObjButton_Pressed
		dd ObjText_Pressed
		dd ObjImage_Pressed
		dd ObjQuadro_Pressed
		dd ObjEdit_Pressed
		dd ObjIcone_Pressed
		dd ObjScroll_Pressed
		dd 0
		dd ObjProgress_Pressed
    ;-------------------------

    ; Solto ==================
	Obj_Table_Soltou:
		dd Object_Soltou.end2
		dd ObjWin_Soltou
		dd ObjButton_Soltou
		dd ObjText_Soltou
		dd ObjImage_Soltou
		dd ObjQuadro_Soltou
		dd ObjEdit_Soltou
		dd ObjIcone_Soltou
		dd ObjScroll_Soltou
		dd 0
		dd ObjProgress_Soltou
    ;-------------------------

  ;-----------------------------------------------

  ; Variaveis ====================================
    Obj_Last dd 0	; O ultimo botao que esteve embaixo do mouse

  ;-----------------------------------------------

;-----------------------------------------------------------------------------------------


;======================== I N C L U D E S ================================================

include "ObjExp_Text.inc"
include "Obj_Scroll.inc"
include "Obj_Window.inc"
include "Obj_Button.inc"
include "Obj_Text.inc"
include "Obj_Image.inc"
include "Obj_Quadro.inc"
include "Obj_Edit.inc"
include "Obj_Icone.inc"
;include "Obj_Frame.inc"
include "Obj_Progress.inc"
;include "Obj_CheckBox.inc"

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Inicializa o sistema de objetos
; IN:
;	nothing
; OUT:
;	nothing

Object_Init:
; Inicia a lista de Janelas
	xor eax, eax
	mov [ObjList_Ini], eax
	mov [ObjList_End], eax
	mov [ObjList_Atual], eax

; Inicia a matriz de objetos
   ; Obtem o total de bytes necessario para a matriz
	movzx eax, word [Screen+IWidth]
	movzx ebx, word [Screen+IHeight]
	mul ebx
	shl eax, 2	; 4 bytes por ponto
   ; Aloca um bloco
	;mov ebx, Video_Heap
   	;call SMM_Heap_Alloc
	call aloca_HB

   ; Define as variaveis do descritor
	mov dword [ObjMatriz_Desc+IPointer], esi
	mov ax, [Screen+IWidth]
	mov word [ObjMatriz_Desc+IWidth], ax
	shl eax, 2
	mov word [ObjMatriz_Desc+IBytesPerLine], ax
	mov ax, [Screen+IHeight]
	mov word [ObjMatriz_Desc+IHeight], ax
	mov byte [ObjMatriz_Desc+IType], 0	; Cuidar aqui quando colocar aceleracao
	mov byte [ObjMatriz_Desc+IBpp], 32
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Fecha o sistema de Objetos
; IN:
;	nothing
; OUT:
;	nothing

Object_Deinit:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Cria um Objeto
; IN:
;	eax= [Y|X] Coordenate
;	ebx= [Y|X] Size
;	ecx= Attr/Tipo/Codigo	2HW -> Attr/Tipo  -  2LW -> codigo 	PP
;	edx= Ponteiro do Objeto-pai
;	  edx = 0  ->  sem objeto-pai	PP
; OUT:
;	edi= Ponteiro do objeto
;	cy = 0  -> OK
;	cy = 1  -> error (falta de memoria)

Object_Create:
	push eax
	push edx
	push esi

	or edx, edx
	jz .create
	cmp dword [edx], 'OBJ1'
	jne .error

; Cria o objeto
  ; Aloca um espaco
.create:push eax
	push ecx
	push esi
	shr ecx, 0x10
	mov di, cx		; salva o atributo e o tipo
	xor ch, ch		; apaga o atributo
	movzx eax, byte [Obj_Table_Size+ecx]
	call aloca_HB
	jc .error
	mov [esi+Obj_Type], di	; coloca o tipo e atributos do objeto
	mov edi, esi
	pop esi
	pop ecx
	pop eax

  ; Coloca os dados passado por parametro no objeto
@@:	push eax
	mov eax, [Task_Current]
	mov [edi+Obj_Program], eax
	xor eax, eax
	mov dword [edi], 'OBJ1'	; assinatura
	mov [edi+Obj_Prox], eax	; Zera todos os ponteiros
	mov [edi+Obj_Ant], eax
	mov [edi+Obj_Down], eax
	mov [edi+Obj_Comentario], eax
	mov [edi+Obj_Mouse], al	; Seleciona cursor padrao
	pop eax
	mov [edi+Obj_Up], edx	; coloca o ponteiro do objeto pai
	mov [edi+Obj_PosX], eax	; coloca PosX e PosY
	mov [edi+Obj_SizeX], ebx	; coloca SizeX e SizeY
	mov [edi+Obj_Code], cx	; coloca o attr, tipo e o codigo
	mov dword [edi+Obj_Color], 0xdddddd	; Cor padrao
	mov word [edi+Obj_Attr], 0x04	; coloca o atributo padrao= Visivel, Nao dependente,
	mov byte [edi+Obj_Status], 0

  ; Coloca o item no final da lista de objetos
    ; Está no topo, ou seja, nao tem objeto pai
	or edx, edx
	jnz .sub
	mov [ObjList_Atual], edi
	mov [ObjList_End], edi
	mov eax, [ObjList_Ini]
	or eax, eax
	jnz .loop
	mov [ObjList_Ini], edi
	jmp .exec

    ; É subordinado à um objeto
.sub:	or word [edi+Obj_Attr], Obj_Attr_Dependente1
	mov eax, [edx+Obj_Down]
	or eax, eax
	jnz .loop
	mov [edx+Obj_Down], edi
	jmp .exec

  ; Vai até o ultimo item da lista
.loop:	cmp dword [eax+Obj_Prox], 0
	jz .endlist
	mov eax, [eax+Obj_Prox]
	jmp .loop
.endlist:	
	mov [eax+Obj_Prox], edi
	mov [edi+Obj_Ant], eax

  ; Marca na matriz o objeto
.exec:  push ebx
  	push ecx
  	call Object_MontaMatriz
  	pop ecx
  	pop ebx

  ; Executa rotina
  	movzx eax, byte [edi+Obj_Type]
  	call dword [Obj_Table_Create+eax*4]

; Fim
.end:	pop esi
	pop edx
	pop eax
.end2:	ret

; Error -> Falta de espaco ou ponteiro do objeto pai invalido
.error:	stc
	pop esi
	pop edx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Destroi um Objeto
; IN:
;	edi: Objeto
; OUT:
;	cy= 0  -> OK
;	cy= 1  -> Error - objeto invalido
; Com problemas

Object_Destroy:
	push eax
	push ebx
	push esi
	push edi

  ; Verifica se é um objeto
  	cmp dword [edi+Obj_Ass], 'OBJ1'
  	jnz .error

  ; Retira o objeto da lista
	mov eax, [edi+Obj_Ant]
	mov ebx, [edi+Obj_Prox]

    ; Se eax == 0
	or eax, eax
	jnz .senao1

      ; -> Se edi->Obj_Up == 0  -> Coloca na lista de objeto do sistema
	mov esi, [edi+Obj_Up]
	or esi, esi
	jnz .sub1
	mov [ObjList_Ini], ebx
	jmp .se2

      ; Senao  -> Coloca na lista do objeto do objeto pai
.sub1:	mov [esi+Obj_Down], ebx
	jmp .se2

    ; Senao -> eax->prox = ebx
.senao1:	mov [eax+Obj_Prox], ebx

    ; Se ebx == 0
.se2:   or ebx, ebx
    	jnz .senao2

       ; ->  Se edi->Obj_Up != 0  -> Coloca na lista de objeto do sistema
       	mov esi, [edi+Obj_Up]
	or esi, esi
	jnz .lib
	mov [ObjList_Atual], eax
	mov [ObjList_End], eax
	jmp .lib

    ;  Senao  -> ebx->Obj_Ant = eax
.senao2:mov [ebx+Obj_Ant], eax

  ; Libera o bloco
.lib:  	call Object_Free

  ; Remonta a matrize redesenha os objetos
  	call Object_MontaAllMatriz
	call Object_SendAllDraw

.end:	pop edi
	pop esi
	pop ebx
	pop eax
.end2:	ret

.error:	stc
	pop edi
	pop esi
	pop ebx
	pop eax
	ret


  ;====================== S U B F U N C T I O N ======================
  ; RECURSIVA - Libera um objeto
  ; IN:
  ;	edi= objeto
  ; OUT:
  ;	nothing

  Object_Free:
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .error

    ; Libera os Sub-bjetos
	push edi
	mov edi, [edi+Obj_Down]
.loop:	or edi, edi		; verifica se terminou a lista
  	jz .lib
	call Object_Free		; libera o objeto
	jc .lib
	mov edi, [edi+Obj_Prox]	; salva o proximo objeto
	jmp .loop

    ; Libera o bloco do objeto
.lib:    	pop edi
	push esi
    	mov esi, edi
    	call lib_HB
	pop esi
	ret

.error:	stc
	ret

  ;-------------------------------------------------------------------

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Retorna a posicao absoluta do objeto
; IN:
;	eax: [Y|X] coordenate
;	edi: Objeto
; OUT:
;	eax: [Y|X] coordenate
;	cy = 0  ->  OK
;	cy = 1  ->  Error - Objeto invalido ou atributo invalido

Object_GetPos:
	push ebx
	push ecx

  ; Ajusta a coordenada e verifica se é o objeto é dependente posicional de outro objeto
	add eax, [edi+Obj_PosX]
	test word [edi+Obj_Attr], Obj_Attr_Dependente1
	jz .end
	mov ebx, [edi+Obj_Up]
.loop1:	or ebx, ebx
	jz .end
	mov cx, [ebx+Obj_Attr]

  ; Ajusta coordenada
	add eax, [ebx+Obj_PosX]

  ; Se dependente==1  -> Pega o ponteiro do objeto pai
	test cx, Obj_Attr_Dependente1
	jz .end
	mov ebx, [ebx+Obj_Up]
	jmp .loop1

.end:	pop ecx
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Muda a posicao do objeto
; IN:
;	eax: [Y|X] coordenate
;	edi: Objeto
; OUT:
;	nothing
;	cy = 0  ->  OK
;	cy = 1  ->  Error - Objeto invalido ou atributo invalido

Object_SetPos:
	push esi
	cmp dword [edi+Obj_Ass], 'OBJ1'
	jne .end

	mov esi, [edi+Obj_Up]
.loop:	or esi, esi
	jz .set
	sub eax, [esi+Obj_PosX]
	mov esi, [esi+Obj_Up]
	jmp .loop
.set:
	mov [edi+Obj_PosX], eax

  ; Se (edi->Obj_Up==0)  -> desenha todos os objetos da tela
	;cmp dword [edi+Obj_Up], 0
	;jnz .sub
	call Object_MontaAllMatriz
	call Object_SendAllDraw		; otimizar para desenhar apenas onde ele estava
	jmp .end

  ; Senao  -> Desenha apenas todos os objetos do objeto pai (No está funcionando direito)
.sub:	;mov edi, [edi+Obj_Up]
	;call Object_MontaMatriz
	;call Object_AllDraw

  ; Fim
.end:	pop esi
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Muda o tamanho do objeto
; IN:
;	eax: [Y|X] coordenate
;	edi: Objeto
; OUT:
;	nothing
;	cy = 0  ->  OK
;	cy = 1  ->  Error - Objeto invalido ou atributo invalido

Object_SetSize:
	mov [edi+Obj_SizeX], eax
	jmp Object_AllDraw


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Muda a cor do objeto
; IN:
;	ecx: color
;	edi: Objeto
; OUT:
;	nothing
;	cy = 0  ->  OK
;	cy = 1  ->  Error - Objeto invalido ou atributo invalido

Object_SetColor:
	mov [edi+Obj_Color], ecx
	jmp Object_Draw


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; Desenha apenas o objeto pai
; IN:
;	edi: Objeto
; OUT:
;	nothing
;	cy = 0  ->  OK
;	cy = 1  ->  Error - Objeto invalido

Object_Draw:
	push eax
	push ebx
	push ecx

  ; Ajusta a coordenada
	xor eax, eax
	test word [edi+Obj_Attr], Obj_Attr_Dependente1
	jz .draw
	mov ebx, [edi+Obj_Up]
.loop:	or ebx, ebx
	jz .draw
	mov cx, [ebx+Obj_Attr]

    ; Se visivel==0  -> Nao desenha
	test cx, Obj_Attr_Visivel
	jz .end

    ; Ajusta coordenada
	add eax, [ebx+Obj_PosX]

    ; Se dependente==0  -> Desenha, pois nao precisa ajustar as coordenadas
	test cx, Obj_Attr_Dependente1
	jz .draw
	mov ebx, [ebx+Obj_Up]
	jmp .loop

  ; Desenha o objeto
.draw:	movzx ebx, byte [edi+Obj_Type]
	call dword [Obj_Table_Draw+ebx*4]

.end:	pop ecx
	pop ebx
	pop eax
.end2:	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  9 =============================================
; RECURSIVA - Desenha o objeto e todos os seus sub-objetos
; IN:
;	edi: Objeto
; OUT:
;	nothing
;	cy = 0  ->  OK
;	cy = 1  ->  Error - Objeto invalido

Object_AllDraw:
	push eax
	push edi

  ; Desenha o objeto
	movzx eax, byte [edi+Obj_Type]
	call Object_Draw

  ; Desenha os seus sub-objetos
	mov edi, [edi+Obj_Down]
.loop:	or edi, edi
	jz .end
	call Object_AllDraw
	mov edi, [edi+Obj_Prox]
	jmp .loop

.end:	pop edi
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  9 =============================================
; Envia comando ao programa para redesenhar o objeto
; IN:
;	edi= Objeto
;	 dl= Atributo
;	  (1- redesenha todos os objetos, 3- redesenha so o objeto)
; OUT:
;	nothing

Object_SendDraw:
	push ecx
	push edx
	push esi

	mov esi, [edi+Obj_Program]
	mov byte [esi+Task_EventoSStatus], 1
	xor cl, cl
	call Task_SendDados

	mov cl, 2
	mov edx, edi
	call Task_SendDados	; Envia o ponteiro do Objeto
	mov byte [esi+Task_EventoSStatus], 0

.end:	pop esi
	pop edx
	pop ecx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  9 =============================================
; Manda redesenhar todos os objetos
; IN:
;	nothing
; OUT:
;	nothing

Object_SendAllDraw:
	push edx
	push edi
	mov dl, 1
  	mov edi, [ObjList_Ini]

.loop:  	or edi, edi
  	jz .end
  	call Object_SendDraw  ; Envia comando ao programa para redesenhar o objetos
  	mov edi, [edi+Obj_Prox]
	jmp .loop

.end:	pop edi
	pop edx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 0 ============================================
; Executa ação quando o mouse está sobre o objeto
; IN:
;	eax: [Y|X] coordenate
; OUT:
;	edi: Objeto
;	cy = 0  ->  OK
;	cy = 1  ->  Error - Objeto invalido

Object_Onhover:
	push eax
	push ebx
	push ecx
	push edi

  ; Pega o objeto
	mov esi, ObjMatriz_Desc
	call getPixel
	cmp dword [ecx], 'OBJ1'
	jne .end

  ; Marca as variveis
	mov edi, [Obj_Last]
  	cmp edi, ecx
  	jne .onhover
  	test byte [edi+Obj_Status], Obj_Status_Onhover
  	jnz .end
.onhover:	
	and byte [edi+Obj_Status], not Obj_Status_Onhover
	movzx eax, byte [edi+Obj_Type]
	mov eax, [Obj_Table_Saiu+eax*4]
	or eax, eax
	jz .end
	call eax
	or byte [ecx+Obj_Status], Obj_Status_Onhover
	mov [Obj_Last], ecx

  ; Executa rotina indexada
	mov edi, ecx
	movzx eax, byte [edi+Obj_Type]
	mov eax, [Obj_Table_Onhover+eax*4]
	or eax, eax
	jz .end
	call eax

.end:	pop edi
	pop ecx
	pop ebx
	pop eax
.end2:	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 1 ============================================
; Executa ação quando o objeto é pressionado
; IN:
;	eax: [X|Y] coordenate
;	ecx: objeto
; OUT:
;	nothing
;	cy = 0  ->  OK
;	cy = 1  ->  Error - Objeto invalido
; Altera:
;	edi

Object_Pressed:
	push eax

  ; Pega o objeto
  	mov esi, ObjMatriz_Desc
	call getPixel
	cmp dword [ecx], 'OBJ1'
	jne .end

  ; Se mudou de objeto  -> Executa Objeto_Saiu
  	mov edi, [Obj_Last]
  	cmp edi, ecx
  	je .a
 	and byte [edi+Obj_Status], not Obj_Status_Pressed
	movzx eax, byte [edi+Obj_Type]
	call dword [Obj_Table_Saiu+eax*4]	; Object_Saiu

  ; Atualiza o status
.a:	test word [ecx+Obj_Status], Obj_Attr_Imediato	; se for imediato, pula..
	jnz @f
	test byte [ecx+Obj_Status], Obj_Status_Pressed	; verifica se ja está pressionado
	jnz .end
@@:	or byte [ecx+Obj_Status], Obj_Status_Pressed	; Marca como pressionado
	and byte [ecx+Obj_Status], not Obj_Status_Onhover

  ; Executa rotina indexada
  	call Object_UpPressed

.end:	pop eax
.end2:	ret


  ;====================== S U B F U N C T I O N  1 =======================================
  ; Recursiva - Quando um objeto tem dependecia funcional 
  ; IN:
  ;	  ecx: Objecto
  ; OUT:
  ;	  nothing
  ; Altera:
  ;	  eax, edi
  ; OBS: NAO MUDE O EDX

  Object_UpPressed:
  	push ecx
  
    ; Verifica se eh um objeto
    	cmp dword [ecx], 'OBJ1'
	jne .end
  
    ; Executa rotina indexada
  	mov edi, ecx
	movzx eax, byte [edi+Obj_Type]
	mov eax, [Obj_Table_Pressed+eax*4]
	or eax, eax
	jz .dep
	call eax		; Obj_Pressed

    ; Se dependente funcional  -> Executa rotina do objeto pai
.dep:	test word [edi+Obj_Attr], Obj_Attr_Dependente2
  	jz .send
	cmp dword [edi+Obj_Up], 0
	jz .send
	mov ecx, [edi+Obj_Up]	
	call Object_UpPressed

    ; Se objeto == Clicavel e Imediato  -> Envia codigo do objeto ao programa
.send:  mov cx, [edi+Obj_Attr]
  	test cx, Obj_Attr_Clicavel
  	jz .end
  	test cx, Obj_Attr_Imediato
  	jz .end
	mov esi, [edi+Obj_Program]
	mov cx, [edi+Obj_Code]
	push edx
	mov dx, Mouse_Pressed
	call Task_SendEvento
	pop edx
  
.end:  	pop ecx
  	ret
  
  ;---------------------------------------------------------------------------------------

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 2 ============================================
; Executa ação quando o mouse soltou o botao
; IN:
;	edi: Objeto
; OUT:
;	nothing
;	cy = 0  ->  OK
;	cy = 1  ->  Error - Objeto invalido

Object_Soltou:
	push eax
	push ecx

  ; Pega o objeto
	mov esi, ObjMatriz_Desc
	call getPixel
	cmp dword [ecx], 'OBJ1'
	jne .end
	and byte [ecx+Obj_Status], not Obj_Status_Pressed	; Desmarca como pressionado
  
  	call Object_UpSoltou

.end:	pop ecx
	pop eax
.end2:	ret


  ;====================== S U B F U N C T I O N  1 =======================================
  ; Recursiva - Quando um objeto tem dependecia funcional 
  ; IN:
  ;	  ecx: Objecto
  ; OUT:
  ;	  nothing
  ; Altera:
  ;	  eax, ecx, edx, esi

  Object_UpSoltou:
  	push edi

	mov edi, ecx
	or edi, edi
	jz .end
  	cmp dword [edi], 'OBJ1'
	jne .end
  
    ; Se dependente funcional
  	test byte [edi+Obj_Attr], Obj_Attr_Dependente2
  	jz @f
  	mov ecx, [edi+Obj_Up]
  	call Object_UpSoltou
  
    ; Executa rotina indexada
@@:	movzx eax, byte [edi+Obj_Type]
	call dword [Obj_Table_Soltou+eax*4]
	
    ; Se (objeto == Clicavel) && (objeto != Imediato)  -> Envia codigo do objeto ao programa
  	mov cx, [edi+Obj_Attr]
  	test cx, Obj_Attr_Clicavel
  	jz .end
  	test cx, Obj_Attr_Imediato
  	jnz .end
	mov esi, [edi+Obj_Program]
	mov cx, [edi+Obj_Code]
	mov dx, Mouse_Pressed
	call Task_SendEvento
  	
.end:  	pop edi
  	ret
  
  ;---------------------------------------------------------------------------------------
  
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 3 ============================================
; Executa ação quando o objeto recebeu um duplo click
; IN:
;	edi: Objeto
; OUT:
;	nothing
;	cy = 0  ->  OK
;	cy = 1  ->  Error - Objeto invalido

Object_Duplo:
	push eax
	movzx eax, byte [edi+Obj_Type]
	;call dword [Obj_Table_Duplo+eax*4]
	pop eax
.end:	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 4 ============================================
; Desenha o fantasma do objeto (Ex: quando é arrastado)
; IN:
;	eax: [Y|X] coordenate
;	edi: Objeto
; OUT:
;	nothing
;	cy = 0  ->  OK
;	cy = 1  ->  Error - Objeto invalido

Object_DrawFantasma:
	push ecx
	push edx
	push edi

  ; Verifica se é um objeto
	;cmp dword [edi+Obj_Ass], 'OBJ1'
	;jne .error

  ; Verifica se é necessario desenhar
	;test byte [edi+Obj_Attr], Obj_Attr_Visivel
	;jnz .end

  ; Desenha a borda
	mov ebx, [edi+Obj_SizeX]
	mov ecx, 0x01000000
	xor edx, edx
	mov edi, Screen
	call draw_Rect

.end:	pop edi
	pop edx
	pop ecx
	ret

.error:	stc
	pop edx
	pop edi
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 7 ============================================
; Verifica o objeto no ponta A (ax,bx)
; IN:
;	eax: coordenada X
;	ebx: coordenada Y
;	edx: objeto
; OUT:
;	cy= 0  -> Coordenada dentro do objeto
;	cy= 1  -> Coordenada fora do objeto

Object_CheckPixel:
	push ebx
	push edx
	push esi
	push edi

  ; Pega o pixel
	mov esi, ObjMatriz_Desc
	imul ebx, [esi+IBytesPerLine]	; ebx = y * y multiplier
     	lea  edi, [ebx+eax*4]	; edi = x*4+(y*y multiplier)
     	add  edi, [esi+IPointer]   	; ebx = where pixel is in memory
     	mov  ebx, [edi]

  ; Pode escrever em cima do objeto
  	test word [ebx+Obj_Attr], Obj_Attr_SobreEscrito
	jnz .end

  ; Compara
  	clc
  	cmp edx, ebx
  	je .end
	stc

.end:	pop edi
	pop esi
	pop edx
	pop ebx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 7 ============================================
; RECURSIVA - Monta a matriz de objetos e de seus sub objetos
; IN:
;	edi: Objeto
; OUT:
;	nothing
; Altera:
;	eax, ebx, ecx

Object_MontaMatriz:
	push edx
	push edi

  ; Ajusta a coordenada e verifica se é necessario marcar na matriz
	xor eax, eax
	xor edx, edx
	test word [edi+Obj_Attr], Obj_Attr_Dependente1
	jz .marca2
	mov ebx, [edi+Obj_Up]

.loop1:	or ebx, ebx
	jz .marca1
	mov cx, [ebx+Obj_Attr]

    ; Se visivel==0  -> Nao desenha
	test cx, Obj_Attr_Visivel
	jz .end

    ; Ajusta coordenada
	add eax, [ebx+Obj_PosX]

    ; Se dependente==0  -> Desenha, pois nao precisa ajustar as coordenadas
	test cx, Obj_Attr_Dependente1
	jz .marca1
	mov ebx, [ebx+Obj_Up]
	jmp .loop1

  ; Marca na matriz
.marca1: mov edx, [edi+Obj_Up]
.marca2: add eax, [edi+Obj_PosX]	; eax = posicao YX da janela
	mov ebx, [edi+Obj_SizeX]	; ebx = tamanho YX da janela
	mov ecx, edi
	mov edi, ObjMatriz_Desc
	call Object_DrawRectFill		  ; Marca na matriz

  ; Marca na matriz os sub-objetos
	mov edx, ecx
	mov edi, [ecx+Obj_Down]
.loop2:	or edi, edi
	jz .end
	call Object_MontaMatriz
	mov edi, [edi+Obj_Prox]
	jmp .loop2

.end:	pop edi
	pop edx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 7 ============================================
; Monta a matriz de todos os objetos
; IN:
;	nothing
; OUT:
;	nothing

Object_MontaAllMatriz:
	push edx
	push edi

	mov edi, [ObjList_Ini]
.loop:	or edi, edi
	jz .end
	cmp dword [edi], 'OBJ1'
	jne .end
	call Object_MontaMatriz
	mov edi, [edi+Obj_Prox]
	jmp .loop

.end:	pop edi
	pop edx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Desenha um retangulo preenchido na matriz de objetos
; (Usa a mesma logica que a rotina draw_RectFill, mas otimizado e especifico)
; IN:
;	eax: [Y|X] coordenada
;	ebx: [Y|X] size
;	ecx: Objeto
;	edx: Objeto Pai
; OUT:
;	nothing

  ; variaveis ================
    sizeY  equ ebp+6	; dw - tamanho Y
    sizeX  equ ebp+4	; dw - tamanho X
    posY 	 equ ebp+2	; dw - posicao Y
    posX   equ ebp		; dw - posicao X
  ;---------------------------

Object_DrawRectFill:
	pushad
	mov ebp, esp
    	sub esp, 8
    	mov [posX], eax
	mov [sizeX], ebx

  ; Verifica as coordenadas
    ; (x>0 e y>0)  && (x<tamanho e y<tamanho
      ; Se posX<0	-> Ajusta coordenada
  	or ax, ax
  	jns @f
  	neg ax
  	sub [sizeX], ax
  	mov word [posX], 0

      ; Se posX>tamanho  -> Sai
@@:     	cmp ax, [ObjMatriz_Desc+IWidth]
      	ja .end

      ; Se posY<0	-> Ajusta coordenada
  	shr eax, 0x10
  	or ax, ax
  	jns @f
  	neg ax
  	sub [sizeY], ax
  	mov word [posY], 0

      ; Se posY>tamanho  -> Sai
@@:      	cmp ax, [ObjMatriz_Desc+IHeight]
      	ja .end

    ; (SizeX>0 && SizeX<Tamanho total)
      ; Se SizeX<0  -> Sai
      	mov ebx, [sizeX]
	or bx, bx
      	js .end

      ; Se PosX+sizeX>Tamanho   -> Ajusta coordenada
	add bx, [posX]
	cmp bx, [ObjMatriz_Desc+IWidth]
	jbe @f
	mov bx, [ObjMatriz_Desc+IWidth]
	mov ax, [posX]
	sub bx, ax
	mov [sizeX], bx

    ; (SizeY>0 && SizeY<Tamanho total)
      ; Se sizeY<0   -> Incrementa
@@:    	mov bx, [sizeY]
	or bx, bx
	js .end

      ; Se sizeY>tamanho -> Ajusta coordenada
	add bx, [posY]
	cmp bx, [ObjMatriz_Desc+IHeight]
	jbe .calc
	mov bx, [ObjMatriz_Desc+IHeight]
	mov ax, [posY]
	sub bx, ax
	mov [sizeY], bx

  ; Calcula o endereco base
.calc:	movzx eax, word [posX]
	movzx ebx, word [posY]
	imul ebx, dword [ObjMatriz_Desc+IBytesPerLine]
	lea edi, [ebx+eax*4]
	add edi, dword [ObjMatriz_Desc+IPointer]

  ; Coloca uma linha na horizontal
.newLine:	push edi
	movzx ebx, word [sizeX]

    ; Se (edx == 0) || (Objeto pai == objeto do pixel) -> coloca o pixel
.put:	or edx, edx
	jz .pula
	cmp [edi], edx
	jne .incX

    ; Coloca o pixel e incrementa o X
.pula:	mov [edi], ecx
.incX:	dec ebx
	jz .incLine
	add edi, 4
	jmp .put

; Incrementa o Y
.incLine:	pop edi
	add edi, dword [ObjMatriz_Desc+IBytesPerLine]
	dec word [sizeY]
	jnz .newLine

.end:	add esp, 8
	popad
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 7 ============================================
; Desenha o objeto puro
; IN:
;	eax: [Y|X] coordenadas base
;	edi: Objeto
; OUT:
;	nothing

Obj_Draw:
	pushad
	add eax, [edi+Obj_PosX]
	mov ebx, [edi+Obj_SizeX]
	mov ecx, [edi+Obj_Color]
	mov edx, edi
	mov edi, Screen
	call draw_RectFill
	popad
	ret


;-----------------------------------------------------------------------------------------
