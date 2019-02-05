



use32
align 4
MM:


; imprime bloco em hexadecimal e ascii
	mov esi, 0x80000000
	xor ecx, ecx
	xor edx, edx
.b:	mov edi, 0xb8000 or OS_BASE		; endereço da tela de video (modo 3)
	push esi
	mov cl, 0x18
.a:	push edi
	call MM_imp_bloco_HexAscii	; rotina que imprime bloco hexa ascii
	pop edi
	add edi, 2*0x50
	loop .a
	pop esi


; busca comando do teclado


	call ObtemTeclaCom		;
	jc .b

	and ax, 0x3f
	cmp al, 0x2c
	ja .b

	cmp al, 3
	jnz .a1
	xchg [tab_comm], esi
	jmp .b


 .a1:	cmp al, 7
	jnz .g
	mov ebx, esi
	add ebx, 0x1000
	cmp ebx, 0x81000000
	jae .b
	mov esi, ebx
	jmp .b

.g:	cmp al, 6
	jnz .h
	mov ebx, esi
	cmp ebx, 0x80000000
	jae .g1
	jb .b
.g1:	sub ebx, 0x1000
	mov esi, ebx
	jmp .b


.h:	cmp al, 8
	jnz .i
	mov ebx, esi
	add ebx, 0x100
	cmp ebx, 0x80ffff00
	jae .h1
	mov esi, ebx
	jmp .b
.h1:	mov esi, 0x81000000-0x18*10
	jmp .b

	jmp .b
.i:	cmp al, 5
	jnz .home
	mov ebx, esi
	sub ebx, 0x100
	cmp ebx, 0x80000000
	jb .b
	mov esi, ebx
	jmp .b


.home:	cmp al, 0x9
	jnz .pgup
	mov esi, 0x80000000
	jmp .b


.pgup:	cmp al, 0xc
	jnz .pgdn
	mov ebx, esi
	sub ebx, 0x10000
	cmp ebx, 0x80000000
	jb .b
	mov esi, ebx
	jmp .b


.pgdn:	cmp al, 0xb
	jnz .b
	mov ebx, esi
	add ebx, 0x10000
	cmp ebx, 0x81000000
	jae .b
	mov esi, ebx
	jmp .b


;-----------------------------------------------------------------------------------------

;; busca comando do teclado
;	mov al, [scancode]	;	esta versao bloqueia o timer quando pressionada
;	test al, 0x80	; 	uma tecla qualquer
;	jnz .b
;
;	cmp al, 0x50
;	jnz .g
;	add esi, 0x1000
;	jmp .fim
;.g:	cmp al, 0x48
;	jnz .h
;	mov ebx, esi
;	sub ebx, 0x1000
;	js .fim
;	mov esi, ebx
;	jmp .fim
;.h:	cmp al, 0x4d
;	jnz .i
;	add esi, 0x100
;	jmp .fim
;.i:	cmp al, 0x4b
;	jnz .home
;	mov ebx, esi
;	sub ebx, 0x100
;	js .fim
;	mov esi, ebx
;	jmp .fim
;.home:	cmp al, 0x51
;	jnz .pgup
;	xor esi, esi
;	jmp .b
;.pgup:	cmp al, 0x49
;	jnz .pgdn
;	mov ebx, esi
;	sub ebx, 0x10000
;	js .fim
;	mov esi, ebx
;	jmp .fim
;.pgdn:	cmp al, 0x47
;	jnz .fim
;	add esi, 0x10000
;.fim:	mov al, [scancode]
;	test al, 0x80
;	jz .fim
;	jmp .b



;-----------------------------------------------------------------------------------------


align 4
MM_imp_bloco_HexAscii:

           mov eax,esi
.a:	         call MM_imp_rd_hex	; imprime...endereço #32
                   mov al,0x20
                   call MM_Imp_al_vtexto
                   call MM_Imp_al_vtexto		; imprime 2 espaços
                   call MM_Imp_al_vtexto
; imprime bloco hex
                   push ecx
                   push esi
                   mov cl,0x10
.b:                mov al,[esi]		; busca byte apontado por ds
                   call MM_imp_bytehex		; imprime 16 colunas de bytes
                   mov al,0x20
                   call MM_Imp_al_vtexto		; imprime espaço
                   inc esi
                   loop .b
		call MM_Imp_al_vtexto
; imprime bloco ascii
                   pop esi		; retorna o ponteiro
                   mov cl,0x10
.c:                mov al,[esi]
                   call MM_Imp_al_vtexto		; imprime 16 colunas caractere ascii
                   inc esi
                   loop .c
                   pop ecx
                   retn


;-----------------------------------------------------------------------------------------

align 4
MM_imp_bytehex:
.a:                aam 0x10
                   cmp ah,0x09
                   jna .b
                   add ah,0x07
.b:                cmp al,0x09
                   jna .c
                   add al,0x07
.c:                add ax,0x3030
                   xchg ah,al
                   call MM_Imp_al_vtexto
                   xchg ah,al
                   call MM_Imp_al_vtexto
	         retn
align 4
MM_imp_wo_hex:
                   push ecx
                   mov cl,2
                   push edx
                   mov edx,eax
.c:                rol edx,0x08
                   mov al,dl
                   call MM_imp_bytehex
                   loop .c
                   pop edx
                   pop ecx
.b:                retn
align 4
MM_imp_rd_hex:
.a:                push ecx
                   mov ecx,0x0004
                   push edx
                   mov edx,eax
.c:                rol edx,0x08
                   mov al,dl
                   call MM_imp_bytehex	;	imprime 1 byte
                   loop .c
                   pop edx
                   pop ecx
.b:                retn

align 4
MM_Imp_al_vtexto:

	mov [edi], al
          inc edi
	mov [edi], byte 7
	inc edi
	retn



;-----------------------------------------------------------------------------------------

MM_fim: