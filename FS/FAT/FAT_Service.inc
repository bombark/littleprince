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
; @Id: FAT_services.inc - 2008-01-27 Modified and adapted by cb-toledo
; Basic routines that enable read/write files in fat system with short filenames
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Create new file descriptor in directory item
; IN:
;	eax= Cluster Base
; 	bl = Partition
;        	esi= Path (retirando a unidade ex:'c:\')
; OUT:
;	ebp = codigo do erro
;	cy=0 nothing -> file name normalize in FAT_FNbuffer
;	cy=1 don't occur
; Testada-OK
; Falta colocar as datas e horas

align 4
FAT_CreateFile:
	push eax
	push edx
	push edi	
	mov edx, esi

; Pega o ponteiro do data block
   ; verifica se a particao está aberta
	movzx edi, bl
	dec edi
	shl edi, 5
	mov esi, [TabelaPart+edi+Part_Padmin]
	or esi, esi			; verifica se a particao está aberta
	jnz .find
   ; abre particao
	call FAT_Open  	
   	mov [TabelaPart+edi+Part_Padmin], esi

  ; Procura se existe um arquivo com o mesmo nome
.find:	push edx		
	call FAT_Item_Search	; eax= cluster, edx= point to item in memory		  	
	pop edx	
	jnc .found	
		
  ; Se o path está incorreto	 -> SAI	
  	cmp ebp, ERROR_PATH_INCORRECT
  	je .error

  ; Cria o item e salva	
	mov al, 0x20		; Tipo = arquivo	
	mov edx, esi		; O nome está padronizado no block data
	add edx, BD_Buffer		; Eh padronizado no FAT_File_Search
	call FAT_Item_Create			
	call FAT_Folder_Write	; Salva o diretorio 
	
	xor ebp,ebp
	clc
	pop edi 
	pop edx
	pop eax
	ret  
	
  ; Error - Ja existe um arquivo com o mesmo nome	ou endereco incorreto  
.found:	mov ebp, ERROR_EXIST_FILE 	; (15)
.error:  	stc
  	pop edi 
	pop edx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Apaga um arquivo
; IN:
;	eax= Cluster Base
;	 bl= Particao
;        	esi= Path
; OUT:
;	cy=0 eax= tamanho do arquivo em bytes, edx= point to item
;	cy=1 ebp= codigo de erro: 5 - file not found
; Testada-OK

align 4
FAT_EraseFile:
	push eax
	push edx
	push edi
	
	mov edx, esi

  ; Pega o ponteiro do data block
    ; verifica se a particao está aberta
	movzx edi, bl
	dec edi
	shl edi, 5
	mov esi, [TabelaPart+edi+Part_Padmin]
	or esi, esi			; verifica se a particao está aberta
	jnz .find
    ; abre particao
	call FAT_Open  	
   	mov [TabelaPart+edi+Part_Padmin], esi
	
  ; Abre o diretorio		
.find:	call FAT_Item_Search	; eax= cluster, edx= point to item in memory	
	jc .error     		; Arquivo nao encontrado

  ; Apaga
	mov [edx], byte 0xE5

	mov eax,[edx+FAT32_HWC]       ; read the HIGH 16bit cluster field
	mov ax,[edx+FAT16_C]          ; read the LOW 16bit cluster field

	mov [edx+FAT16_C], word 0	; clear first cluster points
	mov [edx+0x14], word 0	;


	call FAT_Folder_Write	; Salva o diretorio	
		
	;call FAT_SetFree		; Libera na fat - FAZER
	;jb .error		; este erro pode complicar a consistencia da unidade
	;call FAT_Write		; CORROMPENDO A FAT
	
  ; FIM	
	xor ebp, ebp	
.error:	pop edi		
	pop edx
	pop eax
	ret			

;-----------------------------------------------------------------------------------------






;======================== F U N C T I O N  3 =============================================
; Retorna informacoes sobre o Arquivo
; IN:
;	eax= Cluster Base
;        	edx= pointer to filename
; 	bl= partition (*)
; 	esi= data block memory base (buscar na tabela de partições) (*)
;
; OUT:
;	cy=0 eax= quantidade de bytes no ultimo setor
;              ecx= size of file/directory em setores
;	     edx= atributos
;	     edi= 1o cluster da cadeia
;	cy=1 ebp= codigo de erro: 5 - file not found, edx, ecx=0

; Testada-OK
align 4
FAT_GetFileInfo:	; copia todo arquivo na memoria regulado pela extensao obtida no diretorio			
	mov eax, [esi+BD_Root_Cluster]		
	call FAT_Item_Search	; eax= cluster, edx= point to item in memory			
	jc .error     		

	mov edi, eax		; edi= cluster
   	mov eax, [edx+Dir_FileSize]	; eax= size of file in sectors???
	movzx edx, byte [edx+0xb]	; edx= atributos
	mov ecx, eax
	shr ecx, 9		; converte em setores
	and eax, 0x1ff		; deslocamento no ultimo setor
	xor ebp,ebp		; erro = 0
	ret

.error:	xor ecx,ecx
	xor edx,edx
	xor edi,edi
	stc
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
;
; IN:
;        	edx= pointer to filename
; 	bl= partition (*)
; 	esi= data block memory base (buscar na tabela de partições)(*)
; OUT:
;	cy=0 eax= uptime+update, edx= base in memory
;	cy=1 ebp= codigo de erro: 5 - file not found
; Testada-OK
align 4
FAT_GetUpDateTime:	; copia todo arquivo na memoria regulado pela extensao obtida no diretorio

  	call FAT_Item_Search	; eax= cluster, edx= point to item in memory
	jc .nf     		; sector of directory REST in memory
  .b:	mov eax, [edx+UpTime]
	ret

  .nf:	mov ebp,ERROR_FILE_NOT_FOUND 	; (5)
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Devolve informacao sobre a Particao
; IN:
;	bl=partição (*)
;	esi= data block (*)
; OUT:
;	eax = FS system (2-fat16, 3-fat32)
;         edx = cluster size in bytes
;         ecx = free clusters on disk
;         edi = total clusters on disk
; 	ebp = 0 OK, 3- unknown FS, 10- access denied

align 4
FAT_GetInfo:

	xor ecx,ecx               		; count of free clusters
	mov eax,2
	mov edi,[esi+BD_HD_LastCluster]		; OK

  .cluster:
	push eax
	call FAT_Get              		; get cluster info
	jb .access_denied

	test eax,eax               		; is it free?
	jnz .used             		; no
	inc ecx

  .used:
	pop eax
	inc eax
	cmp eax,edi               		; is above last cluster?
	jbe .cluster          		; no. test next cluster

	dec edi                   		; cluster count
	imul edx,[esi+BD_FAT_SPC],512 	; cluster size in bytes
	xor eax,eax
	inc eax
	inc eax
	test word [esi+BD_FAT_SPF], 0xffff	; e- fat16, 3- fat32
	jnz .inc
	inc eax
  .inc:	xor ebp,ebp
	ret

  .unit_closed:
	mov ebp,ERROR_UNIT_CLOSED
  .fim:  	xor edx,edx
    	xor edi,edi
    	xor ecx,ecx
    	ret

  .access_denied:
	add esp,4
	mov ebp,ERROR_ACCESS_DENIED
	jmp .fim

; colocar base root e extensao na tabela partição..(no lugar DM_base e DM_ext).


;-----------------------------------------------------------------------------------------




;======================== F U N C T I O N   ==============================================
; Cria um diretorio
; IN:
;	eax= Cluster Base
; 	bl = Partition
;        	esi= Path
; OUT:
;	ebp = codigo do erro
;	cy=0 nothing -> file name normalize in FAT_FNbuffer
;	cy=1 don't occur

FAT_Folder_Create:
	push eax
	push edx
	push edi	
	mov edx, esi

	call FAT_GetDataBloco

  ; Procura se existe um arquivo com o mesmo nome
.find:	push edx		
int 1	
	call FAT_Item_Search	; eax= cluster, edx= point to item in memory		  	
int 1	
	pop edx	
	jnc .found	
		
  ; Cria o item do diretorio	
	push edx
	push ebp
	mov edx, esi		; O nome está padronizado no block data
	add edx, BD_Buffer		; Eh padronizado no FAT_File_Search
	call FAT_Folder_Create0
	pop ebp
	pop edx
	
  ; Se deu erro no path  -> cria outra pasta ateh chegar na ultima	
	cmp ebp, ERROR_PATH_INCORRECT
  	je .find
	
	xor ebp,ebp
	clc
	pop edi 
	pop edx
	pop eax
	ret  
	
  ; Error - Ja existe um arquivo com o mesmo nome	ou endereco incorreto  
.found:	mov ebp, ERROR_EXIST_FILE 	; (15)
.error:  	stc
  	pop edi 
	pop edx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Devolve uma tabela com os itens do diretorio
; IN:
;	 bl= particao
;	esi= pointer to path
;	edi= Ponteiro do descritor de diretorio
; OUT:
;	esi= inicio da lista
; OBS= ARRUMAR A EXTENSAO DO NOME (IMG2      BMP para IMG2.BMP)


FAT_Folder_Open:
	push eax
	push edx
	mov edx, esi

  ; Abre o diretorio
	call FAT_GetDataBloco
	call FAT_Item_Search		
	jc .end
	
  ; Coloca as variaveis no descritor	
	mov [edi+FD_Item_Lba], eax
	xor eax, eax
	mov [edi+FD_List_Ini], eax
	mov [edi+FD_List_End], eax
	mov [edi+FD_List_Atual], eax
	mov dword [edi+FD_List_Size], FL_ItemSize
	mov [edi+FD_Item_Qtde], eax
	mov [edi+FD_Item_Size], eax
	mov [edi+FD_Partition], bl
	mov word [edi+FD_Ass], FD_Assinatura
	
  ; Fim	
.end:	xor ebp, ebp
	pop edx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Devolve uma tabela com os itens do diretorio
; IN:
;	edi= Ponteiro do descritor de diretorio
; OUT:
;	nothing


FAT_Folder_Close:
	push eax
	push ecx
	push esi
		
	xor eax, eax
	mov esi, [edi+FD_List_Ini]
	mov [edi+FD_List_Ini], eax
	mov [edi+FD_List_End], eax
	mov [edi+FD_List_Atual], eax
	;mov [edi+FD_List_Size], eax
	;mov [edi+FD_Item_Qtde], eax
	;mov [edi+FD_Item_Size], eax
	;mov [edi+FD_Item_Lba], eax
	;mov [edi+FD_Item_Partition], al
	;mov [edi+FD_Ass], ax
	
.loop:	or esi, esi
	jz .end
	mov edi, [esi+FL_Prox]
	mov ecx, [esi+FL_Name]
	call SMM_MainHeap_Free
	mov esi, ecx
	call SMM_MainHeap_Free
	mov esi, edi
	jmp .loop
	
.end:	xor ebp, ebp
	pop esi
	pop ecx
	pop eax
	ret

.error:	stc
	pop esi
	pop ecx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Devolve uma tabela com os itens do diretorio
; IN:
;	 bl= particao
;	edi= Ponteiro do descritor de diretorio
; OUT:
;	esi= inicio da lista
; OBS= ARRUMAR A EXTENSAO DO NOME (IMG2      BMP para IMG2.BMP)


FAT_Folder_List:
	pushad

  ; Abre o diretorio desejado
	call FAT_GetDataBloco	
	mov eax, [edi+FD_Item_Lba]	
	call FAT_Folder_OpenCluster	
	mov ebx, edi
	lea edx, [esi+BD_Folder]
	xor ebp, ebp			; Pula o . e o .. 

  ; Verifica se eh uma pasta
	cmp eax, 2			; Se for Root nao tem . e ..
	jbe .a
	cmp byte [edx], '.'
	jne .error
	cmp word [edx+0x20], '..'
	jne .error
	mov ebp, 0x40
	
  ; Enquanto nao terminar os itens do diretorio
.a:	cmp byte [edx+ebp], 0
	jz .end
	cmp byte [edx+ebp], 0x35			; item apagado
	je .next
	cmp byte [edx+ebp+FileAttr], 0x10	; Pasta  - Verificar melhor os atributos do item
	je .create 	
	cmp byte [edx+ebp+FileAttr], 0x20	; Arquivo - Verificar melhor os atributos do item
	je .create
	jmp .next

    ; Cria um item na lista
.create: 	xor eax, eax
	call List_AddItem

    ; Coloca as variaveis no item
	inc dword [ebx+FD_Item_Qtde]
	mov word [edi+FL_Data], 0
	mov word [edi+FL_Hora], 0
	mov word [edi+FL_Data2], 0
	mov word [edi+FL_Hora2], 0
	call FAT_GetType
	mov [edi+FL_SubTipo], ax
	mov byte [edi+FL_Color], 0
	mov word [edi+FL_Ass], FL_Assinatura

   ; Coloca o nome
	mov eax, 12 
	call SMM_MainHeap_Alloc	
	jc .error
	mov [edi+FL_Name], esi

      ; Copia o nome
	mov edi, esi
	lea esi, [edx+ebp]
	call Fat_NormaToName
	;call normalize_ShortName

   ; Pega o proximo item
.next:	add ebp, 0x20
	cmp ebp, 0x200
	jb .a

   ; Busca a outro cluster do diretorio
   	;xor ebp, ebp
   	;call FAT_Folder_Read
   	;jc .end
   	;jmp .a

; Fim
.end:	clc
	xor ebp, ebp
	popad
	ret

.error:	stc
	popad
	ret

;-----------------------------------------------------------------------------------------











; Encontrar um lugar melhor para essa rotina
;======================== S U B F U N C T I O N  1 =======================================
; Faz um merge no endereco do programa com o endereco relativo
; IN:
;	edx= endereco relativo
; 	esi= Buffer
; OUT:
;	edx= endereco absoluto

MergePath:
	push ebx
	push esi
	push edi

	mov edi, esi
	mov esi, [Task_Current]
	mov esi, [esi+Task_Path]

; Copia o path
	call cpystr_Z

	add edi, ecx
	mov esi, edx
	dec edi

.b:	inc edi			; Um treco pra ajustar o :
.a:	mov eax, [esi]
	and eax, 0xffffff
	cmp eax, '../'
	je .back
	cmp eax, '..\'
	je .back
.copy:	call cpystr_Z

;int 1
	pop edi
	pop esi
	pop ebx
	ret


.back:	add esi, 3
	dec edi
.loop:	mov al, [edi-1]			; Volta uma barra
	cmp al, ':'
	je .b
	cmp al, '/'
	je .a
	cmp al, '\'
	je .a
	dec edi
	jmp .loop

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  2 =======================================
; Devolve o tipo do arquivo
; IN:
;	edx= Ponteiro do BD_Folder
;	ebp= Ponteiro do item a ser analizado
; OUT:
;	al= subtipo
;	ah= tipo

FAT_GetType:
	push edx
	
	xor ax, ax
	inc ah		; coloca ah= 1  -> Pasta
	cmp byte [edx+ebp+FileAttr], 0x10
	je .end

	mov ah, 2
	mov al, 1
	lea edx, [ebp+8]
	mov edx, [edx]
	and edx, 0xffffff

; Tipo imagem
	cmp edx, 'BMP'
	jne @f
	mov ah, 3
	mov al, 1
	jmp .end

; Tipo de audio
@@:	cmp edx, 'WAV'
	jne .end
	mov ah, 4
	mov al, 1

.end:	pop edx
	ret



;-----------------------------------------------------------------------------------------