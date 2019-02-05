;-----------------------------------------------------------------------------------------
;
; LITTLE PRINCE Operational System
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
; @Id: LPFS_services.inc  - 2008-3-04, cb-toledo
; LPFS services routines
; For compilation use Flat Assembler (http://flatassembler.net)
;-----------------------------------------------------------------------------------------



;======================== F U N C T I O N  1 =============================================
;
; IN:
;	ax= tipo
;	esi= ponteiro do nome
;	cx= extensao do nome	; nao é necessário....ir pelo valor armazenado no item
;	ebx= base do diretório	; pasta aberta
; OUT:
;	cy=0
;	eax= extensao do arquivo em bytes
;	ecx= extensao do arquivo em setores
;	edx= attrib/tipo (hdw/ldw)
; 	esi= soma dos bytes
;	edi= x
;	ebx= x

align 4
GetFileInfo:
	call busca_arquivoTipoAX	; esi= item achado se cy=0
	jb .end
	call GetSize		; ecx= file size
	mov eax, [ebx+esi+I_SizeSectores]; extensao do arquivo em bytes
	mov edx, [ebx+esi+I_tipo]
	mov esi, [ebx+esi+I_soma]	; esi= soma de bytes
	xor ebp,ebp
.end:	ret			; error - come from busca_arquivo


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
;
; IN:
;	ax= tipo
;	esi= ponteiro do nome
;	cx= extensao do nome	; nao é necessário....ir pelo valor armazenado no item
;	ebx= base do diretório	; pasta aberta
; OUT:
;	cy=0
;	eax= extensao do arquivo em bytes
;	ecx= extensao do arquivo em setores
;	edx= attrib/tipo (hdw/ldw)
; 	esi= soma dos bytes
;	edi= x
;	ebx= x

align 4
GetItemInfo:
	call busca_arquivoTipoAX	; esi= item achado se cy=0
	jb .end
	call GetSize		; ecx= file size
	mov eax, [ebx+esi+I_SizeSectores]; extensao do arquivo em bytes
	mov edx, [ebx+esi+I_tipo]	; edx= attrib/tipo, esi= point to item
	xor ebp,ebp
.end:	ret			; error - come from busca_arquivo

; A rotina GetFileInfo é diferente da anterior apenas na saida de esi. Na primeira esi
; traz a informaçao da soma dos bytes do arquivo (esta soma nao considera o cabeçalho)
; Na segunda esi traz o ponteiro do item. Esta ultima é para uso interno do sistema.
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Cria arquivo (aloca item com extensao zerada)
; IN:
;	ax= tipo (max 255)
; 	ebx= diretorio inicial
;	cx= extensao do nome
;	esi= base do nome
; OUT:
;	edi= item criado, nao altera ebx
;
align 4
CreateFile:
	push esi
	call busca_arquivoTipoAX	; se o item nao existe esi=0, ebx=updated
	pop esi
	jae .error		; o item já existe...sai

; Cria o arquivo	
.a:	call cria_item		; edi= item criado
   
   ; Save directory	
	push dword [ebx+esi+I_diretorio]
	and [ebx+esi+I_diretorio], dword 0
	call salva_diretorio
	pop dword [ebx+esi+I_diretorio]
	mov esi, [ebx+esi+I_soma]	; esi= soma de bytes
	xor ebp,ebp
	ret

; Error - o arquivo já existe
.error:	mov ebp, ERROR_EXIST_FILE
	stc
	ret

; fazer rotina que busque arquivo usando apenas o nome associado à extensao registrada no
; item
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; save file on item already create
; IN:
;	ax: tipo
;	ebx: diretorio inicial
;	esi: string (diretorio/diretorio/arquivo.ext)
;	cx: string extension
;	edi: data memory base
; 	edx: quantidade de bytes
; OUT:
;
;
align 4
SaveFile:

	call busca_arquivoTipoAX	; esi= item achado se cy=0

.a:	push edx
	push edi
	jb .error			; se nao achou, criar o item primeiro...
; verifica se existe extensao. sim: usa o ponteiro, nao: cria item
	mov ecx, edx		; ecx=quant bytes
	xor eax, eax		; eax=0
	mov edi, [ebx+esi+I_continue]
	or edi, edi
	jnz .c
;1a.: o item existe, mas é vazio: salva o arquivo, alocando espaço para o mesmo
	mov edi, esi		; esi= item encontrado
	jmp InstalaArquivo.a	; salva arquivo quando ext=0

; extensao já existe, comparar com ecx...se igual simplesmente salva
.c:	add eax, [ebx+edi+F_ext]	; eax=extensao total do arquivo
	mov edi, [ebx+edi+F_continue]
	or edi,edi
	jnz .c
;2a.: o item existe com a mesma extensao: apenas salva o arquivo na mesma posição
	;mov edx, ecx		; edx= size in bytes
	add ecx, 0x1ff		; round to sector size
	and ecx, 0xfffffe00
	shr ecx, 9
	cmp eax, ecx
	jz InstalaArquivo.f		; a extensao é maior que o espaço existente.

; o item existe mas a extensao é diferente...
	mov ebp, ecx		; provisorio, edi=0 neste ponto
	sub ecx, eax		; diferença (>0)
	js .d
;3a.: o item existe, mas a nova extensao é maior que a existente...aloca novo fragmento
	mov edi,esi		; esi=base do item

	call aloca_espaco_item
;int 1
	jb .error1		; ????????????
	add ecx,eax		; se o bloco adjacente é livre retira dele...OK
	jmp InstalaArquivo.f	; se precisa fragmentar funciona bem...se precisa diminuir ...nao foi testado ainda

;4a.: o item existe, mas a nova extensao é menor que a existente
; atualizar o item e liberar o espaço excedente
; verificar primeiro se o ultimo frag tem capacidade de suportar o desconto
.d:
	;neg ecx			; -(ecx)= quantidade de setores a retirar
	;cmp ecx, [ebx+esi]		; esi neste ponto nao indica o ultimo frag...devria comparar com a extensao do ultimo frag
	;jb .e

	mov eax,ebp		; ebp= nova extensao requerida para o arquivo
	call RetiraFrag_item	; altera ecx
;int 1
	mov ecx,ebp
	jmp InstalaArquivo.f	; salva o arquivo

;.........................................................................................
;.e:
;	mov eax,[ebx+esi+F_lba1]
;	mov edx,[ebx+esi+F_lba2]
;	sub [ebx+esi+F_lba1], ecx
;	sub [ebx+esi], ecx
;	sbb dword [ebx+esi+F_lba2], 0
;	push ebx
;	call TakeDMpos		; ebx= DM position
;	call add_ItemEV
;	pop ebx
;	jmp InstalaArquivo.f	; salva o arquivo
;.........................................................................................
.error1:	pop edx			;
 	ret

;0a.: o item nao existe: salva arquivo integral, criando o item e alocando espaço para o mesmo
.error:
	push edx
	jmp InstalaArquivo.c	; nao configura o tipo de arquivo

; Obs: esta rotina nao configura o tipo de arquivo nem outros atributos. apenas salva
; os dados
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; apagar - rotina de item a fazer...se o arquivo for nulo retirar apenas o item do diretorio
; se tiver extensao deve-se recuperar o espaço vazio do disco. Se o arquivo for o ultimo
; arquivo gravado seria somente somar a quantidade de setores liberados ao bloco, mas essa
; possibilidade é remota, de forma que de inicio deve-se gerar um item de lista.
; para apagar um arquivo fazer...correr o diretório para encontrar o item, criar um item
; equivalente na lista de espaço vazio, liberar o item do nome, do comentario se existir
; e os fragmentos.

; IN:
;	ax= tipo (max 255)
; 	ebx= diretorio inicial
;	cx= extensao do nome
;	esi= base do nome

DeleteFile:

	call busca_arquivoTipoAX
	jb .error
 	call apaga_item
	jb .error
	call salva_diretorio		 ; salva diretorio e DM
	mov esi, [ebx+esi+I_soma]
	xor ebp,ebp
.error:	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; renomeia arquivo
; IN:
;	ax: tipo
;	ebx: diretorio (*)
; 	dx: extensao do novo nome
;	edi: novo nome
;	esi: nome
;	cx: extensao do nome

; OUT:
;	cy=1 - error
align 4
RenameFileFolder:
	call busca_arquivoTipoAX	; esi= item encontrado
	jb .error
	xchg esi,edi	; ebx: diretorio, edi: item, esi= novo nome
	mov ecx, eax	; cl: extensao do nome
	mov eax, edx	; al: extensao do novo nome
	call renomeia_item
	jb .error1
	call SaveActualFolder
	jb .error			; nao obteve sucesso na gravaçao
	mov esi, [ebx+esi+I_soma]
	xor ebp,ebp
.error:	ret			; error: arquivo nao encontrado (error 15)

; falta espaço no heap local do diretorio (realocar)
.error1:	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; changes attributes of a file
; IN:
; 	ax: tipo
;	ebx: diretorio (*)
;	esi: nome
;	cx: extensao do nome
; 	eax: attrib/tipo   (o icone é associado ao tipo de arquivo)
; OUT:
;	cy= 1 - error1
align 4
ChangeFileAttrib:
	call busca_arquivoTipoAX
	jb .error
	mov [ebx+esi+I_tipo], eax
	mov esi, [ebx+esi+I_soma]		; retorna a soma dos bytes do arquivo
	xor ebp,ebp
.error:	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; get time and data of a file
; IN:
;	ax: tipo
;	ebx: diretorio
;	esi: nome
;	cx: extensao do nome
; OUT:
;	cy= 1 - error1
align 4
GetDataTime:
	call busca_arquivoTipoAX
	jb .error
	mov eax,[ebx+esi+I_datamod]	; data & time of last modificaçao in file
	mov ecx,[ebx+esi+I_datacriacao]	; data da criação do arquivo
	mov edx, [ebx+esi+I_datacriacao]	; data e hora do ultimo acesso
	mov esi, [ebx+esi+I_soma]		; retorna a soma dos bytes do arquivo
	xor ebp,ebp
.error:	ret

;-----------------------------------------------------------------------------------------





;-----------------------------------------------------------------------------------------
;
; FOLDER SERVICES
;
;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

; Estrutura da lista de arquivos de uma pasta

	FL_Prox	   equ 0x0	; dd - ponteiro para proximo item
	FL_Ant	   equ 0x4	; dd - ponteiro para item anterior
	FL_Ass	   equ 0x8	; dw - assinatura
	FL_Data	   equ 0xa	; dw - data de criacao
	FL_Hora	   equ 0xc	; dw - hora de criacao
	FL_Data2     equ 0xe	; dw - data de modificacao
	FL_Hora2     equ 0x10	; dw - hora de modificacao
	FL_SubTipo   equ 0x12	; db - subtipo do item (jpeg,bmp,mp3,mpeg,...)
	FL_Tipo	   equ 0x13	; db - tipo do item (pasta,texto,musica,...)
	FL_Rs1	   equ 0x14	; dw - free
	Fl_Rs2	   equ 0x16	; db - free
	FL_Color     equ 0x17	; db - cor do nome
	FL_Comment   equ 0x18	; dd - ponteiro para o comentario
	FL_Name	   equ 0x1c	; dd - ponteiro para o nome

FL_ItemSize equ 0x20
FL_Assinatura equ "FL"

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Create folder								ok
; in:
;	esi= ponteiro do nome
;	cx= extensao do nome
;	ebx= base do diretório (*)
; out:
;	ebx=ponteiro do diretório pai
;	edi=ponteiro do novo diretorio
align 4
CreateFolder:
	push esi
	mov eax, 1		; tipo: pasta
	call busca_arquivoTipoAX	; se o item nao existe esi=0, ebx=updated, eax= x
	pop esi
	jae .error		; o item já existe...sai
	call cria_item		; edi= item criado
	mov esi, edi
	push ebx
	call cria_diretorio_n0	; ebx= aponta DM (altera DM )
	jb .error1
	call SaveActualFolder
	jb .error1
	pop ebx
	call salva_diretorio
.error1:	ret
.error:	stc
	mov ebp, ERROR_EXIST_FILE 	; pasta já existe
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; changes attributes in a folder
; IN:
;	ebx: diretorio (*)
;	esi: nome
;	cx: extensao do nome
; 	ax: attrib
; OUT:
;	cy= 1 - error1
align 4
ChangeFolderAttrib:
	mov eax, 1
	call busca_arquivoTipoAX
	jb .error
	;mov ax, 0x1			; nao muda o tipo, apenas os atributos
	;mov [ebx+esi+I_tipo], eax		; idem
	mov [ebx+esi+I_attrib], ax
	mov esi, [ebx+esi+I_soma]		; retorna a soma dos bytes do arquivo
	xor ebp,ebp
.error:	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; apaga arquivo
; IN:
;	ebx: diretorio (*)
;	esi: nome
;	cx: extensao do nome
;
align 4
DeleteFolder:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Lista uma pasta
; IN:
;	bl = partition
;	ecx= tamanho da string
;	esi= path
; OUT:
;	esi= ponteiro do primeiro item da lista

LPFS_Folder_List:
	push eax
	push ebx
	push ecx
	push edx
	push edi

; Abre o diretorio
	mov ebx, [BootAdmFolderBase]
	call LPFS_Folder_Open_n0		; Abre o diretorio
	jc .error

; Cria a lista de arquivos
.c:	xor ecx, ecx
	mov esi, [ebx+D_ini_item]
	or esi, esi
	jz .end

   ; Cria o item
	push esi
	mov eax, FL_ItemSize
	call SMM_MainHeap_Alloc
	jc .error
	mov ecx, esi		; salva em ecx o ponteiro do primeiro item
	mov edi, esi
	mov dword [edi+FL_Ant], 0
	pop esi
	jmp .e

; Enquanto nao terminar os itens do diretorio
.d:	or esi, esi
	jz .end
   ; Aloca um bloco
	push esi
	mov eax, FL_ItemSize
	call SMM_MainHeap_Alloc
;int 1
	jc .error
	mov edi, esi
	pop esi

   ; Coloca as variaveis no bloco
   	mov [edx+FL_Prox], edi
   	mov dword [edi+FL_Ant], edx
.e:	mov dword [edi+FL_Prox], 0
	mov word [edi+FL_Data], 0
	mov word [edi+FL_Hora], 0
	mov word [edi+FL_Data2], 0
	mov word [edi+FL_Hora2], 0
	mov byte [edi+FL_Tipo], 0
	mov byte [edi+FL_Color], 0
	mov word [edi+FL_Ass], FL_Assinatura
	mov edx, edi

   ; Coloca o nome
	movzx eax, byte [ebx+esi+I_ext_nome]
	inc eax
	push esi
	call SMM_MainHeap_Alloc
;int 1
	jc .error
	dec eax
	mov [edi+FL_Name], esi
	mov edi, esi
	pop esi

      ; Copia o nome
	push ecx
	push esi
	mov esi, [ebx+esi+I_nome]
	add esi, ebx
	mov ecx, eax
	rep movsb
	mov byte [edi], 0
	pop esi
	pop ecx

   ; Pega o proximo item
	mov esi, [ebx+esi]
	jmp .d

; Fim
.end:	mov esi, ecx
	pop edi
	pop edx
	pop ecx
	pop ebx
	pop eax
	ret

; Error - Pasta invalida ou sem memoria
.error:	stc
	xor esi, esi
	pop edi
	pop edx
	pop ecx
	pop ebx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Lista uma pasta
; IN:
;	esi= ponteiro do primeiro item da lista
; OUT:
;	cy=0	- ok
;	cy=1	- error

LPFS_Folder_Close:
	push eax
	push esi

.a:	or esi, esi
	jz .end
	cmp word [esi+FL_Ass], FL_Assinatura
	jne .error
	mov eax, [esi]
	call lib_HB;Heap_Lib
	mov esi, eax
	jmp .a

.error:	stc
.end:	pop esi
	pop eax
	ret

;-----------------------------------------------------------------------------------------
