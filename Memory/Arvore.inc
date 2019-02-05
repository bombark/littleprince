; Arvore binária #32
;
;-----------------------------------------------------------------------------------------
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Arvores binarias (uso geral. O heap deve estar inicializado)
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; INICIALIZA ARVORE #32
; O bloco inicial é alocado no inicio do heap local
; Entrada:
;	eax= bytes a reservar para dados no item
;	ebx= deve apontar a base do heap local
;	edi= deve apontar o fim do heap local (base+extensão)
; Saida:
;	cy=0 eax= ponteiro do bloco alocado

align 4
IniTree_HL32 :

	push eax
	mov eax,dword 0x0c
	call aloc32		; aloca bloco fixo com 12 bytes
	pop eax
	jb .a			; esi = ponteiro do bloco
	xor ebx,ebx
	mov dword [esi],ebx		; zera inicio
	mov dword [esi+0x04],ebx	; zera final
	mov dword [esi+0x08],ebx	; zera atual
	mov dword [esi+0x0c],eax 	; bytes reservados
.a:	ret



;-----------------------------------------------------------------------------------------

; KillTree_HL32:
; Elimina a arvore do heap: USAR: libera_HB para destuir o heap local como um todo.


;-----------------------------------------------------------------------------------------
;
; ADICIONA ITEM NA ÁRVORE ALOCADA NO HEAP LOCAL
; Entrada:
;	esi= aponta a string
;	eax= extensao
;	ebx = deve apontar a base do heap local
;	edi = deve apontar o fim do heap local (base+extensão)
; Saida:
;	cy=1 	erro - falta espaço no heap local

align 4
AddItemTree_HL32:

rec00:	push ebx
	push ecx
	push edx
	push ebp
	push eax
	push esi
	mov ebx,8
	add ax,[ebx+08]
	call aloc32		; rotina aloca bloco no heap local indicado por es
	jb $			; no caso de erro (falta de espaço no heap)
	mov ebp,esi
	mov edi,esi
	add edi,[ebx+08]
	pop esi
	pop ecx
	push ecx
	push esi
	call TrStr_DSesi_DSedi	; transfere string retirando o excesso de espaços
	xor esi,esi
	mov [ebp+00],esi
	mov [ebp+04],esi
	mov [ebp+08],esi
	cmp [ebx],esi
	jne .ec60
	mov [ebx],ebp
.saida:	mov edi,ebp
	pop esi
	pop eax
	pop ebp
	pop edx
	pop ecx
	pop ebx
	retn

;-----------------------------------------------------------------------------------------

.ec60:	mov edi,[ebx]
.ec65:	pop esi
	pop eax
	push eax
	push esi
	mov [ebx+04],edi
	add edi,[ebx+08]
	mov dx,[edi-02]
	call CmpStr32		; rotina compara string
	je .ecc4
	jg .eca4
	mov esi,[ebx+04]
	mov edi,[esi+04]
	or edi,edi
	jne .ec65
	mov [esi+04],ebp
	mov [ebp+00],esi
.eca2:	jmp .saida

;-----------------------------------------------------------------------------------------

.eca4:	mov esi,[ebx+04]
	mov edi,[esi+08]
	or edi,edi
	jne .ec65
	mov [esi+08],ebp
	mov [ebp+00],esi
	jmp .saida

;-----------------------------------------------------------------------------------------

.ecc4:	test byte [ebx+10],01
.ecca:	jne .ecca
	mov esi,ebp
	call lib32		; libera bloco no heap local
	stc
	jmp .saida


;-----------------------------------------------------------------------------------------
;
; PESQUISA ÁRVORE
; Entrada:
;	esi= aponta a string
;	eax= extensao
;	ebx = deve apontar a base do heap local
;	edi = deve apontar o fim do heap local (base+extensão)
; Saida:
;	cy=1- falta espaço no heap local
; Altera:	edi

align 4
FindItemTree_HL32 :

red00:	push ebx
	push ecx
	push edx
	push ebp
	mov ebx,8
	cmp byte [ebx],00
	jne .ed1c
	stc
.saida:	pop ebp
	pop edx
	pop ecx
	pop ebx
	retn

;-----------------------------------------------------------------------------------------

