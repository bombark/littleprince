; EXCEÇOES: Os Ponteiros devem ser posicionados na IDT
; Na reentrada vai pra tela que estava anteriormente. No modo 1, aparece a letra caracte-
; ristica do lado esquerdo dos valores de registradores. No modo 2, uma mensagem deve ser
; mostrada na linha abaixo da instrucao.

;-----------------------------------------------------------------------------------------


; INFORMACOES:
; Apontar as exceçoes na IDT com a rotina DPint1 para a exc1...exemplos

;	p_int0		dw DPexc00
;	rseg_i0		dw s_exe16_0 	; code_seg substituido por s_exe16
;	byte_i0		db 0
;	desc_i0		db 0x8f 		; trap #32 bits... terminar com iretd
;	ext_i0		dw 0
;
; 	; int 1 - depuração passo a passo
;	p_int1		dw DPint1 	;r_excDP
;	rseg_i1		dw s_exe16_0 	; code_seg substituido por s_exe16
;	byte_i1		db 0
;	desc_i1		db 0x8f 		; trap #16 bits... terminar com iret
;	ext_i1		dw 0

use32

;-----------------------------------------------------------------------------------------

align 4

DP_ExceptionReg	db 0				; exception ocorrured
DP_res		db 0

DP_EnterModeReg	db 0		; enter mode
DP_FlagsRegs	db 0		; flags (ver def.)
DP_MemoryBaseReg	dd 0		; base memory for re-enter point

;DP_opcode	equ 0x100 or OS_BASE	;
;regDp0		equ 0x104 or OS_BASE	;
;DP_seg		equ 0x108 or OS_BASE	;
;regDp7		equ 0x10c or OS_BASE	 ;

DP_posX	dw 0
DP_posY	dw 0
DP_color	dd 0
DP_CurrentColor	dd 0
DP_Window		dd 0

DP_BackColor	equ 0x00000000
DP_InstrColor1	equ 0xffff0000	; e
DP_InstrColor2	equ 0xcc00cc00	; a
DP_InstrColor3	equ 0x00990000	; d
DP_InstrColor4	equ 0xcc000000	; c
DP_InstrColor5	equ 0xff000000
DP_InstrColor6	equ 0x00ee0000
DP_NormalColor	equ 0xeeeeee00	; preto

DPStartX_pos	equ 0x10
DPStartY_pos	equ 0x20
DP_CharColor	equ 0x6970c300	; hex color

DP_RegColor	equ 0xbbbbbb00
DP_RegSegColor	equ 0xdd99dd00


;-----------------------------------------------------------------------------------------

align 4
DPexc00:	; retomavel 	; exc00- erro de divisão, retomável

 	mov [DP_ExceptionReg], 0x30	; posiciona tipo de excecao ocorrida
          mov [DP_color], DP_InstrColor3
	jmp DPexcRet

align 4
DPexc01:	; retomavel- DPint1: permite até 255 entradas se necessário DPint1

align 4
DPexc03:	; retomavel	; exc03- int por software, retomável

 	mov [DP_ExceptionReg], 0x33
          mov [DP_color], DP_InstrColor2
	jmp DPint1

align 4
DPexc04:	; retomavel	; exc04- Overflow, retomável

 	mov [DP_ExceptionReg], 0x34
	mov [DP_color], DP_InstrColor1
	jmp DPexcRet

align 4
DPexc05:	; retomavel	; exc05- instruçao fora dos limites, retomável

 	mov [DP_ExceptionReg], 0x35
          mov [DP_color], DP_InstrColor1
	jmp DPexcRet

align 4
DPexc06:	; retomavel	; exc06- instruçao inválida, retomável

 	mov [DP_ExceptionReg], 0x36
          mov [DP_color], DP_InstrColor1
	jmp DPexcRet

align 4
DPexc07:	; retomavel	; exc07- cpu sem unidade aritmética, retomável

 	mov [DP_ExceptionReg], 0x37
          mov [DP_color], DP_InstrColor1
	jmp DPexcRet

align 4
DPexc08:		; exc08- dupla falha, NÃO RETOMÁVEL...tem código de erro

 	mov [DP_ExceptionReg], 0x38
          mov [DP_color], DP_InstrColor4
	jmp DPexcD

