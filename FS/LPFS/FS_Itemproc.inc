;-----------------------------------------------------------------------------------------

; ROTINAS DE MANIPULAÇÃO DE ITENS

;-----------------------------------------------------------------------------------------


;======================== O B S E R V A C A O ============================================

; Tipos do item - nivel alto do tipo do item
;	01 -> Pasta
;	02 -> Executavel
;	03 -> Textos/binario
;	04 -> Imagem
;	05 -> Audio
;	06 -> Video

; Subtipo do item - nivel baixo do tipo do item
;	03 01 -> bmp
;	03 02 -> jpeg
;	03 03 -> gif
;	03 ...

;	04 01 -> wave
;	04 02 -> mp3
;	04 03 -> mp4
;	04 ...

;	05 01 -> mpeg
;	05 02 -> avi
;	05 ...

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Aloca item
; Entrada:
;	eax: tamanho
;	ebx: base do diretório
; Saida:
;	esi: ponteiro do bloco no diretorio
align 4
aloca_item :							;	OK
	push edi
.a: 	mov edi, [ebx+D_tam_heap] 	; tamanho do heap
	push ebx
	add ebx, [ebx+D_ini_heap]	; inicio do heap
	call aloc32		; aloca bloco no heap local do diretório
	pop ebx
	jc .e
	sub esi, ebx
	mov [ebx+D_topo_heap], esi	; posiciona topo do heap local
	clc
	pop edi
	ret

; realocaçao na memória
.e:	push eax
	push ecx
	mov eax, [ebx+D_ext]
	mov ecx, eax
	push ecx
	shl eax, 9
	add eax, DM_tam
	push ebx
	mov ebx, [ini_Heap]
	mov edi, [size_Heap]
	call real32		; esi= nova base
	pop ebx
	jb .error
	mov ebx, esi
	shr eax, 9
	mov ecx, eax
	mov [ebx+D_ext], eax
	mov eax, DM_tam
	add [ebx+D_tam_heap], eax	; tamanho do heap local
	mov edi, [ebx+DM_topo_heap]
	add edi, [ebx+edi-4]
	and edi, 0xfe
	add [ebx+edi], eax

; realocaçao no HD
	call ocupa_EV
	jb .error
	xchg [ebx+D_lba1], eax
	xchg [ebx+D_lba2], edx
	pop ecx
	call verifica_proxEV
	jnb .b
	call add_ItemEV
	jb .error

; salvar diretórios (DM ou dir+DM)..ajustar ponteiros antes de salvar
.b:	cmp word [ebx], 0xd0d0
	jnz .dir
; se for DM...atualiza ponteiro da base na tabela de partiçoes
	movzx eax, word [ebx+D_part]
	dec eax
	shl eax, 5
	lea eax,[eax+TabelaPart]
	mov [eax+Part_DMpos], ebx
	pop eax
	call SaveDM
	jmp .a			; retorna para alocar o item

.error:	pop eax
	pop edi
	ret
; se for outro diretório...atualiza ponteiros dos diretórios abertos e do diretório pai
.dir:     jmp .a			; incompleto (fazer mais tarde)
; 0- apos a realocaçao...
; 1- percorrer todos os itens do diretório para ver quais se encontram abertos
; 2- os arquivos abertos devem ser desprezados...
; 3- as sub-pastas abertas devem repor o novo ponteiro...
; 4- ajusta os ponteiros de cada sub-pasta aberta, salva a pasta e salva DM;

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Libera um espaço de um bloco						ok
; Entrada:
;	ebx: diretorio
;	esi: item
; Saida:
;	cy= 0 - OK
;	cy= 1 - error, ponteiro invalido!!!
; Testar
align 4
libera_item :

	push ebx
	push edi
	push ebp
	push esi
.a:	mov ebp,ebx
	add esi, ebp
	mov edi, [ebx+D_tam_heap]
	mov ebx, [ebx+D_ini_heap]
	add ebx, ebp
	call lib32
	jc .erro
	clc
