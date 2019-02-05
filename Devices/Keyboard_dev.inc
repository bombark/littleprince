;-----------------------------------------------------------------------------------------
;
; LITTLE PRINCE Operational System
; Copyright (C) 2008 to 2011 - LP Development Team
;
;-----------------------------------------------------------------------------------------
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;
;-----------------------------------------------------------------------------------------
;
; @Id: Keyboard_dev.inc  - 2005-3-07, cb-toledo
; Keyboard interruption routine
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------
;
; INFORMAÇÕES:
; A entrada é dada por Tec_int
; A tarefa ativa deve se apossar do man. teclado ao entrar, fazendo-o
; escrever num buffer circular no segmento de memoria da tarefa
;
; INICIALIZAÇÃO:
; o video sempre carrega a tabela 860 para qualquer teclado
; as variáveis r472 e r474 devem serem inicializadas na partida.
; provisoriamente são definidas com ret (não faz nada)
; definir o tempo de espera padrão na variável fs: rdbe
;
; DEFINITIONS:
; códigos para os comandos tabela de tradução usada nas tabelas de comandos
; especiais 0, 1 e 2.
; 00- nada       01- esc		02- backscape	03- tab	     04- enter
; 05- s_cima     06- s_esquerda	07- s_direira	08- s_baixo    09- home
; 0a- end	       0b- pg_up		0c- pg_dn		0d- insert     0e- delete
; 0f- nao usado  10- aviso troca	11- alt esc	12- alt BS     13- alt tab
; 14- alt enter  15- alt s_cima	16- alt s_esq	17- alt s_dir  18- alt s_baixo
; 19- alt home   1a- alt end		1b- alt pg_up	1c- alt pg_dn  1d- alt insert
; 1e- alt delete 1f- car recebido	20- nao usado	21- F1	     22- F2
; 23- F3	       24- F4		25- F5		26- F6	     27- F7
; 28- F8	       29- F9		2a- F10		2b- F11	     2c- F12
; 2d- S F1       2e- S F2		2f- S F3		30- S F4	     31- S F5
; 32- S F6       33- S F7		34- S F8		35- S F9	     36- S F10
; 37- S F11      38- S F12		39- A F1		3a- A F2	     3b- A F3
; 3c- A F4       3d- A F5		3e- A F6 		3f- A F7	     40- A F8
; 41- A F9       42- A F10		43- A F11		44- A F12	     45- S esc
; 46- S enter    47- S tab		48- S BS		49- reservado  4a- reservado
; 4b- reservado  4c- reservado	4d- reservado	4e- reservado  4f- reservado
;
;
;-----------------------------------------------------------------------------------------

; PORTOES PARA O TECLADO, TODOS NIVEL 3 (a fazer):
;> serviços diversos- troca de tabela, inicialização do teclado, alterar tempo de espera
;> parâmetros do beep, alterar caracteres de uma das tabelas, travar o teclado...etc
;> alterar leds do teclado;
;> beep;
;> informações do modo de operação;
;> inicializar modo padrao;
;> inicializar modo bufferizado;
;> aguarda digitação no modo padrao ou bufferizado
;> busca digitação no modo padrão ou bufferizado
;> desativa o teclado e marca inicialização
;> aguarda a liberação de tecla. (qualquer tecla de caractere);
;> aguarda comando no modo padrão ou bufferizado;
;> aguarda comando no modo de escrita direta;
;> busca comando no modo de escrita direta;
;> inicializa o modo de escrita direta;
;> altera a base no modo de escrita direta;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


align 16
;db 'teclado---------'
align 4 ; manipulador do teclado... fs e gs devem estar inicializados...
Tec_int:

	push eax
	push ebx
	push ecx
	push edx

	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx

	call r83ac	; aguarda sinal da controladora

; desabilita o teclado, desativa a linha de clock e seta #4 do registro de comando

	mov al,byte 0xad
	out byte 0x64,al
	call r83ac		; idem
	in al,byte 0x60		; busca byte da interupção

.teste:	or al,al
	jz beep_t

	cmp al,byte 0xf0		; recebeu resposta de comando?
	ja .com_rec		; sim. filtra comando