; exc09- reserved: 		; point to iretd

align 4
DPexc0a:	; retomavel	; exc0a- invalid tss

 	mov [DP_ExceptionReg], 0x41
          mov [DP_color], DP_InstrColor1
	jmp DPexcRet

align 4
DPexc0b:	; retomavel	; exc0b- segment not present, retomável

 	mov [DP_ExceptionReg], 0x42
          mov [DP_color], DP_InstrColor1
	jmp DPexcRet

align 4
DPexc0c:	; exc0c- stack exception, NÃO RETOMÁVEL

 	mov [DP_ExceptionReg], 0x43
          mov [DP_color], DP_InstrColor4
	jmp DPexcD

align 4
DPexc0d: 	; exc0d- general protection fault, NÃO RETOMÁVEL e código de erro

 	mov [DP_ExceptionReg], 0x44
          mov [DP_color], DP_InstrColor4
	jmp DPexcD


align 4
DPexc0e:	; retomavel
 	mov [DP_ExceptionReg], 0x45
          mov [DP_color], DP_InstrColor1
	jmp DPexcD

; exc0f- reserved	- point to iretd

; exc10- reserved	- point to iretd

; exceçoes 11 a 1F: - reserved 	- point to iretd


;-----------------------------------------------------------------------------------------

align 4
DPint1:	mov [DP_ExceptionReg], 0x31	; code and instruction color
	mov [DP_color], DP_InstrColor3

DPexcRet: push eax			; error code for exc 8, C e D

; enter point to exceptions 8, C & D
DPexcD:	pushad			;
	push gs
	push fs
	push ss
	push es
	push ds

	mov bl,[DP_EnterModeReg]	;
	and ebx,0x07		; 8 ways for re-enter
	shl ebx,2
	mov ebp, esp
	call dword [ebx+DP_RoutineTable]

.a:	pop ds
	pop es
	pop ss
	pop fs
	pop gs
	popad
	add esp, 4	;(OK)pop eax
r_iretd:	iretd


;-----------------------------------------------------------------------------------------

; Routines table

align 16
DP_RoutineTable:
DP_rot00	dd DP_DebugRoutine	; natural start for int01 - OKDP_DebugRoutineMM
DP_rot01	dd DB_null;DP_ent2	; debug mode...all multiuse regs are clear
DP_rot02	dd DB_null;	;
DP_rot03	dd DB_null;
DP_rot04	dd DB_null;
DP_rot05	dd DB_null;
DP_rot06	dd DB_null
DP_rot07  dd DB_null;r8600	;


;-----------------------------------------------------------------------------------------

align 4
DP_DebugRoutine:

; verify enter mode

	mov byte [Timer_flags], 1		; trava troca de tarefas
	test byte [DP_FlagsRegs], 0x40
	jnz reentrada

	or byte [DP_FlagsRegs], 0x40		; set flag. reset on exit

	mov esi, OS_BASE+0x202000		; memory shows at start address
	or byte [ebp+0x41], 1


   ; Cria uma janela
	mov eax, 0x00100010			;pos
	mov ebx, 0x02c00280			;size (altura/largura)
	call SWN_Window_Create
	mov [DP_Window], edi

	;test [DP_FlagsRegs], 1
	;jnz Ent_reg			; prepara modo registrador...si aponta buffer dados

	mov [DP_posX], DPStartX_pos-0x10
	mov [DP_posY], DPStartY_pos-0x10

	mov edx, DP_BackColor
	shl edx, 0x10
	mov dl, 0x20

	mov ecx, 0x2c50
.l1:	call Imp_dl_gtexto
	dec cl
	jnz .l1
	mov [DP_posX], DPStartX_pos-0x10
	add [DP_posY], 0x10
	mov cl, 0x50
	dec ch
	jnz .l1

.a2:	mov edx, [DP_color]
.a:	mov dl, byte [DP_ExceptionReg]	; int type



; show memory block
	call DP_ShowRegs

	mov [DP_CurrentColor], DP_NormalColor

; imprime bloco em hexadecimal e ascii
	xor ecx, ecx
.b:	mov [DP_posX], DPStartX_pos
	mov [DP_posY], DPStartY_pos
	push esi
	mov cl, 0x20
  @@:	call DP_HexAscii_graph		; rotina que imprime bloco hexa ascii
	mov [DP_posX], DPStartX_pos
	add [DP_posY], 0x10
	loop @b
	pop esi