.erro:	pop esi
	pop ebp
	pop edi
	pop ebx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Libera fragmento								ok
; Entrada:
;	ebx: diretorio
;	esi: fragmento
; Saida:
;	cy= 0 - OK
;	cy= 1 - error, ponteiro invalido!!!
; Testar
align 4
libera_fragmento :

	push ebx
	push edi
	push ebp
	push esi

	push ebx
	mov ebp,ebx		; ebp= base dir
	add esi, ebp		; esi= endereço linear base item
	mov edi, [ebx+D_tam_heap]
	mov ebx, [ebx+D_ini_heap]
	add ebx, ebp
	call lib32
	pop ebx
	jc .erro

; ebx= ini heap, ebp=diretorio, esi=base fragmento, edi= tam heap
	push eax
	call TakeDMpos		; ebx= DM position
	mov eax, [esi+F_lba1]
	movzx edx, word [+esi+F_lba2]
	mov ecx, [esi+F_ext]

	call verifica_proxEV	; edi=item, eax=extensao
	jb .f

; agrega espaço liberado no espaço livre adjacente
	add eax, ecx
	mov [ebx+edi+EV_ext], eax
	sub [ebx+edi+EV_lba1], ecx
	sbb dword [ebx+edi+EV_lba2], 0
	pop eax

.erro:	pop esi
	pop ebp
	pop edi
	pop ebx
	ret

.f:	pop eax
	call add_ItemEV
	jmp .erro


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Libera nome do item							ok
; entrada:
; 	ebx= diretorio
;	esi= ponteiro do item
; saida:
;	cy= 0 - OK
;	cy= 1 - Erro, ponteiro invalido
; Testada...OK
align 4
libera_Inome :

	push esi
	mov esi, [ebx+esi+I_nome]
	call libera_item
	pop esi
	jc .erro
	mov byte [ebx+esi+I_ext_nome], 0
	mov word [ebx+esi+I_nome], 0
	clc
.erro:	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; cria item (arquivo ou o item da pasta)					OK-
; Entrada:
;	ax: tipo (pasta=1, arquivo=4 em diante , atalho=2,3)
;	ebx: base do diretório
;	esi: ponteiro do nome
;	cl: ext do nome
; Saida:
;	edi= item criado, nao altera ebx
; Funcionando
align 4
cria_item :		;

	push eax
	push ecx
	and ecx, 0xff
	push eax
	mov eax, extItem		;
  	push esi
  	call aloca_item               ; aloca o item com extensao igual a extItem
     	jb .erro
     	mov edi, esi		; edi= base do item
     	pop esi
	pop eax

; define parametros
	mov word [ebx+edi+I_tipo], ax
	mov byte [ebx+edi+I_ext_nome], cl
	mov byte [ebx+edi+I_cor_nome], 0x7	; cor padrao
	mov dword [ebx+edi+I_continue], 0

     	call ajusta_item_ponteiros	; ajusta a ordem alfabética
;
	push edi
	push esi
	mov eax, ecx
	call aloca_item		; aloca bloco para o nome
	mov [ebx+edi+I_nome], esi
	mov edi, esi
	add edi, ebx
	pop esi

	push esi
	rep movsb   		; escreve o nome no diretório
	pop esi
	pop edi

	clc
.erro:	pop ecx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; apaga item e item fragmentado (este não mexer nos ponteiros)			ok
; Entrada:
;	ebx: diretorio
;	esi: item
; Saida:
;	cy=0 - OK
;	cy=1 - Error, o ponteiro do item eh invalido
; Testar
align 4
apaga_item :
;int 1
  	push esi
.a:  	mov esi, [ebx+esi+I_continue]
 	or esi, esi
	jz .fim
	call libera_fragmento	; libera item de fragmento
	jc .error
	jmp .a
.fim:  	pop esi
  	call libera_Inome
 	call libera_item
 	push ebp
	mov ebp, D_ini_item
	call ajusta_ponteiroR
	pop ebp
	clc
	ret
