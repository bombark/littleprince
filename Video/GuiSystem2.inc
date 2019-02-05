;-----------------------------------------------------------------------------------------

; GUI System

;-----------------------------------------------------------------------------------------


;======================== R E L A T O R I O ==============================================

; Criar Janela
; 

; Criar Botao

; Criar Label
; | | PosXY | SizeXY | Codigo | Cor da Font | Pointeiro 	

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

       ; Estados do atributo Obj_Type
         Obj_Type_Reserved	equ 0x0
         Obj_Type_Hidden	equ 0x1
         Obj_Type_Button	equ 0x2
         Obj_Type_Button2	equ 0x3
         Obj_Type_Text	equ 0x4
         Obj_Type_Input	equ 0x5
         Obj_Type_Input2	equ 0x6
         Obj_Type_Option	equ 0x7
         Obj_Type_Radio	equ 0x8
         Obj_Type_Checkbox	equ 0x9
         Obj_Type_Image	equ 0xa
         Obj_Type_Quadro	equ 0xb					
       ;------------------------------	

     ;--------------------------------------------
     
   ;----------------------------------------------



   ; Estrutura de um Quadro ======================
   	  Quadro_NumberX	equ 0		; dw - Numero do quadro no eixo X
   	  Quadro_NumberY	equ 2		; dw - Numero do quadro no eixo Y
   	  Quadro_SizeX	equ 4		; dw - Tamanho X do quadro
   	  Quadro_SizeY	equ 6		; dw - Tamanho Y do quadro
   	  Quadro_Image	equ 8		; dd - Ponteiro do descritor de imagem
	Obj_QuadroSize	equ Obj_Size+0x10	; Tamanho do objeto icone
; Quadro seria mostrar parte de uma imagem (utiliza a rotina blit_Image)

   ;----------------------------------------------


;-------------------------------------------------


; Tabela de bibliotecas ==========================

GUI_Libraries_Table:
	dd GUI_Window_Table

;-------------------------------------------------


; Tabela de Rotinas ==============================

GUI_Window_Table:
	dd 0;GUI_Create_Obj		; 0- Cria um objeto puro
	dd 0;GUI_Create_Text	; 1- Cria uma Janela
	dd GUI_Create_Button	; 2- Cria um botao
	dd GUI_Create_Text		; 3- Cria um label
	dd 0;GUI_Create_Option	; 4- Cria campo de opcoes
	dd 0;GUI_Create_Bitmap	; 5- Cria um bitmap limpo
	dd 0;GUI_Create_Image	; 6- Cria imagem
	dd 0;GUI_Create_Quadro	; 7- Cria quadro
	dd 0;GUI_Create_Icone	; 8- Cria icone
	;dd GUI_Create_Hidden	; 9- Cria um objeto invisivel
	;dd GUI_Create_Frame	; a- Cria um frame ( igual ao um iframe do html)


;-------------------------------------------------


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Executa um Script
; IN:
;	edx: Objeto pai
;	esi: script
; OUT:
; OBS=  Sempre manter o esi como objeto fonte e edx como ponteiro da janela

GUI_Run:
	pushad
	
; Loop Principal
.loop:	xor ebx, ebx		; Zera as variaveis
	xor ebp, ebp
	mov ebx, [esi]
	
  ; Se (bh==0xff)  ->  Executa uma tarefa ou cria um objeto pai 	
	or ebx, ebx
	jz .end
	mov bp, bx
	shr ebx, 0x10
	cmp bh, 0xff
	jne .next		
	mov ebx, GUI_Window_Table;[tabela+ebx]
	add esi, 4
	call dword [ebx+ebp*4]		
	jmp .loop	

  ; Senao    -> Pega o proximo dword
.next:	add esi, 4
	jmp .loop

; Fim
.end:	popad
	ret

; Error - objeto fonte invalido ou janela invalida
.error:	stc
	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; IN:
;	edx: objeto pai
;	   edx = 0  -> Criar objeto pai
;	   edx != 0  -> Criar sub-objeto ligado ao objeto apontado por edx
;	esi: objeto fonte
; OUT:
;	cy = 0  -  OK
;	cy = 1  -  error

GUI_Create_Button:
; Cria o objeto
	mov eax, [esi]		; PosX/PosY
	add esi, 4
	mov ebx, [esi]		; SizeX/SizeY
	add esi, 4
	mov ecx, [esi]		; Type/Code
	add esi, 4
	call Object_Create	
	mov dword [edi+Button_TextDesc+Text_Color], 0x2ff0000
	mov ecx, [esi]
	add esi, 4
	mov [edi+Button_TextDesc+Text_Pointer], ecx
	
; Desenha o objeto
	call Object_Draw	;GUI_Button_Draw			
	ret

; Error - Sem memoria
.error:	stc
	pop esi
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; IN:
;	ebx: objeto pai
;	   ebx = 0  -> Criar objeto pai
;	   ebx != 0  -> Criar sub-objeto ligado ao objeto apontado por ebx
;	esi: objeto fonte
; OUT:
;	cy = 0  -  OK
;	cy = 1  -  error

GUI_Create_Option:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; IN:
;	ebx: objeto pai
;	   ebx = 0  -> Criar objeto pai
;	   ebx != 0  -> Criar sub-objeto ligado ao objeto apontado por ebx
;	esi: objeto fonte
; OUT:
;	cy = 0  -  OK
;	cy = 1  -  error


