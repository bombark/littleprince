;-----------------------------------------------------------------------------------------
;
; LITTLE PRINCE Operational System
; Copyright 2002 Paolo Minazzi, paolo.minazzi@inwind.it
; Copyright (C) KolibriOS team 2004-2007. All rights reserved
; Copyright (C) 2008 to 2011 - LP Development Team
;
;-----------------------------------------------------------------------------------------
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;
;-----------------------------------------------------------------------------------------
;
; @Id: fat32.inc - 2008-01-27 Modified and adapted by cb-toledo
; This is the main compilable file that groups all others include files of the project
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------
;
; FAT16/32 functions for LP_system
; The fat units have not any protection and can be open by any user
; This routines enable read/write files with short filenames
;
;
; Principio de funcionamento basico:
; Para usar uma unidade fat, o usuario deve antes abrir a unidade. isto cria um bloco de
; dados basicos no heap, com 0x800 bytes, usados pelo SO para manipular o boot, fat e dir
; da unidade. Por enquanto, a manipulação destes dados é feita a um setor de cad
;
;
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
;
; in:
;	eax = cluster
;	edx = value to save
;	bl= partition (*)
;	esi= unit data block (*)
; out:
;	EDX = old value

align 4
set_fat:

	push eax ebx ;ebp
	lea edi,[esi+FAT_base]
	cmp eax,2
	jb .error    		; cluster deve ser maior que 2
	cmp eax,[esi+Last_cluster]
	ja .error

	test byte [esi+FAT_Type], 0x01	; confirma FAT16
	jz @F				; Se for 16 pula
	add eax,eax

  @@:	add eax,eax
	mov ebp,511
	and ebp,eax		; esi = position in fat sector
	shr eax,9			; eax = fat sector
	add eax,[esi+FATbase_lba]
	cmp eax,[esi+FAT_lba]	; is fat sector already in memory?
	je .fat_loaded		; yes

	cmp byte [esi+FAT_changed],0	; is fat changed?
	je .no_changed		; no
	call write_fat_sector	; yes. write it into disk
	jb .error

  .no_changed:
	mov [esi+FAT_lba],eax    	; save fat sector
	call ReadSector0
	jb .error

  .fat_loaded:
	test word [esi+FAT_SPF], 0xffff
	je .test32

  .set16:
	xchg [edi+ebp],dx		; save new value and get old value
	jmp .write

  .test32:
	mov eax,FATmask		;

  .set32:
	and edx,eax
	xor eax,-1                	; mask for high bits
	and eax,[edi+ebp]         	; get high 4 bits
	or eax,edx
	mov edx,[edi+ebp]         	; get old value
	mov [edi+ebp],eax         	; save new value

  .write:
	mov byte [esi+FAT_changed], 1 ; fat has changed

  .nonzero:
	and edx,FATmask		; ???

  .error:
	pop  ebx eax	;ebp
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
;
; bloco da unidade deve estar no heap.
; in:
;	EAX = cluster
;	esi= unit data block
;	bl= partition
; out:
;	EAX = next cluster

align 4
get_fat:

	push edi		;ebp
	lea edi,[esi+FAT_base]		; edi= base fat_buffer

	test byte [esi+FAT_Type], 0x01	; confirma FAT16
	jz @F				; Se for 16 pula
	add eax,eax
  @@:
	add eax,eax		; cluster = bloco de setores.

	mov ebp,0x200-1
	and ebp,eax		; esi = position in fat sector
	shr eax,9			; eax = fat sector
	add eax,[esi+FATbase_lba]	; salva a lba da fat em memoria
	cmp eax,[esi+FAT_lba]	; is fat sector already in memory?  (FAT_lba)
	je .fat_loaded

	cmp byte [esi+FAT_changed],0	; is fat changed?
	je .fat_no_changed		; no
	call  write_fat_sector	; yes. write it into disk
	jb .error1

  .fat_no_changed:			; busca novo segmento da fat
	mov [esi+FAT_lba], eax	; eax = lba da fat

	call ReadSector0		; altera eax

	jb .error1

  .fat_loaded:
	mov eax,[edi+ebp]		; obtem cluster
	and eax,FATmask		; ????

  .error1:
	pop edi ;ebp
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
;
; pesquisa cluster livre na fat
; in:
;	EAX = # cluster for start the searching
;	bl = partition
;	esi= data block memory base
; out:
;	if CARRY=0 EAX = # first cluster found free
;	if CARRY=1 disk full

