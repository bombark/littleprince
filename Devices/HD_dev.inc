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
; @Id: HD_dev.inc  - 2006-3-20, cb-toledo
; Driver for HD units (PIO mode - support to CHS, LBA & LBA extended devices)
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------


; definiçoes das posiçoes dos buffers para unidades IDE (variaveis)

;         si+0     dd- eax- lba3, lba2, lba1, lba0	cabeça, trilha, setor
;	si+4     dw- lba ext ()
;	si+6     db- x
; 	si+7     db- ATAFeatures
;         si+8     dd- edi= end. mem
;         si+c     dw- si- seletor para es:		; nao usado
;         si+e     dw- tarefa solicitante
;         si+10    dd- cx- quant. setores
;         si+14    dw- q. trilhas da unidade-------------cte-----------------------------
;	si+16    dw- q.setores/q. cabeças -------------cte-----------------------------
;	si+18    dd- q. total de setores da unidade----cte-----------------------------
;	si+1c    db- modo de endereçamento 1110 0000-flags de controle
;	si+1d    db- codigo de erro da unidade
;	si+1e    db- unidade -
;	si+1f    db- serviço - se não tem serviço a unidade está liberada

; ERROS: retornam em BL quando ocorrem
;
; 01- A unidade não estava pronta após a inicializaçao. FATAL- OK;
; 02- A unidade não respondeu com intFD ou intHD após solicitaçao do serviço. FATAL;
; 03- A inicializaçao não pode ser completada. FATAL;
; 04- A calibraçao não foi completada. FATAL;
; 05- Erro na busca após 2 tentativas. 3x em HD - OK;
; 06- A unidade não estava pronta para o envio da palavra de comando. FATAL;
; 07- Falha na controladora;
; 08- Não há disco na unidade;
; 09- Midia estranha ou desconhecida. Ocorre quando o disco não pertence ao sistema;
; 0a- X (setor escondido inválido);
; 0b- Disco trocado. A unidade foi aberta e o disco trocado;
; 0c- Disco protegido contra gravação indevida;
; 0d- Disco protegido contra formataçao indevida;
; 0e- Disco de acervo impróprio;
; 0f- Disco não formatado;
; 10- Setor não encontrado. Problema na identificaçao de trilha;
; 11- Setor não encontrado. Trilha defeituosa
; 12- Setor não encontrado. Setor acima do permitido na trilha;
; 13- Setor inexistente. O endereço não existe.
; 14- Setor marcado como ruim;
; 15- Setor não encontrado;
; 16- Erro não classificado;
; 17- Midia não identificada;
; 18- Falha na gravação;
; 19- Corrigido erro de dados ECC;
; 1a- CRC ou ECC inválidos;
; 1b- X
; 1c- Disco cheio. Atingido final do disco;
; 1d- Comando inválido;
; 1e- Unidade off-line;
; 1f- Unidade inexistente;


; definicao de si+6
; 	#0-	encriptacao ativada
;	#1-	modo lba estendido
;	#2-
;         
; Manipulador HDFD (entrada)
;         entrada:  eax= setor linear
;		bx= bh=unidade/bl=serviço (80,81,82,83 ou 00,01,02 ou 03);
;		cx= quantidade de setores a ser manipulados
;		dx= x
;		esi= x
;		edi= posiçao de memoria

;         saida:    cy=0 OK
;                   cy=1 erro em bl


;DrvWaitTime	equ 0x1000


align 4
discoHDFD:

          test bh,0x80	; hd: 80, 81, 82 e 83
          jz .erro1f	; se for zero buscar em disquete (desviado para erro 1f)
          cmp bl, 0xb	; valida serviço
          ja .erro1d
          cmp bh,0x83
	jbe HDD_Enter	; se nao tem erro...manipulador HD

; se não acha a unidade marca o erro com o código 0x1f para indicar unidade inexistente
; o codigo de erro fica armazenado na posicao 0x1d da tabela

