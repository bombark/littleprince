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
; @Id: FAT_ReadFile.inc - 2008-02-10 - cb-toledo
; fat32 HD reading routines
; For compilation use Flat Assembler (http://flatassembler.net)
;
;----------------------------------------------------------------------------------------



;======================== F U N C T I O N   ==============================================
;
; carrega toda cadeia referente ao cluster eax. usada em conjunto com FAT_GetHDinfo 					OK
; input:
;	EAX = first cluster to read
;         EDI = pointer to buffer
;	bl= partition (*)
; 	esi= data block memory base (*)
; output:
;	if CARRY=0 ok -> eax= FATreserved, edi= UPDATED TO NEXT POSITION,
;         if CARRY=1 cluster out of range
; Note:	if IN cluster=0 it's changed to read rootdir (NOT IMPEMENTED)
; OBS= A rotina tah lendo 2 setores a mais


align 4
FAT_GetDCCC:
	push ecx
	;;mov byte [esi+FAT16_root],0
.new:	cmp eax,[esi+Last_cluster]
	ja .error             	; too big cluster number, something is wrong

; ver o que fazer futuramente....
;    cmp eax,2
;    jnb .cluster
;jmp .error	; se for menor que 2...erro ou buscar o setor root
    ;mov eax,[esi+FAT_RootCluster]   ; if cluster < 2 then read rootdir
    ;;cmp byte [esi+FAT_type],16
    ;;jne .cluster
    ;mov eax,[esi+RootBase]
    ;mov ecx,[esi+Root_lba]    	; Note: not cluster size
    ;;mov byte [esi+FAT16_root],1       ; flag for fat16 rootdir
    ;jmp .read


  .cluster:
	push eax			; cluster vira setor
	sub eax,2
	movzx ecx, byte [esi+FAT_SPC]
	imul eax,ecx		; ecx= tamanho do cluster
	add eax,[esi+Database_lba]	; eax= lba do cluster
	call ReadSegEdiUp	;ReadSegA		; le o cluster todo, atualiza edi
	jb .error_HD		; vem com codigo de erro da controladora

  .fim_cluster:
	pop eax
	call get_fat               	; get next cluster
	jb .error_HD		; eax= proximo cluster

	cmp eax,FATreserved     	; end of file
	jnb .eof_OK
	cmp eax,2                 	; incorrect fat chain
	jnb .new

  .error:
	mov ebp,ERROR_FAT_TABLE	; (9)
	pop ecx
	stc
	ret

  .eof_OK:
	pop ecx
	ret

  .error_HD:
	pop eax ecx
	mov ebp,ERROR_ACCESS_DENIED   ; (a)
	stc
	ret


; OBS:
; permite selecionar apenas 1 setor num determinado cluster, onde ebp indica a quantidade
; de setores a pular e edx indica a quantidade total de setores a serem lidos...
; tipo de 8, pular 4 e ler 1: ebp=4, edx=5
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  X X ===========================================
;
; IN:
;        	eax= pointer to filename
;	edi= pointer to memory for file
;	edx= quantidade de setores a serem lidos dentro do cluster
;	ecx= quantidade de setores iniciais a ser pulados
; 	bl= partition (*)
; 	esi= data block memory base (buscar na tabela de partições) (*)
;
; OUT:	cy=0 OK- eax= 1o cluster
;	ebp = 0 - ok
;               3 - unknown FS
;               5 - file not found
;               6 - end of file
;               9 - fat table corrupted
;               10 - access denied

align 4
FAT_PartialFileRead:	; Carrega parte de um arquivo

	push edx
	mov edx,eax

	call get_cluster_of_a_path	; eax= cluster, edx= point to item in memory
	jc .nf     		; arquivo nao encontrado
	push edx
	push eax			; salva 1o cluster

	;mov ebp,ecx        	; number of blocks to skip
	mov edx,[esp+8]        	; number of blocks to read

	call FAT_ReadClusterChain0	;
 	pop eax edx		; edx= ponteiro do item no diretorio
 .nf:	add esp, 4
  	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N   ==============================================
;
; IN:
;        	eax= first cluster
;	edx= quantidade de setores a serem lidos
; 	bl= partition (*)
; 	esi= data block memory base (buscar na tabela de partições) (*)
;	ecx= setores a ser desconsiderados
;	edi= destin memory base
; OUT:
;	cy=0 eax= 0, edx, ebp = 0, edi, esi, ebx = inalterados
;	cy=1 ebp= 6 - end of file
;                   9 - fat table corrupted
;                  10 - access denied

align 4
FAT_ReadClusterChain0:  	; le a cadeia limitada a quantidade de setores a ser lido

  .new:	call get_data_cluster
	jc .eof         		; end of file or cluster out of range

	test edx,edx               	; is all read?
	je .OK          		; yes

	call get_fat               	; get next cluster
	jb .access_denied

	cmp eax,FATreserved     	; end of file
	jnb .eof
	cmp eax,2                 	; incorrect fat chain
	jnb .new
	mov ebp,ERROR_FAT_TABLE		; (9)
	ret

  .eof:
	mov ebp,ERROR_END_OF_FILE 		; (6)
	ret

  .OK:
	xor ebp,ebp
	ret

  .access_denied:
	mov ebp,ERROR_ACCESS_DENIED		; (10)
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N   ==============================================
;
; IN:
;        	eax= pointer to filename
;	edi= pointer to memory for file
;	edx= quantidade de setores a serem lidos dentro do cluster
;	ecx= quantidade de setores iniciais a ser pulados
; 	bl= partition
; 	esi= data block memory base (buscar na tabela de partições)
; OUT:
;	eax = 0 - ok
;               3 - unknown FS
;               5 - file not found
;               6 - end of file
;               9 - fat table corrupted
;               10 - access denied
;          ecx = size of file/directory (bytes or clusters)

;align 4
;FAT_AllFileRead:		; funciona, mas nao permite conhecer o tamanho do arquivo
;			; com antecedencia
;	pushad
;	mov edx,eax
;	xor ebp, ebp		; ebp=0 -> carrega cluster a partir do inicio
;	call get_cluster_of_a_path	; eax= cluster, edx= point to item in memory
;	jc .not_found     		; sector of directory is in memory
;
;	mov edx, [edx+Dir_FileSize]	; edx= size of file in sectors???
;	mov PUSHAD_ECX, edx		; tamanho do arquivo em bytes
;	add edx, 0x200
;	shr edx, 9		; converte em setores (
;
;	call FAT_ReadClusterChain0	;
;
;	popad
;	ret

; OBS:
; copia todo arquivo na memoria regulado pela extensao obtida no diretorio
; nao permite reserva de memoria porque a priori nao se conhece o tamanho do arquivo
; pode-se usar a rotina FAT_GetFileInfo para obter essas informações.

;-----------------------------------------------------------------------------------------