align 4
get_free_fat:

	push ecx
	mov ecx,[esi+Last_cluster]	; counter for full disk
	sub ecx,2			; começa do cluster dois

  .test:
	cmp eax,[esi+Last_cluster]	; if above last cluster start at cluster 2
	jbe .in_range
	mov eax,2

  .in_range:
	push eax
	call get_fat		; get cluster state
	jb .not_found_1

	test eax,eax		; is it free?
	pop eax
	je .found 		; yes
	inc eax 			; next cluster
	dec ecx  			; is all checked?
	jns .test			; no
	jmp .not_found

  .not_found_1:
	add   esp,4
  .not_found:
	pop   ecx			; yes. disk is full
	stc
	ret

  .found:
	pop   ecx
	clc
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
;
; write changed fat to disk							OK
; in:
;	bl= partition
;	esi= unit data block

align 4
write_fat_sector:

	push eax edi ecx

	and byte [esi+FAT_changed],0
	mov eax,[esi+FAT_lba]
	cmp eax,-1
	jz .fim
	lea edi, [esi+FAT_base]		; edi= fat base
	mov ecx,[esi+FAT_QTDE]

  .next_fat:
	push eax
	call WriteSector0			; write command
	jb .error
	pop eax

	add eax, [esi+FAT_SPF]
	dec ecx
	jnz .next_fat

  .fim:	pop ecx ebx eax			; if cy=1 -> error
	ret

  .error: add esp, 4
	jmp .fim

;-----------------------------------------------------------------------------------------




;======================== F U N C T I O N  6 =============================================
;
; carrega dados referente ao cluster eax. (permite carga parcial)			OK
; input:
;	EAX = first cluster to read
;	EDI = pointer to buffer
;	EDX = # blocks to read in buffer
;	EBP = # blocks to skip over
;	bl= partition
; 	esi= data block memory base
; output:
;	if CARRY=0 ok -> eax, edi= no changes,
;	if CARRY=1 cluster out of range
; Note:	if cluster=0 it's changed to read rootdir

align 4
get_data_cluster:
	push eax ;ecx
	mov ebp, ecx

	;;mov byte [esi+FAT16_root],0
	cmp eax,[esi+Last_cluster]
	ja .error             		; too big cluster number, something is wrong

; ver o que fazer futuramente....
;    cmp eax,2
;    jnb .cluster
;jmp .error	; se for menor que 2...erro ou buscar o setor root
    ;mov eax,[esi+FAT_RootCluster]   ; if cluster < 2 then read rootdir
    ;;cmp byte [esi+FAT_type],16
    ;;jne .cluster
    ;mov eax,[esi+RootBase]
    ;mov ecx,[esi+Root_lba]    		; Note: not cluster size
    ;;mov byte [esi+FAT16_root],1        	; flag for fat16 rootdir
    ;jmp .read


  .cluster:
	sub eax,2
	movzx ecx, byte [esi+FAT_SPC]
	imul eax,ecx
	add eax,[esi+Database_lba]	; deve apontar a posição root_lba - posição do cluster 1

  .read:
	test ebp,ebp               	; first wanted block
	je .SectorRead                 	; yes, skip count is 0
	dec ebp
	test edx,edx               	; is all read?
	je .out_of_read
	inc eax                   	; next sector
	dec edx			; avança setores sem leitura
	jnz .read

  .SectorRead:
	call ReadSector0		;
	jb .out_of_read		; vem com codigo de erro da controladora
	add edi,512               	; update pointer
	dec ecx
	je .out_of_read		; cluster is over

	inc eax                   	; next sector
	dec edx
	jnz .read

  .out_of_read:
	pop eax 			;ecx
	ret

  .error:

	pop  eax			; ecx
	stc
	ret