.erro1f:	mov bl, 0x1f		; unidade inexistente
.erro:	push esi			; o erro deve vir definido em bl
          push ebx
          xchg bl,bh		; fica bl = unidade, bh= serviço
          mov esi,ebx		; si aponta o buffer de dados da unidade escolhida
	and esi, 3
          shl esi, 5
          lea esi,[esi+baseHDD or OS_BASE]	; na saida fica:
	xchg bl,bh		; bh = unidade, bl = codigo de erro
	pop ebx			; eax = inalterado
	mov [esi+0x1d], bl		; si = inalterado
	mov ebp, ebx
	pop esi			; edi = inalterado
	stc			; cy = 1
	ret



;-----------------------------------------------------------------------------------------

.erro1d:
	mov bl, 0x1d		; comando inválido
	jmp .erro



;-----------------------------------------------------------------------------------------

; colocar o comando de encriptacao em dl=1
; Manipulador HD (entrada)
;         entrada:  eax= setor linear
;		bx= unidade/serviço
;		cx= quantidade de setores a ser manipulados
;		dx= x
;		si= seletor de segmento
;		edi= posiçao de memoria

;         saida:    cy=0 OK
;                   cy=1 erro em AH

; se a tabela não tem dados o programa trava...

recicloHD:	;call troca_tarefa		; passa o controle para a tarefa seguinte se existir
		jmp HDD_Enter.a		; colocar um contador


p_ide0	equ 0x1f0
p_ide1	equ 0x170

align 4
HDD_Enter:
;int 1
          push edx
          push esi			; seleciona unidade, quatro unidades no máximo
          xchg bl,bh		; fica bl = unidade, bh= serviço
          mov esi,ebx		; si aponta o buffer de dados da unidade escolhida
	test bl, 2
	jnz .b
	mov edx, p_ide0		; porta basica 0x1f0
	jmp .c
.b:	mov edx, p_ide1		; porta basica 0x170
.c:	and esi, 3
	shl esi, 5
	lea esi,[esi+baseHDD or OS_BASE]

; verifica se a unidade existe
	test byte [esi+0x1c], 0x20
	jz .erro1f		; Nao existe tabela definida

; verifica se a unidade está ocupada
.a:	test byte [esi+0x1f],0xff	; se existe serviço a unidade está ocupada
	jnz recicloHD

	mov [esi+0x1f], bh		; Salva serviço solicitado
          push eax
          test byte [esi+0x1c], 0x40	; testa se o modo é lba
          jnz .a2
	call calculaTCS
	jmp .a3			; TEMPORARIAMENTE DESATIVA
; verifica se disco comporta lba...converte chs se necessário e salva corretamente
.a2:	cmp eax, [esi+0x18]
	ja .erro1c		; erro: fim do disco

.a3:	mov [esi],eax		; end linear do setor
.cc2b:	mov [esi+8],edi		; endereçamento na memoria
          ;str [esi+0xe]		; salva a tarefa solicitante
	mov [esi+0x10],cx		; quantidade de setores (extensao)
	mov [esi+0x1e],bx		; salva o unidade e serviço requisitado
	mov ecx, 3		; tres tentativas
.cd05:	push ecx
	mov bl,[esi+0x1f]		; bl= serviço requisitado
	and ebx,0x1f		; registro de unidade perdido
	shl ebx,0x2
; escolhe entre chs/lba ou lba_est
          test byte [esi+6], 2
          jnz .modo_lba_estendido
          call dword [ebx+servicos_HD or OS_BASE]
; executa rotina de serviço solicitada
          jb .retorno_com_erro
.r:	pop ecx
	xor bl,bl			; bl= zero...tudo OK
	mov bh, [esi+0x1e]		; repoe unidade
	mov ecx, [esi+0x10]
	pop eax			; aponta o setor chamado...conflita com pop word [fs: si+0xc]

.fim:     mov [esi+0x1d],bl       	; salva código de erro. 00 se OK...aqui
          mov byte [esi+0x1f],0	; libera a unidade, zerando o serviço
          pop esi
          pop edx
	xor ebp,ebp

