;-----------------------------------------------------------------------------------------
;
; LITTLE PRINCE Operational System
; Copyright (C) 2008 to 2011 - LP Development Team
;
;-----------------------------------------------------------------------------------------
;
; This program is free software; you can  redistribute it and/or modify
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
; @Id: FS_filesystem.inc  - 2008-2-01, fb-toledo & cb-toledo
; This is the main compilable file that groups all others include files of the project
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------


;======================== T A B E L A S ==================================================
; Deixar os serviços com nomes curtos na parte alta da tabela (futuramente)
; Fazer, no SFS_Path_Init, procurar por dispositivos cadastrados
;   #pd1  -> pendrive 1 
;   #pd2  -> pendrive 2
;   #cd1  -> cdrom 1
;   #cd2  -> cdrom 2

; Tabela de Servico do LPFS ============
align 4
SFS_Table:

	dd SFS_Reserved		; 0- info service (service description)
	dd SFS_GetInfo		; 1- get FS_info (LPFS, FAT or other)

; Gerenciamento
	dd SFS_File_Create		; 2- create file (,FAT)			
	dd SFS_File_Delete		; 3- delete file			
	dd SFS_File_Copy		; 4- copy file
	dd SFS_File_Move		; 5- move file
	dd SFS_File_ChangeAttr		; 6- change attributes of the file		
	dd SFS_File_GetInfo		; 7- get size of the file			
	dd SFS_Link_Create          	; 8- create link
	dd SFS_File_Execute		; 9- execute file
	dd error12			; a- x

; Gerenciamento de Pasta
	dd SFS_Folder_Create		; b- create folder				
	dd SFS_Folder_Delete		; c- delete folder
	dd SFS_Folder_Copy		; d- copy folder
	dd SFS_Folder_Move		; e- move folder
	dd SFS_Folder_ChangeAttr	; f- change attributes of the folder		
	dd error12			; 10- x
	dd SFS_Folder_GetInfo		; 11- get size of the folder
	dd SFS_Folder_Open		; 12- open folder
	dd SFS_Folder_Close		; 13- close folder
	dd SFS_Folder_List      	; 14- list folder
	dd SFS_Folder_Up		; 15- up folder
	dd SFS_Folder_Down		; 16- down folder

; Manipulacao de arquivo (Rotinas no arquivo FileManager.inc)
	dd SFS_File_Open	; 1a- open file
	dd SFS_File_Close	; 1b- close file
	dd SFS_File_AllRead	; 1c- load complete file
	dd SFS_File_Read	; 1d- read partial file
	dd SFS_File_AllWrite	; 1e- save complete file
	dd SFS_File_Write	; 1f- write partial file
	dd SFS_File_Seek		

; Gerenciamento de ambos
	dd 0;error12		; 20- x
	dd 0;GetDataTime	; 21- get date & time of a file		
	dd 0;RenameFileFolder	; 22- rename folder				
	dd 0;error12		; 23- x

SFS_Table_Qtde	equ 0x23

;---------------------------------------


; Codigos de Erro ======================
; Arquivo (FS_Constant.inc)
;	ERROR_SUCCESS        = 0
;	ERROR_DISK_BASE      = 1
;	ERROR_UNSUPPORTED_FS = 2
;	ERROR_UNKNOWN_FS     = 3
;	ERROR_PARTITION      = 4
;	ERROR_FILE_NOT_FOUND = 5
;	ERROR_END_OF_FILE    = 6
;	ERROR_MEMORY_POINTER = 7
;	ERROR_DISK_FULL      = 8
;	ERROR_FAT_TABLE      = 9
;	ERROR_ACCESS_DENIED  = 10
;	ERROR_UNIT_CLOSED	 = 11
;	ERROR_SERVICE	 = 12
;	ERROR_BAD_NAME	 = 13
;	ERROR_NOT_FORMATTED	 = 14
;	ERROR_EXIST_FILE	 = 15

;---------------------------------------



;======================== E N T R A D A ==================================================
; Entrada  do servico de memoria
; IN:
;	registradores= parametros
;	ebp= numero da funcao
;	int 0x45
; OUT:
;	ebp = codigo de erro
;	  ebp = 0 -> OK
;	cy = 0 -> OK
;	cy = 1 error code in ebp

align 4
SFS_Enter:	
	and ebp, 0x3f		; ebp= desire service
	cmp ebp, 0x2f
	ja .error1
	shl ebp, 2	
 	call dword [SFS_Table+ebp] 
 	jc .error2
	and byte [esp+0x8], 0xfe		; desmarca o flag carry
	iret				; OK-> ebp= 0, erro -> ebp <>0		

.error1:	mov ebp, ERROR_SERVICE 
.error2:	or byte [esp+0x8], 0x01		; marca o flag carry
	iret

  ; Error - Servico invalido
