; vesa driver
;
;-----------------------------------------------------------------------------------------

; ver as variaveis em VariaveisFixas.cb

; Buffer temporario para as funcoes Vesa
VBEbuffer		equ 0x0c00	;

; Definicoes para a Funcao 01
	ModeAttributes		equ ModeInfoBlock		; dw
	WinAAttributes		equ ModeInfoBlock+2		; db
	WinBAttributes		equ ModeInfoBlock+3		; db
	WinGranularity		equ ModeInfoBlock+4		; dw
	WinSize			equ ModeInfoBlock+6		; dw
	WinASegment		equ ModeInfoBlock+8		; dw
	WinBSegment		equ ModeInfoBlock+0x0a	; dw
	WinFuncPtr		equ ModeInfoBlock+0x0c	; dd
	BytesPerScanLine		equ ModeInfoBlock+0x10	; dw
; Informações para a versão 1.2
	XResolution		equ ModeInfoBlock+0x12	; dw
	YResolution		equ ModeInfoBlock+0x14	; dw
	XCharSize			equ ModeInfoBlock+0x16	; db
	YCharSize			equ ModeInfoBlock+0x17	; db
	NumberOfPlanes		equ ModeInfoBlock+0x18	; db
	BitsPerPixel		equ ModeInfoBlock+0x19	; db
	NumberOfBanks		equ ModeInfoBlock+0x1a	; db
	MemoryModel		equ ModeInfoBlock+0x1b	; db
	BankSize			equ ModeInfoBlock+0x1c	; db
	NumberOfImagePages		equ ModeInfoBlock+0x1d	; db
	MIBreserved		equ ModeInfoBlock+0x1e	; db
; Direct Color
	RedMaskSize		equ ModeInfoBlock+0x1f	; db
	RedFieldPosition		equ ModeInfoBlock+0x20	; db
	GreenMaskSize		equ ModeInfoBlock+0x21	; db
	GreenFieldPosition		equ ModeInfoBlock+0x22	; db
	BlueMaskSize		equ ModeInfoBlock+0x23	; db
	BlueFieldPosition		equ ModeInfoBlock+0x24	; db
	RsvdMaskSize		equ ModeInfoBlock+0x25	; db
	RsvdFieldPosition 		equ ModeInfoBlock+0x26	; db
	DirectColorModeInfo 	equ ModeInfoBlock+0x27	; db- atributos
; Informações para versão 2.0
	PhysBasePtr		equ ModeInfoBlock+0x28	; dd
	MIBreserved1		equ ModeInfoBlock+0x2c	; dd- sempre 0
	MIBreserved2		equ ModeInfoBlock+0x30	; dw- sempre 0
; Informações para versão 3.0 e acima
	LinBytesPerScanLine 	equ ModeInfoBlock+0x32	; dw
	BnkNumberOfImagePages	equ ModeInfoBlock+0x34	; db
	LinNumberOfImagePages	equ ModeInfoBlock+0x35	; db
	LinRedMaskSize		equ ModeInfoBlock+0x36	; db
	LinRedFieldPosition		equ ModeInfoBlock+0x37	; db
	LinGreenMaskSize		equ ModeInfoBlock+0x38	; db
	LinGreenFieldPosition	equ ModeInfoBlock+0x39	; db
	LinBlueMaskSize		equ ModeInfoBlock+0x3a	; db
	LinBlueFieldPosition	equ ModeInfoBlock+0x3b	; db
	LinRsvdMaskSize		equ ModeInfoBlock+0x3c	; db
	LinRsvdFieldPosition 	equ ModeInfoBlock+0x3d	; db
	MaxPixelClock		equ ModeInfoBlock+0x3e	; db



;-----------------------------------------------------------------------------------------
; Define a resolucao e as cores do video	(Modo Real)
; Entrada:
;	nada
; Saida:
;	nada

align 2
DefineModoVesa:
	cmp byte [VbeVersion], 3
	jae .a				; se for vesa3, continua...

; se for vesa2...?
	cmp byte [VbeVersion], 2
	jz .b

; se for vesa 1.2....?....senao avaliar vga
.b:
; altera para o modo desejado
.a:
	mov bx, 0x112
	mov cx, 0x112
	call VesaR02
	call VesaR01
