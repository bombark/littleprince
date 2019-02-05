;======================== F U N C T I O N  1 =============================================
; Cria um item
; IN:
;	 al= Atributo do Item 
;	edx= Nome normalizado do item
;	esi= Data Block
; OUT:
;	edx= Posicao do item no BD_Folder
; O diretorio já deve estar aberto no Data Block

align 4
FAT_Item_Create:
	push eax
	push ecx
	push esi	
		
  ; Procura um item livre		
.loop1:	xor ecx, ecx
	add esi, BD_Folder
.loop2:	cmp byte [esi+ecx], 0
	je .found
	add ecx, 0x20
	cmp ecx, 0x200
	jb .loop2
	call FAT_Folder_Read 	
	jnc .loop1
	;call FAT_Folder_Expand
	jmp .loop1
	
.found:	push ecx
	add esi, ecx
	
  ; Coloca as variaveis
	xor ecx, ecx
	mov [esi+FileAttr], al
	mov [esi+Dir_FileSize], ecx	; zera extensao
	mov [esi+FAT32_HC], cx	; 1o cluster
	mov [esi+FAT16_C], cx	; 1o cluster

    ; Coloca a data e hora	
	; FAZER	
		
    ; Coloca o nome curto    		
	mov ecx, 11
.a:	mov al, [edx]
	mov [esi], al
	inc esi
	inc edx
	loop .a
	pop edx
	
  ; FIM	
.end:	pop esi
	pop ecx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Elimina um item
; IN:
; OUT:


align 4
FAT_Item_Destroy:
  ; Elimina o item
  ; Zera os clusters do item

	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Pesquisa um Item apenas em um diretorio
; IN:
;	eax= first cluster of the directory
; 	 bl= partição
;	edx= pointer to filename
;	esi= Data Block Memory Base
; OUT:
;	eax = primeiro cluster do item
;	edx = Aponta para o proximo nome apos a barra (c:\felipe\programas\jogos\)
;         edi = pointer in buffer (BD_Folder)
;	cy = 0 -> OK
;	cy = 1 -> Item nao encontrado

align 4
FAT_Item_Search0:
	push ecx
	push esi		
					
	call FAT_Item_NormalizeName					
	call FAT_Folder_OpenCluster				
	
	mov edi, esi
	add edi, BD_Folder
	mov ebp, edi	
	add ebp, 0x200	; Tamanho do diretorio na memoria 
	
  ; Procura o item no setor carregado	
.search:	;cmp [FileAttr], apagado
	;je .next
	;cmp [FileAttr], nome
	;je .next
	cmp dword [edi], 0
	je .error
	
	push esi
	push edi
	add esi, BD_Buffer  
	mov ecx, 0xb
	rep cmpsb
	pop edi
	pop esi
	je .found
	
.next:	add edi, 0x20
	cmp edi, ebp
	jb .search	
	
  ; Lê o proximo setor do diretorio	
	call FAT_Folder_Read
	jz .error	
	jmp .search

  ; Fim -> Encontrou o item
.found:	mov eax, [edi+FAT32_HWC]
	mov ax, [edi+FAT16_C]
	xor ebp, ebp
	pop esi
	pop ecx
	ret

  ; Error -> Nao encontrou o item
.error:	stc
	mov ebp, ERROR_FILE_NOT_FOUND
	pop esi
	pop ecx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Procura um arquivo ou pasta apontado por uma path (\Jogos\TC\arquivo.txt)
; IN:
;	eax= Cluster do diretorio
;	edx= Path
;	esi= Data Block Memory Base
; OUT:
;	eax= Cluster do item
;	edx= Ponteiro do item na memoria
;	cy= 0  -> OK
;	cy= 1  -> Error

align 4
FAT_Item_Search:
	push edi

.search:	cmp byte [edx], 0
	jz .end
	call FAT_Item_Search0					
	jc .error			; error -> Arquivo nao encontrado
	
	cmp byte [edx], '/'
	jnz @f
	inc edx
	jmp .search
	