.a:	mov cx,word [r470]		; se for digitaçao válida busca byte status

	mov [scancode], al

	mov ah,cl
	mov dh,[KB_leds]		; DH = situaçao dos leds
	xor ah,dh			; verifica se houve mudança na situação dos leds
	and ah,byte 0x07
	je .b

; (com defeito)..virtualbox nao aceita, bochs aceita

	;call r8188		; aciona leds

.b:	xor ah,ah
	cmp al,byte 0xe0		; interrupçao tipo E0, marca e sai
	je .E0rec
	cmp al,byte 0xe1		; interrupçao tipo E1
	je .E1rec
	test cl,byte 0x40		; E1. pause pressionada anteriormente
	jne .pause
	mov dx,ax			; ax = scancode do teclado
	mov ebx, TabInd-1		; busca a base do mapa, bloco 0

	test al,byte 0x80		; tecla liberada? al= break code
	jne .i

	test cl,byte 0x20		; E0=1?
	jne .tec_esp
	and ch,byte 0xc7

.d:	xlatb			; busca o indice

	test cl, 0x80		; modo 1, modo comando
	jnz .busca_com		; pula diretamente para a rotina que busca o comando
	add bx,dx			; compõe o endereço do indice
	mov [r476],bx		; salva o endereço do indice - permite substituir o codigo.....causa exceçao D ???????????????
	or al, 0x40		; marca tecla pressionada
	mov [r47b],al		; salva indice e marca tecla pressionada

.e:	and ax, 0x0f		; indice limitado a 16 combinacoes. pode ser ampliado até 128.
	shl al,2
	mov ebx,eax		; compõe o ponteiro da rotina relativa ao indice

	call dword [ebx+base_rt]	; aciona a rotina indexada adequada

.saida1:	mov word [r470], cx		; salva estado dos leds e flags

.saida2:	mov al, byte 0x20		; libera outras interrupções (não é ncessário)
	out byte 0x20,al
.saida3:
; habilita o teclado
	call r83ac		; aguarda resposta do teclado
	mov al,byte 0xae		;
	out byte 0x64, al		;

	pop edx
	pop ecx
	pop ebx
	pop eax
	;sti
	iretd



;-----------------------------------------------------------------------------------------

