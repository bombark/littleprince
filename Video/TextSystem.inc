;-----------------------------------------------------------------------------------------

; TEXT SYSTEM

;-----------------------------------------------------------------------------------------
; OBS:

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 ==============================================
; Inicia o modo Texto
; Funcionamento:
; IN:
; OUT:

init_TextSystem:
	mov dword [Screen+IPointer], 0xb8000
	mov dword [Screen+IBytesPerLine], 80*2
	mov word [Screen+IWidth], 25
	mov word [Screen+IHeight], 80
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 2 ==============================================
; Finaliza o modo Texto
; Funcionamento:
; IN:
; OUT:

TextSystem_deinit:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
; Coloca um char na tela
; Funcionamento:
; IN:
;	al: caracter
;	ah: cor
;	dl: X coordenate
;	dh: Y coordenate
; OUT:
;	nothing

TextSystem_putChar:
	push edi
	and edx, 0xffff
	call TextSystem_CalcLinear
	stosw
	call TextSystem_CalcBilinear
.end:	pop edi
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Coloca uma string com extensao definida na tela
; Funcionamento:
; IN:
;	ah: cor
;	esi: pointer of string
;	ecx: extension of string
;	dh: coordenada Y
;	dl: coordenada X 
; OUT:
;	dx: coordenada atualizada

TextSystem_putString:
	push eax
	push esi
	push edi
	and ecx, 0xffff
	and edx, 0xffff
.a:	lodsb
	call TextSystem_putChar
	loop .a
	pop edi
	pop esi
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 6 ==============================================
; Coloca uma string terminada em 0 na tela
; Funcionamento:
; IN:
;	ah: cor
;	esi: pointer of string
;	dl: X coordenate
;	dh: Y coordenate
; OUT:
;	dx: coordenada atualizada

TextSystem_putStringZ:
	push eax
	push esi
	push edi
	and edx, 0xffff
	call TextSystem_CalcLinear
.a:	lodsb
	or al,al
	jz .end
	stosw
	jmp .a
.end:	call TextSystem_CalcBilinear
	pop edi
	pop esi
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 7 ==============================================
; Coloca uma string terminada em 0 na tela
; Funcionamento:
; IN:
;	eax: numero
;	bh: cor
;	bl: quantidade de numeros a aparecer
;	dl: X coordenate
;	dh: Y coordenate
; OUT:
;	dx: coordenada atualizada

TextSystem_putHexa:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 8 ==============================================
; Coloca uma string terminada em 0 na tela
; Funcionamento:
; IN:
;	eax: numero
;	bh: cor
;	bl: quantidade de numeros a aparecer
;	dl: X coordenate
;	dh: Y coordenate
; OUT:
;	dx: coordenada atualizada

TextSystem_putInteger:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 9 ==============================================
; Calcula o endereco linear
; Funcionamento:
; IN:
;	dl: X coordenate
;	dh: Y coordenate
; OUT:
;	edi: endereco linear

TextSystem_CalcLinear:
	push eax
          push edx
          xor edi,edi
	or dh, dh
	jz .b
.a:       add edi,[Screen+IBytesPerLine]
          dec dh
          jnz .a
.b:       add edi, edx
          add edi, edx
          ;add edi, [fs:p_base_tela_ativa] 	; soma a base da tela ativa
          add edi, [Screen+IPointer]
          
          pop edx
          pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 0 ============================================
; Calcula o endereco bilinear
; Funcionamento:
; IN:
;	edi: endereco linear
; OUT:
;	dl: X coordenate
;	dh: Y coordenate

TextSystem_CalcBilinear:
	push eax
	push ebx
	sub edi, [Screen+IPointer]
	;mov ax, [fs:p_base_tela_ativa]
	;sub edi, eax
	mov eax, edi
	mov bl, 0xa0
	div bl
	shr ah, 1
	mov dl, ah
	mov dh, al
	pop ebx
	pop eax
	ret

;-----------------------------------------------------------------------------------------