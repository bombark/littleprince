;-----------------------------------------------------------------------------------------

; Memory Manage

;-----------------------------------------------------------------------------------------
; OBS:

;-----------------------------------------------------------------------------------------

;======================== V A R I A V E I S ==============================================
;page_Table	equ 0x2000 ; pointer page table - (kernel.asm)
;size_page_Table 	equ 0x1000 ; size of page table - (kernel.asm)

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 ==============================================
; Inicia a memoria (paginada)
; Funcionamento:
;	zera todo espaco da tabela de paginas
;	coloca os descritores na tabela de paginas
;	FIM
; IN: nothing
; OUT: nothing
; Altera:
;	eax, ecx, edi, esi

init_Memory:
; Zera o bloco das variaveis
	mov ecx, 0x100		;zera idt antiga (0 a 400)
	xor edi, edi
	xor eax, eax
	rep stosd

; init Heap
	mov edi, 0x200000+OS_BASE	; Base do heap   (Base do heap+OS_Base+assinatura)
	mov eax, 0xe00000   	; Extensao do heap
	mov [ini_Heap], edi
	mov [size_Heap], eax
	sub eax, 4
	;mov dword [edi-4], 'HEAP'	; coloca a assinatura do heap
	mov dword [edi], eax	; coloca o tamanho do heap no inicio