error12:	mov ebp, ERROR_SERVICE	
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  0 =============================================
; Devolve as informacoes do servico (help & definitions)
; IN:
; OUT:

SFS_Reserved:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  0 =============================================
; Devolve as informacoes do servico (help & definitions)
; IN:
; OUT:

SFS_GetInfo:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Cria um Arquivo
; IN:
;	esi= nome do arquivo
; OUT:
;	ebp= numero do erro

SFS_File_Create:
	push eax
	push ebx
	push esi	
	call SFS_Path_Init	; eax= lba/cluster base, bh - tipo de FS, bl particao 	
	jc .end		; Erro: particao invalida

	cmp bh, 0x10
	je .lpfs
	cmp bh, 0x30
	je .fat
	cmp bh, 0x20
	je .fat
	jmp .error	; Erro: sistema de arquivo invalido

.lpfs:	call strlen
	call CreateFile
	jmp .end

.fat:	call FAT_CreateFile

.end:	pop esi
	pop ebx
	pop eax
	ret

.error:	stc
	mov ebp, ERROR_UNSUPPORTED_FS
	pop esi
	pop ebx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Apaga um Arquivo
; IN:
;	esi= nome do arquivo
; OUT:
;	ebp= numero do erro

SFS_File_Delete:
	push eax
	push ebx
	push esi
	
	call SFS_Path_Init	; eax= lba/cluster base, bh - tipo de FS, bl particao 
	jc .end		; Erro: particao invalida
	
	cmp bh, 0x10
	je .lpfs
	cmp bh, 0x30
	je .fat
	cmp bh, 0x20
	je .fat
	jmp .error	; Erro: sistema de arquivo invalido

.lpfs:	;call strlen
	;call DeleteFile
	jmp .end

.fat:	call FAT_EraseFile

.end:	pop esi
	pop ebx
	pop eax
	ret

.error:	stc
	mov ebp, ERROR_UNSUPPORTED_FS
	pop esi
	pop ebx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Copia um arquivo
; IN:
;	esi= Path fonte
;	edi= Path destino
; OUT:
;	ebp= numero do erro

SFS_File_Copy:
	push eax
	push ebx
	push esi
	
	call SFS_Path_Init	; eax= lba/cluster base, bh - tipo de FS, bl particao 
	jc .end		; Erro: particao invalida
	
	cmp bh, 0x10
	je .lpfs
	cmp bh, 0x30
	je .fat
	cmp bh, 0x20
	je .fat
	jmp .error	; Erro: sistema de arquivo invalido

.lpfs:	;call strlen
	;call CreateFile
	jmp .end

.fat:	;call FAT_CreateFile

.end:	pop esi
	pop ebx
	pop eax
	ret

.error:	stc
	mov ebp, ERROR_UNSUPPORTED_FS
	pop esi
	pop ebx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; Move um arquivo
; IN:
;	esi= nome do arquivo a ser movido
;	edi= endereco destino
; OUT:
;	ebp= numero do erro

SFS_File_Move:
	push eax
	push ebx
	push esi
	
	call SFS_Path_Init	; eax= lba/cluster base, bh - tipo de FS, bl particao 
	jc .end		; Erro: particao invalida
	
	cmp bh, 0x10
	je .lpfs
	cmp bh, 0x30
	je .fat
	cmp bh, 0x20
	je .fat
	jmp .error	; Erro: sistema de arquivo invalido

.lpfs:	;call strlen
	;call CreateFile
	jmp .end

.fat:	;call FAT_CreateFile

.end:	pop esi
	pop ebx
	pop eax
	ret

.error:	stc
	mov ebp, ERROR_UNSUPPORTED_FS
	pop esi
	pop ebx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 4 ===========================================
; Muda os atributos de um arquivo
; IN:
;	eax= novo atributo
;	esi= nome do arquivo
; OUT:
;	ebp= numero do erro

SFS_File_ChangeAttr:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 4 ===========================================
; Devolve informacoes do arquivo
; IN:
;	esi= nome do arquivo
; OUT:
;	ebp= numero do erro

SFS_File_GetInfo:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 4 ===========================================
; Cria um atalho
; IN:
; OUT:

SFS_Link_Create:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 4 ===========================================
; Executa um Arquivo
; IN:
;	esi= Path
; OUT:
;	ebp= numero do erro

SFS_File_Execute:
	ret

;-----------------------------------------------------------------------------------------

;======================== F U N C T I O N  1 2 ===========================================
; Cria uma pasta
; IN:
;	esi= nome da pasta
; OUT:
;	ebp= numero do erro