.error:   pop esi
	stc
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Encontra nome na lista - pesquisa item no diretorio apontado por ebx		OK
; Entrada:
;	ebx: diretorio
;	esi: nome
;	ax: extensão no nome
; Saida:
;	cy=0 -> O item foi encontrado - edi= base do item , ;cx= tipo
;	cy=1 -> não foi encontrado o item - edi= 0
; OK

align 4
procura_item:
	push edx
	or ax, ax
	jz .error
	mov edi, [ebx+D_ini_item]

; Procura o item
   ; Verifica se terminou a lista de itens
.a:	or edi, edi
	jz .error

   ; Compara o item com a string (esi)
	movzx edx, byte [ebx+edi+I_ext_nome]
	push edi
	mov edi, [ebx+edi+I_nome]
	add edi, ebx
	call CmpStr32		; comparação exata insensivel M/m
	pop edi
	jz .achou

   ; Proximo item
.b:	mov edi, [ebx+edi+I_prox]
	jmp .a

; Item achado
.achou:	clc
	;mov cx, [ebx+edi+I_tipo]
	pop edx
	ret
.error:	stc
	pop edx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; Encontra nome na lista, tipo exato - pesquisa item no diretorio apontado por ebx		OK
; Entrada:
;	ebx: diretorio
;	esi: nome
;	ax: extensão no nome
;	cx: tipo do item (0= procura qualquer item)
; Saida:
;	cy=0 -> O item foi encontrado - edi= base do item , cx= tipo
;	cy=1 -> não foi encontrado o item - edi= 0
; OK

align 4
procura_tipoI:
	push edx
	mov edi, [ebx+D_ini_item]

; Procura o item
   ; Verifica se terminou a lista de itens
.a:	or edi, edi
	jz .erro

   ; Compara o item com a string (esi)
	movzx edx, byte [ebx+edi+I_ext_nome]
	push edi
	mov edi, [ebx+edi+I_nome]
	add edi, ebx
	call CmpStr32		; comparação exata insensivel M/m
	pop edi
	jz .achou

   ; Proximo item
.b:	mov edi, [ebx+edi+I_prox]
	jmp .a

; Item achado
.achou:	or ecx, ecx
	jz .fim
	or cl, cl
	jne .geral

	cmp cx, [ebx+edi+I_tipo]	; arquivo especifico (jpeg,bmp,...)
	jne .b
	jmp .fim
.geral:	cmp ch, [ebx+edi+I_tipo+1]	; arquivo geral (imagem,audio,texto,...)
	jne .b

.fim:	clc
	mov cx, [ebx+edi+I_tipo]
	pop edx
	ret
.erro:
	stc
	pop edx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  9 =============================================
; Encontra nome na lista, maior que cx - pesquisa item no diretorio apontado por ebx		OK
; Entrada:
;	ebx: diretorio
;	esi: nome
;	ax: extensão no nome
;	cx: tipo do item (0= procura qualquer item)
; Saida:
;	cy=0 -> O item foi encontrado - edi= base do item , cx= tipo
;	cy=1 -> não foi encontrado o item - edi= 0
; OK

align 4
procura_tipoM:
	push edx
	mov edi, [ebx+D_ini_item]
.a:	or edi, edi
	jz .erro
	movzx edx, byte [ebx+edi+I_ext_nome]
	push edi
	mov edi, [ebx+edi+I_nome]
	add edi, ebx
	call CmpStr32		; comparação exata insensivel M/m
	pop edi
	jz .achou
.b:	mov edi, [ebx+edi+I_prox]
	jmp .a
.achou:
	or ecx, ecx
	jz .fim
	cmp cx, [ebx+edi+I_tipo]
	jb .b			; se menor continua a procura
.fim:	clc
	mov cx, [ebx+edi+I_tipo]
	pop edx
	ret
.erro:
	stc
	pop edx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  B =============================================
