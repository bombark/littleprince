;-----------------------------------------------------------------------------------------

; Rotinas de String

;-----------------------------------------------------------------------------------------

; nomeclatura:
; cpy -> copy
; cmp -> compara
; str -> string
; Z-> string terminada com 0
; Dif -> diferencia M/M

;======================== F U N C T I O N  1 =============================================
;
; Copia uma String (strcpy)
; Entrada:
;	esi: string fonte
;	edi: string destino
;	ecx: tamanho da string fonte
; Saida:
;	nothing
align 4
cpystr:
	push ecx
	push esi
	push edi
	push ecx

	or ecx, ecx
	jz .error

	shr ecx, 2
	rep movsd
	pop ecx
	and ecx, 3
	rep movsb

	pop edi
	pop esi
	pop ecx
	ret

.error:	pop ecx
	pop edi
	pop esi
	pop ecx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
;
; Copia uma String (strcpy)
; Entrada:
;	esi: string fonte
;	edi: string destino
; Saida:
;	ecx= tamanho da string fonte
; OBS: zero termina a string


cpystr_Z:
	push eax
	push esi
	push edi

	xor ecx, ecx
.a:	lodsb
	or al, al
	je .end
	stosb
	inc ecx
	jmp .a

.end:	mov byte [edi], 0
	pop edi
	pop esi
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
;
; Compara uma String (strcmp) - Sem diferenciar M/m	(simples)
; Entrada:
;	esi: string fonte
;	edi: string destino
;	ecx: tamanho da string
; Saida:
;	nothing

cmpstr:
	push eax
	push esi
   	push edi
   	push ecx
	push eax
	and cx, 0xffff
	or ecx, ecx
	jz .b

.a:	mov al, [esi]
	mov ah, [edi]
	or ax, 0x2020
	cmp al, ah
	jnz .b
	inc esi
	inc edi
	loop .a
	pop eax
	sub cx, cx		; seta flag igual a 0
	pop ecx
	pop edi
	pop esi
	pop eax
	ret
.b:	pop eax
	pop ecx
	pop edi
	pop esi
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
;
; Compara uma String (strcmp) - Diferencia M/m		(simples)
; Entrada:
;	esi: string fonte
;	edi: string destino
;	ecx: tamanho da string
; Saida:
;	nothing

cmpstrDif:
	push eax
	push esi
   	push edi
   	push ecx
	push eax
	and cx, 0xffff
	or ecx, ecx
	jz .b
.a:	mov al, [esi]
	mov ah, [edi]
	cmp al, ah
	jne .b
	inc esi
	inc edi
	loop .a
	pop eax
	sub cx, cx		; seta flag igual a 0
	pop ecx
	pop edi
	pop esi
	pop eax
	ret
.b:	pop eax
	pop ecx
	pop edi
	pop esi
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
;
; Compara uma String (strcmp) - Sem diferenciar M/m
; Entrada:
;	esi: string fonte
;	edi: string destino
; Saida:
;	nothing
; OBS: zero termina a string

cmpstr_Z:

	push eax
	push esi
	push edi

.a:	lodsb
	or al, al
	je .end
	mov ah,al
	mov al, [edi]
	inc edi
	or al, al
	je .ok
	or ax, 0x2020
	cmp al,ah
	je .ok
	jmp .a

.end:	stc

.ok:	pop edi		; se iguais Z=1 se diferentes Z=0 cy=1 -> string fonte menor
	pop esi		;			    cy=0 -> string destino menor
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
;
; Compara uma String (strcmp) -
; Entrada:
;	esi: string fonte
;	edi: string destino
; Saida:
;	nothing
; OBS: zero termina a string

cmpstr_ZDif:

	push eax
	push esi
	push edi

.a:	lodsb
	or al, al
	je .end
	mov ah,al
	mov al, [edi]
	inc edi
	or al, al
	je .ok
	cmp al,ah
	je .ok
	jmp .a

.end:	stc

.ok:	pop edi		; se iguais Z=1 se diferentes Z=0 cy=1 -> string fonte menor
	pop esi		;			    cy=0 -> string destino menor
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
;
; COMPARA STRING INSENSIVEL m/M
; Entrada:
; 	esi= string a comparar
; 	ax= extensao da string
; 	edi= string padrao
; 	dx= extensao da string padrao
; Saida:
;	z= 1 strings iguais
;	z= 0 	s=1	string  < padrao
;		s= 0 	string >  padrao


align 4
CmpStr32 :
	push eax
	push esi
	push edi
	push ecx
	push eax

	mov cx, dx
	cmp ax, dx
	ja .a
	mov cx, ax
.a:	mov al, [esi]
	mov ah, [edi]
	or eax, 0x2020
	cmp al, ah
	jne .b
	inc esi
	inc edi
	loop .a

	pop eax
	sub eax, edx
	pop ecx
	pop edi
	pop esi
	pop eax
	ret

.b:	pop eax
	pop ecx
	pop edi
	pop esi
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
;
; COMPARA STRING (DESPREZA O EXCESSO DE ESPACOS) insensivel a M / m
; Entrada:
;	esi= string a comparar
;	ax= extensao da string
;	edi= string padrao
;	dx= extensao da string padrao
; saida:
;	z= 1 strings iguais
;	z= 0 	s=1	string  < padrao
;		s= 0 	string >  padrao