; sair com os dados de entrada...eax setor+1, bh=unid, bl=00, si=inalt, edi=pos.ini+200
          ret			; saida sem erros: cy=0


; condições de erro:

align 4
.retorno_com_erro:
          pop ecx
          dec ecx
          jz .erro			;(sai com erro já indicado)
          call iniciaHD		; ?
          jb .erro			;
          and dl, 0xf0		; dx = 1x0 basica
          jmp .cd05


.erro1f:
;int 1
	xchg bl,bh
	mov bl, 0x1f		; tabela inexistente
	pop esi
	pop edx
	mov ebp, ebx
	stc
	ret

.erro1c:	mov bl, 0x1c		; disco cheio. Atingido final do disco;
.erro:	mov bh, [esi+0x1e]		; repoe unidade
	mov [esi+0x1d],bl       	; salva código de erro. 00 se OK...aqui
          mov byte [esi+0x1f],0	; libera a unidade, zerando o serviço
;int 1
	mov ebp, ebx

	pop eax
	pop esi
	pop edx
	stc
	ret



.modo_lba_estendido:
	call dword [ebx+serv_HDlba_est]
          jb .retorno_com_erro
	jmp .r


;-----------------------------------------------------------------------------------------

align 4
calculaTCS:								;OK

          inc eax
          push edx

          mov edx, eax
          shr eax, 0x10
          xchg edx,eax
          push ecx
          xor ecx, ecx
          mov cl, [esi+0x17]		; quantidade de cabeças da setores
	div ecx
	push edx			; q_setores
	xor edx,edx
	inc eax
	mov cl, [esi+0x16]
	inc cl
	div ecx			; ax= trilhas, dx= cab

	dec dl			;
	xchg edx,eax		; al= cabeça inicial
.a:	ror eax, 0x10		;
	or eax, edx		; ax= trilha inicial
	rol eax, 8
	pop edx
	mov al,dl			; al=setor inicial
	pop ecx
	pop edx
	ret


;-----------------------------------------------------------------------------------------

; Verifica limites (limitada aos modos chs e lba simples)
align 4
verifica_limHD:
	test byte [esi+0x1c], 0x40
	jz .chs
; modo lba
.lba:	mov eax, [esi]
	inc eax
	cmp [esi+0x18],eax		; q. total de setores da unidade
	jb .erro
	mov [esi], eax
	ret
; modo chs
.chs:	mov eax,[esi]		; al=q. setores, ah= trilhaL
          inc al			; aumenta setor
          cmp [esi+0x17],al		; quantidade de setores por trilha
          jb .a			; se passou do limite
          mov [esi],al		; ah = trilhaL
          retn
.a:       mov al,0x01		; faz setor = 1
	mov [esi], al		; salva
          mov al,[esi+3]		; al= cabeças
          inc al
          cmp [esi+0x16],al		; quantidade de cabeças
          jb .b
          mov [esi+0x3],al		; eax(3)=cabeça
          retn
.b:       xor al,al			; zera cabeça
          mov [esi+0x03],al		; salva
          mov al, [esi+02]		; al=trilhaH
          xchg al,ah		; troca
          inc eax			; incrementa
          cmp [esi+0x14],eax		; quantidade de trilhas
          jb .erro
          mov [esi+1],eax		; salva
          retn
.erro:    mov bl,0x1C       		; erro: atingido final do disco
          stc
          retn


;-----------------------------------------------------------------------------------------

align 4
servicos_HD:
	dd hd_serv1.a	; 0- reservado
	dd hd_serv1	; 1- leitura de setor
	dd hd_serv2	; 2- gravaçao de setor
	dd hd_serv3	; 3- leitura de segmento
	dd hd_serv4	; 4- gravaçao de segmento
	dd hd_diag	; 5- auto-diagnostico
	dd hd_identifica	; 6- identifica
	dd iniciaHD	; 7- inicializa a unidade
;	dd hd-servc	; 8- res


;-----------------------------------------------------------------------------------------

; Nao existe o serviço 0 porque este é filtrado na entrada do portao.
; a encriptaçao usa o bit#7 [si+1e] (serviço)