.d2:	call ObtemTeclaCom
	jc .b
	mov bl, al
	and ebx, 0x3f
	cmp bl, 0x2c
	ja .b
	shl ebx, 2
	call dword [ebx+DP_com-0x4]
	jae .b
	call beep
	jmp .b

align 4
; exit with ALT_ENTER

DB_StepExit:

	and byte [ebp+0x41], 0xfe	; bit T = 0


; exit with ENTER

DB_StepOut:

	or byte [ebp+0x42], 1	; enable bit  ??????
	mov [DP_MemoryBaseReg],esi	; save memory base to show
	mov byte [Timer_flags], 0	; libera troca de tarefas
.a:	xor eax,eax
	mov dr6,eax		; dr6 = 0

		mov eax, dr7
		or eax,0x2303
		mov dr7,eax	; bit GD enable

	add esp, 0x4
DB_null:	ret



;re-enter point

reentrada:

 	mov eax,dr6		; virtualbox nao pega passo a passo
	;mov [0x80000200], eax

	test ah,0x40		; passo a passo ativado?
	jne .f			; sim -> e5

; debug stop points

	or byte [ebp+0x41], 1	; step by step execution activated
	test al,0x0f		; verify stop points
	je .d
	mov [DP_ExceptionReg], 0x50	; P- stop point
          mov edx, DP_InstrColor3
	jmp .f1

; task exchanges

.d:	test ah,0x80
	je .f2	;.e
	mov [ DP_ExceptionReg], 0x54	; T - task change
          mov edx, DP_InstrColor3
	jmp .f1

; tentativa de acesso com GD=1 (falha)

.e:	test ah,0x20
	jne DB_null
	jmp .f2
.f:	mov [ DP_ExceptionReg], 0x53	; S - step by step execution
          mov edx, DP_InstrColor3
.f1:	mov esi, [DP_MemoryBaseReg]

	;test [DP_FlagsRegs], 1
	;jnz Ent_reg.a

	jmp DP_DebugRoutine.a

; exceptions enter

.f2:	mov esi, [DP_MemoryBaseReg]

	;test [DP_FlagsRegs], 1
	;jz DP_DebugRoutine.a2

	jmp DP_DebugRoutine.a2

	;call DP_ShowRegs		;
	;jmp Ent_reg.c		; modo registrador (reentrada)



;-----------------------------------------------------------------------------------------

align 4
DP_com:

	dd DB_Exit	; 1-esc		; saida do depurador
	dd DB_null	; 2-bs		;
	dd tab_com	; 3-tab		;
	dd DB_StepOut	; 4-enter		; executa um passo
	dd r2564_1a	; 5-s_cima	;  x100
	dd r25a8_1a	; 6-s_esquerda	; -x1000
	dd r25c0_1a	; 7-s_direita	;  x1000
	dd r257c_1a	; 8-s_baixo	;  x100
	dd r2600_1a.b	; 9-home		; home -  go to the start of memory
	dd r2550_1a	; a-end		; end - go to the end of memory
	dd r2620_1a	; b-pg_up		;  x10000
	dd r2600_1a	; c-pg_dn		; -x10000
	dd DB_null	; d-ins
	dd DB_null	; e-del
	dd DB_null	; f- X
	dd DB_null	; 10-X
	dd DB_Exit	; 11-alt esc	; termina a rotina em teste
	dd DB_null	; 12-alt bs
	dd DB_null	; 13-alt tab	;
	dd DB_StepIn	; 14-alt enter	; executa rotina passo a passo
	dd seta_up	; 15-alt s_cima	;  x1
	dd seta_up	; 16-alt s_esq	; -x10          seta_left
	dd seta_down	; 17-alt s_dir	;  x10          seta_right
	dd seta_down	; 18-alt s_baixo	; -x1
	dd DB_null	; 19-alt home
	dd DB_null	; 1a-alt end
	dd DB_null	; 1b-alt pg_up
	dd DB_null 	; 1c-alt pg_dn
	dd DB_null	; 1d-alt ins	;
	dd DB_null 	; 1e-alt del	;
	dd DB_null 	; 1f-X
	dd DB_null	; 20-X
	dd DB_null	; 21-F1
	dd DB_null	; 22-F2
	dd DB_null	; 23-F3		;
	dd DB_null 	; 24-F4		;
	dd DB_null	; 25-F5		;
	dd DB_null	; 26-F6		;
	dd DB_null	; 27-F7		;
	dd DB_null 	; 28-F8 		;
	dd DB_null	; 29-F9		;
	dd DB_null	; 2a-F10		;
	dd DB_null	; 2b-F11		;
	dd DB_null	; 2c-F12		;

