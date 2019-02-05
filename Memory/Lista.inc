;-----------------------------------------------------------------------------------------

; ROTINAS DE LISTA

;-----------------------------------------------------------------------------------------

;======================== V A R I A V E I S ==============================================
; Descritor da lista
ListDesc_Ext	equ -4	; dd - Extensao do bloco
ListDesc_Ini	equ 0x0	; dd - Primeiro item da lista
ListDesc_End	equ 0x4	; dd - Ultimo item da lista
ListDesc_Atual	equ 0x8	; dd - Item atual da lista
ListDesc_Bytes	equ 0xc	; dd - Bytes reservados

size_ListDesc	equ 0x10

; Estrutura do item
ListItem_Ext	equ -4	; dd - Extensao do bloco
ListItem_Prox	equ 0x0	; dd - Proximo item
ListItem_Ant	equ 0x4	; dd - Item anterior
ListItem_Bytes	equ 0x8	; rb ? - bytes reservados para os dados

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 ==============================================
; Inicializa uma lista
; Funcionamento:
;	Aloca um bloco de 16 Bytes
;	Zera o bloco (que eh o descritor)
;	FIM
; Entrada:
;	eax= bytes a reservar para dados no item
; Saida:
;	ebx= ponteiro do bloco alocado
;
; ESTRUTURA DO BLOCO INICIALIZADO
; base - 4: extensão do bloco (resetar bit #0)
; base + 0: inicio
; base + 4: fim
; base + 8: atual
; base + C: quantidade bytes a reservar

align 4
List_Init:
	push esi
	push edi
	push eax
	mov eax,dword 0x10
	call SMM_MainHeap_Alloc	; aloca bloco #32
	jb .error		; esi = ponteiro do bloco
	pop eax
	xor ebx,ebx
	add eax, 8	; 2 ponteiros
	mov dword [esi],ebx		; zera os 4 dwords iniciais
	mov dword [esi+ListDesc_End],ebx
	mov dword [esi+ListDesc_Atual],ebx
	mov dword [esi+ListDesc_Bytes],eax
	mov ebx,esi
	pop edi
	pop esi
	retn

.error:	stc
	pop eax
	pop edi
	pop esi
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N X ==============================================
; Destroi uma lista
; Entrada:
;	ebx= descritor da lista
; Saida:
;	ebx= 0
;	cy= 0 -> OK
;	cy= 1 -> descritor invalido
; OBS: Estudar uma maneira de verificar se ebx eh um descritor de lista

List_Kill:
	push eax
	push esi
	push edi
	mov eax, [ebx]

; libera o descritor da tabela
	mov esi, ebx
	call SMM_MainHeap_Free

; libera os itens da tabela
	mov esi, eax	; coloca o ponteiro no inicio da lista
.delItem:	or esi, esi	; compara se terminou
	jz .end
	mov eax, [esi]	; salva o proximo item em eax
	call lib32	; libera o bloco
	mov esi, eax	; coloca o item em esi
	jmp .delItem

.end:	xor ebx, ebx
	pop edi
	pop esi
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 2 ==============================================
; Adiciona um item na lista
; Entrada:
;	eax= ext título (string)
;	esi= ponteiro da string
;	ebx= descritor da lista
; Saida:
;	edi= ponteiro do item
; OBS: O ponteiro do item atual nao é modificado

align 4
List_AddItem:
	push ecx			; equ f2d8
	push eax
	push esi
	push ebx	
	add eax, [ebx+ListDesc_Bytes]
	call SMM_MainHeap_Alloc
	pop ebx
	jb .end			; error - falta de espaco na memoria
	mov edi,esi
	mov dword [edi+ListItem_Prox], 0
	mov dword [edi+ListItem_Ant], 0
	pop esi
	pop ecx
	push ecx
	push esi
	push edi
	add edi,[ebx+ListDesc_Bytes]
	call cpystr	;rbf0	; transfere nome (ds:esi-> ds:edi)
	xor esi,esi
	pop edi
	cmp dword [ebx],esi
	jne .ba4
	mov dword [edi],esi
	mov dword [edi+ListItem_Ant],esi
	mov dword [ebx],edi
	mov dword [ebx+ListDesc_End],edi
	mov dword [ebx+ListDesc_Atual],edi
	mov dword [edi],0
	mov dword [edi+ListItem_Ant],0
.end:	pop esi
	pop eax
	pop ecx
	retn

.ba4:	mov esi,[ebx+ListDesc_End]
	mov dword [ebx+ListDesc_End],edi
	;mov dword [ebx+ListDesc_Atual],edi
	mov dword [esi],edi
	mov dword [edi], 0
	mov dword [edi+ListItem_Ant],esi
	jmp .end

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 3 ==============================================
; Retira um item da lista
; Entrada:
;	ebx= descritor da lista
;	esi= Ponteiro do item a ser eliminado
;	  esi=0 -> elimina o item atual 
; Saida:
;	nothing
; OBS: caso o item a ser excluido é o item atual, o novo item atual passa ser o proximo
;      da lista


align 4
List_DelItem:
	push ecx
	push esi
	push edi

  ; Se descritor é valido 	
	cmp dword [ebx],0x00
	je .error
	
    ; Ajusta os ponteiros 	
	or esi, esi
	jnz @f
	mov esi, [ebx+ListDesc_Atual] 	
@@:	mov ecx, [esi+ListItem_Ant]
	mov edi, [esi+ListItem_Prox]	

      ; Se o anterior==0  -> Coloca o edi no inicio da lista
	or ecx, ecx
	jnz .senao1
	mov [ebx+ListDesc_Ini], edi
	jmp .se2
.senao1:	mov [ecx+ListItem_Prox], edi

      ; Se o proximo==0  -> Coloca o ecx como fim da lista
.se2:    	or edi, edi
      	jnz .senao2
      	mov [ebx+ListDesc_End], ecx
      	mov edi, [ebx+ListDesc_Ini]
      	mov [ebx+ListDesc_Atual], edi
      	jmp .end
.senao2:	mov [ebx+ListDesc_Atual], edi
	mov [edi+ListItem_Ant], ecx 

    ; Libera o bloco
.end:	call lib_HB
.error:	pop edi
	pop esi
	pop ecx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
; Retorna endereco do primeiro item da lista
; Funcionamento:
;	Pega o primeiro item no descritor
;	coloca no descritor o item
;	FIM
;
; Entrada:
;	ebx= descritor da lista
; Saida:
;	cy=0, edi= ponteiro da string, ecx= extensão string

align 4
List_FirstItem:

	mov edi, [ebx]			; pega o 1o da lista
	mov [ebx+ListDesc_Atual], edi		; recoloca no atual
	retn

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Retorna o proximo item
; Funcionamento:
;	Pega o item ATUAL
;	se item==0
;	   sai
;	Pega o proximo item
;	se item==0
;	   pega o primeiro e seta carry
;	Coloca no descritor o item
;	FIM
; Entrada:
;	ebx: descritor da lista
; Saida:
;	edi= ponteiro do proximo item
;	  se cy= 1 -> fim da lista e edi passa apontar o primeiro item

align 4
List_NextItem:
	mov edi, [ebx+ListDesc_Atual]		; pega ponteiro do atual
	or edi, edi
	jz .error
	mov edi, [edi]
	or edi, edi
	jz .error				; se estiver no ultimo item da lista pula para
	mov [ebx+ListDesc_Atual], edi		; atualiza o ponteiro o primeiro item e retorna com o carry
	retn

.error:	mov edi, [ebx]
	mov [ebx+ListDesc_Atual], edi
	stc
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 6 ==============================================
; Retorna o endereco do item anterior
; Entrada:
;	ebx: descritor da lista
; Saida:
;	edi: item anterior
;	  se cy= 1 -> fim da lista e edi passa apontar o ultimo item

align 4
List_PrevItem:

	mov edi,[ebx+ListDesc_Atual]
	or edi,edi
	jz .error
	mov edi,[edi+ListItem_Ant]		; busca item anterior
	or edi,edi
	jz .error
	mov [ebx+ListDesc_Atual], edi
	ret

.error:	mov edi, [ebx+ListDesc_End]
	mov [ebx+ListDesc_Atual], edi
	stc
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 7 ==============================================
; Retorna o ultimo item da lista
; Entrada:
;	ebx: descritor da lista
; Saida:
;	edi: ultimo item da lista

align 2
List_LastItem:

	mov edi, [ebx+ListDesc_Ini]		; pega o 1o da lista
	mov [ebx+ListDesc_Atual], edi
	ret



;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 8 ====================================
; Coloca o item atual para inicio da lista
; IN:
;	ebx= descritor de lista
; OUT:
;	cy = 0   - OK
;	cy = 1   - Nao tem item na lista ou o item já é o primeiro da lista

List_tofirstItem:
	push eax
	push edx
	push esi
	mov esi, [ebx+ListDesc_Atual]

; compara se ja nao eh o primeiro
	cmp esi, [ebx+ListDesc_Ini]
	je .error

; Retira da posicao atual
	mov eax, [esi+ListItem_Ant]		; eax= item anterior
	mov edx, [esi+ListItem_Prox]		; edx= proximo item
	mov [eax+ListItem_Prox], edx
	or edx, edx
	jz .pula
	mov [edx+ListItem_Ant], eax

; Coloca na primeira posicao da lista
.pula:	mov dword [esi+ListItem_Ant], 0
	mov edx, [ebx+ListDesc_Ini]
	mov [edx+ListItem_Ant], esi
	mov dword [esi+ListItem_Prox], edx
	mov [ebx+ListDesc_Ini], esi

	pop esi
	pop edx
	pop eax
	ret

.error:	stc
	pop esi
	pop edx
	pop eax
	ret


;-------------------------------------------------------------------------------


;======================== F U N C T I O N 9 ====================================
; Coloca o item atual para o fim da lista
; IN:
;	ebx= descritor de lista
;	esi= item a ser colocado na ultima posicao
; OUT:
;	cy = 0   - OK
;	cy = 1   - Nao tem item na lista ou o item já é o primeiro da lista

List_tolastItem:
	push eax
	push edx
	push esi
	mov esi, [ebx+ListDesc_Atual]

; compara se ja nao eh o ultimo item
	cmp esi, [ebx+ListDesc_End]
	je .error

; Retira da posicao atual
	mov eax, [esi+ListItem_Ant]		; eax= item anterior
	mov edx, [esi+ListItem_Prox]		; edx= proximo item
	or  eax, eax
	jnz .notfirst
	mov [ebx+ListDesc_Ini], edx
	jmp .pula

.notfirst:mov [eax+ListItem_Prox], edx
.pula:	mov [edx+ListItem_Ant], eax



; Coloca na ultima posicao da lista
	mov dword [esi+ListItem_Prox], 0
	mov edx, [ebx+ListDesc_End]
	mov [edx+ListItem_Prox], esi
	mov [esi+ListItem_Ant], edx
	mov [ebx+ListDesc_End], esi

	pop esi
	pop edx
	pop eax
	ret

.error:	stc
	pop esi
	pop edx
	pop eax
	ret



;-------------------------------------------------------------------------------


;======================== F U N C T I O N 1 0 ============================================
; Procura um item na lista
; Entrada:
;	ebx= descritor da lista
;	esi= ponteiro da string
;	ecx= tamanho da string
;	  se ecx=0, a string so termina quando encontra um 0
; Saida:
;	edi= item encontrado
;	 cy=0 - OK
;	 cy=1 - Erro, item nao encontrado

align 2
List_FindItem:
	mov edi, [ebx]
.loop:	or edi, edi
	jz .erro
	push edi
	add edi, [ebx+ListDesc_Bytes]
	call cmpstr
	pop edi
	je .find
	mov edi, [edi]		; pega o proximo item
	jmp .loop
.find:	clc
	ret

.erro:	stc		; nao encontrou nenhum item
	ret



;-----------------------------------------------------------------------------------------