; OBS:
; permite selecionar apenas 1 setor num determinado cluster, onde ebp indica a quantidade
; de setores a pular e edx indica a quantidade total de setores a serem lidos...
; tipo de 8, pular 4 e ler 1: ebp=4, edx=5


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
;
; obtem cluster de um item percorrendo o path					OK
; input:
;	edx = pointer to a path string
;         (example: the path "/files/data/document" become
;                             "files......data.......document...0"
;         '.' = space char
;         '0' = char(0) (ASCII=0) !!! )
; 	esi= data block
;	bl= partition
;
; output : if (CARRY=1) -> ERROR in the PATH
;          if (CARRY=0) -> EAX=cluster, edx= item memory point in directory
; Testada-OK
align 4
get_cluster_of_a_path:

  .a:	mov eax,[esi+FAT_RootCluster]
  .b: 	mov ebp, edx

.search:
	cmp byte [ebp],0		; fim do path
	je .not_found		; je .found
	mov edx,ebp

	call analyze_directory	; bl=partiçao, eax, lba, edx= filename, esi= data block
	jc .found

	mov eax,[edx+FAT32_HWC]       ; read the HIGH 16bit cluster field
	mov ax,[edx+FAT16_C]          ; read the LOW 16bit cluster field

	and eax,FATmask
	add ebp,11                	; 8+3 (name+extension)
	jmp .search

  .found:
	clc                         	; no errors
	ret

  .not_found:
	stc                         	; errors occour
	ret

; obs:
; corre desde o root mestre em busca do nome do item. se encontra retorna eax= 1. cluster
; e edx= ponteiro do item achado. setor se encontra na memoria

;


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
;
; Set current time/date for file entry
; input:
;	edi= file entry pointer

align 4
set_update_time:

	push  eax
	call  get_time_for_file     	; update files date/time (see cmos_dev.inc)
	mov   [edi+22],ax		; 0x16
	call  get_date_for_file		;
	mov   [edi+24],ax		; 0x18
	pop   eax
	ret


set_sign:
	call  get_time_for_file
	mov dx, ax
	call  get_date_for_file
	shl eax, 0x10
	or ax, dx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  9 =============================================
;
; input:
;	bl= partition
;	ecx = cluster count
;	esi= data block memory base
; Note:
;	negative = remove clusters from free space
;	positive = add clusters to free space

align 4
add_disk_free_space:

	test  ecx,ecx               		; no change
	je .add_dfs_no
	test byte [esi+FAT_Type], 0x01	; confirma FAT16
	jz @F				; Se for 16 pula

	push  eax ebx

	cmp dword [esi+0x1fc],0xaa550000 	; check sector id
	jne .add_not_fs

	lea edi, [esi+0x200]
	add [edi+0x1e8],ecx
	movzx eax, word [esi+FSInfo_sector]
	call WriteSector0

  .add_not_fs:
	pop ebx eax

  .add_dfs_no:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  A =============================================
;
; input:
;	eax = first cluster (0=rootdir)
;	bl= partition
;	esi= data block memory base
; output:
;	eax = directory size in bytes
; Testada-OK
align 4
get_dir_size:

	push edx
	xor edx,edx               	; count of directory clusters
	test eax,eax
	jnz .next

	;mov eax,[esi+Root_lba]	; root dir (tamanho do cluster)
	;shl eax,9                 	; fat16 rootdir size in bytes

	;test byte [esi+FAT_Type], 0xff
	;jz .exit			; fixed size
	mov eax,[esi+FAT_RootCluster]

  .next:
	cmp eax,2                 	; incorrect fat chain
	jb .end
	cmp eax,FATreserved 	; end of directory ((((???????????????))))
	ja .end
	call get_fat              	; get next cluster
	jb .error

	inc edx
	jmp .next

  .end:
	movzx eax,byte [esi+FAT_SPC]	; cluster size in bytes
	imul eax, 0x200
	imul eax,edx

  .exit:
  .error:
	pop edx
	ret

; obs:
; fornece o tamanho em bytes de um diretório


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  B =============================================
;
; input:
;	eax = first cluster
;	bl= partition
;	esi= data block memory base

align 4
clear_cluster_chain:

	push eax ecx edx
	xor ecx,ecx               	; cluster count

  .clean_new_chain:
	cmp eax, [esi+Last_cluster] 	; end of file
	ja .delete
	cmp eax,2                 	; unfinished fat chain or zero length file
	jb .delete
	cmp eax,[esi+FAT_RootCluster]   	; don't remove root cluster
	jz .delete

	xor edx,edx
	call set_fat               	; clear fat entry
	jb .error			; access_denied_01 error

	inc ecx			; update cluster count
	mov eax,edx		; old cluster
	jmp .clean_new_chain

  .delete:
	call add_disk_free_space	; add clusters to free disk space

  .error:
	pop edx ecx eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  C =============================================
