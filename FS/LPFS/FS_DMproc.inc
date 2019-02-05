;-----------------------------------------------------------------------------------------

; Rotinas do Diretorio Mestre

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================
;
; TABELA DE PARTICOES (definições): ver variaveis.inc
;
; Part_ini	equ 0	; Inicio da particao		4 bytes
; Part_ext	equ 4	; Extensao da particao		4 bytes
; Part_lbaDM	equ 8	; LBA do diretorio mestre		4 bytes
; Part_extDM	equ 12	; Extensao do diretorio mestre	2 bytes
; Part_unidade	equ 14	; Unidade (01,80,81,...)		1 byte
; Part_tipo	equ 15	; Tipo (C5, FAT,...)		1 byte
; Part_DMpos	equ 16	; Local do diretorio mestre na memoria  4 byte
;
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Carrega diretório mestre no heap
; O diretório mestre de cada unidade é posicionado na montagem da tabela de dispositivos
; Quando o diretório é carregado para a memoria, a sua posição nela é salva na tabela,
; na posição base_partição+0x10
; Para acessar o diretório usar a variavel Part_DMpos somada à base da partição
; IN:
;	bl = partição (1, 2, 3, etc (max. 32))
; OUT:
;	cy=0 	ebx = endereço DM na memoria
;	cy=1 	erro

align 4
Load_DM :
	or bl,bl
	je .erro2
	push eax
	push ecx
	push esi
	push edi
	and ebx, 0x1f		; 32 partições possíveis
;
	push ebx
	dec ebx
	mov eax, ebx
	shl ebx, 5		; deve ser igual a ext_part (0x20)

; verifica a existencia na memoria

	mov edi, [TabelaPart+ebx+16]
	or edi,edi
	jne .fim			; se nao for zero, DM já se encontra na mem
	movzx eax, word [TabelaPart+ebx+12]	; ext DM (setores - LBA)
	shl eax, 9		; converte setores em bytes

; aloca bloco para o diretório mestre
	call aloca_HB		; ver Ger_memory.inc
	jb .erro1
;
; posiciona o local do diretorio mestre na memoria
	mov [TabelaPart+ebx+16], esi	; posição na memoria
	mov eax, [TabelaPart+ebx+8]	; endereço LBA do diretório mestre
	mov cx, [TabelaPart+ebx+12]	; extensão
	mov edi, esi
	pop ebx

; transfere do hd para a memoria

	call ReadSeg0	;leia	; ver ReadWriteDev.inc
	jc .erro

; define parametros
	mov [edi+2], bx		; DMpart
	mov dword [edi+4], 0	; DManterior----irrelevante...não existe dir ant
	mov dword [edi+8], 0	; DMitemDA  ----irrelevante...não existe item ant
	mov dword [edi+12], edi	; DMmestre  ----irrelevante...aponta ele mesmo
.fim:	mov ebx, edi		; ebx= pos memoria onde o diretorio está carregado

.saida:	pop edi
	pop esi
	pop ecx
	pop eax
	ret

.erro1:	pop ebx
.erro:	pop edi			; libera_bloco (fazer futuramente)
	pop esi
	pop ecx
	pop eax
.erro2:	ret



;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; posiciona Partição ativa (endereço do item na tabela de dispositivos)		OK
; IN:
;	bl= partição (1..32) - doesn't be zero
; OUT:
;	bl filtrado

align 4
DefineBootPartition:

	and ebx, 0x1f		; 32 partições possíveis
	mov [BootPartition], bl		; define a partição de boot (temp)
	push ebx
	dec ebx
	shl ebx, 5		; 0x20
	add ebx, TabelaPart
	mov [BootPartitionBase], ebx	; partition base on table of partitions
	pop ebx
	ret



;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
;
; Navegação direta dentro do diretório mestre da partiçao ativa
; varias são as possibilidades...ebx pode apontar o bloco base na tabela de partições ou
; a base na memoria do diretorio mestre.
;
; IN:
;	bl= partiçao
; OUT:
;	esi= endereço desejado (endereço relativo ao heap local do diretório)
;	edi= endereço de memoria

align 4
DM_PrimeiroItem:

	or ebx, ebx
	jz .erro
.a:	push ebx
	and ebx, 0x1f		; 32 partições possíveis
	dec ebx
	shl ebx, 5		; 0x20
	mov ebx, [ebx+Part_DMpos]	; posição na memoria
	or ebx, ebx
	je .erro			; se zero...retornar com erro
	mov esi, [ebx+DM_ini_item]
	mov [ebx+DM_atual_item], esi	; posiciona item atual (não necessário)- foi para tabela de dispositivos
	pop ebx
	ret
.erro:	stc			; se cy ...diretorio ausente na memoria
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
;
; IN:
; OUT:

align 4
DM_UltimoItem:

	or ebx, ebx
	jz .erro
.a:	push ebx
	and ebx, 0x1f		; 32 partições possíveis
	dec ebx
	shl ebx, 5		; 0x20
	mov ebx, [ebx+Part_DMpos]	; posição na memoria
	or ebx, ebx
	je .erro			; se zero...retornar com erro
	mov esi, [ebx+DM_topo_item]
	mov [ebx+DM_atual_item], esi	; posiciona item atual (não necessário)- foi para tabela de dispositivos
	pop ebx
	ret