SFS_Folder_Create:
	push eax
	push ebx
	push esi
	
	call SFS_Path_Init	; eax= lba/cluster base, bh - tipo de FS, bl particao 
	jc .end		; Erro: particao invalida
	
	cmp bh, 0x10
	je .lpfs
	cmp bh, 0x30
	je .fat
	cmp bh, 0x20
	je .fat
	jmp .error	; Erro: sistema de arquivo invalido

.lpfs:	;call strlen
	;call CreateFile
	jmp .end

.fat:	call FAT_Folder_Create

.end:	pop esi
	pop ebx
	pop eax
	ret

.error:	stc
	mov ebp, ERROR_UNSUPPORTED_FS
	pop esi
	pop ebx
	pop eax
	ret

;-----------------------------------------------------------------------------------------

;======================== F U N C T I O N  1 9 ===========================================
; Apaga um diretorio
; IN:
;	esi= Path
; OUT:
;	ebp= numero do erro

SFS_Folder_Delete:
	ret

;-----------------------------------------------------------------------------------------

;======================== F U N C T I O N  1 9 ===========================================
; Copia um Diretorio
; IN:
;	esi= Path fonte
;	edi= Path destino
; OUT:
;	ebp= numero do erro

SFS_Folder_Copy:
	ret

;-----------------------------------------------------------------------------------------

;======================== F U N C T I O N  1 9 ===========================================
; Move um Diretorio
; IN:
;	esi= Path fonte
;	edi= Path destino
; OUT:
;	ebp= numero do erro

SFS_Folder_Move:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 8 ===========================================
; Muda os atributos de um Diretorio
; IN:
;	esi= Path
; OUT:
;	ebp= numero do erro

SFS_Folder_ChangeAttr:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 8 ===========================================
; Devolve informacoes do diretorio 
; IN:
;	esi= Path
; OUT:
;	ebp= numero do erro

SFS_Folder_GetInfo:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 8 ===========================================
; Abre um Diretorio
; IN:
;	???= Folder Desc
;	esi= Path
; OUT:
;	ebp= numero do erro

SFS_Folder_Open:
	push eax
	push ebx
	push esi
	
	call SFS_Path_Init	; eax= lba/cluster base, bh - tipo de FS, bl particao 
	jc .end		; Erro: particao invalida
	
	cmp bh, 0x10
	je .lpfs
	cmp bh, 0x30
	je .fat
	cmp bh, 0x20
	je .fat
	jmp .error	; Erro: sistema de arquivo invalido

.lpfs:	;call strlen
	;call CreateFile
	jmp .end

.fat:	call FAT_Folder_Open

.end:	pop esi
	pop ebx
	pop eax
	ret

.error:	stc
	mov ebp, ERROR_UNSUPPORTED_FS
	pop esi
	pop ebx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 8 ===========================================
; Fecha um Diretorio
; IN:
;	edi= Folder Desc
; OUT:
;	ebp= numero do erro

SFS_Folder_Close:
	push eax
	push ebx
	push esi
	
  ; Verifica a assinatura do descritor	
	cmp word [edi+FD_Ass], FD_Assinatura
	jne .end
	
  ; Pega a particao e o tipo da particao	
	mov bl, [edi+FD_Partition]
	movzx ebp, bl
	dec ebp
	shl ebp, 5
	cmp dword [TabelaPart+ebp], 0
	je .error				; particao nao existe
	mov bh, [TabelaPart+ebp+Part_tipo]
	and bh, 0xf0
	
	cmp bh, 0x10
	je .lpfs
	cmp bh, 0x30
	je .fat
	cmp bh, 0x20
	je .fat
	jmp .error	; Erro: sistema de arquivo invalido

.lpfs:	;call strlen
	;call CreateFile
	jmp .end

.fat:	call FAT_Folder_Close

.end:	pop esi
	pop ebx
	pop eax
	ret

.error:	stc
	mov ebp, ERROR_UNSUPPORTED_FS
	pop esi
	pop ebx
	pop eax
	ret

.error2:	stc
	mov ebp, ERROR_PARTITION
	pop esi
	pop ebx
	pop eax
	ret

;-----------------------------------------------------------------------------------------

;======================== F U N C T I O N  1 8 ===========================================
; Lista um Diretorio
; IN:
;	esi= Folder Desc
; OUT:
;	edx= descritor do arquivo
;	ebp= numero do erro

SFS_Folder_List:
	push eax
	push ebx
	push esi

  ; Verifica a assinatura do descritor			
	cmp word [edi+FD_Ass], FD_Assinatura
	jne .end
	
  ; Pega a particao e o tipo da particao	
	mov bl, [edi+FD_Partition]
	movzx ebp, bl
	dec ebp
	shl ebp, 5
	cmp dword [TabelaPart+ebp], 0
	je .error				; particao nao existe
	mov bh, [TabelaPart+ebp+Part_tipo]
	and bh, 0xf0
	 	
	cmp bh, 0x10
	je .lpfs
	cmp bh, 0x30
	je .fat
	cmp bh, 0x20
	je .fat
	jmp .error	; Erro: sistema de arquivo invalido