.ed1c:	mov edi,[ebx]
.ed21:	mov [ebx+04],edi
	add edi,[ebx+08]
	mov dx,[edi-02]
	push esi
	push eax
	call CmpStr32		; compara string sensivel M/m ou não
	pop eax
	pop esi
	je .ed68
	jg .ed54
	mov edi,[ebx+04]
	mov edi,[edi+04]
	or edi,edi
	jne .ed21
	jmp .saida

;-----------------------------------------------------------------------------------------

.ed54:	mov edi,[ebx+04]
	mov edi,[edi+08]
	or edi,edi
	jne .ed21
	jmp .saida

;-----------------------------------------------------------------------------------------

.ed68:	mov edi,[ebx+04]
	jmp .saida
	ret


;-----------------------------------------------------------------------------------------
;
; RETIRA ELEMENTO DA ÁRVORE ALOCADA NO HEAP LOCAL
; Entrada:
;	esi= aponta a string
;	eax= extensao
;	ebx = deve apontar a base do heap local
;	edi = deve apontar o fim do heap local (base+extensão)
; Saida:
;	cy=1 erro - elemento nao encontrado ou arvore vazia

align 4
DeleteItemTree_HL32 :

red70:	push ebx
	push ecx
	push edx
	push edi
	push ebp
	mov ebx,8
	cmp byte [ebx],00
	jne .ed90
.erro:	stc
.saida:	pop ebp
	pop edi
	pop edx
	pop ecx
	pop ebx
	retn			; saida sem alteraçoes

;-----------------------------------------------------------------------------------------

.ed90:	mov edi,[ebx]
.ed95:	mov [ebx+04],edi
	add edi,[ebx+08]
	mov dx,[edi-02]
	push esi
	push eax
	call CmpStr32	; r960	; compara string sensivel M/m ou não
	pop eax
	pop esi
	je .eddc
	jg .edc8
	mov edi,[ebx+04]
	mov edi,[edi+04]
	or edi,edi
	jne .ed95
	jmp .erro

;-----------------------------------------------------------------------------------------

.edc8:	mov edi,[ebx+04]
	mov edi,[edi+04]
	or edi,edi
	jne .ed95
	jmp .erro

;-----------------------------------------------------------------------------------------

.eddc:	mov ebp,[ebx+04]
	mov edi,[ebp+00]
	mov esi,[ebp+04]
	or esi,[ebp+08]
	je .ee1f
	cmp byte [ebp+04],00
	jne .ee0c
	mov esi,[ebp+08]
	mov [esi],edi
	jmp .ee1f

;-----------------------------------------------------------------------------------------

.ee0c:	cmp byte [ebp+08],00
	jne .ee4f
	mov esi,[ebp+04]
	mov [esi],edi
.ee1f:	or edi,edi
.ee22:	jne .ee37
	mov [ebx],esi
.ee29:	mov esi,[ebx+04]
	call lib32		; libera bloco no heap local
	jmp .saida

;-----------------------------------------------------------------------------------------

.ee37:	cmp [edi+04],ebp
	jne .ee47
	mov [edi+04],esi
	jmp .ee29

;-----------------------------------------------------------------------------------------

.ee47:	mov [edi+08],esi
	jmp .ee29

;-----------------------------------------------------------------------------------------

.ee4f:	mov esi,[ebp+08]
	cmp byte [esi+04],00
	jne .ee73
	mov edi,[ebp+04]
	mov [esi+04],edi
	mov [edi],esi
	jmp .eec3

;-----------------------------------------------------------------------------------------

.ee73:	mov esi,[esi+04]
	cmp byte [esi+04],00
	jne .ee73
	push esi
	push dword [esi+08]
	mov esi,[esi]
	pop dword [esi+04]
	pop esi
	mov edi,[ebp+04]
	mov [esi+04],edi
	or edi,edi
	je .eead
	mov [edi],esi
.eead:	mov edi,[ebp+08]
	mov [esi+04],edi
	or edi,edi
	je .eec3
	mov [edi],esi
.eec3:	mov edi,[ebp+00]
	mov [esi],edi
	or edi,edi
	jne .eedb
.eed3:	mov [ebx],esi
	jmp $			; falta descobrir onde vai este endereço

;-----------------------------------------------------------------------------------------

.eedb:	cmp [edi+04],ebp
	jne .eeec
	mov [edi+04],edi
	jmp $			; idem

;-----------------------------------------------------------------------------------------

.eeec:	mov [edi+04],edi
	jmp $			; idem



;-----------------------------------------------------------------------------------------
