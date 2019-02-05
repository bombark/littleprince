;-----------------------------------------------------------------------------------------

; Servicos de Memoria

;-----------------------------------------------------------------------------------------


;======================== T A B E L A ====================================================
; SMM = Servico de Memoria

SMM_Table:
	dd SMM_reserved		; 0- Get information

; Rotinas de Heap
	dd SMM_Heap_Init		; 1- Inicia o Heap
	dd SMM_Heap_Deinit		; 2- Finaliza o Heap
	dd SMM_Heap_Resize		; 3- Aumenta o tamanho do heap
	dd SMM_MainHeap_Alloc	; 4- Aloca um bloco no heap principal
	dd SMM_MainHeap_Realloc	; 5- Realoca um bloco no heap principal
	dd SMM_MainHeap_Free	; 6- Libera um bloco no heap principal
	dd SMM_Heap_Alloc		; 7- Aloca um bloco
	dd SMM_Heap_Realloc		; 8- Realoca um bloco
	dd SMM_Heap_Free		; 9- Libera um bloco

; Rotinas de String
	dd SMM_strcpy		; 0xa- copia string
	dd SMM_strcpyZ		; 0xb- copia string Z
	dd SMM_strcmp		; 0xc- compara string
	dd SMM_strcmpZ		; 0xd- compara string Z
	dd 0;SMM_			; 0xe- pesquisa coincidencia
	dd 0;SMM_			; 0xf- pesquisa coincidencia em string Z
	dd SMM_strlen		; 0x10- devolve o tamanho da string (string Z)

; Rotinas de Conversao
	dd 0;SMM_AtoI		; 0x11- Converte ascii para inteiro
	dd SMM_ItoA		; 0x12- Converte inteiro para ascii  
;	dd SMM_AtoF		; 0x13- Converte ascii para float 
;	dd SMM_FtoA		; 0x14- Converte float para ascii
;	dd SMM_ItoF		; 0x15- Converte inteiro para float
;	dd SMM_FtoI		; 0x16- Converte floar para inteiro
;	dd SMM_end		; 0x17- free
;	dd SMM_AtoU		; 0x18- Converte ascii para unicode 
;	dd SMM_UtoA		; 0x19- Converte unicode para ascii

; Rotina de Lista
;	dd SMM_List_Init		; 0x18- inicia uma lista
;	dd SMM_List_Deinit		; 0x19- finaliza uma lista
;	dd SMM_List_addItem		; 0x1a- adiciona um item na lista
;	dd SMM_List_remItem		; 0x1b- remove um item na lista
;	dd SMM_List_findItem	; 0x1c- encontra um item na lista
;	dd SMM_List_firstItem	; 0x1d- posiciona o primeiro item da lista
;	dd SMM_List_lastItem	; 0x1e- posiciona o ultimo item da lista
;	dd SMM_List_proxItem	; 0x1f- posiciona o proximo item da lista
;	dd SMM_List_antItem		; 0x20- posiciona o item anterior da lista
;	dd SMM_List_tofirstItem	; 0x21- transfere item atual para inicio da lista
;	dd SMM_List_tolastItem	; 0x22- transfere item atual para fim da lista
;	dd SMM_List_torightItem	; 0x23- transfere item atual para direita
;	dd SMM_List_toleftItem	; 0x24- trannsfere item atural para esquerda

; Rotinas de Arvore
;	dd SMM_Tree_Init		; 0x22- inicia uma arvore
;	dd SMM_Tree_Deinit		; 0x23- finaliza uma arvore
;	dd SMM_Tree_addItem		; 0x24- adiciona um item na arvore
;	dd SMM_Tree_remItem		; 0x25- remove um item na arvore
;	dd SMM_Tree_findItem	; 0x26- encontra um item na arvore

; Rotinas de Buffer
;	dd SMM_Buffer_Create	; - Cria um buffer (Linear ou Circular)
;	dd SMM_Buffer_Destroy	; - Destroi um buffer
;	dd SMM_Buffer_PutByte	; - Coloca um byte no buffer
;	dd SMM_Buffer_PutWord	; - Coloca um word no buffer
;	dd SMM_Buffer_PutDword	; - Coloca um dword no buffer
;	dd SMM_Buffer_GetByte	; - Pega um byte no buffer
;	dd SMM_Buffer_GetWord	; - Pega um word no buffer
;	dd SMM_Buffer_GetDword	; - Pega um dword no buffer

