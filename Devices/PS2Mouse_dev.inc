;-------------------------------------------------------------------------------

; Driver do Mouse PS/2

;-------------------------------------------------------------------------------


;======================== F U N C T I O N  1 ===================================
; Inicializa um mouse PS/2  (COPYRIGHT - Menuetos)
; IN:	nothing
; OUT:
;	cy=0	- OK
;	cy=1	- erro
; altera: al, bl

PS2_Mouse_Init:
  ; disable keyboard interface
  	;mov bl, 0xad
  	;call kb_cmd

  ; enable mouse cmd
	mov     bl,0xa8
 	call    kb_cmd
 	call    kb_read

  ; Reseta o mouse
	mov     al,0xff               ; for mouse
 	call    PS2_Mouse_Cmd
	call    kb_read		; Get ???
	call    kb_read		; Get ID

  ; get command byte
 	mov     bl,0x20
 	call    kb_cmd
 	call    kb_read
 	or      al,3                   ; enable interrupt
 	mov     bl,0x60                ; write command
 	push    eax
 	call    kb_cmd
 	pop     eax
 	call    kb_write

  ; enable mouse device
 	mov     al,0xf4               ; for mouse
 	call    PS2_Mouse_Cmd
 	call    kb_read           	; read status return

  ; set mode stream
	mov     al,0xea               ; for mouse
 	call    PS2_Mouse_Cmd
	
  ; Scrolling
	;mov     bl,0xd4               ; for mouse
 	;call    kb_cmd
 	;mov     al,0xf3
 	;call    kb_write
 	;call    kb_read           ; read status return
	;call    kb_read
	
  ; enable keyboard interface
  	;mov bl, 0xae
  	;call kb_cmd	
	
	ret

  ; Error - Mouse PS2 invalido
.error:	stc
	ret

;-------------------------------------------------------------------------------


;======================== F U N C T I O N  2 ===================================
; Inicializa um mouse PS/2
; IN:	nothing
; OUT:
;	cy=0	- OK
;	cy=1	- erro

PS2_Mouse_Deinit:
	ret


;-------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
; Recebe uma informacao do mouse
; IN:
;	nothing
; OUT:
;	al = informacao

PS2_Mouse_Read:
	push ecx
	mov ecx, 0xfff
.rep:	in al, 0x64
	test al, 0x21
	jnz .read
	loop .rep
	pop ecx
	stc
	ret

.read:	in al, 0x60
	pop ecx
	ret

;------------------------------------------------------------------------------------


;======================== F U N C T I O N 3 ==============================================
; Manda um comando para o mouse
; IN:
;	al = Comando
; OUT:
;	al =  0xfa || cy = 0  -> OK
;	al != 0xfe || cy = 1  -> erro
; Altera:
;	bl, dl, cl

PS2_Mouse_Cmd:
	mov     cl, 5
	mov     dl, al	
.send:	mov     bl,0xd4               ; for mouse
 	call    kb_cmd
 	mov     al, dl
 	call    kb_write
 	call    kb_read           ; read status return
 	
 	cmp     al, 0xfa
 	jne .resend
 	clc 
	ret

.resend:cmp al, 0xfe
	jne .error		; aqui nao foi testado, mantenha o int 1	
	dec cl
	jnz .send
	
.error:	stc
	ret	

;------------------------------------------------------------------------------------


;======================== F U N C T I O N 3 ==============================================
; Manda um comando para o mouse
; IN:
;	nothing
; OUT:
;	nothing

PS2_Mouse_Wait:
	push ecx
	xor ecx, ecx
.loop:	in   al, 0x64
	test al, 00000010b
	loopnz .loop
        pop  ecx    
	ret	

;------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 ===================================
; Pega as informacoes do mouse (botoes, coordenadaX, coordenadaY)
; IN:
;	nothing
; OUT:
;	ax= coordenada Y
;	bx= coordenada X
;	dl= botoes

PS2_Mouse_GetData:

; Mouse Button
	xor ah, ah
	call PS2_Mouse_Read
	jc .error
	test al, 1000b
	jz .error
	mov dl, al
	and dl, 0x7

; Mouse_X
	xor ah, ah
	call PS2_Mouse_Read
	or al, al
	jns .a1
	mov ah, 0xff
.a1:	mov bx, ax

; Mouse Y
	xor ah, ah
	call PS2_Mouse_Read
	or al, al
	jns .fim
	mov ah, 0xff
.fim:	clc
	ret

.error:	xor ax, ax
	xor bx, bx
	ret

;-------------------------------------------------------------------------------