; ou manter o modo texto padrao (modo 3)
	ret



;-----------------------------------------------------------------------------------------
; obtem informaçoes VESA. Usa função 00. (Modo Real)
; Entrada:
;	es:di= aponta buffer que recebe o bloco de informações
; Saida:	
;	al= 4f OK. Se diferente, a função não é suportada
;	ah= 00 OK. 01- funçao falhou, 02- funçao não suportada, 03- chamada invalida
; 	           no presente modo de video.

align 2
VesaR00:	mov ax,0x4f00	; funçao 00, usar no modo real
	mov di,VBEbuffer
	int 0x10		;
	cmp ax,0x004f
	je .a
	stc		; sai com CY=1
	ret
; versao
.a:	mov ax,[es: di+4]
	mov byte [fs: VbeVersion], ah
	mov byte [fs: VbeVersion+1], al	; salva versao
; fabricante, modelo...
	mov eax, [es: di+6]
	mov [fs: OEMStringPtr], eax		; ponteiro do fabricante, modelo, etc...
; capacidades
	mov eax, [es: di+0x0a]
	mov [fs: VbeCapabilities], eax	; flags
; busca os modos graficos disponiveis...
	mov ax, [es: di+0x0e+2]
	mov ds, ax
	mov ax, [fs: di]			; ds:ax aponta a tabela de modos
	xor si,si				; ponteiro dos modos disponíveis
	mov bx, di
	xor di,di
	mov bp, TabelaModosBasicos		; modos vesa desejados
.d:	mov ax, [bx+di]
	cmp ax, 0xffff
	jz .b
	cmp ax, [gs: bp+si]
	jz .c
	add di, 2				; avança na tabela Vesa
	jmp .d
.c:	add si, 2
	add di, 2
	jmp .d
.b:	xor ax, ax
.b1:	add si, 2				; zera ponteiros de modos inexistentes
	cmp si, 0xf
	ja .e
	mov [gs: bp+si], ax
	jmp .b1
; salva ponteiro que identifica o dispositivo: nome do fabricante, modelo, etc
.e:	mov ax, [es: di+0x12]
	mov [fs: VbeTotalMemory], ax 		; total de memoria em blocos de 64KB
	ret



;-------------------------------------------------------------------------------
; Retorna informações a respeito do modo solicitado em cx
; Entrada:
;	cx= modo desejado
; 	es:di= aponta buffer que recebe o bloco de informações
; saida:
;	idem funçao 00

align 2
VesaR01:	mov ax,0x4f01		; funçao 01, usar no modo real
	mov di,VBEbuffer
	int 0x10			;
	cmp ax,0x004f
	je .a
	stc			; sai com CY=1
	ret
; transferir o conteudo do buffer para as variaveis a partir de ModeInfoBlock
.a: 	mov cx, 0x11
	mov si, di
	mov di, ModeInfoBlock
	rep movsd
	ret



;-------------------------------------------------------------------------------
; Posiciona modo de video (Modo real)
; Entrada: 
;	bx= modo desejado
;	#0-#8: modo desejado, #9-#a: reservado
;	#b: 0, usa taxa de refrescamento atual, 1, usa valor armazenado em registrador
;	#c-#d: reservado para VBE/AF
;	#e: 0, modo janela, 1, modo linear (acesso direto)
;	#f: 0, limpa display, 1, não limpa display
align 2
VesaR02:	mov ax,0x4f02		; funçao 02, usar no modo real
	int 0x10			;
	cmp ax,0x004f
	je .a
	stc			; sai com CY=1
	ret
.a: 	mov [fs: VBEMode], bx
	ret



;-------------------------------------------------------------------------------
; retorna o modo de video atual. (Modo Real)
; Saida:	
;	ax= ver funçao 00
;	bx= Modo de video corrente
; valem apenas os bits 0-13. 	#14=0, modo janela, 1, modo linear
;			#15=0, momoria limpada, 1, memoria intacta

align 2
VesaR03:	mov ax,0x4f03		; funçao 03, usar no modo real
	int 0x10			;
	cmp ax,0x004f
	je .a
	stc			; sai com CY=1
	ret