; renomeia item (arquivo, folder ou link)					ok
; Entrada:
;	ebx: diretorio
;	edi: item
;	esi= novo nome
;	cl: extensao do nome
; Saida:
;	Nenhuma (Unico erro que pode haver eh o nome ser maior que 256 bytes).
; Testar
align 4
renomeia_item :

	push eax
	push ecx			; 1°
	push esi
	push edi
	and ecx, 0xff		; limita nome a 255 caracteres
	xchg esi, edi		; 2° - libera o bloco do nome
	call libera_Inome
	jc .erro
	xchg esi, edi
	mov [ebx+edi+I_ext_nome], cl
	push esi
	mov eax, ecx
	call aloca_item
	mov [ebx+edi+I_nome], esi
	mov edi, esi
	pop esi
	add edi, ebx
	rep movsb
    	clc
.erro:    pop edi
	pop esi
	pop ecx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  C =============================================
; Dá ao item um espaco no disco						OK
; entrada:
;       ebx= base do diretorio DM
;       edi= ponteiro do item
;       ecx= extensão excedente requerida no HD (em setores)
; saida:
;       edi= ponteiro do novo item fragmentado
; Verifica se o bloco a ser ocupado é adjacente anterior ao um bloco livre
; neste caso apenas atualizar o item e subtrair espaço do bloco livre
; OBS: depois, o melhor eh colocar em bytes ao inves de setores

align 4
aloca_espaco_item :
	push eax
	push ecx
	push edx
	push esi
	push ebp

	mov ebp, edi
	mov esi, edi
	mov edi, [ebx+edi+I_continue]
	or edi, edi
	jz .c
.a:	mov esi, edi
	mov edi, [ebx+edi+I_continue]
	or edi, edi
	jnz .a

.b:	call verjunta_blocoAdjacente	; se existe espaço vazio ao lado do bloco alocado então aumenta
	jc .error			; nao altera edi
	or ecx, ecx
	jz .fim
				; PODER-SE-IA TENTAR O PROXIMO ITEM EV SE VAZIO
.c:	call cria_itemFrag		; se não é possivel juntar aloca outro bloco

	mov edi, esi		; edi=ponteiro do novo item
	jb .error
	call ocupa_EV		; ocupa espaço vazio
	jb .error

	mov [ebx+esi+F_lba1], eax
	mov [ebx+esi+F_lba2], edx
	mov [ebx+esi+F_ext], ecx

.fim:	clc
.error:	pop ebp
	pop esi
	pop edx
	pop ecx
	pop eax
	ret

	

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  D =============================================
; Cria um item fragmentado no arquivo (sem o lba e a ext)				OK
; Entrada:
;	ebx: diretorio
;	esi: arquivo
; Saida:
;	esi: item fragmentado com os ponteiros certos
align 4
cria_itemFrag :

	push eax
	mov edi, esi
.a:	mov esi, [ebx+esi+F_continue]
	or esi, esi
	jz .b
	mov edi, esi
	jmp .a
.b:	mov eax, extFrag
	call aloca_item			;
	jc .erro
	mov dword [ebx+esi+F_continue], 0
	mov [ebx+edi+I_continue], esi
.fim:	clc
.erro:	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  E =============================================
; Verifica se existe bloco livre adjacente e junta eles
; Entrada:
;	ebx: diretorio
;	esi: o ultimo Icontinue do item
;	ecx: espaço requerido

; Saida:
;	ecx= 0 -> ele alocou todo o bloco requirido
;	ecx!= 0 -> ele não alocou ou alocou parte do bloco requirido
;		o restante vem em ecx
;	cy = 1 -> erro, falta de espaco no disco
; Arrumar - tem coisa errada - ja .a - TEstar
align 4
verjunta_blocoAdjacente :

	push eax
	push edx
	push esi
	push edi

	push ebx
	push ecx
	mov eax, [ebx+esi+F_lba1]	; pega a base do ultimo segmento
	mov dx, [ebx+esi+F_lba2]
	mov ecx, [ebx+esi]		;+F_ext extensao do ultimo segmento
	call TakeDMpos		; ebx= DM position