;-----------------------------------------------------------------------------------------




;-----------------------------------------------------------------------------------------

; SAIDA COM ESC - Esta saida executa toda rotina pendente...trava se a rotina
; tiver defeito

align 4
DB_Exit:

; testa tipo de entrada

	;test [DP_FlagsRegs], 0x80
	;jz .a
	;add esp, 2
	;mov esp, [ regESP]		; obtem ponteiro da pilha da entrada
	;sub esp, 0x2		; repoe ponteiro da pilha (40 + a)
	;add dword [ r_esp0_0], 0x50	; recompoe esp no TSS
	;and byte [ebp+0x37+0x50], 0xfe
	;jmp .b
	and byte [DP_FlagsRegs], 0xbf	; reset re-enter mode
.a:	and byte [ebp+0x41], 0xfe	; desmarca bit T, Saida com ESC
	jmp DB_StepIn.a		; salva ponteiros e sai



;-----------------------------------------------------------------------------------------

align 4

DB_StepIn: 	; comando para fazer passo a passo em rotinas

	mov ebx,[ebp+0x38]	; proxima instruçao a ser executada
	;mov [fs: si+0xc],ebx	; salva o ponteiro da ultima instruçao (si nao disponivel)
.a:	mov al,[ebx]	; leitura com ds=cs...mudado provis. para DS
	call DP_pref	; se tiver prefixos antes da instrucao, rejeita
	jnb .b
	inc ebx
	jmp .a		; verifica se proximo tambem é prefixo

; al point to instruction opcode

.b:	cmp al,0xe8	; call near
	je .e
	cmp al,0xe0	; loop
	je .e
	cmp al,0xe1	; loopne
	je .e
	cmp al,0xe2	; loope
	je .e
	cmp al,0xe3	; jcxz
	je .e
	cmp al,0x9a	; call far
	je .e
	cmp al,0xf2	; rep
	je .e
	cmp al,0xf3	; repne
	je .e
	cmp al,0xff	; call rmv
	je .d
.c:	jmp DB_StepOut	; se nao for nenhum, sai como ENTER



align 4
; instruçoes do bloco 0xff

.d:	inc ebx
	mov ah, [ebx]	; analisa o proximo byte
	test ah,0x20	;
	jne .c
	test ah,0x10	;
	je .c
.e:	mov bx, cs
	call RetornaBaseDescritorBX	; retorna eax=base do descritor (GDT), bl= tipo
	mov [DP_seg],eax		; recupera endereço da proxima instruçao
	add eax,[DP_opcode]		; calcula end fisico
	mov [regDp0], eax		;
	mov dr0,eax		; posiciona ponto de parada
	and eax, 0x7fffffff
	mov dr1,eax
	mov eax, dr7
	or eax, 0x000003ff		; 00-GD-0 00GE-LE G3L3G2L2 G1L1G0L0
	mov [regDp7],eax		; GD=1, GE=1, LE=1 G0=1, L0=1 tipo: exec
	mov dr7,eax		; (0-exec, 1-wb, 3-wrb, 5-ww, 7-wrw, d-wd, f-wrd)
	;jmp DB_StepOut
	jmp DB_StepExit		; sai como alt_enter



align 4
DP_pref: 	; REJEITA BYTES DE PREFIXO
	push ebx		; bx aponta bloco de comparaçao em GS
	push ecx
	mov ecx,0xa
	mov ebx, b_pref	; 476c (ver desa_cte.cb)
.a:	cmp al,[ebx]	; busca
	je .b
	inc ebx
	loop .a
	pop ecx
	pop ebx
	clc
	retn
.b:	pop ecx		; prefixo encontrado, cy=1
	pop ebx
	stc
	retn



