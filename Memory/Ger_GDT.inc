; Gerenciador da GDT, Dez/2005
; Autor: Clovis Bombardelli
; Usa: Fasm Assembler
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; INFORMAÇOES:
; Rotinas: aloca TSS, aloca descritor de dados...
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; ALOCA DESCRITOR NA GDT
; Entrada:
;	bl= tipo de descritor, bh= 00 -> #16, 0x40 -> #32
; 	eax= extensao do segmento
;	esi= base  do segmento
; Saida:
;	bx= seletor alocado
;	cy=0 - OK
;	cy=1 - falta de espaço no heap#16 ou falta de espaço na gdt

align 2
aloca_desc :		; (PARA USAR EM ROTINAS #16)
	push eax
	push ebx

	; busca base fisica
	push eax
	mov ebx, ds
	;and bl, 0xff		; COLOCAR 0xFA -> permite escrita e leitura
	add ebx, b_gdt
	mov al, [fs: ebx+04]
	mov ah, [fs: ebx+07]	;
	shl eax, 0x10
	mov ax, [fs: ebx+02]	;
	add esi, eax		; soma com base atual
	pop eax

	; procura descritor vazio
	mov ebx, b_gdt+8		; inicio da GDT alocavel
.a: 	cmp byte [fs: bx+5],00
	je .c			; encontrou posiçao vazia
	add ebx,08		; nao, continua procurando
	cmp ebx, e_prg_gdt
	jne .a
	pop ebx
	stc			; sai com erro
	pop eax
	retn

	; configura
.b:	shr eax,0x0c
	or eax,0x800000
	dec eax
	mov [fs: ebx],ax		; limite (16 bits)
	shr eax,0x10
	;or al, bh		; insere #32
	jmp .e
.c:	cmp eax,0x100000		; maior que 1M
	ja .b
.d:	dec eax
	mov [fs: ebx],ax		; limite (16 bits)
	shr eax,0x10
.e:	mov byte [fs: ebx+6],al		; +
	mov eax,esi
	mov [fs: ebx+2],ax
	shr eax,0x10
	mov [fs: ebx+4],al
	mov [fs: ebx+7],ah
	pop eax
	and ah, 0xf0
	or [fs: ebx+6],ah
	mov [fs: ebx+5],al
	shr al,5
	and al,3
	or bl,al			; ajusta nivel 0, 1, 2 ou 3
	pop eax
	sub ebx, [fs: virt+e_virtual] ; ajusta endereco
	ret


;-----------------------------------------------------------------------------------------
;
; LIBERA TSS E DESCRITOR
; Entrada:
; 	ax= seletor do TSS
; Saida:
; 	nada
align 2
libera_tssD :
	add eax, [fs: virt+e_virtual] ; ajusta endereco
r7884:	push ebx
	mov ebx, b_gdt
	add ebx, eax
	push si
	cli
	mov si,[fs: ebx+02]		; si= base do tss no heap#16

	; Habiltar (compilaçao) erro
	;call libera_tssD_bloco	; libera bloco no heap#16

	jb .78a6
	mov byte [fs: ebx+05],00	; libera descritor na GDT
	mov ebx,[fs: si+0x50]	; pega seletor da pilha
	mov byte [fs: ebx+05],00	; libera descritor da pilha
.78a6:	sti
	pop si
	pop ebx
	ret


;-----------------------------------------------------------------------------------------
;
; ALOCA TSS E PILHA NO HEAP#16 E FIXA OS DESCRITORES PARA AMBOS (TSS E PILHA)
; Entrada:
;	nada
; Saida:
;	si= base TSS alocado no heap#16,
;	bx= descritor TSS,
;	ax= 0x180 (extensão do bloco)

align 2
aloca_TSS :
r78ac:	push es
	mov ax,fs
	mov es,ax
	mov ax,0x0180		; extensao do bloco alocado
	call aloca_bloco		; aloca bloco
	jb .78e9
;	push si
;	push cx
;	push di
;	mov di, Tss_Padrao		; 1e80		; base do tss padrao
;	xchg si,di
;	mov cx,0x34
;	gs repe movsw		; transfere tss padrao
;	pop di
;	pop cx
;	pop si
; aloca descritores...
	mov ebx, b_gdt+8
.78d4:	cmp byte [fs: ebx+05],00
	je .78ec
.a:	add ebx, 8
	cmp ebx, e_prg_gdt		; compara com fim da regiao alocavel
	jb .78d4
.78e4:	call libera_bloco		; libera bloco
	stc
.78e9:	pop es
	retn			; saida com erro. Nao encontrou espaco na gdt
; define descritor para o tss
.78ec:	mov [fs: ebx+02],si
	mov byte [fs:ebx+04],0
	mov word [fs: ebx+06],0
	mov word [fs: ebx],0x67
	push ebx
; procura novo local para descritor da pilha..seguido ao descritor TSS
.7902:	add ebx,08
	cmp ebx, e_prg_gdt		; 0800
	je .b			; ????
	cmp byte [FS: ebx+05],00
	jne .7902
; define descritor para a pilha (ss)
	mov word [ebx],0xff67	; ext
	mov byte [ebx+04],0xff	; base
	mov word [ebx+06],0xff00
	mov byte [ebx+05],0x96	; tipo
	mov [ebx+02],si		; base
	add [ebx+02],ax		; base
	mov eax, ebx
	sub eax, [virt+e_virtual] ; ajusta valor do descritor
	mov [si+0x50],eax	; salva seletor SS no TSS
	pop ebx
	mov byte [ebx+05],0x89	; tipo de descritor
	sub ebx, [virt+e_virtual] ; ajusta valor do descritor
	jmp .78e9
.b:	pop ebx
	jmp .78e4
	ret


;-----------------------------------------------------------------------------------------
;
; ALOCA TSS E 2 PILHAS (N0 E N3) NO HEAP#16 E DESCRITORES PARA TSS E PILHAS
; Entrada:
;	nada
; Saida:
;	si= base TSS alocado no heap#16,
;	bx= descritor TSS,
;	ax= 0x280 (extensão do bloco)

align 4
aloca_TSS2P :		; (PARA USAR EM ROTINAS #16)
	push es
	mov ax,fs
	mov es,ax
	mov ax,0x280		; extensao do bloco alocado
	call aloca_bloco		; aloca bloco
	jb .78e9
;	push si
;	push cx
;	push di
;	mov di, Tss_Padrao		; 1e80		; base do tss padrao
;	xchg si,di
;	mov cx,0x1a
;	gs repe movsd		; transfere tss padrao
;	pop di
;	pop cx
;	pop si

	; aloca descritores
	mov ebx, b_gdt+8
.78d4:	cmp byte [ebx+05],00
	je .78ec
.a:	add ebx, 8
	cmp ebx, e_prg_gdt		; compara com fim da regiao alocavel
	jb .78d4
.78e4:	call libera_bloco		; libera bloco
	stc
.78e9:	pop es
	retn			; saida com erro. Nao encontrou espaco na gdt

	; define descritor para o tss
.78ec:	mov [ebx+02],si		; base
	mov byte [ebx+04],0	; 0
	mov word [ebx+06],0	; 0
	mov word [ebx],0x67	; limite
	push ebx

	; procura novo local para descritor da pilha..seguido ao descritor TSS
.7902:	add ebx,08
	cmp ebx, e_prg_gdt		; 0800
	je .b			; ????
	cmp byte [ebx+05],00
	jne .7902

	; define descritor para a pilha (ss) Nivel 0
	mov word [ebx],0xfdff-(0x80-0x68); ext
	mov byte [ebx+04],0xff	; base
	mov word [ebx+06],0xff00
	mov byte [ebx+05],0x96	; tipo
	mov [fs: ebx+02],si		; base
	add [fs: ebx+02],ax		; base
	mov eax, ebx
	sub eax, [virt+e_virtual] ; ajusta valor do descritor
;	mov [fs: si+0x8],eax	; salva seletor SS0 no TSS
;	mov word [fs: si+0x4], 0xfffe	; fixa esp

	; Outra pilha, nivel 3
	add ebx,08
	cmp ebx, e_prg_gdt		; 0800
	je .b			; ????
	cmp byte [FS: ebx+05],00
	jne .7902

	; define descritor para a pilha N3
	mov word [ebx],0xfeff	; ext
	mov byte [ebx+04],0xff	; base
	mov word [ebx+06],0xff00
	mov byte [ebx+05],0xf6	; tipo
	mov [fs: ebx+02],si		; base
	add word [ebx+02],0x380	; ax
	sub ebx, [virt+e_virtual] ; ajusta valor do descritor
	or ebx, 3			; nivel 3
	mov [fs: si+0x50],ebx	; salva seletor SS3 no TSS

	; define tss
	pop ebx
	mov byte [ebx+05],0x89 	; descritor tss
	sub ebx, [virt+e_virtual] ; ajusta valor do descritor
	mov [fs: si+0x8],eax	; salva seletor SS0 no TSS
	jmp .78e9
.b:	pop ebx
	jmp .78e4
	ret


;-----------------------------------------------------------------------------------------
;
; ALTERA A EXTENSAO DO DESCRITOR
; Entrada:
;	ecx= nova extensao
;	bx= seletor
; Saida:
;	ax=seletor da GDT
; Para ler a extensao do descritor usar a instrucao lsl reg, seletor
align 2
altera_ext_descritor :
r7944:	test bl,04		;
	jne r797c
r7949:	push ebx
	push ecx
	add ebx, b_gdt		; o descritor deve ser válido
	cmp ecx,0x100000
	jna .7965
	shr ecx,0x0c
	or ecx,0x800000
.7965:	dec ecx
	and bl,0xf8
	mov [bx],cx
	shr ecx,0x10
	and byte [ebx+06],0x70
	or [fs: ebx+06],cl
	pop ecx
	pop ebx
	retn
r797c:	call t_nula		; gancho para alterar extensao na ldt atual
	ret


;-----------------------------------------------------------------------------------------
;
; RETORNA A EXTENSAO DO SEGMENTO APONTADO NO DESCRITOR. USAR A INSTRUÇAO LSL...
; Entrada:
;	nada
; Saida:
; 	eax= extensao
align 2
busca_ext_descritor :
r7980:	test al,04
	jne .7e00				; r79a8
	push ebx
	mov ebx,eax
	and bl,0xf8
	mov al,[fs: ebx+06]
	and al,0x0f
	shl eax,0x10
	mov ax,[ebx]
	test byte [ebx+06],0x80
	je .79a5
	shl eax,0x0c
.79a5:	pop ds
	pop ebx
	retn

.7e00:	push ecx
	push edi
	mov ecx,[edi-04]
.7e09:	cmp byte [edi+05],0
	je .7e2c
	add edi,08
	cmp edi,ecx
	jb .7e09
	stc
	pop edi
	pop ecx
	retn
.7e20:	shr eax,0x0c
	or eax,0x800000
	jmp .7e34
.7e2c:	cmp eax,0x1000
	ja .7e20
.7e34:	mov [fs: edi],ax
	shr eax,0x10
	mov [edi+06],al
	mov [edi+05],bl
	mov ebx,esi
	mov [fs: edi+02],ebx
	shr ebx,0x10
	mov [edi+04],bl
	mov [edi+07],bh
	mov ebx,edi
	pop edi
	sub ebx,edi
	shl ebx,08
	or bl,04
	pop ecx
	ret


;-----------------------------------------------------------------------------------------
;
; ELIMINA DESCRITOR...ZERA BYTE TIPO DO DESCRITOR
; Entrada:
;	ebx= descritor
; Saida:
;	nada
align 2
libera_descritor :		; (exemplo)
;r79ec:	and bl, 0xf8		; filtra bits TI+RPL do descritor
;	add ebx, b_gdt		; soma a base da GDT
;	mov byte [fs: ebx+05],0	; marca tipo = 0
	ret


;-----------------------------------------------------------------------------------------
;
; OBTEM BASE DO DESCRITOR DS
; Entrada:
; 	DS= descritor
; Saida:
;	eax= base do descritor DS
; o descritor DS deve ter nível de acesso 3
align 4

retorna_b_descritorDS :	; (PARA USAR EM ROTINAS #16)
r61b8:	push ebx
	mov ebx, ds
	and bl, 0xf8
	add ebx, b_gdt
	mov al, [ebx+04]
	mov ah, [ebx+07]	;
	shl eax, 0x10
	mov ax, [ebx+02]	;
	pop ebx
	ret


;-----------------------------------------------------------------------------------------
;
; BUSCA BASE DE UM DESCRITOR APONTADO POR BX
; Entrada:
; 	ebx= descritor
; Saida:
; 	eax=base do descritor.
; Altera:
;	eax
align 4
RetornaBaseDescritorBX :	; esta rotina esta OK (maio/2006)
	push cx			;
	and bl, 0xf8		; zera RPL
	add ebx, b_gdt
	mov al, [ebx+04]
	mov ah, [ebx+07]	;
	shl eax, 0x10
	mov ax, [ebx+02]	;
	mov cl, [ebx+5]
	xor ebx, ebx
	mov bl, cl
	pop cx			; bx= retorna o tipo do descritor
	ret			; eax= base do descritor


;-----------------------------------------------------------------------------------------

