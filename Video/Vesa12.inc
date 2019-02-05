;-----------------------------------------------------------------------------------------

; VESA 1.2

;-----------------------------------------------------------------------------------------
; OBS:

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 ==============================================
; Inicia o VESA 1.2
; Funcionamento:
; IN:
; OUT:

Vesa12_init:
	; carregar o VESA na memoria
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 2 ==============================================
; Finaliza o VESA 1.2
; Funcionamento:
; IN:
; OUT:

Vesa12_deinit:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 3 ==============================================
; Pega pixel 24
; Funcionamento:
; IN:
; OUT:

Vesa12_getPixel24:


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
; Pega pixel 32
; Funcionamento:
; IN:
; OUT:

Vesa12_getPixel32:


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Coloca pixel 24
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	ecx: color
; OUT:
;	nothing

Vesa12_putPixel24:
	push eax
	push ebx
	push edx
	push edi
	
	mov edi,eax 	; x
	mov ebx, eax	; y
	lea  edi,[edi+edi*2]
	movzx  ebx, word [Screen+IBytesPerLine]      
	mul  ebx
	add  edi,eax
	mov  eax,edi
	shr  eax,16
	;call Vesa12_setBank
	and  edi,65535
	add  edi, 0xa0000
	mov eax, ecx
	stosw
	shr  eax,16
	mov  [edi],al
	pop edi
	pop edx
	pop ebx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 6 ==============================================
; Coloca pixel 32
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	ecx: color
; OUT:
;	nothing

Vesa12_putPixel32:
	push eax
	push ebx
	push edx
	push edi
	mov  edi,eax ; x
       	mov  eax,ebx ; y
	shl  edi,2
	mov  ebx,[Screen+IBytesPerLine]
	mul  ebx
	add  edi,eax
	mov  eax,edi
	shr  eax,16
	call Vesa12_setBank
	and  edi,65535
	add  edi, 0xa0000
	mov  [edi],edx
	pop edi
	pop edx
	pop ebx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 7 ==============================================
; Seleciona o banco
; Funcionamento:
; IN:
;	al: numero do banco
; OUT:
;	nothing

Vesa12_setBank:
	pushfd
	cli
	cmp al,[Bank_Atual]
	je retsb
	mov [Bank_Atual],al
	push ax
	push dx
	push cx
	mov cl, al
	mov dx, 0x3D4
	mov al, 0x38
	out dx, al     ;CR38 Register Lock 1 ;Note: Traditionally 48h is used to
	               ;unlock and 00h to lock
	inc dx
	mov al, 0x48
	out dx, al     ;3d5 -?
	dec dx
	mov al, 0x31
	out dx, al     ;CR31 Memory Configuration Register
	;0  Enable Base Address Offset (CPUA BASE). Enables bank operation if set, ;disables if clear.
	;4-5  Bit 16-17 of the Display Start Address. For the 801/5,928 see index 51h,
	;for the 864/964 see index 69h.
	
	inc dx
	in al, dx
	dec dx
	mov ah, al
	mov al, 0x31
	out dx, ax
	mov al, ah
	or al, 9
	inc dx
	out dx, al
	dec dx
	mov al, 0x35
	out dx, al   ;CR35 CRT Register Lock
	inc dx
	in al, dx
	dec dx
	and al, 0xF0
	mov ch, cl
	and ch, 0x0F
	or ch, al
	mov al, 0x35
	out dx, al
	inc dx
	mov al, ch
	out dx, ax
	dec dx
	mov al, 0x51  ;Extended System Control 2 Register
	out dx, al
	inc dx
	in al, dx
	dec dx
	and al, 0xF3
	shr cl, 2
	and cl, 0x0C
	or cl, al
	mov al, 0x51
	out dx, al
	inc dx
	mov al, cl
	out dx, al
	dec dx
	mov al, 0x38
	out dx, al
	inc dx
	xor al, al
	out dx, al
	dec dx
	pop cx
	pop dx
	pop ax
retsb:
	popfd
	ret

;-----------------------------------------------------------------------------------------