.lpfs:	;call strlen
	;call CreateFile
	jmp .end

.fat:	call FAT_Folder_List

.end:	pop esi
	pop ebx
	pop eax
	ret

.error:	stc
	mov ebp, ERROR_UNSUPPORTED_FS
	pop esi
	pop ebx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 8 ===========================================
; Abre um Diretorio acima do aberto
; IN:
;	???= Folder Desc
; OUT:
;	edx= descritor do arquivo
;	ebp= numero do erro

SFS_Folder_Up:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 8 ===========================================
; Abre um Diretorio abaixo do já aberto
; IN:
;	???= Folder Desc
;	esi= Nome da pasta
; OUT:
;	ebp= numero do erro

SFS_Folder_Down:
	ret

;-----------------------------------------------------------------------------------------




; As rotinas de manipulacao de arquivo estao no FS/FileManager.inc
;======================== F U N C T I O N  1 8 ===========================================
; Abre um Arquivo
; IN:
;	esi= Nome do arquivo
; OUT:
;	edx= descritor do arquivo
;	ebp= numero do erro

SFS_File_Open:
	jmp File_Open

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 9 ===========================================
; Fecha um Arquivo
; IN:
;	edx= descritor do arquivo
; OUT:
;	ebp= numero do erro

SFS_File_Close:
	jmp File_Close

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 A ===========================================
; Carrega todo o Arquivo
; IN:
;	edx= descritor do arquivo
; OUT:
;	edi= ponteiro do arquivo na memoria
;	ebp= numero do erro

SFS_File_AllRead:
	jmp File_AllRead

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 B ===========================================
; Lê parte do Arquivo
; IN:
;	ecx= quantidade de bytes
;	edx= descritor do arquivo
;	edi= destino
; OUT:
;	ebp= numero do erro

SFS_File_Read:
	jmp File_Read

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 D ===========================================
; Grava todo o Arquivo
; IN:
;	edx= descritor do arquivo
;	edi= ponteiro do arquivo na memoria
; OUT:
;	ebp= numero do erro

SFS_File_AllWrite:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 E ===========================================
; Grava parte do Arquivo
; IN:
;	edx= descritor do arquivo
;	edi= ponteiro do arquivo na memoria
; OUT:
;	ebp= numero do erro

SFS_File_Write:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 F ===========================================
; Modifica a posicao do Cursor do arquivo
; IN:
;	eax= posicao de leitura do arquivo (setores)
;	 cx= posicao de leitura do arquivo (deslocamento do ultimo setor)
;	edx= descritor do arquivo
; OUT:
;	ebp= numero do erro

SFS_File_Seek:
	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  1 =======================================
; Analisa path fornecido
; IN:
;	esi= Path
; OUT:
;	eax= LBA/Cluster do diretorio base
;	bl = Particao 
;	bh = Sistema de arquivo 
;	esi= Path sem a unidade (c:\)

SFS_Path_Init:	
	movzx ebx, byte [esi]
	cmp byte [esi+1], ':'
	jne .rel
	
    ; Retira a Unidade do Path (c:\)
.loop:   	mov al, [esi]
    	or al, al
    	jz .error
    	cmp al, '/'
    	je @f
   	cmp al, '\'
   	je @f
   	inc esi
   	jmp .loop 	
@@:    	inc esi
	xor eax, eax		; Lba inicial = 0 -> Root		
	jmp .getFS

    ; Endereco relativo
.rel:    	push esi
    	mov esi, [Task_Current]
    	mov eax, [esi+Task_PathLba]	; Lba do diretorio
    	mov esi, [esi+Task_Path]
    	movzx ebx, byte [esi]	; pega a unidade
    	pop esi 

       ; Retira a primeira barra: "/pasta/jogos/"
.loop2:	cmp byte [esi], '/'
	jne @f
@@:	cmp byte [esi], '\'
	jne .getFS
	inc esi
	jmp .loop2	

  ; Verifica o tipo do sistema de arquivo
.getFS:	or bl, 0x20
	sub bl, 0x60
	movzx ebp, bl
	dec ebp
	shl ebp, 5
	cmp dword [TabelaPart+ebp], 0
	je .error				; particao nao existe
	mov bh, [TabelaPart+ebp+Part_tipo ]
	and bh, 0xf0
	xor ebp, ebp
	clc
	ret

  ; Error - Particao invalida
.error:	stc
	mov ebp, ERROR_PARTITION
	ret  

;-----------------------------------------------------------------------------------------