;-----------------------------------------------------------------------------------------

; Servicos de Entrada e Saida

;-----------------------------------------------------------------------------------------


;======================== C O M E N T A R I O S ==========================================
; PORTAO DE SERVI�OS DO TECLADO...(rotinas definitivas)
; Autores: Clovis Bombardelli e Felipe Gustavo Bombardelli
; Abril/2006
;
; Usa rotinas: 	teclado_cte.cb
;		teclado_dev.cb

; Teclas de Comando
	; 1-esc		
	; 2-bs		
	; 3-tab		
	; 4-enter	
	; 5-s_cima	
	; 6-s_esquerda	
	; 7-s_direita	
	; 8-s_baixo	
	; 9-home	
	; a-end		
	; b-pg_up	
	; c-pg_dn	
	; d-ins
	; e-del
	; f- X
	; 10-X
	; 11-alt esc
	; 12-alt bs
	; 13-alt tab
	; 14-alt enter
	; 15-alt s_cima	
	; 16-alt s_esq	
	; 17-alt s_dir	
	; 18-alt s_baixo	
	; 19-alt home	
	; 1a-alt end	
	; 1b-alt pg_up	
	; 1c-alt pg_dn	
	; 1d-alt ins
	; 1e-alt del
	; 1f-X
	; 20-X
	; 21-F1		
	; 22-F2		
	; 23-F3		
	; 24-F4		
	; 25-F5		
	; 26-F6		
	; 27-F7		
	; 28-F8 	
	; 29-F9		
	; 2a-F10	
	; 2b-F11	
	; 2c-F12

;-----------------------------------------------------------------------------------------





;-----------------------------------------------------------------------------------------

; Entrada  do servico de memoria
; IN:
;	registradores= parametros
;	ebp= numero da funcao
;	int 0x44
; OUT:
;	ebp= codigo de erro....usar or ebp,ebp para verificar a ocorrencia


align 4
SDD_Enter:
        and ebp, 0x0f		; define a quantidade de servi�os
.b:	cmp ebp, 0x6		; quantidade de servi�os disponiveis
        ja .saida
        shl ebp,2
        call dword [ebp+b_tab908]	; busca uma das rotinas da tabela
	jc .error
	and byte [esp+8], 0xfe
.saida: iretd

.error:	or byte [esp+8], 1		; seta CY=1 se houve erro
	iretd

; retornar informa��o de erro


;-----------------------------------------------------------------------------------------

; N�o mudar mais a posi��o das rotinas existentes. Se necesitar de
; rotina diferente, acrescentar a mesma na sequ�ncia, alterando a linha -.b da rotina
; principal e fazer um DESCRITIVO sucinto para a mesma.
align 2
b_tab908:
	dd reservado6		;0 -reservado
	dd AguardaEspa�o		;1 -espera digita�ao de espa�o ou enter
	dd AguardaEnter		;2 -espera enter ser digitada
	dd Aguarda_teclaBH		;3 -espera pela tecla apontada por BH
	dd ObtemTeclaCar		;4 -obtem caracter digitado (AL=carater)
	dd ObtemTeclaCom		;5 -obtem comando digitado  (AL=comando)
	dd ObtemQqTecla		;6 -obtem qualquer tecla
	dd RepOpPadrao		;7 -repoe operadores padrao do teclado
	dd PosOpComando		;8 -posiciona operador de comando
	dd PosOpEscrita		;9 -posiciona operador de escrita
	dd AguardaQqTecla		;a -espera a digita�ao de qualquer tecla
;	dd Chama_depurador		;b -chama servi�o depurador passo a passo

;
; O loop de comando deve ficar no nivel de execu�ao em que se encontra o programa (N3). O
; operador de comando pode escrever o comando numa variavel acessivel a este nivel ou pode
; retornar em AL.
; Os operadores de teclado fornecem um recurso a mais permitindo redefinir onde o driver
; do teclado pode entregar os carateres e os comandos digitados.

;-----------------------------------------------------------------------------------------
reservado6:
	ret

;-----------------------------------------------------------------------------------------

; Operadores de teclado genericos para uso geral
align 2
AguardaQqTecla:	; aguarda ate que uma tecla seja pressionada

r830e:	and byte [r47b], 0x7f	; 1011
.a:	test byte [r47b], 0x80
	je .a
	and byte [r47b ], 0x7f
	mov ax, word [KB_char]
	clc
	ret