;
; write changed fat and cache to disk
; Testando

align 4
update_disk:

	cmp byte [esi+FAT_changed],0        	; is fat changed?
	je .no_changed
	call write_fat_sector
	jb .error

  .no_changed:
	;call  write_cache
  .error:	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  D =============================================
; IN:
;	edx= point to filename
; 	bl= partition
; 	esi= data block memory base
;
; OUT:
;	eax= setor relativo ao 1o cluster da cadeia


simple_search:

	mov eax, [esi+Dirbase_clt]	; start from open directory
  	mov ebp, edx
  .search:
	cmp byte [ebp],0		; fim do path
	je .b
	mov edx,ebp
	call analyze_directory	; bl=partiçao, eax, lba, edx= filename, esi= data block
	jc .b2

	mov eax,[edx+FAT32_HWC]       ; read the HIGH 16bit cluster field
	mov ax,[edx+FAT16_C]          ; read the LOW 16bit cluster field

	and eax,FATmask
	add ebp,11                	; 8+3 (name+extension)
	jmp .search
  .b:	ret

  .b2:
  	mov ebp, ERROR_FILE_NOT_FOUND      ; 5
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  E =============================================
; IN:
;        	edx= pointer to filename
; 	bl= partition
; 	esi= data block memory base (buscar na tabela de partições)
; OUT:
;	cy=0 nothing -> file name normalize in FAT_FNbuffer, edx= point to buffer
;	cy=1 don't occur
; Testada-OK

align 4
normalize_name:	;(short name)

	push esi edi ecx eax
	lea edi, [esi+FAT_FNbuffer]
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
  .verify:
  	cmp al, '/'
	jz .continue
	cmp al, '\'
	jz .continue
.end: 	mov [edi],word 0    ; nao precisa ter zero no final para terminar um nome normal

	pop eax ecx edi esi
	lea edx, [esi+FAT_FNbuffer]
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
	jmp .verify

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

; Transfere o nome do caminho para um buffer normalizando tamanho e letras e
; retirando o excesso de espaços (a fazer)...
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Transforma nome curto para normal (arq01   bmp para arq01.bmp)
; IN:
;	esi: string com nome curto
;	edi: string com nome normal  (terminado em ex: db "felipe",0)
; OUT:
;	nothing

normalize_ShortName:
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
.fim:	pop edi
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



;======================== F U N C T I O N  F =============================================
;
; carrega dados referente ao cluster eax. (permite carga parcial)			OK
; input:
;	EAX = first cluster to read
;	EDI = pointer to buffer
;	EDX = # blocks to read in buffer
;	EBP = # blocks to skip over
;	bl= partition
; 	esi= data block memory base
; output:
;	if CARRY=0 ok -> eax, edi= no changes,
;	if CARRY=1 cluster out of range
; Note:	if cluster=0 it's changed to read rootdir

align 4
set_data_cluster:
	push eax ;ecx
	mov ebp, ecx

	;;mov byte [esi+FAT16_root],0
	cmp eax,[esi+Last_cluster]
	ja .error             		; too big cluster number, something is wrong

; ver o que fazer futuramente....
;    cmp eax,2
;    jnb .cluster
;jmp .error	; se for menor que 2...erro ou buscar o setor root
    ;mov eax,[esi+FAT_RootCluster]   ; if cluster < 2 then read rootdir
    ;;cmp byte [esi+FAT_type],16
    ;;jne .cluster
    ;mov eax,[esi+RootBase]
    ;mov ecx,[esi+Root_lba]    		; Note: not cluster size
    ;;mov byte [esi+FAT16_root],1        	; flag for fat16 rootdir
    ;jmp .read


.cluster:
	sub eax,2
	movzx ecx, byte [esi+FAT_SPC]
	imul eax,ecx
	add eax,[esi+Database_lba]	; deve apontar a posição root_lba - posição do cluster 1

.read:
	test ebp,ebp               	; first wanted block
	je .SectorWrite                 	; yes, skip count is 0
	dec ebp
	test edx,edx               	; is all read?
	je .out_of_write
	inc eax                   	; next sector
	dec edx			; avança setores sem leitura
	jnz .read

