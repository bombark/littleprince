use32
align 4

; header
Mmi:
dd 'LP01'		; assinatura (tipo de arquivo)
dd Mm-Mmi		; ponto de entrada
dd Mm_fim-Mmi	; extensao do arquivo
rb 0x200-0xc


StartX_pos	equ 0x10
StartY_pos	equ 0x20
CharColor		equ 0xffffff
BackColor		equ 0x000070

MM_Window	dd 0
texto 	db "Felipe aaaaaaaaaaaa",0

;button1 db "posX=16 posY=550 height=32 width=32 code=1>",0
;button2 db "posX=64 posY=550 height=32 width=32 code=2>",0
;imagem  db "src=d:\img1.bmp posX=100 posY=100>",0


Script	db "SCP1"
	dd 0xff000002,0x02200020,0x00200020,0x00020001
	dd 0xff000002,0x02200050,0x00200020,0x00020002
	dd 0xff000002,0x02200080,0x00200020,0x00020003
	;dd 0xff000006,0x00500020,0x00200020,0x00040006,0x0,0x0,0x0
	;dd 0xff000001,0x00500020,0x01000100,0x00040001,0xff0000,texto
	dd 0x0
	
use32
align 4
Mm:
; Cria uma janela	
	mov eax, 0x00200020		;
	mov ebx, 0x02c00280	;0x03000280		;size
	mov ebp, 1
	int 0x42	
	mov [MM_Window], edi

; Cria os botoes	
	;mov edx, edi
	;mov esi, Script
	;call SWN_Script2_Run	

; imprime bloco em hexadecimal e ascii	
	mov esi, OS_BASE+0x200000		; position
	xor ecx, ecx
	xor edx, edx
	mov ebp, BackColor		;0x445566
.b:	mov [MM_posX], StartX_pos
	mov [MM_posY], StartY_pos
	push esi
	xor ecx, ecx
	mov cl, 0x20
.a:	call MM_HexAscii_graph	; rotina que imprime bloco hexa ascii
	mov [MM_posX], StartX_pos
	add [MM_posY], 0x10
	loop .a

	pop esi

; Busca evento
pushad
call Mouse_Click
popad
;call SWN_Event_Run	
	
	;mov ebp, 6		; Executa eventos padroes
	;int 0x42
	mov ebp, 3		; Pega eventos
	int 0x42
	
	cmp cx, 0x01
	jne .bt2
	mov ebx, esi
	sub ebx, 0x10
	cmp ebx, 0x80000001
	jb .b
	mov esi, ebx

.bt2:	cmp cx, 0x02
	jne .bt3
	add esi, 0x10

.bt3:	cmp cx, 0x03
	jne .kb
	add esi, 0x1000

; busca comando do teclado
.kb:
	mov ebp, 5
	int 0x44		;call ObtemTeclaCom		;	
	jc .b

	and eax, 0x3f
	cmp al, 0x2c
	ja .b

	cmp al, 7
	jnz .g
	add esi, 0x1000
	jmp .b

.g:	cmp al, 6
	jnz .h
	mov ebx, esi
	sub ebx, 0x1000
	cmp ebx, 0x80000000
	jb .b
	mov esi, ebx
	jmp .b

.h:	cmp al, 8
	jnz .i
	add esi, 0x200
	jmp .b

.i:	cmp al, 5
	jnz .home
	mov ebx, esi
	sub ebx, 0x200
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
	add esi, 0x10000
	jmp .b


MM_posX	dw 0x0
MM_posY	dw 0x0



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
MM_HexAscii_graph:
          mov edx,esi
.a:	call MM_graph_rd_hex	; imprime...endereço #32   
          mov dl,0x20
          call MM_Imp_al_gtexto         
          call MM_Imp_al_gtexto	; imprime 2 espaços   
          call MM_Imp_al_gtexto

; imprime bloco hex
          push ecx
          push esi
          xor ecx, ecx
          mov cl,0x10
.b:       mov al,[esi]		; busca byte apontado por ds
          call MM_graph_bytehex	; imprime 16 colunas de bytes
          mov dl,0x20
          call MM_Imp_al_gtexto	; imprime espaço
          inc esi
          loop .b
	mov dl, 0x20
	call MM_Imp_al_gtexto

; imprime bloco ascii
          pop esi			; retorna o ponteiro
          mov cl,0x10
.c:       mov dl,[esi]
          call MM_Imp_al_gtexto	; imprime 16 colunas caractere ascii
          inc esi
          loop .c
          pop ecx
          retn


;-----------------------------------------------------------------------------------------

align 4
MM_graph_bytehex:

.a:       aam 0x10
          cmp ah,0x09
          jna .b
          add ah,0x07
.b:       cmp al,0x09
          jna .c
          add al,0x07
.c:       add ax,0x3030
          mov dl, ah
          call MM_Imp_al_gtexto
          mov dl, al
          call MM_Imp_al_gtexto
	retn


align 4
MM_graph_wo_hex:
          push ecx
          mov cl,2
.c:       rol edx,0x08
          mov al,dl
          call MM_graph_bytehex
          loop .c
          pop ecx
.b:       retn


align 4
MM_graph_rd_hex:
.a:      	push ecx
          mov ecx,0x0004
.c:       rol edx,0x08
          mov al,dl
          call MM_graph_bytehex	;	imprime 1 byte        
          loop .c
          pop ecx
.b:       retn


MM_Imp_al_gtexto:	; dl= caractere to print	
	push eax
	push ecx
	push esi
	push edi
	mov eax, dword [MM_posX]
	mov ecx, CharColor		;0xffccee
	mov esi, BackColor
	mov edi, [MM_Window]
	mov ebp, 0x11
	int 0x43
	mov dword [MM_posX], eax
	pop edi
	pop esi
	pop ecx
	pop eax
	ret



;-----------------------------------------------------------------------------------------

Mm_fim: