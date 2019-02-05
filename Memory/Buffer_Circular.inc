;-----------------------------------------------------------------------------------------

; Rotinas de Buffer Circular

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

; Descritor do Buffer Circular
   BC_Pread	equ 0	; db - Ponteiro de leitura
   BC_Pwrite	equ 1	; db - Ponteiro de escrita
   BC_Size	equ 2	; db - Tamanho do buffer
   BC_Rs		equ 3	; db - Free
   BC_Buffer	equ 4	; dd - Ponteiro do buffer

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Cria um buffer circular
; IN:
;	ebx: descritor do Heap
;	   ebx = 0  -> aloca no heap principal
;	   ebx!= 0  -> aloca no heap apontado por ebx 
;	 cl: tamanho do buffer (Maximo 256 bytes)
;	esi: descritor do buffer
; OUT:
;	cy= 0 -> OK
;	cy= 1 -> Error, sem espaço 

BC_Create:
	push eax
	push esi
	push edi
	mov edi, esi
	or cl, cl
	jnz .a
	mov cl, 0x20
	
; Zera as variaveis	
.a:	xor al, al
	mov [edi], al	; zera ponteiro de escrita
	mov [edi+BC_Pread], al
	mov [edi+BC_Size], cl
	movzx eax, cl
	or ebx, ebx
	jnz .local
	
; Aloca buffer no heap principal	
	call SMM_MainHeap_Alloc
	mov eax, esi
	jc .error
	jmp .ok

; Aloca buffer no heap local	
.local:	mov eax, ecx	
	call SMM_Heap_Alloc
	jc .error
.ok:	mov [edi+BC_Buffer], eax

	pop edi
	pop esi
	pop eax
	ret

; Error - Sem espaco
.error:	mov dword [edi+BC_Buffer], 0
	pop edi
	pop esi
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Destroi um buffer circular
; IN:
;	ebx: descritor do heap
;	  ebx= 0 -> heap principal
;	  ebx!=0 -> heap apontado por ebx
;	esi: descritor do buffer
; OUT:
;	nothing

BC_Destroy:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Colocar um byte no buffer circular 
; IN:
;	al: dados
;	esi: descritor do buffer
; OUT:
;	cy= 0 -> ok
;	cy= 1 -> error, buffer cheio

BC_putByte:
	push ebx
	
; Arruma os ponteiros
	movzx ebx, byte [esi+BC_Pwrite]
	inc bl
	cmp bl, [esi+BC_Size]
	jb @f
	xor bl, bl
@@:	cmp bl, [esi+BC_Pread]
	je .error				; Buffer full
	mov [esi+BC_Pwrite], bl
	add ebx, [esi+BC_Buffer]
	
; Coloca os dados	
.put:	clc
	mov [ebx], al		; tipo da acao
	pop ebx
	ret

; Error -> Buffer Cheio
.error:	stc
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Colocar um word no buffer circular 
; IN:
;	ax: dados
;	esi: descritor do buffer
; OUT:
;	cy= 0 -> ok
;	cy= 1 -> error, buffer cheio

BC_putWord:
	push ebx
	
; Arruma os ponteiros	
	movzx ebx, byte [esi+BC_Pwrite]
	inc bl
	inc bl
	cmp bl, [esi+BC_Size]
	jb @f
	xor bl, bl
@@:	cmp bl, [esi+BC_Pread]
	je .error				; Buffer full
	mov [esi+BC_Pwrite], bl
	add ebx, [esi+BC_Buffer]
	
; Coloca os dados	
.put:	clc
	mov [ebx], ax		; tipo da acao
	pop ebx
	ret

; Error -> Buffer Cheio
.error:	stc
	pop ebx
	ret
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Colocar um dword no buffer circular 
; IN:
;	eax: dados
;	esi: descritor do buffer
; OUT:
;	cy= 0 -> ok
;	cy= 1 -> error, buffer cheio

BC_putDword:
	push ebx
	
; Arruma os ponteiros	
	movzx ebx, byte [esi+BC_Pwrite]
	add bl, 4 
	cmp bl, [esi+BC_Size]
	jb @f
	xor bl, bl
@@:	cmp bl, [esi+BC_Pread]
	je .error				; Buffer full
	mov [esi+BC_Pwrite], bl
	add ebx, [esi+BC_Buffer]
	
; Coloca os dados	
.put:	clc
	mov [ebx], eax		; tipo da acao
	pop ebx
	ret

; Error -> Buffer Cheio
.error:	stc
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Pega um byte no buffer circular 
; IN:
;	esi: descritor do buffer
; OUT:
;	al: dados
;	cy = 0 -> ok
;	cy = 1 -> Buffer vazio

BC_getByte:
	push ebx

; Ajusta ponteiros de leitura
	movzx ebx, byte [esi+BC_Pread]
	cmp bl, [esi+BC_Pwrite]
	je .empty			; Buffer full
	inc bl
	cmp bl, [esi+BC_Size]	
	jb .a
	xor bl, bl
.a:	mov [esi+BC_Pread], bl
	add ebx, [esi+BC_Buffer]	
	
; Lê o byte	
	mov al, [ebx]
	pop ebx
	ret

; Error - Buffer vazio
.empty:	xor al, al
	stc
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Pega um word no buffer circular 
; IN:
;	esi: descritor do buffer
; OUT:
;	ax: dados
;	cy = 0 -> ok
;	cy = 1 -> Buffer vazio

BC_getWord:
	push ebx

; Ajusta ponteiros de leitura
	movzx ebx, byte [esi+BC_Pread]
	cmp bl, [esi+BC_Pwrite]
	je .empty			; Buffer full
	inc bl
	inc bl
	cmp bl, [esi+BC_Size]	
	jb .a
	xor bl, bl
.a:	mov [esi+BC_Pread], bl
	add ebx, [esi+BC_Buffer]	
	
; Lê o byte	
	mov ax, [ebx]
	pop ebx
	ret

; Error - Buffer vazio
.empty:	xor ax, ax
	stc
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; Pega um dword no buffer circular 
; IN:
;	esi: descritor do buffer
; OUT:
;	eax: dados
;	cy = 0 -> ok
;	cy = 1 -> Buffer vazio

BC_getDword:
	push ebx

; Ajusta ponteiros de leitura
	movzx ebx, byte [esi+BC_Pread]
	cmp bl, [esi+BC_Pwrite]
	je .empty			; Buffer full
	add bl, 4
	cmp bl, [esi+BC_Size]
	jb .a
	xor bl, bl
.a:	mov [esi+BC_Pread], bl
	add ebx, [esi+BC_Buffer]	
	
; Lê o byte	
	mov eax, [ebx]
	pop ebx
	ret

; Error - Buffer vazio
.empty:	xor eax, eax
	stc
	pop ebx
	ret

;-----------------------------------------------------------------------------------------