.SectorWrite:
	call WriteSector0		;
	jb .out_of_write		; vem com codigo de erro da controladora
	add edi,512               	; update pointer
	dec ecx
	je .out_of_write		; cluster is over

	inc eax                   	; next sector
	dec edx
	jnz .read

.out_of_write:
	pop eax 			;ecx
	ret

.error:
	pop  eax			; ecx
	stc
	ret

;-----------------------------------------------------------------------------------------






;======================== F U N C T I O N 2 ==============================================
; Busca o proximo setor do diretorio
; IN:
;	eax= 1° cluster
;	 bl= particao
;	ecx= novo tamanho do arquivo em bytes
;	edx= ponteiro do item do arquivo
;	esi= data block memory base
; OUT:
;	ecx= 0
;	cy= 0	- OK
;	cy= 1 	- Fim de diretorio

   ; Variavel ================
     SPC_Table:
        db 3	; 8 setores    <- shr setor, 3
        db 4	; 16 setores   <- shr setor, 4
        db 0
        db 5	; 32 setores
        db 0
        db 0
        db 6	; 64 setores

FAT_File_Resize:
	push eax
	push edx
	push ebp
int 1
; Calcula quantos clusters serao expandidos (ecx - tamanho atual do arquivo)
	mov ebp, [esi+edx+Dir_FileSize]
	add ecx, 0x1ff		; converte bytes em setores
	add ebp, 0x1ff
	shr ecx, 9
	shr ebp, 9

	push ebx
	movzx ebx, byte [esi+FAT_SPC]
	shr ebx, 3
	mov bl, byte [SPC_Table+ebx]
	xchg ecx, ebx
	shr ebx, cl
	shr ebp, cl
	mov ecx, ebx
	pop ebx


	;push edx
	;push eax
	;xor edx,edx
	;mov eax, ecx
	;idiv word [esi+FAT_SPC]	; converte setores em clusters
	;mov ecx, eax
	;mov eax, ebp
	;idiv word [esi+FAT_SPC]
	;mov ebp,eax
	;pop eax
	;pop edx
int 1
	sub ecx, ebp		; verifica se aumenta ou diminui o arquivo
	js .diminui


; Aumenta o arquivo ==========
   ; Corre a cadeia ate chegar no ultimo cluster do arquivo
.a:	mov ebp, eax
	call get_fat
	cmp eax, FATeof
	je .expand
	jmp .a

   ; Procura um cluster vazio
.expand:	mov eax, ebp
	call get_free_fat		; verifica espaco livre depois do arquivo
	jnc .setFat
	mov eax, 2
	call get_free_fat		; verifica espaco livre antes do arquivo
	jc .error

   ; Atualiza a FAT
.setFat:	mov edx, eax
	mov eax, ebp
	mov ebp, edx
	call set_fat		; Salva o novo cluster
	jc .error2
	loop .expand
	mov eax, ebp		; Desloca a marca de EOF do arquivo
	call set_fat
	jc .error2

   ; Fim
	pop ebp
	pop edx
	pop eax
	ret

;.............................


; Diminui o arquivo ==========
.diminui:	add ecx, ebp
	dec ecx

   ; Coloca a marca de EOF
.b:	call get_fat
	loop .b
	mov edx, FATmask
	call set_fat
	jc .error2

   ; libera os clusters nao utilizados
	call clear_cluster_chain
	jc .error2

   ; Fim
	pop ebp
	pop edx
	pop eax
	ret

;.............................


; Erros ======================
   ; Sem espaco no disco
.error:	stc
	pop ebp
	pop edx
	pop eax
	ret

   ; Erro de gravacao
.error2:	stc
	pop ebp
	pop edx
	pop eax
	ret

;.............................

;-----------------------------------------------------------------------------------------



;======================== F U N C T I O N  5 =============================================
; KOLIBRI OS - COPYRIGHT
; Pesquisa item num diretorio						OK
; IN:
;	eax= first cluster of the directory
; 	 bl= partição
;	edx= pointer to filename
;	esi= base do bloco de dados da unidade
;
; OUT:
;	eax = primeiro cluster do item
;         edx = pointer in buffer
;           [buffer .. buffer+511]
;	cy = 0 -> OK
;	cy = 1 -> Arquivo nao encontrado
;
; Note   : if cluster=0 it's changed to read rootdir
;          save 2 previous directory sectors in longname_sec