;-----------------------------------------------------------------------------------------

align 2		; 05
ObtemTeclaCom:	; n�o espera digita�ao. Cy=1 se nao houver tecla pressionada
	test byte [r471], 0x80
	je .a
	and byte [r471], 0x7f
	mov ax, word [KB_char]
	clc
	ret
.a:	stc
	ret


;-----------------------------------------------------------------------------------------

align 2
ObtemQqTecla:	; n�o espera digita�ao. Cy=1 se nao houver tecla pressionada

	test byte [r47b], 0x80
	je .a
	and byte [r47b], 0x7f
	mov ax, word [KB_char]
	clc
	ret
.a:	stc
	ret


;-----------------------------------------------------------------------------------------

align 2
ObtemTeclaCar:	; n�o espera digita�ao. Cy=1 se nao houver tecla pressionada

	test byte [KB_leds], 0x80
	je .a
	and byte [KB_leds], 0x7f
	mov ax, word [KB_char]
	clc
	ret
.a:	stc
	ret


;-----------------------------------------------------------------------------------------

align 2
AguardaEspa�o: 	; espera tecla de espa�o ser pressionada
	and byte [r47b], 0x7f
.a:	test byte [r47b], 0x80
	je .a
	and byte [r47b], 0x7f
	mov ax, word [KB_char]
	cmp al, 0x20
	je .b
	cmp al, 0x4	; aguarda enter (verificar qual � o bit que fica setado ...)
	jne .a
.b:	ret


;-----------------------------------------------------------------------------------------

align 2
AguardaEnter:	ret


;-----------------------------------------------------------------------------------------

Aguarda_teclaBH:	; aguarda digita�ao cujo codigo seja igual a BH
	and byte [r47b], 0x7f
.a:	test byte [r47b], 0x80
	je .a
	and byte [r47b], 0x7f
	mov ax, word [KB_char]
	cmp al, bh
	jne .a
	ret


;-----------------------------------------------------------------------------------------

align 2
RepOpPadrao:
	cmp al, 1
	ja .b
	jb .a
	mov [ r472], dword r8020	; 0 -repoe ambos
	mov [ r474], dword r8020
	jmp .c
.a:	mov [ r472], dword r8020	; 1 -apenas escrita
	jmp .c
.b:	mov [ r474], dword r8020	; 2 -apenas comando
.c:	xor ax, ax
	mov word [ KB_char], ax	; reseta caractere pressionado
	mov word [ r470], ax	; reseta leds e eflags
	call r8020		; modifica eflags
	ret


;-----------------------------------------------------------------------------------------

align 2
PosOpEscrita:	; define ponteiros para escrita direta no segmento DS
	;mov [ p_escritaCar], eax
	;mov [ contador_escritaCar], edx
	;mov [ p_fimEscritaCar], ecx
	;mov [ r472], escrita_direta	; 1 -apenas escrita
	;jmp RepOpPadrao.c
align 2
escrita_direta:
	;mov ebx, [ p_escritaCar]
	;cmp ebx, [ p_fimEscritaCar]
	;ja .a
	;mov [ebx],al	; escreve sempre no segmento DS do usuario
	;mov ebx, [contador_escritaCar]
	;inc word [ebx]	;
.a:	;ret


;-----------------------------------------------------------------------------------------

align 2
PosOpComando:	; define ponteiro para escrita dos comandos no segmento DS
	;mov [ p_comando], eax
	;mov [ r474], escrita_comando
	;jmp RepOpPadrao.c
align 2
escrita_comando:
	;mov ebx, [ p_comando]
	;or al, 0x80
	;mov [ebx], ax		; escreve word na posi�ao comando com #7 setado
	;ret


;-----------------------------------------------------------------------------------------

; outros servi�os: 	(id�ias)
;		redefinir a tabela de caracteres
;		introduzir execu�ao direta via teclado
; 		redefenir tempo de repeti�ao e espera
;		definir tempo de espera para chamar programa vigia
;		o programa do usuario sai do anel de execu��o at� o teclado inserir
; 		novo comando ou caractere...quando recoloca a tarefa em execu��o.
;		O driver do teclado escreve num buffer circular nivel 3


;-----------------------------------------------------------------------------------------