; init Table Page (zera todas as tabelas de pagina
	xor eax, eax
	mov ecx, (PageTable_Size*PageTable_QTDE)/4	; Todas as tabelas / 4 bytes
	mov edi, PageTable+0x1000	; Pula a tabela do sistema
.a1:	stosd
	loopd .a1

; init mapPage
	mov eax, 1024/8			; 1024 paginas / 8 bit
	call aloca_HB
	mov [mapPage], esi
	mov dword [size_mapPage], 1024/8	; mudar para word
	mov byte [esi], 0xf0		; ocupada a pagina do sistema

	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 2 ==============================================
; Verifica a quantidade de memoria
; Funcionamento:
; IN: nothing
; OUT:
;	eax: qtde de memoria

cont_Memory:
	mov esi, 0x100000
.a:	add esi, 0x1000000
	mov byte [esi], 0xcb
	mov al, [esi]
	cmp al, 0xcb
	jz .a
	sub esi, 0x1000000
.b:	add esi, 0x100000
	mov byte [esi], 0xcb
	mov al, [esi]
	cmp al, 0xcb
	jz .b
	sub esi, 0x100000
.c:	add esi, 0x10000
	mov byte [esi], 0xcb
	mov al, [esi]
	cmp al, 0xcb
	jz .c
	mov eax, esi
	sub eax, 0x100000		; desconta 1o. megabyte
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 3 ==============================================
; Aloca uma pagina
; Funcionamento:
;	Procura uma pagina no mapa de bit (0- livre, 1- ocupada)
;	Verificar se a pagina está livre
;	   marca ocupada na mapPage
;	   marca ocupada na page_Table
;	Senao
;	   sai da rotina com erro (CF)
;	FIM
;

; IN:
;	ebx= page table
; OUT:
;	nothing

aloc_Page:
	push eax
	push ecx
	push esi
	push edi

; find a free page
	xor esi, esi
	mov edi, [mapPage]
.a1:	mov al, [edi]
	mov ecx, 8	; QTDE de bit de eax
.a2:	shl al, 1
	jnc .Pagefounded
	inc esi
	loop .a2
	inc edi
	cmp esi, 1024			; verifica se terminou a tabela
	ja .erro				; todas as paginas ocupadas
	jmp .a1

.Pagefounded:
; Encontra um descritor livre na tabela de pagina
	mov eax, ebx
	xor edi, edi
.b1:	cmp dword [eax+edi], 0
	jz .findDesc
	add edi, 4
	cmp edi, 0x1000			; Fim da tabela
	jae .erro
	jmp .b1

.findDesc:
	mov ax, 0x87
	shl esi,22
	call marcar_Page
	pop edi
	pop esi
	pop ecx
	pop eax
	ret

.erro:	stc
	pop edi
	pop esi
	pop ecx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
; Libera uma pagina
; Funcionamento:
;	Transformar o numero da pagina em byte
;	liberar no mapPage
;	liberar na page_Table
;	FIM
; IN:
;	esi: numero da pagina
;	edi: Ponteiro da tabela de pagina
; OUT:
;	nothing
; OBS (Problema)= Quando a pagina esta livre, ele marca como ocupada


Page_Free:
	push eax
	push ecx
	push edx
	push esi

; Desmarca pagina na tabela
	shl esi, 2
	add esi, edi
	cmp dword [esi], 0
	jz .end			; a pagina já está livre?
	mov ecx, [esi]
	mov dword [esi], 0

; Desmarca pagina no mapa de pagina
	shr ecx, 22	; ecx /4mb (tamanho da pagina)
	mov esi, [mapPage]
	mov al, 10000000b
	mov edx, ecx
	shr edx, 3	; divide por 8 (1 byte) = qual byte da pagina
	and cl, 7		
	add esi, edx
	shr al, cl

	xor al, [esi]	; lodsd
	mov [esi], al

.end:	pop esi
	pop edx
	pop ecx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Marca uma pagina na tabela de pagina
; Funcionamento:
; IN:
;	 ax= tipo
;	ebx= page table
;	esi= posicao de memoria
;	edi= posicao na tabela de pagina (0-4kb)
; OUT:
;	nothing
; OK

marcar_Page:
	push eax
	push edi

; Marca na tabela de pagina
	push esi
	and eax, 0xfff
	or eax, esi
	mov [ebx+edi], eax
	pop esi

; Marca no mapa de pagina
	push esi
	mov al, 0x80
	shr esi, 22
	mov ecx, esi
	shr esi, 3
	and cl, 0x7
	shr al, cl
	add esi, [mapPage]
	or [esi], al
	pop esi

	pop edi
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Aloca um Bloco
; Funcionamento:
; IN:
;	eax= tamanho do bloco
; 	ebx= base do heap
; 	edi= tamanho do heap
; OUT:
; 	esi= ponteiro da alocaçao
;	cy = 0	- OK
;	cy = 1	- falta de espaco no heap
; OK

aloc32:
	push ebx
	push edi
	add edi, ebx		; Pega o fim do heap
	inc eax			;
	and al, 0xfe		; multiplo de 2
.g:	push eax			;
	push ecx			;
	mov ecx, eax
.c:	mov eax, [ebx]
	add ebx, 0x04
	test al ,0x1
	jne .a
	cmp eax, ecx
	jae .b
.a:	and al, 0xfe
	add ebx, eax
	cmp ebx, edi
	jne .c			; jbe ???
	stc
	jmp .d
.b:	mov esi, ebx
	je .e
	sub eax, ecx
	cmp eax, 0x4
	jae .f
	add eax, ecx
	jmp .a
.f:	add ebx, ecx
	sub eax, 0x4
	mov [ebx], eax
.e:	or cl, 0x1
	mov [esi-0x04], ecx
.d:	pop ecx			;
	pop eax			;
	pop edi
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 6 ==============================================
; Realoca um bloco
; Funcionamento:
; IN:
;	eax= novo tamanho para o bloco
;	esi= ponteiro do bloco
; 	ebx= base do heap
; 	edi= tamanho do heap
; OUT:
; 	cy= 0 OK	esi= ponteiro da alocaçao
;	cy=1 	erro: falta de espaco no heap
; OK

align 4
real32:
	push edi
	add edi, ebx		; pega o final do heap
	call verp32
	jb .end2
	push eax
	push ecx
	push ebx
	inc eax
	and al, 0xfe
	mov ecx, eax
	mov ebx, esi
	mov eax, [ebx-0x04]		; eax= tamanho do bloco
	and al,0xfe		; limpa bit de ocupado
	cmp ecx, eax		; compara com o tamanho desejado
	ja .a
	je .end
	sub eax, ecx		; bloco novo < bloco antigo
	cmp eax, 0x04		; desmembra bloco em dois
	jb .end
	mov [ebx-0x04], ecx		; novo bloco colocado no mesmo lugar do antigo
	or [ebx-0x04], byte 0x01	; marca bloco ocupado
	add ebx, ecx		; base do bloco restante
	sub eax, 0x04		; eax-4
	mov [ebx], eax
	jmp .end			; ok (jan2008)

; o bloco ;a ser alocado é maior que o antigo...deve-se buscar espaço para realocar o
; novo blo;co ...verifica se bloco seguinte está livre
.a:	add ebx, eax		; bloco novo> bloco antigo
	cmp ebx, edi
	je .c			; tenta realocação percorrendo todo heap
	test [ebx], byte 0x01	; testa bloco livre
	jne .c                        ; ocupado...tenta realocação
	add eax, [ebx]		; livre...desmembra
	add eax, 0x04		; eax=bloco antigo + bloco seguinte
	cmp ecx, eax		; compara
	ja .c			; tem tamanho suficiente...desmembra bloco
	mov [esi-0x04], ecx		; senão continua a busca
	or [esi-0x04], byte 0x01
	sub eax, ecx
	je .end
	mov ebx, esi
	add ebx, ecx
	sub eax, 0x04
	mov [ebx], eax
.end:	clc
	pop ebx			; saida normal, nao precisa aglomerar blocos
	pop ecx
	pop eax
.end2:	pop edi
	retn

; realocação de bloco maior
.c:	mov eax, ecx		; eax= tamanho desejado bloco novo
	pop ebx			; recupera base heap
	push ebx
	push esi			; bloco antigo fica na pilha
	call aloc32		; aloca novo bloco, esi= aponta novo bloco
	pop ebx			; ebx= ponteiro bloco antigo
	jb .end3

	push esi
	and [ebx-0x04], byte 0xfe	; libera bloco antigo
	mov ecx, [ebx-0x04]		; extensão do bloco antigo
	;and ecx, 0xfe
	push edi			; transfere conteudo para bloco novo
	mov edi, esi		; edi=destino
	mov esi, ebx		; esi=fonte
	ror ecx, 02		; divide por 4
	rep movsd
	pop edi
	pop esi

.end3:	pop ebx
	call lib32.e		; junta blocos adjacentes
	clc			; nao tem como ocorrer erros
	pop ecx
	pop eax
	pop edi
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 7 ==============================================
; Libera um bloco
; Funcionamento:
; IN:
;	esi= ponteiro do bloco
;	ebx= base do heap
;	edi= tamanho do heap
; OUT:
;	cy=0 OK
;	cy=1 erro: ponteiro invalido
; OK

align 4
lib32:
	add edi, ebx
	call verp32
	jb .a
.f:	and [esi-0x4], byte 0xfe
.e:	push eax			; aloca blocos adjacentes livres
	push ebx
	push esi
	mov esi, ebx
.c:	mov ebx, esi
.d:	mov eax, [ebx]
	add ebx, 0x04
	mov esi, eax
	and esi, -0x2
	add esi, ebx
	cmp esi, edi
	je .b
	test al, 0x01
	jne .c
	test [esi], byte 0x01
	jne .c
	add eax, [esi]
	add eax, 0x04
	sub ebx, 0x04
	mov [ebx], eax
	jmp .d
.b:	pop esi
	pop ebx
	pop eax
.a:	sub edi, ebx		; Retorna o valor inicial de edi
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 8 ==============================================
; VERIFICA A VALIDADE DO PONTEIRO
; Entrada:
;	esi= ponteiro duvidoso
;	ebx= base do heap
;	edi= fim do heap
; Saida:
;	cy=0 OK
; 	cy=1 erro: ponteiro invalido
; OK

align 4
verp32:
	push eax
	push ebx
	test [esi-0x4], byte 0x1
	je .a
.c:	mov eax, [ebx]
	add ebx, 0x04
	cmp ebx, esi
	je .b
	and al, 0xfe
	add ebx, eax
	cmp ebx, edi
	jne .c
.a:	stc
.b:	pop ebx
	pop eax
	ret

;--------------------------------------------------------------------------------------------


;======================== F U N C T I O N 9 ==============================================
; Aloca bloco no heap do sistema
; IN:
;	eax= tamanho do bloco
; OUT:
;	esi= ponteiro do bloco

aloca_HB:
 	push ebx
	push edi
	
;mov byte [Timer_flags], 1	
	mov ebx, [ini_Heap]
	mov edi, [size_Heap]
	call aloc32
;mov byte [Timer_flags], 0	
	
	pop edi
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 0 ============================================
; Aloca bloco no heap do sistema
; IN:
;	eax= tamanho do novo bloco
; OUT:
;	esi= ponteiro do novo bloco

realoc_HB:
	push ebx
	push edi
	mov ebx, [ini_Heap]
	mov edi, [size_Heap]
	call real32
	pop edi
	pop ebx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 1 =============================================
; Aloca bloco no heap do sistema
; IN:
;	esi= ponteiro do bloco
; OUT:
;	nothing

lib_HB:
	push ebx
	push edi
	mov ebx, [ini_Heap]
	mov edi, [size_Heap]
	call lib32
	pop edi
	pop ebx
	ret

;-----------------------------------------------------------------------------------------