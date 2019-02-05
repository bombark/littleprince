; Manager file nominated by AsciiZ
;
; Abre o arquivo -> diretorio/diretorio/arquivo.ext = fopen
; devolve em esi= ponteiro do item e ebx= diretorio
; Entrada:
;	ebx: diretorio inicial
;	esi: string (diretorio/diretorio/arquivo.ext) (stringz)
;	cx: ext da string
; Saida:
;	ebx: o diretorio onde está o arquivo
;	esi: aponta o arquivo
; testar
; nao deve ter a unidade, pois admite-se aqui que esta já tenha sido escolhida em função
; do tipo da partição. O nome deve estar normalizado...isto é nao deve ter espaços em
; excesso, pois estes contam na extensao do nome.
align 4
SearchFile:

	push eax
	push ecx
	push edx

; corre o nome atras de / ou \
	xor edx, edx
.a:	mov al, [esi+edx]	; busca arquivo se chegou ao final se achar / ou \
	or al, al
	jz .c
	cmp al, '/'
	je .b
	cmp al, '\'
	je .b
	inc edx
	jmp .a		; repete até o final do nome

; busca diretório se achar / ou \. repete a busca por arquivo
.b:
	call SearchFolder
	jc .error
	inc edx
	inc edx
	jmp .a

; busca arquivo
.c:
;int 1
	push edi
	call SearchItem			; eax= attrib/type, edi=item
	mov esi, edi
	pop edi
	jc .error
.fim:	clc
	pop edx
	pop ecx
	pop eax
	ret

.error:	stc
	pop edx
	pop ecx
	pop eax
	ret


; pesquisa existencia de pasta - se ela existe é transferida para a memoria
; Entrada:
;	ebx: diretorio
;	esi: string
; Saida:
;	ebx: diretorio do item (novo diretório carregado)
; testar
align 4
SearchFolder :
	push eax
	push ecx
	push edi
;int 1
	call SearchItem		;
;int 1
	jc .fim

	mov eax, [ebx+edi+I_diretorio]; <> 0 se estiver na memoria

	or eax, eax
	jz .fim
	mov esi, edi

	call carrega_diretorio	; transfere o diretório para a memoria

	pop edi
	pop ecx
	pop eax
	ret

.fim:	stc
	pop edi
	pop ecx
	pop eax
	ret



;
; Encontra nome na lista - pesquisa item no diretorio apontado por ebx		OK
; Entrada:
;	ebx: diretorio
;	esi: nome

; Saida:
;	cy=0 -> O item foi encontrado - edi= base do item , eax= attrib/tipo
;	cy=1 -> não foi encontrado o item - edi= 0
;
; testar
align 4
SearchItem:

	push ecx
	mov edi, [ebx+D_ini_item]
.a:	or edi, edi
	jz .erro
	movzx ecx, byte [ebx+edi+I_ext_nome]
	push edi
	mov edi, [ebx+edi+I_nome]
	add edi, ebx
	call cmpstr		; comparação insensivel M/m, ecx= ext strin
	pop edi
	jz .achou
.b:	mov edi, [ebx+edi+I_prox]
	jmp .a

.achou:	mov ax, [edi+I_attrib]
	shl edx, 0x10
	mov ax, [ebx+edi+I_tipo]	; tipo (pasta, icone, arquivo,...) 	2 bytes
	clc
	pop ecx
	ret
.erro:	stc
	pop ecx
	ret