.a: 	mov [fs: VBEModeAtual], bx
	ret



;-------------------------------------------------------------------------------
; Retorna dados sobre a interface para o modo protegido #32...
; Entrada:	
;	nada
; Saida:	
;	ax= status
;	es= tabela de segmentos para o modo real
;	di= deslocamento
;	cx= comprimento da tabela
; funciona apenas para versão VBE2.0 ou superior. Funcao obsoleta para Vesa 3
align 2
VesaR0a:	mov ax,0x4f0A
	mov bx,0x0
	int 0x10
	cmp ax,0x004f
	je .a
	stc			; sai com CY=1
	ret
.a:	xor eax,eax
	xor ebx,ebx
	mov ax,es
	shl eax,4
	mov bx,di
	add eax,ebx  		; eax= endereço linear da tabela
	xor ebx,ebx
	mov [fs: VBETabela], eax	; VBETabela= end. linear
	mov [fs: ExtVBETabela], cx
	ret



;-----------------------------------------------------------------------------------------

; busca codigo para o modo protegido. transfere o codigo da area c000:0000-c000:8000
; para a RAM e cria os respectivos descritores dos segmentos. Permite o uso de portoes
; no modo protegido para acessar as funcoes Vesa basicas

; Definicoes da estrutura de PMInfoBlock
PMSignature 	equ [Base_PMID]
PMEntryPoint	equ [Base_PMID]+0x04
PMInicialize	equ [Base_PMID]+0x06
BIOSDataSel	equ [Base_PMID]+0x08
A000Sel		equ [Base_PMID]+0x0a
B000Sel		equ [Base_PMID]+0x0c
B800Sel		equ [Base_PMID]+0x0e
CodeSegSel	equ [Base_PMID]+0x10
InProtectMode	equ [Base_PMID]+0x12
CheckSum		equ [Base_PMID]+0x13

; ent: 	nada
; sai:	altera ah
align 2
Busca_VideoBIOS:	; prepara o modo protegido e monta o portao Vesa. (P_VesaPM)
; aloca bloco no heap baixo
	;cmp byte [fs: VbeVersion], 3
	;jne .prepVesa2		; Se vesa3 nao eh suportada, entao pesquisa e
	push es			; prepara o vesa2...senao permanece no modo texto
	push ds
	push ebx
	push ecx
	push edx
	push esi
	push edi
	mov ax, s_heap_b
	mov ds, ax
	mov eax, 0x10000
	call far s_exe32_0:aloca_bl_HB16a	; rotina #32
	jb .fm				; pode emitir aviso se faltar memoria