;-----------------------------------------------------------------------------------------

align 4
imp_stringzGS:
.b:	mov dl,[esi]
	or dl,dl
	je .a
	inc esi
	call Imp_dl_gtexto
	jmp .b
.a:	ret


align 4
DP_HexAscii_graph:

          mov edx,esi
.a:	call graph_rd_hex		; imprime...endereço #32
          mov dl,0x20
          call Imp_dl_gtexto
          call Imp_dl_gtexto		; imprime 2 espaços
          call Imp_dl_gtexto

; imprime bloco hex
          push ecx
          push esi
          mov cl,0x10
.b:       mov al,[esi]		; busca byte apontado por ds
          call graph_bytehex		; imprime 16 colunas de bytes
          mov dl,0x20
          call Imp_dl_gtexto		; imprime espaço
          inc esi
          loop .b
	mov dl, 0x20
	call Imp_dl_gtexto

; imprime bloco ascii
          mov edx, DP_InstrColor1
          pop esi			; retorna o ponteiro
          mov cl,0x10
.c:       mov dl,[esi]
          call Imp_dl_gtexto		; imprime 16 colunas caractere ascii
          inc esi
          loop .c
          pop ecx
          retn


;-----------------------------------------------------------------------------------------

align 4
graph_bytehex:
	push edx
	mov edx, [DP_CurrentColor]
.a:       aam 0x10
          cmp ah,0x09
          jna .b
          add ah,0x07
.b:       cmp al,0x09
          jna .c
          add al,0x07
.c:       add ax,0x3030
          mov dl, ah
          call Imp_dl_gtexto
          mov dl, al
          call Imp_dl_gtexto
	pop edx
	retn


align 4
graph_wo_hex:
          push ecx
          mov ecx,2
.c:       rol edx,0x08
          mov al,dl
          call graph_bytehex
          loop .c
          pop ecx
.b:       retn


align 4
graph_rd_hex:
.a:      	push ecx
          mov ecx,0x0004
.c:       rol edx,0x08
          mov al,dl
          call graph_bytehex	;	imprime 1 byte
          loop .c
          pop ecx
.b:       retn


Imp_dl_gtexto:	; dl= caractere to print
	push eax
	push ecx
	push edi
	push ebp
	mov ecx, edx
	shr ecx, 0x8
	mov ebp, DP_BackColor
	mov eax, dword [DP_posX]
	mov edi, [DP_Window]
	call draw_Char2
	mov dword [DP_posX], eax
	pop ebp
	pop edi
	pop ecx
	pop eax
	ret



;-----------------------------------------------------------------------------------------

; Mapa de entrada: (apontado por ebp)
; base Dep0
	; efl		+40	bit T = 0x41
          ; cs		+3c
          ; eip		+38

          ; cod_erro	+34	push eax
; pushad
          ; eax		+30
          ; ecx		+2c
          ; edx		+28
          ; ebx		+24
          ; esp		+20
          ; ebp		+1c
          ; esi		+18
          ; edi		+14
; sel
          ; gs		+10
          ; fs		+c
          ; ss		+8
          ; es		+4
          ; ds		+0

;-----------------------------------------------------------------------------------------

align 4
DP_ShowRegs:

	mov [DP_posX], 0x10
	mov [DP_posY], 0x230
	call Imp_dl_gtexto		; print al
	mov [DP_posX], 0x68

	mov [DP_CurrentColor], DP_RegSegColor

	mov edx,[ebp-2]		; DS
	call graph_wo_hex		;
	mov edx,[ebp+0x2]		; ES
	call graph_wo_hex		;
	add [DP_posX], 0x28+0x48

;; testa mudança de nivel
;	test byte [ebp+0x32], 3
;	jz .ss0			; acessa pilha nivel 0
;.ss1:	mov eax, [ebp+0x3e]		;ss1
;	mov bx, ax		;bx=ss1
;	call graph_wo_hex		;r8a5b
;	add di, 0x4
;	push ebp
;	mov ebp, [ebp+0x3a]		; ebp=esp1
;	cmp ebp,0xfffe
;	jb .c2			; Menor, busca valor na pilha
; prenche espaco da pilha com espacos vazios
;	mov ax, [di]		; Maior ou igual, espaços com a cor da tela
;	mov al, 0x20		;
;	push ax
;	shl eax, 0x10
;	pop ax
;	mov [di], eax
;	mov [di+4], eax
;	mov [di+8], eax
;	mov [di+0xc], eax
;	add di, 0x10
;	jmp .h1