@@:	cmp byte [edx], '\'
	jnz .end			; item encontrado
	inc edx
	jmp .search
	
	;cmp byte [edi+FileAttr], 0x10	; trava - colocar isso no FAT_Item_Search0
	;jne .search

  ; Item encontrado
.end:	clc			; cmp seta o carry 
	mov edx, edi
	pop edi
	ret

  ; Error - Item nao encontrado ou endereco invalido
.error:	mov ebp, ERROR_FILE_NOT_FOUND		; arquivo nao encontrado
	cmp byte [edx], '/'
	je .path	
    	cmp byte [edx], '\'
    	je .path
    	jmp .Erend

.path:	cmp byte [edx+1], 0
	jz .Erend
	mov ebp, ERROR_PATH_INCORRECT		; ou endereco incorreto

.Erend:	xor edx, edx
	stc
	pop edi
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Devolve as informacoes do item
; IN:
;	esi= Data Block Memory Base
; OUT:

align 4
FAT_Item_GetInfo:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; IN:
;        	edx= pointer to filename
; 	esi= data block memory base (buscar na tabela de partições)
; OUT:
;	cy=0 nothing -> 
;	 	edx= proxima pasta
;		file name normalize in FAT_FNbuffer 
;	cy=1 don't occur
; Testada-OK - (short name)

align 4
FAT_Item_NormalizeName:
	push esi edi ecx eax
	lea edi, [esi+BD_Buffer]
	mov esi, edx
	xor ecx,ecx
	mov cl, 8
  .a:	mov al, [esi]
	or al,al
	jz .complete	; zero ou ponto termina o nome
	cmp al, '.'	; zero= completa todo o resto com espaço inclusa a extensao
	jz .ponto		; ponto= completa o resto de 8 com espaço e busca a extensao
	cmp al, '/'
	jz .complete
	cmp al, '\'
	jz .complete


  .restart:
	or al, al
	jz .end
	cmp al, 0x61
	jb @f
	and al, 0xdf
 @@:	mov [edi], al
	inc esi
	inc edi
	dec ecx
	jnz .a

;verify long name
	mov al, [esi]
	cmp al, '.'
	jnz .trunk
	inc esi
	jmp .extension

  
  
  .ponto:			; completa o restante com espaços
	inc esi
	mov al, 0x20
  .b:	mov [edi], al
	inc edi
	dec ecx
	jnz .b

; transfer extension name
  .extension:		; poe a extensao e termina
	mov cl, 3
  @@:	mov al, [esi]
	and al, 0xdf
	mov [edi], al
	inc esi
	inc edi
	loop @b

  	
.end: 	mov edx, esi
	mov [edi],word 0    ; nao precisa ter zero no final para terminar um nome normal
	pop eax ecx edi esi
	ret



; complete name
  .complete:				; completa nome e extensao com espaços
	add cl, 3			; se encontra / ou \ continua, senao termina
	mov ah, 0x20
  @@:	mov [edi], ah
	inc edi
	dec ecx
	jnz @b
	mov cl, 8
	jmp .end

  .trunk:
	mov cl, 3
  .e:	mov ah, 0x20
	mov [edi], ah
	inc edi
	loop .e
	mov [edi], word 0x0f00
  .f:	inc esi
	mov al, [esi]
	cmp al, 0
	jz .end
	cmp al, '/'
	jz .continue
	cmp al, '\'
	jnz .f

  .continue:
	inc esi
	mov al, [esi]
	jmp .restart
	
;-----------------------------------------------------------------------------------------



;======================== F U N C T I O N  1 =============================================
; Transforma nome curto para normal (arq01   bmp para arq01.bmp)
; IN:
;	esi: string com nome curto
;	edi: string com nome normal  (terminado em ex: db "felipe",0)
; OUT:
;	nothing

Fat_NormaToName:
	push eax
	push ecx
	push esi
	push edi
	xor ecx, ecx
	mov cl, 7

  ; copia as letras do nome