; ent:	dx= porta basica
align 4
hd_serv1:		; serviço 1 - leitura de setor absoluto       (OK)

.c:	call IntHD_marca
	call EnviaDados_IDE		; envia parâmetros dx=1x7 ok
	mov al,0x20		; al = comando
.b:       out dx,al			; envia comando
	call aguarda_intHD		; aguarda intHD...verifica se intHDDrec=0
	jb .a
	call obtem_status		; analisa status e erros, dx=1x7
	jb .a
	mov ecx,0x0100
	and dl,0xf0		; coleta dx=1x0
	cld
	;cli
	repe insw
	;sti
	mov [esi+0x8],edi
.a:	retn


align 4
hd_serv2:		; serviço 2 - gravação de setor absoluto
          call IntHD_marca
          call EnviaDados_IDE
          mov ah, 0x30		; gravaçao
.b:       call envia_comG
          jb .a
          push esi
          mov esi,[esi+0x8]	; ds já vem marcado na entrada
          mov ecx,0x0100
          sub edx,0x07
          cld
          ;cli
          repe outsw		; transfere em 16 bits
          ;sti
          mov eax, esi
          pop esi
          mov [esi+0x08],eax
	mov edi, eax
          call aguarda_intHD
          jb .a
          add edx,0x07
          call obtem_status		; rotina conversora de erro
.a:       retn


align 4
hd_serv3:		; serviço 3 - leitura de segmento

.a:	call hd_serv1
	jb rfim
.d:       dec word [esi+0x10]
          je rfim
          call verifica_limHD
          jb rfim
          add edx,0x07
          call aguarda_ok
          jb rerro1
          and dl, 0xf0		; garante dx= porta basica
          jmp .a


align 4
hd_serv4:		; serviço 4 - gravaçao de segmento
.a:	call hd_serv2
          jb rfim
          dec word [esi+0x10]
          je rfim
          call verifica_limHD		; incrementa setor, cab, trilha
          jb rfim
          call aguarda_ok
          jb rerro1
          and dl, 0xf0		; garante dx= porta basica
          jmp .a
rerro1:   mov bl, 0x1c
rfim:     retn



align 4
hd_diag:		; serviço 5 - DIAGNÓSTICO					OK
	add edx, 7
	mov ah,0x90
          call envia_comL
          jb .b
          sub edx,06
          in al,dx		; le 1x1
          xor ah,ah
          cmp al,01
          jne .a
          retn
.a:     	mov bl,03
          stc
.b:    	retn



align 4
hd_identifica:		; serviço 6 - IDENTIFICA UNIDADE				OK
	add edx, 6
	mov al, [esi+0x1c]
.c:	out dx, al
	inc edx
	mov al,0xec		; comando 0XEC
	out dx, al
        	mov cx,0xffff
.a:	jcxz .erro1		; rotina de espera
        	in al,dx
        	test al,0x80   		; BSY- ocupado
        	loopnz .a
        	test al,1     		; ERRO
        	jnz .erroI
        	test al,0x08   		; DRQ
        	loopz .a
	les edi,[esi+0x8]
          mov ecx,0x0100		; possivel fazer coleta em #32
          and dl,0xf0		; coleta dx=1x0
          cld
          repe insw
          retn
.erro1:  	mov bl,01
          stc
.b:    	retn
.erroI:	mov bl,0xff		; erro a classificar
	stc
	ret



; ent:	dx= qualquer porta
;	si= base do buffer de dados da unidade
; sai:	dx= 1x7
align 4
iniciaHD: 	; REINICIALIZA A UNIDADE (nao depende de interrupçao)		OK
	test byte [esi+0x1e], 2	; ide0 ou ide1, libera interrupçao presa
	jnz .a			; o bit da interrupçao é usado pelas
.b:	and byte [intHDrec], 0x7f	; outras unidades para evitar sobreposi-
	jmp .c			; çao de uso. enquanto o bit ficar preso