.c2:	;push ds			;retirado...tentar com descritor imagem de pilha
	;mov al,0x92		;
	;call aloca_descritor_imagem
	;mov ds,bx
	;mov eax, [ds: ebp]		; busca dados na pilha 1 (usa DS)
	;call graph_rd_hex
	;and bl, 0xf8
	;add ebx,b_gdt	;+e_virtual
	;mov byte [fs: ebx+5], 0	; libera descritor
	;pop ds			;

; falta liberar o descritor imagem
.h1:	;add di, 0x04		; era 4
	;pop ebp
	;jmp .j

;imprime ss, e conteudo da pilha
.ss0:	mov edx,[ebp+0x6]		; SS
	call graph_wo_hex		;r8a5b
	add [DP_posX], 0x8

mov [DP_CurrentColor], DP_InstrColor5
	push ebp
	add ebp, 0x44		;
	cmp ebp,0x9fffe or OS_BASE
	jb .c			; Menor, busca valor na pilha
	mov ecx, 8
	xor dl,dl		; Maior ou igual, imprime espaços
  @@:	call Imp_dl_gtexto
	loop @b
	jmp .h

.c:	mov edx, [ebp]		; imprime o conteudo da pilha
	call graph_rd_hex

.h:	pop ebp
	add [DP_posX], 0x8

mov [DP_CurrentColor], DP_NormalColor

.j:	mov edx, [ebp+0x40]		; flags e condiçoes

	push edx
	call graph_rd_hex		; efl
	pop edx			; fazer uma rotina para desabilitar
	add [DP_posX], 0x8

	call DP1800		; imprime condiçoes dos flags
	add [DP_posX], 0x38
	mov ebx, c_flags
	test al,0x01
	call DP6750		; Carry flag
;		test dl,0x04	; despreza P flag
;		call DP6750
	add ebx, 4			;
	test al,0x40
	call DP6750		; Zero flag
	test al,0x80
	call DP6750		; Signal flag

	add [DP_posY], 0x10
	mov [DP_posX], 0x20



	mov edx, [ebp+0x30]
	call graph_rd_hex		; eax
	add [DP_posX], 0x8

	mov edx, [ebp+0x24]
	call graph_rd_hex		; ebx
	add [DP_posX], 0x8

	mov edx, [ebp+0x2c]
	call graph_rd_hex		; ecx
	add [DP_posX], 0x8

	mov edx, [ebp+0x28]
	call graph_rd_hex		; edx
	add [DP_posX], 0x8

; testa mudança de nivel
	;test byte [ebp+0x32], 3
	;jz .esp0
	;mov eax, [ebp+0x3a]		; esp1
	;jmp .esp1

; imprime esp nivel 0
.esp0:	mov edx, [ebp+0x20]		; esp0
	add edx, 0x10
.esp1:	call graph_rd_hex		; esp
	add [DP_posX], 0x8

	mov edx, [ebp+0x1c]
	call graph_rd_hex		; ebp
	add [DP_posX], 0x8

	mov edx, [ebp+0x18]
	call graph_rd_hex		; esi
	add [DP_posX], 0x8

	mov edx, [ebp+0x14]
	call graph_rd_hex		; edi
	mov [DP_posX], 0x20
	add [DP_posY], 0x20

	call DPtipoCS		; ?
	add [DP_posX], 0x10

; get code segment descriptor

	mov edx, [ebp+0x3a]
	;mov ds, ax		; transfere cs para ds para leitura do opcode
mov [DP_CurrentColor], DP_RegSegColor
	call graph_wo_hex		; cs (usar assim enquanto for nivel 0)
	add [DP_posX], 0x8

;mov [DP_CurrentColor], DP_NormalColor
	mov eax, [DP_color]
	mov [DP_CurrentColor], eax

	mov edx, [ebp+0x38]		; eip
	mov ebx, edx		; ebx= ponteiro da instruçao
	call graph_rd_hex

	mov eax,[ebp+0x3a]		; ax=cs
	lar eax,ax
	bt eax,0x16
	jb .b
	or ch,0xc0
	jmp .c1