; Rotinas de Criptografia
;	dd SMM_Encriptar		; - Encripta dados
;	dd SMM_Desencriptar		; - desencripta dados

SMM_Table_Qtde	equ 0x12

; OBS= Heap principal vai ser o primeiro heap criado pelo usuario
;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

; Estruruta do descritor do Heap
Heap_Ass		equ 0	; dd - Assinatura do descritor ('DEHE') (DEscritor HEap)
Heap_Ini		equ 4	; dd - Ponteiro do inicio do Heap
Heap_Size		equ 8	; dd - Tamanho do heap

Heap_AssinaturaDE	equ "DEHE"	; Assinatura do descritor do heap
Heap_Assinatura		equ "HEAP"	; Assinatura do heap

;-----------------------------------------------------------------------------------------


;======================== E N T R A D A ==================================================
; Entrada  do servico de memoria
; IN:
;	registradores= parametros
;	ebp= numero da funcao
;	int 0x41
; OUT:
;	depende

SMM_Enter:
	cmp ebp, 0x28
	ja .error
	call dword [cs:SMM_Table+ebp*4]
	iretd

.error:	stc
	iretd

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  0 =============================================
; Devolve informacoes sobre o servico
; IN:
; OUT:

SMM_reserved:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Inicia o HEAP
; IN:
;	eax= tamanho do Heap
;	ebx= descritor do Heap
;	esi= inicio do Heap
; OUT:
;	cy= 0 	- OK
;	cy= 1	- Erro
; OBS: O bloco do Heap deve estar dentro dos 2gb e devera ser maior que 0x10 bytes

SMM_Heap_Init:
; Verifica os parametros para que nao sejam maiores que 0x80000000 (OS_BASE)
	and esi, 0x7fffffff
	and ebx, 0x7fffffff
	cmp eax, 0x10
	jb .error
	mov ecx, eax
	add ecx, esi
	cmp ecx, OS_BASE
	jae .error

; Inicia o HEAP
	sub eax, 8
	mov dword [esi], Heap_Assinatura
	mov dword [esi+4], eax

; Coloca as variaveis no descritor do HEAOP
	mov dword [ebx+Heap_Ass], Heap_AssinaturaDE
	add esi, 4
	mov [ebx+Heap_Ini], esi
	mov [ebx+Heap_Size], eax
	ret

.error:	stc
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Finaliza o HEAP
; IN:
;	ebx= descritor do HEAP
; OUT:
;	nothing

SMM_Heap_Deinit:
	push eax
	cmp dword[ebx], Heap_Assinatura
	jne .error
	xor eax, eax
	mov [ebx], eax
	mov [ebx+Heap_Ini], eax
	mov [ebx+Heap_Size], eax
	clc
	pop eax
	ret

.error:	stc
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Aumenta o HEAP
; IN:
;	eax= novo tamanho do Heap
;	ebx= descritor do Heap
;	esi= mova base do Heap
; OUT:
;	nothing

SMM_Heap_Resize:
	push eax
	cmp dword[ebx], Heap_Assinatura
	jne .error
	xor eax, eax
	mov [ebx], eax
	mov [ebx+Heap_Ini], eax
	mov [ebx+Heap_Size], eax
	clc
	pop eax
	ret

.error:	stc
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Aloca um bloco no heap principal (1° heap criado)
; IN:
;	eax= tamanho do bloco
; OUT:
;	esi= bloco alocado
;	cy= 0	- OK
;	cy= 1	- Error -> sem espaco ou descritor invalido
; OK

SMM_MainHeap_Alloc:
	push ebx
	push edi
	mov esi, [Task_Current]
	mov ebx, [esi+Task_Heap_Base]
	or ebx, ebx
	jz .error
	mov edi, [esi+Task_Heap_Size]
	call aloc32
	pop edi
	pop ebx
	ret

.error:	stc
	xor esi, esi
	pop edi
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Realoca um bloco no heap principal (1° heap criado)
; IN:
;	eax= novo tamanho do bloco
;	esi= bloco a ser realocado
; OUT:
;	esi= bloco alocado
;	cy= 0	- OK
;	cy= 1	- Error -> sem espaco ou descritor invalido
; OK