.a:	and byte [intHDrec], 0xbf	; a outra unidade nao pode usar a mesma
.c:	and dl, 0xf0		; 1x0
        	add edx,6			; 1x6h
        	mov al,[esi+0x1c]
        	out dx,al			; A0 ou A1 -> P1x6h
        	mov al,8
        	inc edx
        	out dx,al			; 08 -> P1x7h
        	push ecx
        	mov ecx,0x80000
.a1:     	jcxz .erro1		; a unidade nao respondeu
        	in al,dx			; 1x7
        	test al,0x80		; unidade ocupada?
        	loopnz .a1
        	pop ecx
        	ret
.erro1: 	mov bl,1
	mov byte [esi+0x1d],bl	; unidade nao respondeu....fatal
        	pop ecx
        	ret



;-----------------------------------------------------------------------------------------

; envia comando e aguarda recebimento da INT HD
; ent:	ah= comando
;	dx= porta
align 4
envia_comL:	;

.b:	call IntHD_marca		; marca a espera para esta interrupçao
          mov al,ah			; al = comando
          out dx,al			; envia comando
	call aguarda_intHD		; aguarda intHD...verifica se intHDDrec=0 	T
.a:       retn			; sai com cy=1 se houverem erros



align 4
envia_comG:			; al= comando, dx= porta basica + 7,
.b:	call IntHD_marca		; marca a espera para esta interrupçao
	mov al, ah
	out dx,al			; envia comando
	mov ecx,0xff00
.a:	dec  ecx			; rotina de espera
	cmp  ecx,0
	je     .erro
	in      al,dx
	test    al,80h	; BSY
	jnz     .a
	test    al,1	; ERR
	jnz     .erro
	test    al,08h	; DRQ
	jnz      .ac
          mov bl, 0xaa
.erro:    stc
.ac:      retn		; saida com erro



;-----------------------------------------------------------------------------------------

align 4
IntHD_marca:		; (verifica se unidade é liberada para receber novo pedido)
   	test byte [esi+0x1e], 2		; pega IDE
	jnz .a
.b:	and byte [intHDrec], 0x7f		; zera int IDE0 em espera
	ret
.a:	and byte [intHDrec], 0xbf		; zera int IDE1 em espera
	ret



align 4
aguarda_intHD: 	; ambas IDE1 e IDE2

   	test byte [esi+0x1e], 2
   	push ecx				;
          mov ecx,0xffffff
	jnz .ide2

.ide1:
.a1:      test byte [intHDrec],0x80		; IDE1 ...int rec zera o bit de espera
          jnz .c1				; int IDE ocupada....trocar de tarefa
          loop .a1
.s:	mov bl, 2
          stc
.c1:      pop ecx
          retn

align 4
.ide2:
.a2:	test byte [intHDrec], 0x40	; IDE2... int recebida zera o bit
	jnz .c1				;
.b2:      in al,0x61
          test al,0x10
          je .b2
          loop .a2
	jmp .s


; Se intHDDrec=1 ou 2, sai, senao fica num loop controlado por cx (1000x). neste sai com cy=1

;-----------------------------------------------------------------------------------------

; rotina para interrupçao

align 4
IntIDE1:		 		;
	push eax
	or [intHDrec], byte 0x80 	; ativa bit7  #7- int 14, #6- int 15

          mov al,0x20
          out 0x20,al
          jmp .cbd5
.cbd5:    jmp .cbd7
.cbd7:    out 0xa0,al
          pop eax
          iretd

align 4
IntIDE2:				;
	push eax
   	or [intHDrec],byte 0x40	; ativa bit6

          mov al,0x20
          out 0x20,al
          jmp .cbd5
.cbd5:    jmp .cbd7
.cbd7:    out 0xa0,al
          pop eax
          iretd

; Não é usada quando é feita a monitoração do bit RDY da porta #1x7.

;-----------------------------------------------------------------------------------------