.b:	and ch,0x3f		; limpa bit que define instruçao de 32 bits

; limpa buffer da instrucao
.c1:	mov [DP_posX], 0xb0

	mov al, 6
	xor dl, dl
.c3:	push ecx
	xor ecx, ecx
	mov cl, 0x30		; limpa espaço da instruçao no video
.f:	call Imp_dl_gtexto
	loop .f
          pop ecx
	mov [DP_posX], 0xb0
	add [DP_posY], 0x10
	dec al
	jnz .c3

	mov [DP_posY], 0x240+2*0x10

; disassembler
	and [opcode_cont], dword 0		; inicializa contador
	and [opcode_cont+4], dword 0
	and [opcode_cont+8], dword 0
	and [opcode_cont+0xc], dword 0
	or ch, 0x20		; habilita #5[ch] para imprimir codigo

	mov [DP_posX], 0xb0
	call desa_ini		; ebx vem apontando a instruçao a ser desassemblada
	mov [DP_opcode], ebx	; salvo para uso com alt_enter  (ext do opcode)
;ret
	mov [DP_CurrentColor], DP_RegColor
	and ch, not 0x20
	mov [DP_posX], 0x68
	add [DP_posY], 0x10
	mov edx, ebx
	call graph_rd_hex
	add [DP_posX], 0x8
	call desa_ini
;ret
	mov [DP_posX], 0x68
	add [DP_posY], 0x10
	mov edx, ebx
	call graph_rd_hex
	add [DP_posX], 0x8
	call desa_ini

	mov [DP_posX], 0x68
	add [DP_posY], 0x10
	mov edx, ebx
	call graph_rd_hex
	add [DP_posX], 0x8
	call desa_ini

	mov [DP_posX], 0x68
	add [DP_posY], 0x10
	mov edx, ebx
	call graph_rd_hex
	add [DP_posX], 0x8
	call desa_ini

	mov [DP_posX], 0x68
	add [DP_posY], 0x10
	mov edx, ebx
	call graph_rd_hex
	add [DP_posX], 0x8
	call desa_ini

.saida:	retn



; analisa flags. apresenta as flags na forma de letras tal como Z ou NZ, C ou NC...
align 4
DP1800:

	mov eax, edx
	and al,0x80
	rol ah,0x04
	and ah,0x80
	xor al,ah
	mov al, dl
	mov edx,DP_InstrColor1
	mov dl,0x4c	;L
	jne .186a
	test al,0x40
	mov dl,0x45	;E
	jne .186a
	mov dl,0x47	;G
.186a:	call Imp_dl_gtexto
	mov dl,0x41	;A
	test al,0x41
	je .187f
	inc dl
	test al,0x01	;B
	jne .187f
	add dl,0x03	;E
.187f:	jmp Imp_dl_gtexto



;-----------------------------------------------------------------------------------------

; BUSCA FLAG CONFORME 0 OU 1 (ROT INTERNA USADA POR 6778)
align 4
DP6750:

	mov ecx,0003
	je .6765
.6758:	inc ebx
	mov edx, DP_InstrColor5
	mov dl,[ebx]
	call Imp_dl_gtexto
	loop .6758
	inc ebx
	retn

.6765:	mov edx, DP_InstrColor6
.6769:	inc ebx
	mov dl,[ebx+nc_flags-c_flags]
	call Imp_dl_gtexto
	loop .6769
	inc ebx
	retn



;-----------------------------------------------------------------------------------------

; Flags do processador
align 4
c_flags:

	db 0x0c,' C ',0x0c,' P ',0x0c,' Z ',0x0c,' S '
	db 0x0c,' O ',0x0c,0x19,0,0,0x0c   ,'T  ',0x0c,'I  '
	db 0x0c,'N  ',0x0c,'R  ',0x0c,'V  '
align 4
nc_flags:

	db 0x0a,'NC ',0x0a,'NP ',0x0a,'NZ ',0x0a,'NS '
	db 0x0a,'NO ',0x0a,0x18,0,0,0x0a   ,'T  ',0x0a,'I  '
	db 0x0a,'N  ',0x0a,'R  ',0x0a,'V  '



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