GUI_Create_Text:
; Cria o objeto
	mov eax, [esi]		; PosX/PosY
	add esi, 4
	mov ebx, [esi]		; SizeX/SizeY
	add esi, 4
	mov ecx, [esi]		; Type/Code
	add esi, 4
	push esi
	xor esi, esi
	call Object_Create
	pop esi

  ; Coloca os dados do objeto texto
	shr ecx, 0x10

	mov eax, 0xbbbbbb		; cor do fundo
	mov [edi+Obj_Color], eax

	mov eax, [esi]		; cor da fonte
	mov [edi+Text_Color], eax
	add esi, 4

	;mov al, [esi]		; tamanho da fonte
	;mov [edi+Text_FontSize], al
	;inc esi

	mov eax, [esi]		; buffer do texto
	mov [edi+Text_Pointer], eax
	add esi, 4

; Desenha o objeto
.draw:	call Object_Draw	;GUI_Text_Draw
	ret

; Error - Sem memoria
.error:	stc
	pop esi
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; IN:
;	ebx: objeto pai
;	   ebx = 0  -> Criar objeto pai
;	   ebx != 0  -> Criar sub-objeto ligado ao objeto apontado por ebx
;	esi: objeto fonte
; OUT:
;	cy = 0  -  OK
;	cy = 1  -  error
;
;GUI_Create_InputText:
;; Cria o objeto
;	mov eax, [esi]		; PosX/PosY
;	add esi, 4
;	mov ebx, [esi]		; SizeX/SizeY
;	add esi, 4
;	mov ecx, [esi]		; Type/Code
;	add esi, 4
;	push esi
;	xor esi, esi
;	call Object_Create
;	pop esi
;
;  ; Coloca os dados
;	shr ecx, 0x10
;	mov byte [edi+Obj_Attr], 0	; Atributo do objeto
;
;	mov eax, 0xbbbbbb		; cor do fundo
;	mov [edi+Obj_Color], eax
;
;	mov eax, [esi]		; cor da fonte
;	mov [edi+Text_FontColor], eax
;	add esi, 4
;
;	;mov al, [esi]		; tamanho da fonte
;	;mov [edi+Text_FontSize], al
;	;inc esi
;
;	mov eax, [esi]		; buffer do texto
;	mov [edi+Text_Pointer], eax
;	add esi, 4
;
;	shr bx, 0x3		; Tamanho maximo de letras na horizontal e vertical
;	dec bx
;	mov [edi+Text_SizeX], bx
;	shr ebx, 0x14
;	dec bx
;	mov [edi+Text_SizeY], bx
;
;; Desenha o objeto
;.draw:	call Object_Draw	;GUI_InputText_Draw
;	ret
;
;; Error - Sem memoria
;.error:	stc
;	pop esi
;	ret
;
;
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; IN:
;	ebx: objeto pai
;	   ebx = 0  -> Criar objeto pai
;	   ebx != 0  -> Criar sub-objeto ligado ao objeto apontado por ebx
;	esi: objeto fonte
; OUT:
;	cy = 0  -  OK
;	cy = 1  -  error
;
;GUI_Create_Image:
;; Cria o objeto
;	mov eax, [esi]		; PosX/PosY
;	add esi, 4
;	mov ebx, [esi]		; SizeX/SizeY
;	add esi, 4
;	mov ecx, [esi]		; Type/Code
;	add esi, 4
;	push esi
;	xor esi, esi
;	call Object_Create
;	pop esi
;
;  ; Carrega a imagem na memoria
;	push esi
;	push edi
;	xor ebx, ebx
;	mov esi, [esi]
;	add edi, Obj_Size
;	call load_Image
;	pop edi
;	pop esi
;	add esi, 4
;
;  ; Atualiza o tamanho da imagem no objeto
;  	mov ebx, [edi+Image_Height]
;  	mov [edi+Obj_SizeX], ebx
;
;; Desenha o objeto
;	call Object_Draw	;GUI_Image_Draw
;	ret
;
;
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; IN:
;	ebx: objeto pai
;	   ebx = 0  -> Criar objeto pai
;	   ebx != 0  -> Criar sub-objeto ligado ao objeto apontado por ebx
;	edx: ponteiro da janela
;	esi: objeto fonte
; OUT:
;	cy = 0  -  OK
;	cy = 1  -  error
; | PosXY | TamXY | Tipo/Codigo | PosXY | TamXY | PosXY | TamXY |
;
;texto_teste db 0,"Felipe",0
;
;GUI_Create_Icone:
;	push esi
;	mov edx, [window_Table+4]
;
;; Cria o objeto pai
;	mov eax, [esi]		; PosX/PosY
;	add esi, 4
;	mov ebx, [esi]		; SizeX/SizeY
;	add esi, 4
;	mov ecx, [esi]		; Type/Code
;	add esi, 4
;	
;	push esi
;	xor esi, esi
;	call Object_Create	
;	pop esi
;
;; Cria o objeto da foto
;	mov eax, [esi]		; PosX/PosY
;	add esi, 4
;	mov ebx, [esi]		; SizeX/SizeY
;	add esi, 4
;	mov ecx, 0x00070001
;	push esi
;	push edi
;	mov esi, edi
;	call Object_Create
;	pop edi
;	pop esi
;
;; Cria o objeto do texto	
;	push edi
;	mov eax, [esi]		; PosX/PosY
;	add esi, 4
;	mov ebx, [esi]		; SizeX/SizeY
;	add esi, 4
;	mov ecx, 0x00010002
;	mov esi, edi
;	call Object_Create
;	
;  ; Coloca os dados do objeto texto
;  	mov dword [edi+Text_FontColor], 0x2ff0000
;  	mov word [edi+Text_SizeX], 0x4
;	mov word [edi+Text_SizeY], 0x2
;	mov dword [edi+Text_Pointer], texto_teste
;	pop edi
;
;; Desenha o objeto
;	call Object_Draw
;	pop esi
;	ret
;
;; Error - Sem memoria
;.error:	stc
;	pop esi
;	ret
;
;
;-----------------------------------------------------------------------------------------