align 4
CmpStr_32RE :
	push ecx
	push ebx
	mov ecx,edx
	cmp eax,edx			; escolhe a menor extesão
	ja .a
	mov ecx,eax
.a:	mov bl,[esi]
	cmp bl,byte 0x20
	je .f
.b:	mov bh,[edi]
	or ebx,dword 0x2020
	cmp bl,bh
	jne .e
	inc esi
	inc edi
.c:	loop .c
.d:	sub eax,edx
.e:	pop ebx
	pop ecx
	retn
.f:	mov bh,[edi]
	or ebx, dword 0x2020
	cmp bl,bh
	jne .e
	inc esi
	inc edi
	dec ecx
	je .d
.g:	mov bl,[esi]
	cmp bl,byte 0x20
	jne .b
	dec eax
	inc esi
	jmp .g
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  9 =============================================
;
; PESQUISA COINCIDÊNCIA
; Entrada:
;	esi= string a procurar
;	ax= extensão da string
;	edi= ponteiro da sting texto
;	dx= extensão string texto (permite comparar uma string com até 64kB)
; Saida:
;	cy=1 	padrão não encontrado
;	cy=0 OK, 	edi aponta o local da coincidência

align 4
Pesq_coinc32 :
	push ebp
	push ebx
	push ecx
	xor ebx,ebx
	xor ecx,ecx
	mov bx,ax
	mov ebp,esi
	dec ebx
	cld
.a:	mov esi,ebp
	lodsb
	mov ecx,edx
	repne scasb
	jne .erro
	mov edx,ecx
	mov ecx,ebx
	repe cmpsb
	je .fim
	add edi,ecx
	sub edi,ebx
	ja .a
	jmp .erro
.fim:	sub edi,ebx
	dec edi
	clc
	pop ecx
	pop ebx
	pop ebp
	retn
.erro:	stc
	pop ecx
	pop ebx
	pop ebp
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 0 ===========================================
;
; COMPARA STRING (uso geral, diferencia M de m)
; Entrada:
;	esi= base da string 1
;	ebx= extensao da string 1
;	edi= base da string 2
;	edx= extensao da string 2
; Saida:
;	z=1 	string iguais
;	z=0 	s=0	string1 > string2
;		s=1	string1 < string2

align 4
Cmp_Mm32 :
	pop ecx
	mov ecx,edx
	cmp ebx, edx
	ja .a
	mov ecx, ebx
.a:	repe cmpsb
	je .b
	pop ecx
	retn
.b:	sub ebx, edx
	pop ecx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 1 ===========================================
;
; PREENCHE BLOCO COM ZEROS
; entrada:
;	eax= extensão em bytes
;	edi= ponteiro
; saida:
;	nada

align 4
limpa_bloco:
	push esi
	push ecx
	mov ecx, eax
	shr ecx, 2
	or ecx, ecx
	jz .b
.a:	mov dword [esi], 0
	add esi, 4
	loop .a
.b:	mov ecx, eax
	and ecx, 0x3
	jz .fim
.c:	mov byte [esi], 0
	inc esi
	loop .c
.fim:	pop ecx
	pop esi
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 2 ===========================================
;
; PREENCHE UM BLOCO COM ZEROS
; entrada:
;	ecx= extensão do bloco
;	ds:esi= ponteiro
; saida:
;	ecx inalterado

align 4
limpa_bloco_CX32 :
	push esi
	push ecx
	shr ecx, 2
	or ecx, ecx
	jz .b
.a:	mov dword [esi], 0
	add esi, 4
	loop .a
.b:	pop ecx
	and ecx, 0x3
	jz .fim
.c:	mov byte [esi], 0
	inc si
	loop .c
.fim:	pop esi
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 3 ===========================================
; Devolve o tamanho da string terminada em 0
; IN:
;	esi= string
; OUT:
;	ecx= tamanho

strlen:
	push eax
	push esi

	xor ecx, ecx
.a:	mov al, [esi]
	or al, al
	jz .end
	inc esi
	inc ecx
	jmp .a

.end:	pop esi
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 4 ===========================================
; Retorna o tamanho da primeira linha encontrada da string
; IN:
;	esi: ponteiro do texto
; OUT:
;	ecx: Tamanho da linha do texto

String_GetLineSize:
	push eax
	xor ecx, ecx
	xor eax, eax

.loop:	mov al, [esi+ecx]
	or al, al
	jz .end2
	cmp ax, 0x0d0a
	je .end1
	mov ah, al
	inc ecx
	jmp .loop

.end1:	dec ecx
.end2:	clc
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 5 ===========================================
; Concatena 2 string
; IN:
;	esi: 1° stringZ 
;	edi: 2° stringZ (onde sera concatenado)
; OUT:
;	ecx: tamanho da nova string
;	edi: final da string

strcat:
	push eax
	push esi
	
  ; Pega o final da string
  	xor ecx, ecx	
.loop:	mov al, [edi]
	or al, al
	jz .copy
	inc edi
	inc ecx
	jmp .loop
	
  ; Copia a string	
.copy:	lodsb
	or al, al
	jz .end
	stosb
	inc ecx
	jmp .copy

  ; Fim
.end:	mov byte [edi], 0
	pop esi
	pop eax
	ret

;-----------------------------------------------------------------------------------------