; eh código de liberação de tecla
.k:	and [r47b],byte 0xbf	; desmarca tecla pressionada (#6[r47b])
	mov ebx,[key_wait_time]	; repõe tempo padrao
	mov [r47c],ebx		; atualiza flags 1011 1111
	jmp .saida1		; sai
; tecla liberada
; se for usado set 3, pula para .ii
.i:	sub al,byte 0x80	; transforma break code em scan code
.ii:	xlatb		; busca indice
	or al,al		; se for zero, desmarca tecla pressionada e sai
	je .k		;
	and cl,byte 0x1f	; 0001 1111 - limpa bits #5, 6 e 7 (modo1, e0 e e1)
	cmp al,byte 0x03	; se al>3 sai
	ja .k
	add al,byte 0x06	; se al<3 executa rotinas correspondentes alternativas
	jmp .e		; 01->7, 02->8, 03->9,

; E0 recebido, marca bit #5[cl]
.E0rec:	or cl,byte 0x20
	jmp .saida1

; E1 recebido, marca bit #6[cl]. pause foi pressionada ou solta
.E1rec:	or cl,byte 0x40
	jmp .saida1
; tecla pause pressionada: pode ser usada para entrar no modo passo a passo (depuração)
.pause:	cmp al,byte 0x9d		; código 9d desprezado
	jne .saida1
	and cl,byte 0xbf		; código 45, ( elimina bit #6[cl]: E1 desmarcado)
	call rot_t0		; rotina associada ao pause (ret)
	jmp .saida1		; sai



; executa comandos diretamente atraves da combinacao ctrldir+alt/shift+tecla
.busca_com:    			; test al, 0x80		; ok
	;jz .saida2		; se #7[indice]=0 sai
	call rot_aux_cd		; senão busca o numero do comando
	or al, al
	jz .saida1
	cmp al, 0xd0
	jb .f
	sub al, 0xd0
	cmp al, 0x07		; maximo de 8 rotinas
	ja .saida1
	shl ax, 2
	mov ebx, eax
	call dword [ebx+r_predef_tc]	; executa rotina pre-definida
	jmp .saida1
.f:	call rot_tc.r8144
	jmp .saida1


; teclas especiais, dependente de E0
.tec_esp:	cmp al,byte 0x46		; Ctrl-break
	je .saida1
	ja .gkey
	cmp al,byte 0x37		; Shift Print Screen (print screen fica nos comandos especiais)
	je .w
	cmp al,byte 0x35		; KP /
	je .kp_trav
	cmp al,byte 0x1c		; KP enter
	je .kp_enter
	cmp al,byte 0x38		; alt direito
	je .r
	cmp al,byte 0x1d		; control direito
	je .q
; qualquer outro valor é descartado
	jmp .saida1
; outros códigos podem ser colocados aqui...multimidia e internet



; rotina para comandos especiais
.gkey:	cmp al, 0x53		; comandos maiores que 53
	ja ComDed
	mov ebx,dword TabCom_esp	; base de comandos especiais
	test cl,byte 0x10		; alt pressionado?
	je .m
	inc ebx			; sim: soma 13 na base. não: usa base normal
.m:  	sub eax,0x47
	shl eax,1
	add ebx,eax
	mov al,[ebx]		; busca o comando especial
.n:	xor ebx,ebx			; zera bx
	mov [r476], bx		; salva endereco indice
	mov byte [r47b], 0xcc	; futuramente rever para deixar somente C
	call rot_tc.r8144		; executa comando indexado por r474
	jmp .saida1		; sai

; escolhe entre enter, shift enter, alt enter
.kp_enter:
	or ch, byte 0x20		; enter numerico pressionado
	test cl, byte 0x80		; comando ativado?
	je .d			; 0= busca codigo para enter
	mov dl, byte 0x2a		;
	call rot_tc		; busca codigo comando 2a
	jmp .n

; control dir pressionada. marca modo comando.
.q:	or cx,word 0x4080		; marca #6[cl] e #7[ch]=comando válido, #7[r470]=1
.u:	and cl,byte 0xdf		; 1101 1111, desmarca #5[cl](E0)
	jmp .saida1		; sai

; alt direito pressionada
.r:	or cx,word 0x1010		; marca #4[cl] e #4[ch]
	jmp .u			; sai, desmarcando E0

; rotina para a tecla kp /
.kp_trav:		; digitação /
	test cl,byte 0x80
	je .d

; comando /
	mov dl,byte 0x38		; válido com ctrldir pressionado
.t:	call rot_tc 		; busca código da 2a digitação, r8098
	jmp .saida1

; rotina para a tecla print screen
.w:	test cl,byte 0x18
	jne .v
	jmp .saida1		;
.v:	mov dl,byte 0x36		;
	call rot_tc		;
	jmp .saida1		;

; o teclado recebeu resposta > 0xee
.com_rec:

	mov [r47f],al		; salva resposta recebida
	cmp al,byte 0xfe
	jne .a2
	or [r47f],byte 0x10		; marca FE recebido (falha)
	jmp .saida2

.a2:	cmp al,byte 0xfa 		; FA=
	jne .saida1  		; .b2
	or [KB_leds],byte 0x08	; marca FA recebido (ok)
	jmp .saida2

.b2:	cmp al,byte 0xf0		; F0=
	jne beep_t		; beep
	call rot_t0		; gancho para rotina se receber F0
	jne .saida2

beep_t:	call beep
	jmp Tec_int.saida3




;-----------------------------------------------------------------------------------------


; Comandos dedicados: multimidia, internet, ligar e desligar....calculadora.
align 4
ComDed:  	cmp al, byte 0x5B		; L_GUI RotDed0
	jne .o5c
	call [RotDed0]
	jmp Tec_int.saida1
.o5c:	cmp al, byte 0x5C		; R_GUI RotDed0
	jne .o5d
	call [RotDed0]
	jmp Tec_int.saida1
.o5d:	cmp al, byte 0x5D		; APPS RotDed1
	jne .o5f
	call [RotDed1]
	jmp Tec_int.saida1
.o5e:	cmp al, byte 0x5e		; power RotDed2
	jne .o5f
	call [RotDed2]
	jmp Tec_int.saida1
.o5f:	cmp al, byte 0x5f		; sleep RotDed3
	jne .o63
	call [RotDed3]
	jmp Tec_int.saida1
.o63:	cmp al, byte 0x63		; wake RotDed4
	jne Tec_int.saida1
	call [RotDed4]
	jmp Tec_int.saida1


;-----------------------------------------------------------------------------------------

; ROTINAS INDEXADAS:

align 4
rot_t1:	or cl,byte 0x08		; L_shift, marca bit #3[r470]
rot_t0:	retn
align 4
rot_t2:	or cx,word 0x0808		; R_shift,
	retn
align 4
rot_t3:  	or cl,byte 0x10		; alt
	retn
align 4
rot_t4:	btc cx,0
	ret
align 4
rot_t5:	btc cx,1
	ret
align 4
rot_t6:	btc cx,2
	ret
align 4	; rotina de break code para shift esquerdo
rot_t7:	and cl,byte 0xf7		; 1111 0111
	retn
align 4	; rotina de break code para shift direito
rot_t8:	and cx,0xf7f7		; 1111 0111 		shift
	retn
align 4	; rotina de break code para alt esquerdo
rot_t9:	and cx,0xefef		; 1110 1111		alt
	retn

; ajusta valor do ponteiro em bx, ok
align 4
r8000:	mov ebx,TabCar	; busca base da 1a tabela
	shl eax, 0x2
	add eax, edx
	sub eax, 0x5		; ajusta
	add ebx, eax
	test cl,byte 0x10		; alt=1?
	jne .b
	test cl,byte 0x04		; CL=1?
	jne .c
	test cl,byte 0x08		; SH=1
	je .a
	inc ebx
	retn
; CL=1, S=0
.c:	add ebx, 0x03		;
	test cl,byte 0x08		; A=1
	jne .a
; A=1
	inc ebx
.a:	retn
; CL=1
.b:	add ebx, 0x2		; bx=tab_num0+0x72=
r8020:	retn


;-----------------------------------------------------------------------------------------

align 4
rot_ta:	; busca caractere
	mov eax,edx		; dx=scancode r8098
	cmp al,byte 0x39 	;se for <39 busca caractere
	ja .CarNum		; busca caractere na tabela numerica
	test ch,byte 0x07	;
	jne .acento	; tem acento marcado???
.n:	call r8000	; ajusta ponteiro
.w:	mov al, [ebx]	; busca caractere e imprime
.m:	mov [r478],bx	; salva o ponteiro do caractere
	mov [KB_char],al	; salva o caractere digitado
	or byte[KB_leds],0x80	; marca caractere digitado....add maio/2006
	and ch, 0x3f	; desmarca comando válido e enter numérico (provisório)
	;call word [r472]	; al= caractere, ah=0, rotina imprime caractere.
.v:  	retn

; busca caractere com acento
.acento:	mov ebx,tab_acent-1	; base da tabela numérica, 8 tabelas (0x0D6E)
	test cl,byte 0x10	; alt=1?
	jne .b
	test cl,byte 0x04	; CL=1?
	jne .a
	test cl,byte 0x08	; SH=1
	jne .a
	add ebx, 0x1E	; 6 TABELAS COM 5 BYTES
; verifica a tecla pressionada...se for
.a:	cmp al,byte 0x1e	; a? tab 6
	je .h
	cmp al,byte 0x12	; e? tab 5
	je .g
	cmp al,byte 0x17	; i? tab 4
	je .f
	cmp al,byte 0x18	; o? tab 3
	je .e
	cmp al,byte 0x16	; u? tab 2
	je .d
	cmp al,byte 0x2e	; c  tab 1
	je .c
	cmp al,byte 0x31	; n  tab 1
	je .c
	cmp al,byte 0x15	; y  tab 1
	je .c
	cmp al,byte 0x39	; outro caractere
	je .j
.b:	and ch,byte 0xf8	; 11111000, zera o tipo de acento (6 ao todo)
	jmp .n		; ok imprime caractere do acento
.c:	add ebx, 0x05	; ý, -, -, ñ ÿ, ...6a tabela
.d:	add ebx, 0x05	; u: 5a tabela ú, ù, û, -, ü,
.e:	add ebx, 0x05	; o: 4a tabela ó, ò, ô, õ, ö,
.f:	add ebx, 0x05	; i: 3a tabela í, ì, î, -, ï,
.g:	add ebx, 0x05	; e: 2a tabela, é, è, ê, -, ë,
.h:	mov al,ch		; a: tabela base á, à, â, ã, ä,
	and al,byte 0x07
	add ebx,eax		; bx=bx+al(deslocamento= 0 a 7 )
	mov al,[ebx]	; busca o caractere com acento
	and ch,byte 0xf8	;
	cmp al,byte 0x00	; despreza se for zero
	je .i
	jmp .m		; salva e sai imprimindo

.i:	mov eax,edx		; se for zero busca o caractere sem acento
	jmp .n		; da mesma posicao

.j:	mov ebx, b_acent-1	; base?
	jmp .h		; (8095)


;-----------------------------------------------------------------------------------------

; busca caractere na tabela numerica
align 4
.CarNum:	sub al,byte 0x46	;
	cmp al,byte 0x00	; se for zero despreza
	jb .v
	cmp al,byte 0x0d	; se al>12 despreza (14 teclas numericas)
	ja .v
	mov ebx, TabNum	; base da tabela numerica
	dec eax
	shl eax, 3
	add ebx, eax
	test cl,byte 0x02	; NL=1?
	jne .r
	inc ebx
	test cl,byte 0x10	;
	jne .w		; busca caractere
	inc ebx
	test cl,byte 0x04
	jne .q
	test cl,byte 0x08
	jne .w
	inc ebx
	jmp .w
.q:	add ebx, 0x2
	test cl,byte 0x08
	jne .w
	inc ebx
	jmp .w
; NL=1
.r:	test cl,byte 0x10	; alt=1?
	jne .s
	test cl,byte 0x08	; sh=1
	je .w
	inc ebx		;
.s:	add ebx, 0x6	; sh=1: busca na tabela base + 60
	jmp .w		; sh=0, CL=0 busca na tabela base


;-----------------------------------------------------------------------------------------

align 4
; rot 0e, marca acento. ao presionar um acento ele é marcado e só.
rot_tb:   mov eax,edx
	and ch,byte 0xf8	; zera bit #0,1,2[r471]
	call r8000	; ajusta bx em funçao de CL, SH ou alt (80bf)
	mov al, [ebx]	; busca caractere do acento. cada tipo de acento tem um código de 1 a 5
	cmp al,byte 0x06	; maximo 5 tipos de acentos
	ja rot_ta
	or ch,al		; marca tipo de acento em r0471
.a:	retn
; a próxima digitação, se for um caractere imprimivel, verifica se existe acento marcado
; se for verdadeiro, então busca a tabela do acento correspondente.
; cada tipo de acento possui uma tabela.


;-----------------------------------------------------------------------------------------

align 4
rot_tc: 	; executa comando
	call rot_aux_cd
.r8144:	mov [r478], bx	; salva ponteiro do comando
	mov [KB_char], al	; salva o numero do comando
	or ch, 0x80	; marca comando válido....marca #7[r471]
	cmp al,0x00	; se for zero não faz nada
	je .b		;
.c:	;call word [r474]	; executa comando indexado
.b:	retn


rot_aux_cd:	; busca valor na tabela TabCom. retorna al com valor, ah=0
	mov ebx,TabCom	; base de comandos-1
	mov eax,edx		; dx = scancode
	shl eax, 1
	add eax, edx	; x3
	sub eax, 0x3
	test cl,byte 0x10	; alt=1, sim: usa a tabela da base
	jne .a
	add ebx, 1		; não: soma 58	(sh=0->2a tabela)
	test cl,byte 0x08	; testa sh=1
	jne .a		;
	add ebx, 1		; sh=1? sim: soma 58 (sh=1->3a tabela)
.a:	add ebx, eax
	mov al, [ebx]	; busca numero do comando. (numero de 0 a 50)
	xor ah,ah		; al = codigo de comando
	ret


;-----------------------------------------------------------------------------------------

align 4
modifica_led:
r8188:	and [KB_leds],byte 0xc7
	cli
	mov dl,al
	mov al,byte 0x20
	out byte 0x20,al
	mov ah,byte 0xed
	call r815c	; envia com. ED
	test dh,byte 0x20	; o teclado não respondeu
	jne .a
	mov ah,cl
	and ah,byte 0x07
	and dh,byte 0xc0
	or dh,ah
	call r815c	; envia leds
	test dh,byte 0x20
	je .b
.a:	mov ah,byte 0xf4	; keyboard enable
	call r815c
.b:	mov [KB_leds],dh
	mov al,dl
	sti
	retn


; envia comando para P60
align 4
r815c:	push ecx
	mov ecx, 0x0003
.d:	push ecx
	call r83ac		;
	mov al,ah			; envia ah -> P60
	out byte 0x60,al
	sti
	xor cx,cx
.a:	test [KB_leds],byte 0x18	; testa byte recebido e testa #3,4
	jne .b
	loop .a
.b:	cli
	pop ecx
	test [KB_leds],byte 0x08	;
	jne .c
	loop .d
	or dh,byte 0x20		; faz #5[dh]=1
.c:	pop ecx
	sti
	retn


; Aguarda ok para envio de comando, do teclado
align 4
r83ac:	cli
	xor ecx,ecx
.a:	in al,byte 0x64
	test al,byte 0x02
	loopne .a
	retn


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 ==============================================
;
; Inicia o rotina de escrita de caracteres
; IN: nothing
; OUT: nothing
align 4
init_Keyboard:
	mov bl, 0xff
	call kb_cmd
	mov dword [r472], rot_ta
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N X ==============================================
;
; Lê um byte do teclado (KOLIBRI - COPYRIGHT)
; IN:	nothing
; OUT:
;	al: byte lido


align 4
kb_read:

	push ecx edx
	mov cx,0xffff
.loop1:	in al,0x64
	test al,1
	jnz .a
	loop .loop1
	mov ah,1
	jmp .saida		; sai com erro 1
.a:	push cx
	mov cx,32
.loop2:	loop .loop2
	pop cx
	in al,0x60
	xor ah,ah
.saida:	pop edx ecx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N X ==============================================
;
; Lê um byte do teclado (KOLIBRI - COPYRIGHT)
; IN:	???
; OUT:	???

align 4
kb_write:		; escreve teclado

	push ecx edx
	mov dl,al
	mov cx,0xffff
.loop1:	in al,0x64
	test al,0x20
	jz .b
	loop .loop1		; aguarda sinal de pronto
	mov ah,1
	jmp .saida		; sai com erro 1
.b:	in al,0x60
	mov cx,0xffff
.loop2:	in al,0x64
	test al,2
	jz .e
	loop .loop2
	mov ah,1
	jmp .saida		; sai com erro
.e:	mov al,dl			; escreve conteudo dl
	out 0x60,al
	mov cx,0xffff
.d:	in  al,0x64
	test al,2
	jz .f
	loop .d
	mov ah,1
	jmp .saida
.f:	mov ah,8
.L4:	mov cx,0xffff
.L5:	in al,0x64
	test al,1
	jnz .g
	loop .L5
	dec ah
	jnz .L4
.g:	xor ah,ah
.saida:	pop edx ecx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N X ==============================================
;
; Lê um byte do teclado (KOLIBRI - COPYRIGHT)
; IN:	
;	bl: comando
; OUT:	
;	

kb_cmd:		; envia byte bl para porta 64

	mov cx,0xffff
.L1:	in al,0x64
	test al,2
	jz .a
	loop .L1
	jmp .error
	
.a:	mov al,bl
	out 0x64,al
	
	mov cx,0xffff
.L2:	in al,0x64
	test al,2
	jz .end
	loop .L2
	jmp .error
	
.end:	clc
	ret

.error:	stc
	ret
	

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N X ==============================================
;
; Dah um beep
; IN: 	nothing
; OUT:	nothing

align 4
beep:

	mov al,byte 0x20
	out byte 0x20,al
	mov bx,word 0x00d0
	mov cx,word 0x0030
	in al,byte 0x61
	mov ah,al
	and al,byte 0xfe
.c3:	push cx
	or al,byte 0x02
	out byte 0x61,al
	mov cx,bx
.a3:	loop .a3
	and al,byte 0xfd
	out byte 0x61,al
	mov cx,bx
.b3:	loop .b3
	pop cx
	loop .c3
	mov al,ah
	out byte 0x61,al
	stc
	ret


;-----------------------------------------------------------------------------------------