.erro:	stc			; se cy ...diretorio ausente na memoria
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
;
; IN:
; OUT:

align 4
DM_ProximoItem:

	or ebx, ebx
	jz .erro
.a:	push ebx
	and ebx, 0x1f		; 32 partições possíveis
	dec ebx
	shl ebx, 5		; 0x20
	mov ebx, [ebx+Part_DMpos]	; posição na memoria
	or ebx, ebx
	je .erro			; se zero...retornar com erro
	mov esi, [ebx+DM_atual_item]
	mov esi, [ebx+esi]
	mov [ebx+DM_atual_item], esi	; posiciona item atual (não necessário)- foi para tabela de dispositivos
	pop ebx			; retorna proximo item se cy=0
	ret
.erro:	stc			; se cy ...diretorio ausente na memoria
	pop ebx			; retorna item atual se cy=1
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
;
; IN:
; OUT:

align 4
DM_AnteriorItem:

	or ebx, ebx
	jz .erro
.a:	push ebx
	and ebx, 0x1f		; 32 partições possíveis
	dec ebx
	shl ebx, 5		; 0x20
	mov ebx, [ebx+Part_DMpos]	; posição na memoria
	or ebx, ebx
	je .erro			; se zero...retornar com erro
	mov esi, [ebx+DM_atual_item]
	mov esi, [ebx+esi+4]
	mov [ebx+DM_atual_item], esi	; posiciona item atual (não necessário)- foi para tabela de dispositivos
	pop ebx			; retorna item anterior se cy=0
	ret
.erro:	stc			; se cy ...diretorio ausente na memoria
	pop ebx			; retorna item atual se cy=1
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Save DM
; IN:
;	bl= partition (0..32)
; OUT:
; 	DM_anterior, DM_itemDA, DM_mestre = 0

align 4
SaveDM:

	push esi
	push ebx
	and ebx, 0x1f	; maximum of 32 partitions
	dec ebx
	shl ebx, 5	; Multiplica o bx por 0x20
	mov eax, [TabelaPart+ebx+Part_lbaDM]
	movzx ecx, word [TabelaPart+ebx+Part_extDM]
	mov edi, [TabelaPart+ebx+Part_DMpos]	; point to DM memory base
	add eax, [TabelaPart+ebx+Part_ini]	; setores anteriores
	movzx ebp, byte [TabelaPart+ebx+0xf]
	mov bh, [TabelaPart+ebx+0xe]		; unidade
	and ebp, 0x0f
	mov bl, 4				; serviço de gravaçao de segmento
	shl ebp, 2
	call dword [Device_Table + ebp - 4]
	pop ebx
	pop esi
	ret				; error on ebp

; obs: busca o ponteiro do diretorio mestre na tabela da partiçao.


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
;
; IN:	ebx= diretorio atual
;
; OUT:	ebx= diretorio destino
;
align 4
LoadDM:		; nao usar busca em diretorio

	push eax
	mov al, [ebx+D_part]	; take al=partition of folder
	shl al, 5
	movzx ebx, al
	mov ebx, [ebx+Part_lbaDM] 	; LBA do diretorio mestre
	pop eax
	ret


;-----------------------------------------------------------------------------------------




;
; O usuario nao deve navegar até o diretorio mestre. por isso o diretorio admin deve ter
; o ponteiro de pasta superior zerada. colocar os ponteiros da pasta administrador e publica
; na tabela de partições se a senha for aceita, senão colocar apenas a pasta publica
;
;-----------------------------------------------------------------------------------------


; DESCARTE (rotinas obsoletas)

; cria item (arquivo ou o item da pasta)
; Entrada:
;       ax: tipo:
;       ebx: diretorio
;       esi= nome
;       cx: ext do nome
; Saida:
;       edi= item criado
; Funcionando
;align 4
;cria_itemDM :
;
;	push eax
;	push ecx
;	and ecx, 0xff
;	push eax
;	push ecx
;	mov eax, extItem
;  	push esi
;	call aloca_itemDM                ; colocar em edi e utilizar ES
;     	mov edi, esi
;     	pop esi
;	call ajusta_item_ponteirosDM
;	pop ecx
;	pop eax
;	mov word [ebx+edi+I_tipo], ax
;	mov byte [ebx+edi+I_ext_nome], cl
;	mov word [ebx+edi+I_continue], 0
;	push edi
;	push esi
;	mov eax, ecx
;	call aloca_itemDM
;	jc .fim ;.error
;	mov [ebx+edi+I_nome], esi
;	mov edi, esi
;	add edi, ebx
;	pop esi
;	push esi
;	call cpystr
;.fim:	pop esi
;	pop edi
;	pop ecx
;	pop eax
;	ret

;.error:   ;mov word [ebx], 0xaaaa
;	jmp .fim