align 4
DPtipoCS:		; busca e imprime o tipo de descritor CS (add leitura)

	mov ax, [ebp+0x3a]
	lar eax,ax
	bt eax,0x16
	jnb .a
	mov al,0x16
	;and byte[Desa_flags],0x3f	; limpa bit que define instruçao de 32 bits
	jmp graph_bytehex
.a:	mov al,0x32
	;or byte [Desa_flags],0xc0	; aciona
	jmp graph_bytehex
	ret			;provisorio



;-----------------------------------------------------------------------------------------

; COMANDOS PARA MOSTRAR A MEMORIA

;------ s_cima--OK------------------------------------------------------------------------

align 4
r2564_1a:
	mov eax,esi
	sub eax,0x100
	cmp eax, 0x80000000
	jb .a		; rotina beep do teclado
	mov esi,eax
.a:	retn



;------ s_baixo--OK-----------------------------------------------------------------------

align 4
r257c_1a:
	mov eax, esi
	mov bx,ds
	lsl ebx,bx	; pega a extensão do descritor de segmento
	jne .b		; nao acontece
	add eax,0x1ff	; verifica se cai fora do intervalo
	cmp ebx,eax
	jb .b
	add esi,0x0100
.b:	retn



;------ s_esq--OK-------------------------------------------------------------------------

align 4
r25a8_1a:
	mov eax,esi
	sub eax,0x1000
	cmp eax, 0x80000000
	jb .a
	mov esi,eax
.a:	retn



;------ s_dir-OK--------------------------------------------------------------------------

align 4
r25c0_1a:
	mov eax,esi
	mov bx,ds
	lsl ebx,bx
	jne .a
	add eax,0x10ff
	cmp ebx,eax	; compara com extensao do segmento
	jb .a		; nao faz nada se estiver no fim
	add esi,0x1000	; senao soma 1000 a esi
.a:	retn



;--pg_dn-OK-------------------------------------------------------------------------------

align 4
r2600_1a:
	mov eax,esi
	sub eax,0x10000
	cmp eax, 0x80000000
	jb .b
	mov esi,eax
	retn
.b: 	mov esi, 0x80000000	; comando HOME
	retn



;---; pg_up-OK----------------------------------------------------------------------------

align 4
r2620_1a:
	mov eax,esi
	mov bx,ds
	lsl ebx,bx
	jne DP_erro	; erro com beep
	add eax,0x100ff
	cmp ebx,eax
	jb r2550_1a.a	; se for maior pega o endereco final valido
	add esi,0x10000	; senao soma 10000
	retn
r2550_1a: mov bx,ds		; Comando END
	lsl ebx,bx
.a:	sub ebx,0xff
	mov esi,ebx
	retn


;---s_baixo-------------------------------------------------------------------------------

align 4
seta_down:	; seta para baixo

.a:       mov eax,esi
          mov bx,ds
          lsl ebx,bx		; obtem a extensao do segmento
          jne DP_erro		; erro com beep
          add eax,0x10f
          cmp ebx,eax
          jb DP_erro			; erro...imp aviso
          add esi,0x10
.b:       retn

align 4
DP_erro:
          stc
          retn


;---; s_cima------------------------------------------------------------------------------

align 4
seta_up:	; seta para cima

.a:	mov eax,esi
	sub eax,0x10
	jb DP_erro			; erro com beep
	mov esi,eax
.b:	ret


;-----------------------------------------------------------------------------------------

; busca base de um descritor apontado por bx
; ent: 	ebx= descritor
; sai: 	eax=base do descritor.
; altera: eax

align 4
RetornaBaseDescritorBX :	; bx= descritor, altera eax...esta rotina esta OK (maio/2006)
	 push ecx			;
	 and bl, 0xf8		; zera RPL
	 add ebx, GDT_Table-8
	 mov al, [ebx+04]
	 mov ah, [ebx+07]	;
	 shl eax, 0x10
	 mov ax, [ebx+02]	;
	 mov cl, [ebx+5]
	 xor ebx, ebx
	 mov bl, cl
	 pop ecx			; bx= retorna o tipo do descritor
	 ret			; eax= base do descritor


;-----------------------------------------------------------------------------------------

tab_com:
	xchg [tab_comm], esi
	ret


;-----------------------------------------------------------------------------------------