; Busca bytes de status							OK
; ent:	dx= porta de status (1x7)
align 4
obtem_status:
	in al,dx		 ; dx= 1x7

	mov bl,00            ;
          test al,0x01
          je .c

          mov bl,0x19          ; corrigido erro de dados de ECC
          test al,0x04
          jne .a
          mov bl,0x18          ; falha na gravaçao
          test al,0x20
          jne .a
          mov bl,0x01          ; A unidade não estava pronta
          test al,0x40
          je .a
          mov bl,0x05          ; falha na busca
          test al,0x10
          je .a

          sub dx,0x06	 ; dx=1x1---------------------------------
          in al,dx		 ;
          mov bl,0x13          ; marca de endereço não encontrada
          test al,0x01
          jne .b
          mov bl,0x1d          ; comando inválido
          test al,0x04
          jne .b
          mov bl,15            ; setor não encontrado
          test al,0x10         ;
          jne .b
          mov bl,0x1a          ; CRC ou ECC inválidos
          test al,0x40
          jne .b
          mov bl,0x05          ; falha na busca
          test al,0x02
          jne .b
          mov bl,0x14          ; setor marcado
          test al,0x80
          jne .b
          mov bl,0x16          ; erro não classificado
.b:    	add dx,0x06
.a:    	stc
.c:    	retn


;-----------------------------------------------------------------------------------------

; rotina de espera, aguarda #7[porta dx]=0 					OK
align 4
aguarda_ok:
	push ecx
          mov ecx,0x8000	;
.a:       in al,dx
          test al,0x80    	;
          je .c
.b:       in al,0x61
          test al,0x10
          je .b
          loop .a
          stc
.c:       pop ecx
          retn


;-----------------------------------------------------------------------------------------

; ent:	si= base do buffer de dados da unidade -				OK
;	dx= porta basica da unidade
; o comando vai na posição esi+0x1d

align 4	; geral...para todos os serviços desejados envolvendo Trilha-Cabeça-Setor ou LBA
EnviaDados_IDE:	; envia comando e sai posicionando o erro
        	mov al, [esi+0x1c]		; modo+driver
        	add dx,6    		;
        	out dx,al			; -> 1x6
        	inc dx			; 1x7
        	mov ecx, DrvWaitTime
.a:      	jcxz .erro1		; a unidade nao respondeu
        	in al,dx			; 1x7
        	test al,80h		; unidade ocupada?
        	loopnz .a
        	test al,08h		; DRQ
        	loopnz .a			; espera
        	;cli
        	and dx, 0xfff1		; dx= porta basica+1
        	mov al,[esi+0x7]		; ATA features..se for zero, pula
        	or al,al
        	jz .a1
        	out dx,al		; envia	; features para 1x2...pula se zero
.a1:     	inc dx			; 1x2h
        	mov al,1			; ATASectorCount....usar sempre 1
        	out dx,al		; envia
        	inc dx			; 1x3h
        	mov eax,[esi]		; ATASectorNumber (lba0)
        	out dx,al		; envia
        	inc dx			; 1x4h
; envia trilha
        	ror eax,8			; Trilhas_ATA	(lba1 e lba2)
        	out dx,al			; envia ambos simultaneamente
        	inc dx			; 1x5h
	ror eax, 8
        	out dx, al
        	inc dx			; 1x6h
; monta 0A+unid+cab
        	ror eax, 8
        	and al, 0x0f
        	or al,[esi+0x1c]		; modo+driver+cab novamente
; modo de endereçamento
        	out dx,al		; envia	; MODO+A0+Unid+Cab -> P1x6h
	inc dx
        	;mov al,[esi+0x1d]		; Envia Comando ATA (não desmarcar)
        	;out dx,al		; envia	; 1x7	(NÃO DESMARCAR)
        	;sti
.fim:    	ret			; #0,1,2,3= erro de operaçao
.erro1: 	or byte [esi+0x1d],2	; erro ocorrido no envio de dados
        	mov bl, 2
        	ret

; obs: se o modo de endereçamento for CHS, o bit lba deverá estar posicionado em zero. No
; modo lba, si+0 até si+3 deverao ter a quantidade de bytes a enviar para as portas 1f3,1f4
; e 1f5.



