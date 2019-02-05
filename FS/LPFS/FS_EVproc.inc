; Rotians de manipulação do espaço vazio no disco
; Autores: Clovis Bombardelli e Felipe Gustavo Bombardelli
; Usa Flat assembler


;-----------------------------------------------------------------------------------------
; mecanica do espaço vazio
; toda unidade parte com um unico segmento contendo toda a capacidade da unidade.
; ao ser ocupado com arquivos, os blocos vao sendo descontados do segmento EV existente
; ao ser liberado qualquer arquivo, um item equivalente em extensao é colocado na lista EV
; ordenada pela LBA
; se um item EV fica vazio entao ele é retirado da lista e os ponteiros reordenados
; se dois itens vazios ficam adjacentes então ambos sao reagrupados num só e o outro é
; retirado.
;-----------------------------------------------------------------------------------------
;
; Ocupa espaco na particao nivel = 1 (nao precisa indicar qual o item)
; Entrada:

;	ecx: ext do bloco
;	ebx: base do diretorio
;	ecx: espaco requerido em setores
; Saida:
;	cy=0 	OK
;         	eax: lba1
;        		edx: lba2
;		ecx: ext
;	cy=1	error - Falta de espaço livre
; Funcionando
align 4
ocupa_EV :	;							ok

	push ebx
	push esi
	call TakeDMpos		; ebx= DM position
	cmp word [ebx], assDM	; assinatura
	jne .error
.a:	call procura_EV		; esi: item com espaço suficiente
	jc .error
	call ocupa_EV_n0		; achou. ocupa (desconta a quantidade de setores)
	pop esi
	pop ebx
	ret
.error: 	stc
	pop esi
	pop ebx
	ret



;-----------------------------------------------------------------------------------------
; Procura e devolve o ponteiro do item que tem espaco suficiente
; Entrada:
;        ebx: diretorio mestre
;        ecx: espaço requerido
; Saida:
;        cy= 0 - esi: item com espaco suficiente
;        cy= 1 - falta de espaço no disco
; Funcionando- corre a lista em busca de um item com extensao maior ou igual ao requerido
align 4
procura_EV :	;							ok

	mov esi, [ebx+DM_ini_EV]
.a:	or esi, esi			; chegou ao fim da lista?
	jz .erro				; sim. cy=1 (nao achou bloco)
	cmp [ebx+esi+EV_ext], ecx		; tem tamanho suficiente???
	jae .fim
.b:	mov esi, [ebx+esi+EV_prox]		; nao. busca o proximo item
	jmp .a
.fim:	ret
.erro:	stc
	ret


; Verifica se existe espaco vazio no disco					ok
; Entrada:
;       ebx: diretorio
;       ecx: tamanho
; Saida:
;       cy =0 - esi: item com espaco suficiente
;       cy =1 - sem espaco no disco
; Funcionando -
; corre a lista de itens EV até encontrar um com tamanho suficiente
; equivalente a procura espaço vazio
align 4
verifica_EV :

  	push ebx
	call TakeDMpos		; ebx= DM position
	mov esi, [ebx+DM_ini_EV]	; pega o ponteiro inicial do espaço vazio
.a:	or esi, esi
	jz .erro
	cmp [ebx+esi+EV_ext], ecx	; tem tamanho suficiente???
	jae .fim
.b:	mov esi, [ebx+esi+EV_prox]
	jmp .a
.fim:	clc
	pop ebx
	ret
.erro:	stc
	pop ebx
	ret

; nao usada por enquanto


;-----------------------------------------------------------------------------------------
; Ocupa espaço da particao nivel = 0						ok
; Entrada:
;         ebx: diretorio mestre
;         ecx: extensão requerida
;       	esi: item EV a ser ocupado
; Saida:
; 	cy= 0 	OK
; 		eax: lba1 (base do bloco restante)
; 		edx: lba2 (base do bloco restante)
; Deve ser usado somente quando a extensao do bloco achado for maior ou igual a extensao
; requerida
align 4
ocupa_EV_n0 :
	mov eax, [ebx+esi+EV_lba1]	; Pega os dados (lba1, lba2 e desloca a ext)
	add [ebx+esi+EV_lba1], ecx	; lba1=lba1+extention
	adc dword [ebx+esi+EV_lba2], 0; colocar a soma do carry
	mov edx, [ebx+esi+EV_lba2]	;
	sub [ebx+esi+EV_ext], ecx	; subtrai a extensão
; se apos descontar o espaço o item tornar-se nulo, retirar da lista
	cmp word [ebx+esi+EV_ext], 0
	jnz .a
; libera item se o mesmo ficou nulo
	push ebp
	mov ebp, DM_ini_EV
	call ajusta_ponteiroR
	pop ebp
	call libera_item             	; testar