SMM_MainHeap_Realloc:
	push ebx
	push edi
	mov edi, [Task_Current]
	mov ebx, [edi+Task_Heap_Base]
	or ebx, ebx
	jz .error
	mov edi, [edi+Task_Heap_Size]
	call real32
	pop edi
	pop ebx
	ret

.error:	stc
	xor esi, esi
	pop edi
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Libera um bloco no heap principal (1° heap criado)
; IN:
;	esi= bloco a ser liberado
; OUT:
;	cy= 0	- OK
;	cy= 1	- Error -> sem espaco ou descritor invalido
; OK

SMM_MainHeap_Free:
	push ebx
	push esi
	push edi
	mov edi, [Task_Current]
	mov ebx, [edi+Task_Heap_Base]
	or ebx, ebx
	jz .error
	mov edi, [edi+Task_Heap_Size]
	call lib32
	pop edi
	pop esi
	pop ebx
	ret

.error:	stc
	pop edi
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Aloca um bloco num heap
; IN:
;	eax= tamanho do bloco
;	ebx= descritor do HEAP
; OUT:
;	esi= bloco alocado
;	cy= 0	- OK
;	cy= 1	- Error -> sem espaco ou descritor invalido
; OK

SMM_Heap_Alloc:
	push ebx
	push edi

; Verifica validade do Heap
	cmp dword [ebx+Heap_Ass], Heap_AssinaturaDE	; "DEHE"
	jne .error
	mov edi, [ebx+Heap_Ini]
	cmp dword [edi-4], Heap_Assinatura		; "HEAP"
	jne .error

; Aloca um bloco
	mov edi, [ebx+Heap_Size]
	mov ebx, [ebx+Heap_Ini]
	call aloc32
	jc .error

	pop edi
	pop ebx
	ret

.error:
	stc
	pop edi
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; Realoca um bloco num heap
; IN:
;	eax= novo tamanho do bloco
;	ebx= descritor do HEAP
;	esi= bloco a ser realocado
; OUT:
;	esi= novo bloco
;	cy= 0	- OK
;	cy= 1	- Error -> sem espaco ou descritor invalido

SMM_Heap_Realloc:
	push ebx
	push edi

; Verifica validade do Heap
	cmp dword [ebx+Heap_Ass], Heap_AssinaturaDE	; 'DEHE'
	jne .error
	mov edi, [ebx+Heap_Ini]
	cmp dword [edi-4], Heap_Assinatura		; 'HEAP'
	jne .error

; Realoca o bloco
	mov edi, [ebx+Heap_Size]
	mov ebx, [ebx+Heap_Ini]
	call real32
	jc .error

	pop edi
	pop ebx
	ret

.error:	stc
	pop edi
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  9 =============================================
; Libera um bloco num heap
; IN:
;	ebx= descritor do HEAP
;	esi= bloco a ser liberado
; OUT:
;	cy= 0	- OK
;	cy= 1	- Descritor invalido ou ponteiro do bloco invalido
; O final do bloco estah errado

SMM_Heap_Free:
	push ebx
	push edi

; Verifica validade do Heap
	cmp dword [ebx+Heap_Ass], Heap_AssinaturaDE	;'DEHE'
	jne .error
	mov edi, [ebx+Heap_Ini]
	cmp dword [edi-4], Heap_Assinatura		;'HEAP'
	jne .error

; Libera o bloco
	mov edi, [ebx+Heap_Size]
	mov ebx, [ebx+Heap_Ini]
	call lib32
	jc .error

	clc
	pop edi
	pop ebx
	ret

.error:	stc
	pop edi
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  A =============================================
; Copia uma string
; IN:
;	esi= string fonte
;	edi= string destino
;	ecx= tamanho
; OUT:
;	cy= 0	- OK
;	cy= 1	- Descritor invalido ou ponteiro do bloco invalido
; Tambem funciona para copiar memoria e mais rapida que a strcpy_Z

SMM_strcpy:
	cmp esi, OS_BASE		; Protecao da memoria do sistema
	jae .error
	add edi, ecx
	cmp edi, OS_BASE
	jae .error
	sub edi, ecx
	jmp cpystr

.error:	stc
	sub edi, ecx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  B =============================================
; Copia uma string terminada em 0
; IN:
;	esi= string fonte
;	edi= string destino
; OUT:
;	cy= 0	- OK
;	cy= 1	- Descritor invalido ou ponteiro do bloco invalido