.loop:	lodsb
	cmp al, 0x20
	je .lp
.put:	stosb
	dec cl
	jnz .loop

  ; coloca a extensao
    ; verifica se existe extensao
.ext:	mov eax, [esi]
	and eax, 0xffffff
	cmp eax, 0x202020
	je .fim

    ; Copia a extensao
	mov byte [edi], '.'
	inc edi
	mov [edi], eax

  ; fim
.fim:	mov byte [edi], 0
	pop edi
	pop esi
	pop ecx
	pop eax
	ret

  ; retira os espacos em branco antes da extensao do nome
.lp:	lodsb
	cmp al, 0x20
	jne .put
	dec cl
	jnz .lp
	jmp .ext

;-----------------------------------------------------------------------------------------



;======================== F U N C T I O N  7 =============================================
; Lê um setor de um cluster de um item (arquivo ou pasta)
; IN:
;	eax = First cluster to read
;	bl  = Partition
;	ecx = # blocks to skip over
;	edx = # blocks to read in buffer
;	esi= Data Block Memory Base
;	edi= Buffer
; OUT:

align 4
FAT_Item_ReadCluster:
	push eax
	push ecx
	mov ebp, ecx

	cmp eax,[esi+BD_HD_LastCluster]
	ja .error1             		; too big cluster number, something is wrong

	sub eax,2
	movzx ecx, byte [esi+BD_FAT_SPC]
	imul eax,ecx
	add eax,[esi+BD_HD_Lba]	; deve apontar a posição root_lba - posição do cluster 1

.read:	test ebp,ebp               	; first wanted block
	je .SectorRead                ; yes, skip count is 0
	dec ebp
	test edx,edx               	; is all read?
	je .error1
	inc eax                   	; next sector
	dec edx			; avança setores sem leitura
	jnz .read

.SectorRead:
	call ReadSector0		;	
	jb .error1		; vem com codigo de erro da controladora
	add edi,512               	; update pointer
	dec ecx
	je .error1		; cluster is over
	inc eax                   	; next sector
	dec edx
	jnz .read

  ; Error - Fora de leitura
.error1:	pop ecx
	pop eax 	
	ret

  ; Error -  
.error2:	pop ecx
	pop eax	
	stc
	ret
	
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; Lê um segmento de um item (arquivo/pasta)
; IN:
;        	eax= first cluster
; 	bl= partition (*)
;	ecx= setores a ser desconsiderados
;	edx= quantidade de setores a serem lidos
; 	esi= data block memory base (buscar na tabela de partições) (*)
;	edi= destin memory base
; OUT:
;	cy=0 eax= 0, edx, ebp = 0, edi, esi, ebx = inalterados
;	cy=1 ebp= 6 - end of file
;                   9 - fat table corrupted
;                  10 - access denied

align 4
FAT_Item_Read:
.new:	
	call FAT_Item_ReadCluster	
	jc .eof         		; end of file or cluster out of range

	test edx,edx               	; is all read?
	je .OK          		; yes

	call FAT_Get              	; get next cluster	
	jb .access_denied

	cmp eax,FATreserved     	; end of file
	jnb .eof
	cmp eax,2                 	; incorrect fat chain
	jnb .new
	mov ebp, ERROR_FAT_TABLE		; (9)
	ret

.eof:	mov ebp, ERROR_END_OF_FILE 		; (6)
	ret

.OK:	xor ebp,ebp
	ret

.access_denied:
	mov ebp,ERROR_ACCESS_DENIED		; (10)
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  9 =============================================
; Escreve dados num item
; IN:
;	eax= Lba
;	ecx= Quantidade de Setores
;	edx= Primeiro Cluster do Arquivo
;	esi= Data Block Memory Base
;	edi= Buffer
; OUT:
;	cy= 0  -> OK
;	cy= 1  -> Error

align 4
FAT_Item_WriteCluster:
	ret


;-----------------------------------------------------------------------------------------
	