;-----------------------------------------------------------------------------------------
;
; ajusta ponteiros para que a lista fique em ordem alfabetica
; entrada:
;       ebx= diretorio
;       esi= nome do item
;       edi= ponteiro do item
;       cx= ext do nome
; saida:
;       nada
; Funcionando
;align 4
;ajusta_item_ponteirosDM :
;
;	push eax
;	push edx
;	push esi
;	push edi
;	mov eax, edi
;	mov edi, [ebx+DM_ini_item]
;	and edx, 0
;.a:	or edi, edi
;	jz .b
;	push edi
;
;	mov edi, [ebx+edi+I_nome]
;	add edi, ebx        ; add Inome, ebx -> nao to lembrado (compilação)
;
;        	;call CmpStr32
;	pop edi
;        	js .b
;	mov edx, edi
;	mov edi, [ebx+edi+I_prox]
;	jmp .a
;
;.b:	mov esi, eax		  ; mov esi, eax
;	mov [ebx+eax+I_prox], edi
;	mov [ebx+eax+I_ant], edx
;	push ebp
;	mov ebp, DM_ini_item              ; ajusta o descritor da lista
;	call ajusta_ponteiroC
;	pop ebp
;
;.fim:	clc
;	pop edi
;	pop esi
;	pop edx
;	pop eax
;	ret



;-----------------------------------------------------------------------------------------
;
; Aloca item
; Entrada:
;	eax: tamanho
;	ebx: diretorio
; Saida:
;	esi: ponteiro do bloco no diretorio
; Funcionando
;align 4
;aloca_itemDM :
;
;	push eax
;	push ebx
;	push edx
;	push edi
;.a: 	mov edi, [ebx+DM_fim_heap]		; ver
;   	add edi, ebx
;	push ebx
;	add ebx, [ebx+DM_ini_heap]		; ver
;	call aloc32
;	pop ebx
;	jc .error
;	sub esi, ebx
;	clc
;.fim:	pop edi
;	pop edx
;	pop ebx
;	pop eax
;	ret
;
;.error:   ;push ax
;	;mov ax, [bx+DMext]   ; compilação extD
;	;add ax, 0x400
;	;call realoca_diretorio
;	;pop ax
;	stc
;	;jmp .a
;	jmp .fim
;


;-----------------------------------------------------------------------------------------
;
; Libera bloco de um item
; Entrada:
;	ebx: diretorio
;	esi: item
; Saida:
;	cy= 0 - OK
;	cy= 1 - error, ponteiro invalido!!!
; Funcionando
;align 4
;libera_itemDM :
;
;	push ebx
;	push edi
;	push ebp
;	push esi
;.a:	mov ebp,ebx
;	add esi, ebp
;	mov edi, [ebx+DM_fim_heap]
;	add edi, ebp
;	mov ebx, [ebx+DM_ini_heap]
;	add ebx,ebp
;	call lib32
;	jc .error
;	clc
;.fim:	pop esi
;	pop ebp
;	pop edi
;	pop ebx
;	ret
;
;.error:	stc
;	jmp .fim



;-----------------------------------------------------------------------------------------
;
; renomeia item (arquivo)

; Entrada:
;	ebx: diretorio
;	edi: item
;	esi= novo nome
;	cl: extensao do nome
; Saida:
;	Nenhuma (Unico erro que pode haver eh o nome ser maior que 256 bytes).
; Funcionando
;align 4
;renomeia_itemDM :
;
;	push ecx				; 1°
;	push esi
;	push edi
;
;	and ecx, 0xff
;
;	xchg esi, edi			; 2° - libera o bloco do nome
;	call libera_InomeDM
;	jc .erro
;	xchg esi, edi
;
;	push esi
;	mov eax, ecx
;	call aloca_itemDM
;	mov [ebx+edi+I_nome], esi
;	mov edi, esi
;	pop esi
;
;	add edi, ebx
;	call cpystr
;	call salva_diretorio
;	clc
;	pop edi
;	pop esi
;	pop ecx
;	ret
;
;.erro:	stc
;	pop edi
;	pop esi
;	pop ecx
;	ret


;-----------------------------------------------------------------------------------------
;
; Libera nome do item
; entrada:
;	es:ebx= diretorio
;	esi= ponteiro do item

; saida:
;	cy= 0 - OK
;	cy= 1 - Erro, ponteiro invalido
; Funcionando
;align 4
;libera_InomeDM :
;
;	push ebx
;	push edi
;	push ebp
;
;.a:	mov ebp, ebx
;	mov edi, [ebx+DM_fim_heap]
;	add edi, ebp
;
;	;mov eax, [es:ebx+esi+Iext_nome] Acho que não precisa saber a extensão do bloco
;	push esi
;	mov esi, [ebx+esi+I_nome]
;	or esi, esi
;	jz .fim
;	add esi, ebp
;	mov ebx, [ebx+DM_ini_heap]
;	add ebx, ebp
;	call lib32
;	pop esi
;	jc .error
;	mov byte [esi+ebp+I_ext_nome], 0
;	mov word [esi+ebp+I_nome], 0
;	clc
;.fim:
;	pop ebp
;	pop edi
;	pop ebx
;	ret
;
;.error:
;	stc
;	jmp .fim



;-----------------------------------------------------------------------------------------