SMM_strcpyZ:
	cmp esi, OS_BASE		; Protecao da memoria do sistema
	jae .error
	add edi, ecx
	cmp edi, OS_BASE
	jae .error
	sub edi, ecx
	jmp cpystr_Z

.error:	stc
	sub edi, ecx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  C =============================================
; Copia uma string terminada em 0
; IN:
;	esi= string fonte
;	edi= string destino
; OUT:
;	cy= 0	- OK
;	cy= 1	- Descritor invalido ou ponteiro do bloco invalido

SMM_strcmp:
	cmp esi, OS_BASE		; Protecao da memoria do sistema
	jae .error
	add edi, ecx
	cmp edi, OS_BASE
	jae .error
	sub edi, ecx
	jmp cpystr_Z

.error:	stc
	sub edi, ecx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  D =============================================
; Copia uma string terminada em 0
; IN:
;	esi= string fonte
;	edi= string destino
; OUT:
;	cy= 0	- OK
;	cy= 1	- Descritor invalido ou ponteiro do bloco invalido

SMM_strcmpZ:
	cmp esi, OS_BASE		; Protecao da memoria do sistema
	jae .error
	add edi, ecx
	cmp edi, OS_BASE
	jae .error
	sub edi, ecx
	jmp cpystr_Z

.error:	stc
	sub edi, ecx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  E =============================================
; Devolve o tamanho de uma string terminada em 0
; IN:
;	esi= string fonte
; OUT:
;	cy= 0	- OK
;	cy= 1	- Descritor invalido ou ponteiro do bloco invalido

SMM_strlen:
	cmp esi, OS_BASE		; Protecao da memoria do sistema
	jae .error
	jmp strlen

.error:	stc
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 1 ============================================
; Converte ascii para inteiro
; IN:
;	 bl= base 
;	 bh= tamanho
;	  0x0T -> se T=1 -> 8#, T=2 -> 16#, T=3 -> 32#, T=4 -> 64#
;	esi= string
; OUT:
;	edx:eax= Numero 
;	cy= 0	- OK
;	cy= 1	- Descritor invalido ou ponteiro do bloco invalido

SMM_AtoI:
	push ebx
	cmp esi, OS_BASE
	jae .end

  ; 8#	
	and bh, 0x0f
	cmp bh, 1
	jne @f
	call I8toA
	
  ; 16#	
@@:	cmp bh, 2
	jne @f
	call I16toA
	
  ; 32#	 
@@:	cmp bh, 3
	jne @f
	call I32toA
	
  ; 64#	
@@:	cmp bh, 4
	jne .end
	call I64toA
	
.end:	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 2 ===========================================
; Converte inteiro para ascii
; IN:  
;     edx:eax= Numero
;	 cl= Base
;	 ch= sinal/tamanho
;	  0xS0 -> se S=1 -> com sinal
;	  0x0T -> se T=1 -> 8#, T=2 -> 16#, T=3 -> 32#, T=4 -> 64#
;	esi= String
; OUT:
;	esi= String com o numero convertido

  ; tabela de rotinas ========
    
    ; Sem sinal
    SMM_ItoA_Function1:
    	dd SMM_ItoA.end2
    	dd I8toA
    	dd I16toA		
    	dd I32toA		; funcionando
	dd I64toA

    ; Com sinal
    SMM_ItoA_Function2:
    	dd SMM_ItoA.end2
    	dd I8StoA
    	dd I16StoA
    	dd I32StoA	; funcionando
	dd I64StoA	

  ;---------------------------

SMM_ItoA:
	push ebx
	push ecx
	
  ; Verifica a protecao e ajusta variavel para a chamada indireta 	
	;cmp esi, OS_BASE
	;jae .end
	xor ebx, ebx
	mov bl, ch
	and bl, 0x0f
	cmp bl, 0x4
	ja .end
	
  ; Se (bh&0x10==0)  -> numero sem sinal 	
	test ch, 0x10	; com sinal???
	jnz .sinal
	movzx ecx, cl
	call dword [SMM_ItoA_Function1+ebx*4]	; conversao sem sinal
	jmp .end
	
  ; Senao  -> numero com sinal	
.sinal:	movzx ecx, cl
	call dword [SMM_ItoA_Function2+ebx*4]	; conversao com sinal
	jmp .end

.end:	pop ecx
	pop ebx
.end2:	ret

;----------------------------------------------------------------------------------------- 