; transfere codigo e define descritor do segmento (#16)- s_vb16 (Seletor de dados)
	mov edi, esi
;	mov [fs: VideoBios], edi		; salva base do BIOS de video (obsoleto)
	mov eax, edi
	mov bx, s_heap_b
	call RetornaBaseDescritorBX
	add eax, edi
	mov [fs: s_vb16c+2+b_gdt], ax		; base do descritor
	mov [fs: s_vb16d+2+b_gdt], ax		; base do descritor
	shr eax, 0x10
	mov cx, 0x4000			; 32 KB = 2000x4
	mov [fs: s_vb16c+4+b_gdt], al
	mov [fs: s_vb16c+7+b_gdt], ah		; byte5= 9a, byte6= 00
	mov [fs: s_vb16d+4+b_gdt], al
	mov [fs: s_vb16d+7+b_gdt], ah		; byte5= 9a, byte6= 00
	mov ax, ds
	mov es, ax			; es= aponta heap baixo
	mov ax, s_allmem
	mov ds, ax
	mov esi, 0xc0000
	push edi
	cld
	db 0x67
	rep movsd
	pop edi				; recupera base
; procura pela assinatura PMID- bloco PMInfoBlock
	mov ax, cs
	mov ds, ax			; ds aponta segmento de codigo (pmid)
	mov dx, 0x7fff			; o bloco deve estar nos primeiros 32KB
.c:	mov esi, pmid			; string a procurar
	mov eax, 4			; extensao
	call Pesq_coinc32                       ; rotina #16
	jb .b				; edi = Base_PMInfoBlock
	mov [fs: base_PMID], edi		; base PMInfoBlock...usa seletor h_baixo
; confere checksum
	mov cx, 0x13
	xor esi,esi
	mov al, [es: edi+0x13]		; al=checksum
.a:	add al, [es: edi+esi]		; a rotina opera com #32
	inc si
	loop .a
	or al, al				; testa se al = 0
	jnz .c				; continua a pesquisa
; CheckSum OK, ajusta ponteiros no bloco PMInfoBlock

	pushf
	pop ax
	or ah,1
	push ax
	popf

; prepara bloco de dados que emula a area de dados do BIOS. Esta area corresponde ao
; endereco desde 0 ate 600. Ao inves de alocar um novo bloco pode-se tentar usar o mesmo
; bloco do bios, desde que seja preparado um seletor para o mesmo.

; opcao 1 - alocar bloco
	;mov ax, s_var
	;mov ds, ax			; ds aponta area de variaveis
	;mov ax, 0x600
	;call aloca_bloco_heap16
	;jb .fm1				; si aponta o bloco, ax= extensao
	;call limpa_bloco16			; zera o bloco
; criar descritor de dados e salvar em BIOSDataSel
	;mov bl, 0x92
	;call aloca_desc_dados
	;jb .fm1
	;mov [es: edi+0x08], bx		; BIOSDataSel OK

; opcao 2 - designar um seletor para o bloco antigo
	;mov ax, [es: edi+0x08]		; pega a base antiga...desnecessario
	;mov word [fs: s_dataBios+2+b_gdt], ax	; salva no descritor ja definido
	mov word [es: edi+0x08], s_dataBios	;

; aponta os descritores...
	mov word [es: edi+0x0a], s_vga	; Seletor A000
	mov word [es: edi+0x0c], s_vtm	; Seletor B000
	mov word [es: edi+0x0e], s_vtc	; Seletor B800
	mov word [es: edi+0x10], s_vb16d	; Codigo VesaBIOS...aponta 0xc000:0000
	mov word [fs: 0x0be2], s_vb16c
	mov byte [es: edi+0x12], 1		; Ativa modo protegido
; inicializacao do servico Vesa
	mov si, word [es: edi+6]		; cria as tabelas das rotinas
	mov [fs:0xbe0], si
	;call far [fs:0xbe0]		; executar para usar o modo protegido
; montagem do portao de servicos Vesa		;
	mov ax, [es: edi+0x04]		; ponto de entrada
	mov [fs: P_VesaPM+b_gdt], ax		; Portao P_VesaPM pronto
	mov word [fs: P_VesaPM+2+b_gdt], s_vb16c     	; salva o seletor s_vb16c
; finaliza
.fim:	xor ah,ah				; CY=0 OK,
	pop edi
	pop esi				; CY=1. AH=0 - falta memoria no heap
	pop edx
	pop ecx
	pop ebx
	pop ds
	pop es
	ret
.fm1:	mov ah,2
	stc
	jmp .b1
.fm:	xor ah,ah
	stc
	jmp .b1
.b:	mov ah, 01
.b1:	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	pop ds
	pop es				; CY=1, AH=1 - pmid nao encontrado
	ret
.prepVesa2:
	ret				; fazer mais tarde


pmid	db 'PMID',0

; exemplo de uso
; 	call Busca_VideoBIOS
;	jb .analise de erros
			; se AH=01...optar pelo modo texto...usar a funcao 0 para
			; evitar esse pormenor


;-----------------------------------------------------------------------------------------

align 4
Vesa_N:		; o portao sera sempre inicializado
Vesa_Nula:	retf


;-----------------------------------------------------------------------------------------

; OBSERVACOES:
; A melhor forma consiste em ativar o modo grafico desejado na partida e nao alterar mais
; Fixar como forma padrao 1024x768 ou 800x600 com 16 bits...ou 32.


;-----------------------------------------------------------------------------------------

aloca_video:
	mov eax, [fs: VbeTotalMemory]
	shl eax, 0x10
	mov esi, [fs:PhysBasePtr]
	mov bx, 0x40fa
	call aloca_desc
	ret