;int 1
	call verifica_proxEV
;int 1
	pop ecx
	pop ebx
	jc .fim			; o proximo bloco nao está livre....

	cmp ecx, eax               	; o bloco adjacente está livre...verifica capacidade
	ja .a			; menor?...sem possibilidades

; o bloco adjacente é maior
	add [ebx+esi], ecx		; usa o mesmo fragmento ampliado
	push ebx                      ; desconta da extensao os setores requisitados
	push esi			; adiciona ecx à base
	mov esi, edi
	call TakeDMpos		; ebx= DM position
	call ocupa_EV_n0		; retorna eax=lba1 base, edx lba2 base
	pop esi
	pop ebx
	and ecx, 0		; ocupou todos setores...zera ecx
	jmp .fim

; o bloco adjacente é menor		; usa o que tem...e procura outro descritor de EV
.a:	sub ecx, eax
	add [ebx+esi+F_ext], eax
	push ebx
	push ecx
	push esi
	mov ecx, eax
	mov esi, edi
 	call TakeDMpos		; ebx= DM position
	call ocupa_EV_n0
	pop esi
	pop ecx
	pop ebx
	jc .error

.fim:	clc
.error:	pop edi
	pop esi
	pop edx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  A =============================================
; encontra o lugar do item para que a lista fique em ordem alfabetica
; entrada:
;       ebx= diretorio
;       esi= nome do item
;       edi= ponteiro do item
;       cx= ext do nome
; saida:
;       nada
; Funcionando
align 4
ajusta_item_ponteiros :

	push eax
	push ecx
	push esi
	push edi

	mov eax, edi
	mov edi, [ebx+D_ini_item]
	and ecx, 0
.a:	or edi, edi
	jz .b
	push ecx
	push edi

	mov cl, [ebx+edi+I_ext_nome]
	mov edi, [ebx+edi+I_nome]
	add edi, ebx
        	call cmpstr		; colocar no segmento de 32 bits
	pop edi
        	pop ecx
        	jz .b
	mov ecx, edi
	mov edi, [ebx+edi+I_prox]
	jmp .a

.b:	mov esi, eax
	mov dword [ebx+eax+I_prox], edi	; atual
	mov [ebx+eax+I_ant], ecx		; anterior
	push ebp
	mov ebp, D_ini_item              ; ajusta o descritor da lista
	call ajusta_ponteiroC
	pop ebp

.fim:	clc
	pop edi
	pop esi
	pop ecx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  F =============================================
;
; Ajusta os ponteiros dos itens posteriores e anteriores quando for inserido um item
; Os ponteiros dos itens anteriores e posteriores devem estar no item
; entrada:
;       ebx= diretorio
;       esi= ponteiro do item
;       ebp= descritor da lista (inicio,atual e fim da lista)
; saida:
;       nada
; Funcionando... nao ajusta pela ordem de grandeza, mas pela ordem de entrada
align 4
ajusta_ponteiroC :

	push ecx
	push edi
	and cl, 0
	mov edi, [ebx+esi+I_ant]
	or edi, edi
	jz .a			;-> inicio da lista
	mov [ebx+edi+I_prox], esi
	inc cl
	jmp .b
.a:	mov [ebx+ebp],esi		; ponteiro do incio da lista
.b:	mov edi, [ebx+esi+I_prox]	;
	or edi, edi
	jz .c
	mov [ebx+edi+I_ant], esi
	jmp .fim
.c:	mov [ebx+ebp+8], esi	; ponteiro do fim da lista
	or cl,cl			; verifica se o atual terá que ser mudado
	jnz .fim
	mov [ebx+ebp+4], esi
.fim:	pop edi
	pop ecx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  10 ============================================
