use32
align 4

; header

dd 'LP01'
dd Exp_Main
dd 0x800
rb 0x200-0xc


;======================== V A R I A V E I S ==============================================

Exp_Window	dd 0	; Ponteiro da Janela
Exp_Frame1	dd 0	; Ponteiro do Frame
Exp_Visor		dd 0	; Ponteiro do visor do path

Exp_Path		db 'd:\',0	; Endereco
		rb 0x1f0

Exp_FolderDesc	rb 0x20

;-----------------------------------------------------------------------------------------


;======================== P R I N C I P A L ==============================================

Exp_Main:
	call Exp_Init
	call Exp_FolderOpen

.loop:	call Mouse_Click
	mov ebp, 4
	int 0x42
	mov ebp, 3
	int 0x42
	or cx, cx
	jz .loop
	
	and ecx, 0xffff
	cmp cx, 0xffff
	je .end
	call Exp_Click
	jmp .loop

.end:	mov ebp, 0x11
	int 0x42
	jmp $

;-----------------------------------------------------------------------------------------



;======================== F U N C T I O N  1 =============================================
; Inicializa a explorer
; IN: 
;	nothing
; OUT:
;	nothing

Exp_Init:
	mov eax, 0x00200020
	mov ebx, 0x02000180
	mov ebp, 0x1
	int 0x42 
	mov [Exp_Window], edi

	mov eax, 0x00200001
	mov ebx, 0x01f0017d
	mov ecx, 0x00000000
	mov edx, edi
	mov ebp, 5
	int 0x42
	mov [Exp_Frame1], edi
	mov dword [edi+Obj_Color], 0xffffff

	mov eax, 0x00140020
	mov ebx, 0x00100140
	mov ecx, 0x00030000
	mov edx, [Exp_Window]
	mov ebp, 5
	int 0x42
	mov [Exp_Visor], edi
	mov dword [edi+Text_Desc+Text_Pointer], Exp_Path

	mov eax, 0x00140010
	mov ebx, 0x000a000a
	mov ecx, 0x0002fff0
	mov edx, [Exp_Window]
	mov ebp, 5
	int 0x42

  ; Passar por servico depois	
	;mov eax, 0x00100010
	;mov ebx, 0x00700050
	;mov ecx, 0x00070001
	;mov edx, [Exp_Frame1]
	;call Object_Create

	mov edi, [Exp_Window]
	mov ebp, 7
	call Object_AllDraw	


	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Abre o diretorio e os cria os objetos
; IN: 
;	nothing
; OUT:
;	nothing

Exp_FolderOpen:
  ; Se diretorio aberto  -> Fecha diretorio
	mov edi, Exp_FolderDesc
	cmp dword [edi], 0x00
	jz .open	
	mov ebp, 0x13
	int 0x45

    ; Destroi os objetos do frame1	 
	mov edi, [Exp_Frame1]
	mov ebp, 0x12
	int 0x42
	
  ; Abre o diretorio	
.open:	mov esi, Exp_Path
	mov edi, Exp_FolderDesc
	mov ebp, 0x12
	int 0x45	

  ; Lista o diretorio	
	mov ebp, 0x14
	int 0x45

  ; Monta os icones
  	xor eax, eax
  	mov ebx, 0x00100050
  	mov ecx, 0x00030001
  	mov esi, dword [Exp_FolderDesc]
  
.loop:	or esi, esi
	jz .end
	
	mov edx, [Exp_Frame1]
	mov ebp, 0x5
	int 0x42
	
	mov edx, [esi+FL_Name]
	mov [edi+0x3c], edx
	or byte [edi+Obj_Attr], Obj_Attr_Clicavel
	
	mov ebp, 0x7
	int 0x42 
	mov esi, [esi]		; pega o proximo item
	add eax, 0x00100000		; incrementa a posicao 
	inc cx			; incrementa o codigo	
	jmp .loop
	

.end:	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; o mouse clica em cima de um objeto do explorer
; IN: 
;	cx = codigo do objeto
; OUT:
;	nothing

Exp_Click:
  ; Se for botao
    ; Se botao acima  
	cmp cx, 0xfff0
	jne @f
	call Exp_BotaoUp

    ; Se botao voltar
@@:	cmp cx, 0xfff1
	jne @f
	;call Exp_FolderBack
	
    ; Se botao avancar
@@:    	cmp cx, 0xfff2
    	jne @f	
	;call Exp_FolderAvancar
	
  ; Senao -> Procura o item (arquivo ou pasta) clicado	; Otimizar depois	
@@:	mov esi, dword [Exp_FolderDesc]
.loop:	or esi, esi
	jz .end
	dec ecx
	jz .found
	mov esi, [esi]
	jmp .loop

  ; Se for diretorio  -> Abre o novo diretorio
.found:	mov ax, [esi+FL_SubTipo]
	cmp ah, 0x01		; pasta
	je .folder
	jmp .arq

    ; Junta o Path  
.folder:	mov esi, [esi+FL_Name]
	mov edi, Exp_Path
	call strcat
	mov word [edi], 0x005c		; \0	
	mov edi, [Exp_Visor]
	mov ebp, 7
	call Object_Draw
	
    ; Abre o novo diretorio
  	call Exp_FolderOpen
  	jmp .end
  
  ; Se for arquivo texto -> Abre o arquivo
.arq:     cmp ah, 0x02
	jne .img
	;call Exp_TextoOpen
	jmp .end	 
       
  ; Se for imagem  -> Abre o arquivo
.img:	cmp ah, 0x03      	
      	jne .exe
      	jmp .end
      
  ; Se for executavel -> Executa
.exe:
	jmp .end
        
          
  
.end:	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; o mouse clica em cima de um objeto do explorer
; IN: 
;	nothing
; OUT:
;	nothing

Exp_BotaoUp:
 	push eax
 	push esi	
 	mov esi, Exp_Path
 	call strlen
 	dec ecx	

.loop: 	dec ecx
	jz .end
	mov al, [esi+ecx]
 	cmp al, '/'
 	je .open
 	cmp al, '\' 
  	je .open	
  	jmp .loop
  
.open:	mov byte [esi+ecx+1], 0
	mov edi, [Exp_Visor]
	mov ebp, 7
	call Object_Draw
	call Exp_FolderOpen
	
.end:	pop esi
	pop eax
	ret


;-----------------------------------------------------------------------------------------