;-----------------------------------------------------------------------------------------

; rotinas para o modo lba estendido
align 4
serv_HDlba_est:
	dd hd_serv1.a	; reservado para informações
	dd hd_lba1	; leitura de setor
	dd hd_lba2	; gravaçao de setor
	dd hd_lba3	; leitura de segmento
	dd hd_lba4	; gravaçao de segmento
	dd hd_diag	; diagnostico
	dd hd_identifica	; identificação
	dd iniciaHD	; inicializa a unidade
;	dd hd-servc	; res
;	dd hd_servd	; res
;	dd hd_serve	; res

;-----------------------------------------------------------------------------------------
; Para usar no modo LBA estendido

align 4
EnviaDados_IDElba:	; envia comando e sai posicionando o erro
        	mov al, [esi+0x1c]		; modo+driver
        	add dx,6    		;
        	out dx,al			; -> 1x6
        	inc dx			; 1x7
        	mov ecx,0xfff
.a:      	jcxz .erro1		; a unidade nao respondeu
        	in al,dx			; 1x7
        	test al,80h		; unidade ocupada?
        	loopnz .a
        	test al,08h		; DRQ
        	loopnz .a			; espera
        	;cli
; envia ATA features
        	and dx, 0xfff1		; dx= porta basica+1
        	mov al,[esi+0x7]		; ATA features..se for zero, pula
        	;or al,al
        	;jz .a1
        	out dx,al		; envia	; features para 1x2...pula se zero
	jmp .e0
.e0:	out dx, al

; envia sector count
.a1:     	inc dx			; 1x2h
; estudar o envio multisetores

.c:      	mov al, 0
; ATASectorCount....usar sempre 1
.b:      	out dx,al			; envia sempre 1
	mov al, 1
	out dx, al
; lba address
        	inc dx			; 1x3h
        	mov eax,[esi]		; ATASectorNumber (lba0)
        	rol eax, 0x8
        	out dx,al			; envia word
        	inc dx			; 1x4h
        	mov ax, [esi+4]
        	out dx, ax
        	and dl, 0xf0
        	or dl, 3			; dx=1x3

        	mov eax,[esi]		; ATASectorNumber (lba0)
        	out dx, ax
        	inc dx			; ah=0
        	inc dx
        	ror eax, 0x10		;
        	out dx,al			;

        	inc dx			; 1x6h
; monta 0A+unid+cab
        	or al,[esi+0x1c]		; modo+driver+cab novamente
	and al, 0x50
; modo de endereçamento
        	out dx,al		; envia	; MODO+A0+Unid+Cab -> P1x6h
	inc dx
        	;sti
.fim:    	ret			; #0,1,2,3= erro de operaçao
.erro1: 	or byte [esi+0x1d],2	; erro ocorrido no envio de dados
        	ret



align 4
hd_lba1:		; serviço 1 - leitura de setor absoluto 			OK
          call IntHD_marca
          call EnviaDados_IDElba
          mov ah, 0x24
          jmp hd_serv1.b

align 4
hd_lba2:		; serviço 2 - gravação de setor absoluto
          call IntHD_marca
          call EnviaDados_IDElba
          mov al, 0x34
          jmp hd_serv2.b

align 4
hd_lba3:		; serviço 3 - leitura de segmento
.a:	call hd_lba1
          jb sfim
          dec word [esi+0x10]
          je sfim
          call verifica_limHD		; incrementa setor, cab, trilha
          jb sfim
          call aguarda_ok
          jb serro1
          and dl, 0xf0		; garante dx= porta basica
          jmp .a
serro1:   mov bl, 0x1c
sfim:     retn


align 4
hd_lba4:		; serviço 4 - gravação de segmento
.a:	call hd_lba2
          jb sfim
          dec word [esi+0x10]
          je sfim
          call verifica_limHD		; incrementa setor, cab, trilha
          jb sfim
          call aguarda_ok
          jb serro1
          and dl, 0xf0		; garante dx= porta basica
          jmp .a


;-----------------------------------------------------------------------------------------