.a:	ret


;-----------------------------------------------------------------------------------------
; Desocupa espaço no disco        						ok
; Entrada:
;         ebx: diretorio mestre
;         eax: LBA1
;         edx: LBA2
;         ecx: extensão em setores
; Saida:
;         cy= 0 -> 	esi: base do novo item EV
;	    1 ->  error, LBA1 e LBA2 é maior que a particao
align 4
add_ItemEV:
int 1
	push ebp
	push esi
	push edi
	mov ebp, [ebx+DM_ini_EV]
.d:	cmp edx, [ebx+ebp+EV_lba2]
	jne .a
	mov edi, [ebx+ebp+EV_lba1]
	cmp eax, edi
	jb .b
	add edi, [ebx+esi+EV_ext]
	cmp eax, edi
	jz .c
.a:	mov ebp, [ebx+ebp+EV_prox]
	or ebp, ebp
	jnz .d
	stc
	jmp .end
; aloca item EV
.b:	push eax
	mov eax, extEV
;int 1
	call aloca_item		; esi= novo item para conter bloco liberado
;int 1

	jb .end
	mov eax, [ebx+ebp+EV_prox]
	mov [ebx+esi+EV_prox],ebp
	mov [ebx+esi+EV_ant], eax
	pop eax
;int 1
	mov ebp, DM_ini_EV
	call ajusta_ponteiroC
;int 1
	mov [ebx+esi+EV_lba1], eax	; (compilacão) lba1
	mov [ebx+esi+EV_lba2], edx	; (compilacão) lba2
	mov [ebx+esi+EV_ext], ecx	; (compilacão) ext
.end:	pop edi
	pop esi
	pop ebp
	ret
; consolida bloco anterior
.c:	add [ebx+edi+EV_ext], ecx
	jmp .end


;-----------------------------------------------------------------------------------------
; Procura e verifica se existe espaço vazio adjacente a lba + Ext			ok
; Entrada:
;	eax: lba1
;	ebx: diretorio mestre
;	ecx: Ext a liberar
;	dx: lba2
; Saida:
;	eax = extensão do item
;	edi = ponteiro do espaço vazio
;	se cy = 1, não existe espaço vazio adjacente para juntar
; Testar
align 4
verifica_proxEV :

	push edx
	add eax, ecx
	adc edx, 0		; se ocorre carry, soma em edx
	mov edi, [ebx+DM_ini_EV]
.a:	or edi, edi
	jz .error
	cmp dx, [ebx+edi+EV_lba2]
	jne .b
	cmp eax, [ebx+edi+EV_lba1]
	je .fim
	;ja .error			; se maior, nao adianta procurar bloco adj...
.b:	mov edi, [ebx+edi+EV_prox]
	jmp .a

.fim:	mov eax, [ebx+edi+EV_ext]	; eax=capacidade do bloco adjacente
	clc
	pop edx
	ret

.error: 	stc			; nao achou bloco adjacente livre
	pop edx
	ret

;-----------------------------------------------------------------------------------------
; Busca base DM
; Entrada:
;       	ebx: diretorio
align 4
TakeDMpos:							;	ok
	movzx ebx, byte [ebx+D_part]	; buscar na tab.partição
	dec ebx
	shl ebx, 5
	lea ebx, [ebx+TabelaPart]
	mov ebx, [ebx+Part_DMpos]	; ebx=posição DM
	ret


;=========================================================================================
; Rotinas descartadas (obsoletas)
; consolida espaço vazio
; IN:
; 	eax: lba1,
;	ecx: qtde setores a liberar,
;	edx: lba2,
;	esi: item a consolidar
; OUT:
;	cy=0 ok
;	cy=1 error - nao consolidado
;align 4
;Consolidate_proxEV:
;	push edi
;	push eax
;	call TakeDMpos		; ebx= DM position
;	call verifica_proxEV	; edi=item, eax=extensao
;	jb .a
;	add eax, ecx
;	mov [ebx+edi+EV_ext], eax
;	sub [ebx+edi+EV_lba1], ecx
;	sbb dword [ebx+edi+EV_lba2], 0
;.a:	pop eax			; retorna sempre a base do bloco
;	pop edi
;	ret