align 4
FAT_Item_Search:		; analyze_directory
analyze_directory:
	push  ecx ebp edi edx   	; ebx = [esp+0]
	xor ecx,ecx
; salva dois ultimos ponteiros de nomes longos (neste caso são zerados)
	mov [esi+longname_sec1],ecx
	mov [esi+longname_sec2],ecx

; verifica se o root adequado se encontra na memoria
  .adr_new_cluster:
	mov [esi+cluster_tmp],eax
	;mov byte [esi+FAT16_root],0	; flag for fat16 rootdir
	cmp eax,[esi+Last_cluster]    ; calcular ao abrir o bloco de dados
	ja .adr_not_found         	; too big cluster number, something is wrong
	cmp eax,2
	jnb .adr_data_cluster	; maior ou igual...busca o setor diretorio

	mov eax,[esi+FAT_RootCluster]    	; if cluster < 2 then read rootdir

; deixar para arrumar depois
	;cmp byte [esi+FAT_type],16
	;jne .adr_data_cluster
	;mov eax,[esi+RootBase]
	;mov ebp,[esi+Root_lba]     		; calcular ao abrir o bloco de dados
	;mov byte [esi+FAT16_root],1        	; flag for fat16 rootdir
	;jmp .adr_new_sector

; calcula o endereço do cluster a partir da base
; a base inclui o diretorio root por isso existe a subtração de 2

  .adr_data_cluster:
	sub eax,2
	movzx ebp, byte [esi+FAT_SPC]
	imul eax,ebp
	add eax,[esi+Database_lba]	;

; busca diretorio, se ele ja está na memoria apenas verifica senão busca o setor
  .adr_new_sector:
	lea edi, [esi+RootBase]	; base do bloco da dados da unidade
	mov edx, edi
	cmp eax, [esi+Root_lba]	; compara com root existente na memoria
	je .a
	push eax
	call ReadSector0	
	pop eax
	jb .adr_not_found
	mov [esi+Root_lba], eax	; novo root_lba

  .a:	mov ecx,512/32		; count of dir entrys per sector = 16

.adr_analyze:
	mov edi,[edx+11]		; file attribute
	and edi,0xf
	cmp edi,0xf
	je .adr_long_filename
	test edi,0x8		; skip over volume label
	jne .adr_long_filename	; Note: label can be same name as file/dir
	push esi
	mov esi,[esp+4]		; filename need to be uppercase
	mov edi, edx
	test [edi], byte 0xff
	jz .free_adr_found
	push ecx
	mov ecx,11
	cld
	rep cmpsb			; compare 8+3 filename
	pop ecx
	pop esi
	je .adr_found

.adr_long_filename:
	add edx,32		; position of next dir entry
	dec ecx
	jnz .adr_analyze

	mov ecx,[esi+longname_sec1]		; save 2 previous directory sectors
	mov [esi+longname_sec1],eax		; for delete long filename
	mov [esi+longname_sec2],ecx
	inc eax			; next sector
	dec ebp			; extensão do diretorio
	jne .adr_new_sector		; até esgotar todos os setores de um cluster
	;cmp byte [esi+FAT16_root], 1		; end of fat16 rootdir
	;je    .adr_not_found

.adr_next_cluster:
	mov eax,[esi+cluster_tmp]
	call get_fat		; get next cluster
	jb .adr_not_found

	cmp eax,2			; incorrect fat chain?
	jb .adr_not_found		; yes
	cmp eax,FATreserved		; is it end of directory?
	jb .adr_new_cluster		; no. analyse it

.adr_not_found:
	pop edi edi ebp ecx   	; first edi will remove ebx
	stc			; file not found
	ret


; Pega o primeiro cluster do item
.adr_found:
	mov eax,[edx+FAT32_HWC]       ; read the HIGH 16bit cluster field
	mov ax,[edx+FAT16_C]          ; read the LOW 16bit cluster field
	and eax,FATmask
;Fim
	pop edi
	pop edi ebp ecx
	clc			; file found
	ret

  .free_adr_found:
	pop esi
	xor eax, eax
	mov edx, edi
	pop edi edi ebp ecx   	; first edi will remove ebx
	stc			; file not found
	ret