; Ajusta os ponteiros dos itens posteriores e anteriores quando for retirado um item
; O Iprox e Iant deve sempre ser 0 e 4, nao mude....
; entrada:
;       ebx= diretorio
;       esi= ponteiro do item
;       ebp= descritor da lista (inicio,atual e fim da lista)
; saida:
;       nada
align 4
ajusta_ponteiroR :

	push eax
	push edi
	mov edi, [ebx+esi+I_ant]
	or edi, edi
	jz .a
	mov eax, [ebx+esi+I_prox]
	mov [ebx+edi+I_prox], eax
	jmp .b

.a:	mov eax, [ebx+esi+I_prox]
	mov [ebx+ebp], eax

.b:	mov edi, [ebx+esi+I_prox]
	or edi, edi
	jz .c
	mov eax, [ebx+esi+I_ant]
	mov [ebx+edi+I_ant], eax
	jmp .d

.c:	mov eax, [ebx+esi+I_ant]
          mov [ebx+ebp+8], eax

.d:
	cmp esi, [ebx+ebp+4]
	jne .fim
	mov eax, [ebx+ebp]
	mov [ebx+ebp+4], eax

.fim:	pop edi
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  11=============================================
; verifica excesso
; entrada:
; 	eax= nova extensão requerida no HD
;       	ebx= base do diretorio
;       	esi= ponteiro do item
; saida:
;       	edi= ponteiro do novo item fragmentado
; 	modifica ecx
; corre todos os fragmentos totalizando a quantidade de setores alocados e descontando da
; nova extensao requerida. quando ficar menor, a quantidade resultante é comparada à capa-
; cidade do ultimo fragmento. Se o mesmo comporta a retirada ele é atualizado e a diferença
; é liberada
; Funciona bem quando retira apenas do ultimo fragmento.
; Quando a retirada é feita em muitos fragmentos não é feita a junçao de EV adjacentes
; anteriores.
align 4
RetiraFrag_item :
	push edx
	push esi
;calcula o total de setores que deve ser liberado
;int 1
.d:	mov edi, [ebx+esi+I_continue]
	or edi, edi
	jz .c			; se for zero, o item nao tem fragmentos

.a:	mov esi, edi
	mov edi, [ebx+edi+I_continue]
	mov ecx, [ebx+esi]		;+F_ext extensao do ultimo segmento
	sub eax, ecx
	jge .a			; se fica negativo, esse é o resto a ser liberado
;int 1
	neg eax
	cmp eax, ecx		; compare extentions
	je .e

; eax<ecx -> a qtde de setores a liberar é menor que a capacidade do ultimo frag	ok
	sub ecx, eax		;
	mov [ebx+esi],ecx
	xchg ecx, eax
	add eax, [ebx+esi+F_lba1]	; base a liberar= base frag + setores liberados
	mov edx, [ebx+esi+F_lba2]
; eax= lba1, ecx= qtde setores a liberar, edx=lba2, esi= item
	push edi
	push ebx
	push eax
	call TakeDMpos		; ebx= DM position
	call verifica_proxEV	; edi=item, eax=extensao
;int 1
	jb .f
; agrega espaço liberado no espaço livre adjacente				ok
	add eax, ecx
	mov [ebx+edi+EV_ext], eax
	sub [ebx+edi+EV_lba1], ecx
	sbb dword [ebx+edi+EV_lba2], 0
	pop eax
	pop ebx
	pop edi

.c:	pop esi
	pop edx
	ret

; adiciona item de espaço vazio...
.f:	pop eax
	call add_ItemEV
	pop ebx
	pop edi
; ...e libera os demais segmentos
.g:	mov esi, edi
	mov eax, [ebx+edi+F_lba1]
	mov edx, [ebx+edi+F_lba2]
	mov ecx, [ebx+edi+F_ext]
	call libera_fragmento
	jb .c
	mov edi, [ebx+edi+I_continue]
	or edi, edi
	jz .c
	jmp .g

; ecx=eax -> retira item: 							OK
.e:	call libera_fragmento
	jmp .c

; os itens EV nao são ordenados pela ordem de grandeza lba, mas pela ordem de entrada.
;

;-----------------------------------------------------------------------------------------