;-------------------------------------------------------------------------------
; verifica se existe espaço vazio adjacente anterior a eax
; Entrada:
;	eax: lba1
;	ebx: diretorio mestre
;	ecx: Ext
;	edx: lba2
;	esi: item EV
; Saida:
;       	edi = ponteiro do espaço vazio
; se cy = 1, entao o edi=0 e não existe espaço vazio adjacente para juntar
; como esta rotina deve ser usada depois de alocar um item EV, esi fica definido
; e pode ser usado para testar somente o item anterior ao inves de correr toda lista EV
; Testar
;align 4
;consolida_antEV :
;
;	push ebp
;
;.b:	mov ebp, [ebx+esi+EV_ant]
;	or ebp, ebp
;	jz .fim2
;
;	push ecx
;	push ebx
;
;	mov edi, [ebx+ebp+EV_lba1]
;	mov ecx, [ebx+ebp+EV_ext]
;	mov ebx, [ebx+ebp+EV_lba2]
;
;	add edi, ecx		; edi= edi+ext
;	adc ebx, 0
;
;	cmp edx, ebx
;	pop ebx
;	jne .fim
;	cmp eax, edi		; eax=lba(esi), edi= lba1(ant)+ext(ant)
;	jne .fim
;int 1
;	add [ebx+ebp+EV_ext], ecx	; consolida bloco ebp
;
;	call libera_item     	; libera item esi
;int 1
;	pop ecx
;	mov esi, edi		; esi aponta bloco consolidado
;	and edi, 0
;	stc
;	pop ebp
;	ret
;
;.fim:	pop ecx
;.fim2:	pop ebp
;	ret


;-----------------------------------------------------------------------------------------
; consolida blocos liberados adjacentes
; Entrada:
;         eax: lba1		(relativo a esi)
;       	ebx: diretorio mestre
;       	ecx: Ext		(relativo a esi)
;        	dx: lba2		(relativo a esi)
;       	esi: item do espaco vazio a ser verificado
; Saida:
;       	edi= 0 -> consolidou bloco
; Quase pronta - Verificar se existe a rotina apaga_item do espaco vazio
; bugs de compilacao
; Testar
;align 4
;junta_EV :
;
;	push edi
;	push ebp
;; verifica bloco adiante apontado por esi
;	mov edi, [ebx+esi+EV_prox]
;	or edi, edi
;	jz .b		; se edi for zero, olha para tras
;
;	push edx
;	push eax
;	add eax, ecx
;	adc edx, 0
;	cmp edx, [ebx+edi+EV_lba2]
;	jne .a
;	cmp ecx, [ebx+edi+EV_lba1]
;	jne .a
;	pop eax
;	pop edx
;
;	mov [ebx+edi+EV_lba1], eax
;	mov [ebx+edi+EV_lba2], edx
;	add ecx, [ebx+edi+EV_ext]
;	mov [ebx+edi+EV_ext], ecx
;
;	call libera_item 		; libera item esi
;	mov esi, edi		; esi= item agregado
;	and edi, 0		; edi=0...houve consolidação de blocos
;	jmp .b
;
;.a:	pop eax
;	pop edx
;
;; verifica bloco anterior
;.b:	mov ebp, [ebx+esi+EV_ant]
;	or ebp, ebp
;	jz .fim2
;
;	push ecx
;	push ebx
;
;	mov ebp, [ebx+ebp+EV_lba1]
;	mov ecx, [ebx+ebp+EV_ext]
;	mov ebx, [ebx+ebp+EV_lba2]
;
;	add edi, ecx
;	adc ebx, 0
;
;	cmp edx, ebx
;	jne .fim
;
;	cmp eax, edi		; eax=lba(esi), edi= lba1(ant)+ext(ant)
;	jne .fim
;
;	pop ebx
;	add [ebx+ebp+EV_ext], ecx	; consolida bloco ebp
;	call libera_item     	; libera item esi
;	pop ecx
;	and edi, 0
;.fim2:	pop ebp
;	pop edi
;	ret
;
;.fim:	pop ebx
;	pop ecx
;	pop ebp
;	pop edi
;	ret


;-----------------------------------------------------------------------------------------
;align 4
;add_item_EV :
;
;	push ebp
;	push eax
;	mov eax, extEV
;	call aloca_item		; esi= novo item (cria item para conter o novo bloco liberado)
;	jb .erro
;	pop eax
;	mov edi, [ebx+DM_ini_EV]	; edi= item inicial
;	or edi, edi
;	jz .erro
;	mov [ebx+esi+EV_prox], edi
;	mov ebp, DM_ini_EV
;	call ajusta_ponteiroC	; arruma os ponteiros na lista (nao ajusta pela ordem)
;int 1
;	call consolida_antEV
;	or edi, edi
;	jz .fim
;	mov [ebx+esi+EV_lba1], eax	; (compilacão) lba1
;	mov [ebx+esi+EV_lba2], edx	; (compilacão) lba2
;	mov [ebx+esi+EV_ext], ecx	; (compilacão) ext
;.erro:
;.fim:	pop ebp
;	ret