;-----------------------------------------------------------------------------------------



;======================== F U N C T I O N  1 =============================================
; KOLIBRI OS - COPYRIGHT
; Abre um diretorio por uma path (c:/diretorio/diretorio/diretorio/)
; IN:
;	eax: diretorio base
;	 bl: particao
;	edx: string
;	esi: bloco de dados
; OUT:
;	eax: cluster do diretorio
;	edx: aponta para o ultima barra do nome
;	esi-> no bloco constara o 1 setor do diretorio

FAT_Folder_Search:

	push ebp
 	mov ebp, edx

; Abre a pasta base
	push eax

   ; Calcula o setor a partir do cluster
	sub eax,2
	movzx edi, byte [esi+FAT_SPC]
	imul eax,edi
	add eax,[esi+Database_lba]

   ; Lê o setor
	lea edi, [esi+RootBase]	; base do bloco da dados da unidade
	mov edx, edi
	cmp eax, [esi+Root_lba]	; compara com root existente na memoria
	je .notread1
	push eax
	call ReadSector0	
	pop eax
	jb .read_error
	mov [esi+Root_lba], eax	; novo root_lba
.notread1:pop eax

; Abre o path
   ; Verifica se terminou o path
.search:	mov edx,ebp
	cmp byte [edx], 0
	jz .last
	call normalize_name2	
	mov ebp, edx
	cmp byte [edx], '/'
	je .busca
	cmp byte [edx], '\'
	je .busca
	jmp .last

   ; Procura o diretorio  (IN: eax=diretorio, bl=partiçao, edx= filename, esi= data block)
.busca:	mov edx, esi
	add edx, FAT_FNbuffer
	call FAT_Item_Search	
	jc .not_found
	cmp byte [edx+FileAttr], 0x10		; verifica se é diretorio
	jne .end

   ; Proximo diretorio do path (pula a / que restou)
	inc ebp
	jmp .search

   ; Lê o primeiro setor do ultimo diretorio no path
.last:	push eax

      ; Calcula o setor a partir do cluster
	sub eax,2
	movzx edi, byte [esi+FAT_SPC]
	imul eax,edi
	add eax,[esi+Database_lba]

      ; Lê o setor
	lea edi, [esi+RootBase]	; base do bloco da dados da unidade
	mov edx, edi
	cmp eax, [esi+Root_lba]	; compara com root existente na memoria
	je .notread
	push eax
	call ReadSector0
	pop eax
	jb .read_error
	mov [esi+Root_lba], eax	; novo root_lba
.notread:	pop eax

; Fim
.end:	clc                         	; no errors
	pop ebp
	ret

; Error - diretorio nao encontrado
.not_found:
	stc                         	; errors occour
	pop ebp
	ret

; Error - erro de leitura
.read_error:
	stc
	pop eax
	pop ebp
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Procura um arquivo por uma path (diretorio/diretorio/arquivo.txt)
; IN:
;	eax= diretorio base
; 	esi= data block
;	 bl= partition
;	edx = pointer to a path string
;         (example: the path "/files/data/document" become
;                             "files......data.......document...0"
;         '.' = space char
;         '0' = char(0) (ASCII=0) !!! )
; OUT :
;         if (CARRY=0) -> 
;		EAX=cluster
;		EDX= item memory point in directory
;	if (CARRY=1) -> ERROR in the PATH
; Testada-OK

FAT_File_Search:
; Busca o arquivo
   ; Abre o diretorio do arquivo   
	call FAT_Folder_Search		
	jc .not_found
   ; Procura o arquivo 
   	mov edx, esi
	add edx, FAT_FNbuffer	
	call FAT_Item_Search	
; Fim
.found:	ret			; no errors

; Error - arquivo nao encontrado
.not_found:
	stc                         	; errors occour
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  E =============================================
; IN:
;        	edx= pointer to filename
; 	bl= partition
; 	esi= data block memory base (buscar na tabela de partições)
; OUT:
;	cy=0 nothing -> 
;	 	edx= proxima pasta
;		file name normalize in FAT_FNbuffer 
;	cy=1 don't occur
; Testada-OK

align 4
normalize_name2:	;(short name)

	push esi edi ecx eax
	lea edi, [esi+FAT_FNbuffer]
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