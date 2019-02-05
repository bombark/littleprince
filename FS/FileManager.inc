;-----------------------------------------------------------------------------------------
;
; LITTLE PRINCE Operational System
; Copyright (C) 2008 to 2011 - LP Development Team
;
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
; @Id: FileManager.inc - 2008-02-22 - fb-toledo
; For operation with files
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------


;======================== C O M E N T A R I O S ==========================================

; File_Table:
;   A tabela de arquivos vai guardar todos os arquivos abertos de todos os programas

; Programa:
; No item do programa vai ter 2 ponteiros para espacos alocados
;   FAT
;     Bloco para FAT
;     Bloco para o diretorio
;   LPFS
;     Bloco para o diretorio
; Logo vao ter 2 ponteiros, 1 para apontar a FAT e ou o diretorio, no LPFS a FAT será 0

; Estrutura de cada item na tabela File_Table:
;   OBS: Se algum dia mudar o tamanho do FileDesc, deverá mudar o calculo
;        para pegar o item na tabela

; Bloco das Lbas:
;   Para o LPFS, o bloco das lbas vai conter uma tabela (Base,Ext)
;   com todas as lbas do arquivo. Ja pra a fat, vai conter apenas o 1° cluster do arquivo

; Lista de erro
; 	1- Arquivo nao encontrado
;	2- Descritor invalido
;	3- Erro na Leitura/Escrita		(a definir junto com o manipulador do HD)

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================
; Posicao da tabela na memoria
File_Table 	dd 0		; dd - Tabela de arquivos
File_Table_Qtde	dw 0		; dw - Quantidade total de itens na tabela

; Estrutura de cada item da tabela (item do arquivo)
  File_Name	equ 0x0		; dd - ponteiro do nome - Nao mudar esse de lugar
  File_Reserved	equ 0x4		; dd - guarda o 1° cluster do arquivo (FAT)
  File_Program	equ 0x8		; dd - ponteiro do item do programa que o abriu
  File_Offset	equ 0xc		; dw - deslocamento no setor (SEEK)
  File_SizeOffset	equ 0xe		; dw - Tamanho do arquivo no ultimo setor (0-0x200)
  File_Lba	equ 0x10		; dd - lba (SEEK)
  File_Size	equ 0x14		; dd - Tamanho do arquivo em setores - 1
  File_Directory	equ 0x18		; dd - Bloco com a FAT ou com o diretorio
  File_Partition	equ 0x1c		; db - Numero da particao
  File_Attr	equ 0x1d		; db - atributos
  File_Rs		equ 0x1e		; dw - free
  File_BufferSize	equ 0x20		; dd - tamanho do buffer
  File_Buffer	equ 0x24		; dd - ponteiro do buffer

FileDesc_Size	equ 0x40

; Estrutura da LBA no bloco alocados
      FileLba_Base	  equ 0	; dd - LPFS
      FileLba_Ext 	  equ 4	; dd - LPFS
      FileCluster	  equ 0	; dd - FAT

; Estrutura do Buffer de arquivo
      FileBuffer_Ass	equ 0	; dw - Assinatura
      FileBuffer_LbaExt	equ 2	; dw - free
      FileBuffer_Lba	equ 4	; dd - lba
      FileBuffer_Status	equ 8	; db - Status do buffer (1->com dados,2->sujo)
      FileBuffer_Rs		equ 9	; db - reservado
      FileBuffer_Cont	equ 0x0a	; dw - contador do buffer
      FileBuffer_Prox	equ 0x0c	; dd - Proximo buffer
      FileBuffer_Start	equ 0x10	; rb - Inicio do buffer (tambem é o tamanho do cabecario)

;-----------------------------------------------------------------------------------------


;======================== F U C T I O N   1 ==============================================
; Inicia a tabela de arquivo
; obs: aloca um bloco com capacidade para 32 descritores (arquivos abertos)
; IN:	nothing
; OUT:	nothing
; Altera: eax, ecx, esi

init_FileManager:
; Aloca o bloco
	mov eax, FileDesc_Size*20		; aloca 20 espacos para os FileDesc
	call aloca_HB
	mov [File_Table], esi
	mov [File_Table_Qtde], 20

; Zera o bloco
	xor eax, eax
	mov ecx, FileDesc_Size*20/4
.a:	mov [esi], eax
	add esi, 4
	loop .a

	ret


;-----------------------------------------------------------------------------------------


;======================== F U C T I O N   2 ==============================================
; Desinicia a tabela de arquivo
; IN:
; OUT:

deinit_FileManager:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U C T I O N   3 ==============================================
; Abre um arquivo
; IN:
;	 cx= tamanho do buffer em setores
;	esi= nome do arquivo (Deve ser uma string terminada em 0)
; OUT:
;	edx= descritor do arquivo
;	al= Numero do Erro
;	cy= 0	- OK
;	cy= 1 	- Erro
; OK - Precisa salvar os registradores

File_Open:
	push eax ebx ecx esi edi

; Procura descritor livre
	mov edi, [File_Table]
	xor edx, edx
.a1:	mov eax, [edi+edx]
	or eax, eax
	jz .found
	add edx, FileDesc_Size
	movzx eax, word [File_Table_Qtde]
	shl eax, 6
	cmp edx, eax
	jae .error			; verificar se está correto o tamanho
	jmp .a1

.found:	add edi, edx

; Coloca as variaveis diretas
   	mov ecx, 1
   	shl ecx, 9	
	mov [edi+File_BufferSize], ecx
	xor ecx, ecx
	mov ebx, [Task_Current]
	mov [edi+File_Program], ebx		; zera tudo
	mov [edi+File_Directory], ecx
	mov [edi+File_Offset], cx
	mov [edi+File_Lba], ecx
	mov [edi+File_Attr], cx		; definir como parametro

; Pega o numero da particao
	mov cl, [esi]
	cmp byte [esi+1], ':'
	je .absoluto
	push esi
	mov esi, [ebx+Task_Path]
	mov cl, [esi]
	pop esi
	jmp .putPart

.absoluto:add esi, 3			; retira o c:\
.putPart:	or cl, 0x20
	sub cl, 0x60
	mov [edi+File_Partition], cl

; Abre o sistema de arquivo para o arquivo
	call File_FsOpen	
	jc .error	;.error1

; Cria um buffer - Depois otimizar o buffer
	push esi
	call File_CreateBuffer	; Verificar pq trava quando entra no jc	
	jc .error
	mov [edi+File_Buffer], esi
	pop esi

; Copia o nome do arquivo
	push esi
	call strlen
	inc ecx			; Pegar o 0 no final da string
	mov eax, ecx
	call aloca_HB		; depois mudar para SMM_Heap_Alloc
	jc .error
	mov [edi+File_Name], esi
	mov edi, esi
	pop esi
	push esi
	call cpystr
	pop esi

; Fim
	shr edx, 6
	pop edi esi ecx ebx eax
	ret

.error:	stc
	pop edi esi ecx ebx eax
	ret

; Arquivo nao encontrado
.error1:
;int 1
	mov esi, [edi+File_Directory]
	call lib_HB
	stc
	pop edi esi ecx ebx eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U C T I O N   4 ==============================================
; Fecha um arquivo
; IN:
;	edx= descritor do arquivo
; OUT:
;	nothing
; depois colocar SMM_Heap_Free ao inves de lib_HB
; OK

File_Close:
	push ebx
	push edx
	push esi
	
	cmp dx, [File_Table_Qtde]
	ja .error
	shl edx, 0x6
	add edx, [File_Table]
	cmp dword [edx], 0
	jz .error

; Libera os blocos alocados
	call File_DestroyBuffers
	mov dword [edx+File_Buffer], 0
	mov esi, [edx+File_Name]	; libera o nome do arquivo
	call lib_HB
	mov dword [edx+File_Name], 0
	movzx ebx, byte [edx+File_Partition]
	dec ebx
	shl ebx, 5
	test byte [TabelaPart+ebx+Part_tipo], 0x20	; compara se eh fat
	jz .pula				; se nao for fat, nao libera o diretorio	
	mov esi, [edx+File_Directory]		; Libera a Fat
	call lib_HB

; Marca o item como livre
.pula:	mov dword [edx], 0 		; Zerar o ponteiro do nome

.error:	pop esi
	pop edx
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U C T I O N   5 ==============================================
; Le um arquivo
; IN:
;	ecx= quantidade de bytes
;	edx= descritor do arquivo
;	edi= buffer
; OUT:
;	ebp= numero do erro
;	cy=0	- OK
;	cy=1	- Erro
; OK - Falta verificar quando o arquivo terminou!!!
;      E ele tah lendo a Fat ao inves do arquivo

File_Read:

	pushad
	mov ebx, ecx		;ebx= contador de bytes restantes

; Pega o descritor
	cmp dx, [File_Table_Qtde]
	ja .error
	shl edx, 6
	add edx, [File_Table]
	cmp dword [edx], 0
	jz .error

; Verifica se terminou o arquivo	
	mov eax, [edx+File_Lba]
	cmp eax, [edx+File_Size]
	jb .loop
	ja .endFile
	movzx ecx, word [edx+File_Offset]
	cmp cx, [edx+File_SizeOffset]
	jae .endFile
	add ecx, ebx
	cmp cx, [edx+File_SizeOffset]
	jb .loop
	movzx ebx, word [edx+File_SizeOffset]
	movzx ecx, word [edx+File_Offset]
	sub ebx, ecx

; Verifica se no buffer já contem as informacoes
.loop:	mov esi, [edx+File_Buffer]
	test byte [esi+FileBuffer_Status], 1
	jz .ler
	cmp eax, [esi+FileBuffer_Lba]
	jb .ler
	push eax
	push ebx
	sub eax, [esi+FileBuffer_Lba]
	mov ebx, [edx+File_BufferSize]
	shr ebx, 9
	sub eax, ebx
	pop ebx
	pop eax
	js .contem

   ; lê o novo setor
.ler:	mov [esi+FileBuffer_Lba], eax
	or byte [esi+FileBuffer_Status], 1
	call File_InterRead

.contem:
; Calcula os bytes restantes e copia os dados	
	mov ecx, ebx
	sub eax, [esi+FileBuffer_Lba]
	shl eax, 9			; transforma para bytes
	add esi, FileBuffer_Start
	add esi, eax
	movzx eax, word [edx+File_Offset]	; Calcula quantos bytes estao dentro do buffer
	add esi, eax
	add eax, ebx
	cmp eax, [edx+File_BufferSize]
	jbe .lastcopy
	mov ecx, [edx+File_BufferSize]
	sub ecx, eax
	sub ebx, ecx
	call File_IncSeek		; Atualiza o Seek	
	call cpystr		; Copia os dados	
	add edi, [edx+File_BufferSize];0x200
	jmp .loop

; ultima copia
.lastcopy:	
	call File_IncSeek	
	call cpystr
	
	popad
	ret

.endFile:	stc
	mov ebp, 1		; Depois definir os erros
	popad
	ret


.error:	stc
	popad
	ret

;-----------------------------------------------------------------------------------------


;======================== F U C T I O N   6 ==============================================
; Escreve um arquivo
; IN:
;	ecx= quantidade de bytes
;	edx= descritor do arquivo
;	edi= buffer
; OUT:
;	 al= numero do erro
;	cy=0	- OK
;	cy=1	- Erro

File_Write:
	pushad
	mov ebx, ecx		;ebx= contador de bytes restantes

; Pega o descritor
	cmp dx, [File_Table_Qtde]
	ja .error
	shl edx, 6
	add edx, [File_Table]
	cmp dword [edx], 0
	jz .error

; Verifica se terminou o arquivo
	mov esi, [edx+File_Buffer]
	add esi, FileBuffer_Start
	mov eax, [edx+File_Lba]
	cmp eax, [edx+File_Size]
	jb .loop
	ja .endFile		; necessario expandir o arquivo
	movzx ecx, word [edx+File_Offset]
	cmp cx, [edx+File_SizeOffset]
	jae .endFile		; necessario expandir o arquivo
	add ecx, ebx
	cmp cx, [edx+File_SizeOffset]
	jb .loop
	movzx ebx, word [edx+File_SizeOffset]
	movzx ecx, word [edx+File_Offset]
	sub ebx, ecx

; Calcula os bytes restantes e copia os dados
.loop:	mov ecx, ebx
	movzx eax, word [edx+File_Offset]	; Calcula quantos bytes estao dentro do buffer
	add esi, eax
	add eax, ebx
	cmp eax, [edx+File_BufferSize]
	jb .lastcopy
	je .a
	mov ecx, [edx+File_BufferSize]
	sub ecx, eax
.a:	sub ebx, ecx
	xchg esi, edi
	call cpystr		; Copia os dados
	;call File_InterWrite
	xchg esi, edi
	call File_IncSeek		; Atualiza o Seek
	add edi, 0x200
	jmp .loop

; Ultima copia (É necessario ler o setor e depois copiar os dados)
   ; Verifica se o setor ja está na memoria
.lastcopy:or ebx, ebx		; verifica se precisa copiar mais dados
	jz .end
	mov esi, [edx+File_Buffer]
	test byte [esi+FileBuffer_Status], 1
	jz .ler
	cmp [esi+FileBuffer_Lba], eax
	je .contem

   ; lê o novo setor
.ler:	mov [esi+FileBuffer_Lba], eax
	or byte [esi+FileBuffer_Status], 1
	call File_InterRead

   ; Coloca os dados no setor
.contem:	xchg esi, edi
	add edi, FileBuffer_Start
	call cpystr
	call File_IncSeek
	or byte [edi+FileBuffer_Status-FileBuffer_Start], 2 ; marca como buffer sujo
.end:	popad
	ret

; Expande o arquivo
.endFile:	
int 1
	call File_InterExpand
	jmp .loop


; Error - 
.error:	stc
	popad
	ret

;-----------------------------------------------------------------------------------------


;======================== F U C T I O N   5 ==============================================
; Le todo arquivo
; IN:
;	edx= descritor do arquivo
;	edi= destino
; OUT:
;	 al= numero do erro
;	cy=0	- OK
;	cy=1	- Erro

File_AllRead:
	pushad

; Pega o descritor
	cmp dx, [File_Table_Qtde]
	ja .error
	shl edx, 6
	add edx, [File_Table]
	cmp dword [edx], 0
	jz .error

; Pega o tipo do sistema de arquivo
	movzx ebx, byte [edx+File_Partition]
	dec ebx
	shl ebx, 5
	test byte [TabelaPart+ebx+Part_tipo], 0x20		; compara se eh fat
	jnz .Fat

; LPFS - leitura de arquivo
	mov ebx, [edx+File_Directory]
	mov esi, [edx+File_Reserved]
	call TrasferConSeg
	popad
	ret

; FAT - leitura de arquivo
.Fat:	mov eax, [edx+File_Reserved]
	mov bl, [edx+File_Partition]
	xor ecx, ecx
	mov esi, [edx+File_Directory]
	mov edx, [edx+File_Size]
	inc edx
	call FAT_Item_Read
	popad
	ret

.error:	stc
	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U C T I O N   6 ==============================================
; Grava todo o arquivo
; IN:
;	ecx= quantidade de bytes a serem gravados
;	edx= descritor do arquivo
;	edi= buffer
; OUT:
;	 al= numero do erro
;	cy=0	- OK
;	cy=1	- Erro

File_AllWrite:

	pushad

; Pega o descritor
	cmp dx, [File_Table_Qtde]
	ja .error
	shl edx, 6
	add edx, [File_Table]
	cmp dword [edx], 0
	jz .error

; Grava o arquivo
	mov eax, [edx+File_Reserved]
	mov bl, [edx+File_Partition]
	mov esi, [edx+File_Directory]
	;call FAT_SetDCCC

.error:	popad
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Devolve o tamanho do arquivo
; IN:
;	edx= descritor do arquivo
; OUT:
;	eax= tamanho do arquivo em bytes (arrendondado para setor)

File_GetSize:
	push edx

; Pega o descritor
	cmp dx, [File_Table_Qtde]
	ja .error
	shl edx, 6
	add edx, [File_Table]
	cmp dword [edx], 0
	jz .error

; Pega o tamanho do arquivo
	mov eax, [edx+File_Size]
	inc eax
	shl eax, 9		; multiplica 512 (tamanho de um setor)
	cmp eax, 0x80000000		; 2gb
	jb .end
	mov eax, 0x80000000
.end:	pop edx
	ret

.error:	stc
	pop edx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; Executa um arquivo
; IN:
;	esi= nome do arquivo
; OUT:
;	nothing
; OBS: melhorar o tratamento de erros

align 4
File_Execute:
	pushad
	sub esp, 0x20	; buffer de leitura
	mov edi, esp
	
; Pega informacoes do programa	
  	call File_Open 
  	jc .error1
  	mov ecx, 0x20 	
  	call File_Read

; verifica assinatura
	cmp [edi], dword 'LP01'
	jne .error2
	mov ebx, [edi+0x4]
	mov eax, [edi+0x8]
	
; Calcula a pilha	
	or eax, eax		; Arredonda eax para multiplos de 4mb
	jnz .st
	inc eax
.st:	add eax, 0x3fffff
	and eax, 0xffc00000

; Cria a tarefa
	mov cl, 0x1
	call Task_Create	
	sub eax, 0x40
	mov ebp, eax
	mov cr3, esi

; Carrega o programa
	xor edi, edi		; carrega no endereço 0
	call File_AllRead		
	mov byte [Timer_flags], 1	; desativa troca d tarefas

; Ajusta os registradores
	xor eax, eax
	mov dword [ebp+reg_RET], int_Timer.fim
	mov [ebp+reg_EDI], eax
	mov [ebp+reg_ESI], eax
	mov [ebp+reg_EBP], eax
	mov dword [ebp+reg_ESP], ebp;eax
	mov [ebp+reg_EBX], eax
	mov [ebp+reg_EDX], eax
	mov [ebp+reg_ECX], eax
	mov [ebp+reg_EAX], eax
	mov dword [ebp+reg_EFLAGS], 001000000010b	; 001100000010b (passoapasso)
	mov dword [ebp+reg_CS], segCode
	mov dword [ebp+reg_EIP], ebx
	mov eax, OS_PageTable
	mov cr3, eax

; Fecha o arquivo	
	call File_Close		
	mov byte [Timer_flags], 0 		; ativa troca de tarefas
	
; Fim	
	add esp, 0x20
	popad
	ret

; Error - Arquivo nao encontrado
.error1:	stc 
	add esp, 0x20
	popad
	ret

; Error - Arquivo nao é um executavel
.error2:	stc 
	call File_Close
	add esp, 0x20
	popad
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; Copia um arquivo
; IN:
;	esi= arquivo fonte
;	edi= arquivo destino
; OUT:
;	cy=0 -> OK
;	cy=1 -> error
;	   falta de espaco
;	   arquivo fonte nao encontrado

align 4
File_Copy:
int 1
	pushad
	
; Abre os arquivos			; eax= arquivo fonte, ebx=arquivo destino
	mov ecx, 0x10		; tamanho do buffer do arquivo
	call File_Open
int 1	
	jc .error
	mov eax, edx
	mov esi, edi
	call File_Open
int1 	
	jc .cria
	mov ebx, edx
	jmp .resize
	
; Cria o arquivo, se necessario
.cria:	

; Redefine o tamanho do arquivo	
.resize:	
	
; Copia o arquivo
   ; Ajusta as variaveis
	shl eax, 6		; Pega o ponteiro do buffer de eax
	add eax, [File_Table]
	mov esi, [eax+File_Buffer]
	shl ebx, 6		; Pega o ponteiro do buffer de ebx
	add ebx, [File_Table]
	mov ecx, [ebx+File_BufferSize]
	mov edi, [ebx+File_Buffer]
	 
   ; Verifica se terminou o arquivo 
.loop:	mov ebp, [eax+File_Lba]
	cmp ebp, [eax+File_Size]
	ja .end
	;mov ebp, [eax+File_OffsetSize]
	;or ebp, ebp
	;jz .end

   ; Leitura e gravacao
	mov edx, eax
	call File_InterRead
int 1 	
	jc .end
	push eax
	call File_IncSeek
	pop eax	
	
	mov edx, ebx
	call cpystr
int 1	
	xchg esi, edi
	call File_InterWrite	; Verificar se nao dá erro ele gravar 10 setores
	push eax
	call File_IncSeek
	pop eax
int 1	
	xchg edi, esi
	
	jmp .loop
	 
; Fecha os arquivos	 
.end:	
int 1
	sub eax, [File_Table]
	sub ebx, [File_Table]
	shr eax, 6
	shr ebx, 6
	mov edx, eax
	call File_Close
	mov edx, ebx
	call File_Close
	
; Fim
	xor ebp, ebp
	popad
	ret

.error:	mov ebp, 1
	popad
	ret

;-----------------------------------------------------------------------------------------




;======================== S U B F U N C T I O N 1 ========================================
; Calcula o novo SEEK
; Ex= /pasta1/pasta2/texto.txt, ele vai devolve o inicio da palavra texto.txt e o tamanho
; dela
; IN:
;	ecx= quantidade de bytes
;	edx= (descritor do arquivo*0x40) + [File_Table]
; OUT:
;	eax= Lba atualizada

File_IncSeek:
	push ecx
	push ecx
	shr ecx, 9
	add [edx+File_Lba], ecx
	pop ecx
	and ecx, 0x1ff
	mov ax, [edx+File_Offset]
	add ax, cx		; cx= quantidade de bytes lidos
	cmp ax, 0x200		; Tamanho do setor
	jb .pula
	and ax, 0x1ff
	inc word [edx+File_Lba]	; incrementa um setor
.pula:	mov [edx+File_Offset], ax
	mov eax, [edx+File_Lba]	
	pop ecx
	ret


;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N 3 ========================================
; Cria um buffer
; IN:
;	edx= (descritor do arquivo*0x40) + [File_Table]
; OUT:
;	esi= ponteiro do buffer
; OBS: foi feita essa rotina para depois facilitar colocar varios buffers no arquivo

File_CreateBuffer:
	push eax
	push edx
	add edx, [File_Table]
	mov eax, [edx+File_BufferSize]
	add eax, FileBuffer_Start	; tamanho do cabecario do buffer
	call aloca_HB		; trocar para SMM_Heap_Alloc
	jc .end
	xor eax, eax
	mov word [esi+FileBuffer_Ass], 'FB'
	mov [esi+FileBuffer_LbaExt], ax
	mov [esi+FileBuffer_Lba], eax
	mov [esi+FileBuffer_Status], al
	mov [esi+FileBuffer_Prox], eax
.end:	pop edx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N 4 ========================================
; Destroi os buffer criados
; IN:
;	edx= (descritor do arquivo*0x40) + [File_Table]
; OUT:
;	nothing

File_DestroyBuffers:	
	push esi
	push edi
	mov esi, [edx+File_Buffer]

; Enquanto tiver buffer -> grava e libera
.loop:	or esi, esi
	jz .end	
	cmp word [esi+FileBuffer_Ass], 'FB'	; verifica se eh o buffer
	jne .end
	test byte [esi+FileBuffer_Status], 2
	jz .free
	call File_InterWrite	
.free:	mov edi, [esi+FileBuffer_Prox]
	call lib_HB	
	mov dword [edx+File_Buffer], 0			
	mov esi, edi
	jmp .loop
	
.end:	pop edi
	pop esi
	ret


;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N 4 ========================================
; Abre unidade deacordo com o sistema de arquivo
; IN:
;	esi= nome do arquivo
;	edi= (descritor do arquivo*0x40) + [File_Table]
;	cl= particao
; OUT:
;	eax=
; Altera:
;	ecx= 0x000000pp -> pp= particao ou o tamanho do arquivo
;	ebx= numero da particao

File_FsOpen:	
	mov bl, cl
	and ecx, 0xff
	dec ecx
	shl ecx, 5
	
	mov cl, [TabelaPart+ecx+Part_tipo]
	and cl, 0xf0
	cmp cl, 0x10
	je .Lpfs 
	cmp cl, 0x20
	je .Fat 
	cmp cl, 0x30
	je .Fat
	stc
	ret

; Abre arquivo no LPFS
.Lpfs:	push eax
	push esi

	mov ebx, [Task_Current]	; Verifica tipo do programa (sistema, programa)
	test byte [ebx+Task_Attr], Task_Attr_Sistema
	jz .a
	mov ebx, [SystemFolderBase]		; abre pasta do sistema
	jmp .b
.a:	mov ebx, [BootAdmFolderBase]		; abre a pasta do usuario

.b:	mov ax, 0x10
	call strlen	
	call busca_arquivoTipoAX			
	jc .errorLPFS
.c:	mov [edi+File_Directory], ebx
	mov [edi+File_Reserved], esi
	mov eax, [ebx+esi+I_SizeSectores]
	shr eax, 9
	mov [edi+File_Size], eax		; posiciona o tamanho do arquivo em setores
	mov ax, [ebx+esi+I_SizeByte]
	mov [edi+File_SizeOffset], ax		; posiciona o final do arquivo em bytes
	
.fim:	pop esi
	pop eax
	ret

   ; Error - arquivo nao encontrado -> será criado entao
.errorLPFS:
      ; Arrumar para que ele vá direto para .a e nao para o .b
	push esi
	call CreateFile.a
	pop esi
	jmp .b

; Abre arquivo na FAT32/16
   ; Carrega o bloco da Fat (FAT32)
.Fat:		
	push edx
	push esi
	mov edx, esi
	call FAT_Open	; estah travando no Fat_open quando unidade nao eh FAT32
	
	jc .errorF
	mov [edi+File_Directory], esi

   ; Verifica se o arquivo existe
	push edi
	xor al, al
	call FAT_GetFileInfo	
	jc .errorF
	mov ebx, edi
	pop edi	
	mov [edi+File_Reserved], ebx		; Salva o 1° cluster do arquivo
	mov [edi+File_SizeOffset], ax		; posiciona o final do arquivo em bytes
	mov [edi+File_Size], ecx		; posiciona o tamanho do arquivo em setores
	pop esi
	pop edx
	ret

; Error - Arquivo nao existe
.errorF:	pop edi
	pop esi
	pop edx
	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N 5 ========================================
; Leitura de arquivo (intermediaria entre o filemanager e o arquivo)
; IN:
;	edx= (descritor do arquivo*0x20) + [File_Table]
;	esi= buffer do arquivo
; OUT:
;	o buffer do arquivo será atualizado

File_InterRead:
	pushad
	mov edi, esi
	add edi, FileBuffer_Start

; Verifica se o buffer está sujo
	test byte [esi+FileBuffer_Status], 2
	jz .limpo
	call File_InterWrite
	and byte [esi+FileBuffer_Status], 0xfd			; zera o bit 2

; Pega o tipo de sistema de arquivos
.limpo:	movzx ebx, byte [edx+File_Partition]
	dec ebx
	shl ebx, 5
	test byte [TabelaPart+ebx+Part_tipo], 0x20		; compara se eh fat
	jnz .Fat

; Leitura no sistema LPFS			;
.Lpfs:	mov eax, [edx+File_Lba]
	mov esi, [edx+File_Reserved]		;
	mov ebx, [edx+File_Directory]
	mov ecx, [edx+File_BufferSize]	
	shr ecx, 9
	call leiaA_n1	
	popad

	ret

; Leitura no sistema FAT16/32
.Fat:	mov eax, [edx+File_Reserved]
	mov bl, [edx+File_Partition]
	mov ecx, [edx+File_Lba]
	mov esi, [edx+File_Directory]
	mov edx, [edx+File_BufferSize]
	shr edx, 9		; divide por 512 bytes (tamanho do setor)
	add edx, ecx		; edx= quantidade de setores + setores pulados
	call FAT_Item_Read
	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N 6 ========================================
; Gravacao de arquivo (intermediaria entre o filemanager e o arquivo)
; IN:
;	edx= (descritor do arquivo*0x20) + [File_Table]
;	esi= ponteiro do cache do arquivo
; OUT:
;	o buffer do arquivo será atualizado

File_InterWrite:
	pushad
	mov edi, esi
	and byte [esi+FileBuffer_Status], 0xfe			; zera o bit 2
	add edi, FileBuffer_Start

; Pega o tipo de sistema de arquivos
	movzx ebx, byte [edx+File_Partition]
	dec ebx
	shl ebx, 5
	test byte [TabelaPart+ebx+Part_tipo], 0x20		; compara se eh fat
	jnz .Fat

; Leitura no sistema LPFS
.Lpfs:	mov eax, [edx+File_Lba]
	mov esi, [edx+File_Reserved]		;
	mov ebx, [edx+File_Directory]
	mov ecx, [edx+File_BufferSize]
	shr ecx, 9
	call graveA_n1
	popad
	ret

; Leitura no sistema FAT16/32
.Fat:	mov eax, [edx+File_Reserved]
	mov bl, [edx+File_Partition]
	mov ecx, [edx+File_Lba]
	mov esi, [edx+File_Directory]
	mov edx, [edx+File_BufferSize]
	shr edx, 9		; divide por 512 bytes (tamanho do setor)
	add edx, ecx		; edx= quantidade de setores + setores pulados
	;call FAT_WriteClusterChain0
	popad
	ret

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N 8 ========================================
; Expande o arquivo (intermediaria entre o filemanager e o arquivo)
; IN:
;	ebx= quantidade de bytes para expandir
;	edx= (descritor do arquivo*0x20) + [File_Table]
; OUT:
;	nothing
; Modificar a rotina para o tamanho do arquivo e nao apenas a quantidaede para expandir

File_InterExpand:
int 1	
	pushad

; Coloca o novo tamanho do arquivo no descritor
	mov ecx, ebx
	push ebx
	shr ebx, 9
	add [edx+File_Size], ebx  
	pop ebx
	and bx, 0x1ff
	add bx, [edx+File_SizeOffset] 
	cmp bx, 0x200
	jb .ok
	sub bx, 0x200
	inc dword [edx+File_Size]
.ok:	mov [edx+File_SizeOffset], bx

; Pega o tipo de sistema de arquivos
.limpo:	movzx ebx, byte [edx+File_Partition]
	dec ebx
	shl ebx, 5
	test byte [TabelaPart+ebx+Part_tipo], 0x20		; compara se eh fat
	jnz .Fat

; Leitura no sistema LPFS			
.Lpfs:	mov edi, [edx+File_Reserved]		
	mov ebx, [edx+File_Directory]
	push ecx
	shr ecx, 9
	;call aloca_espaco_item		; aloca novo espaco para o arquivo
	add [ebx+edi+I_SizeSectores], cx 	; atualiza os valores no item principal
	pop ecx
	and ecx, 0x1ff
	add ecx, [ebx+edi+I_SizeByte]
	cmp ecx, 0x200
	jb .salva
	inc dword [ebx+edi+I_SizeSectores]
	sub ecx, 0x200	
.salva:	mov [ebx+edi+I_SizeByte], cx
	call salva_diretorio		; salva o diretorio
	popad
	ret

; Leitura no sistema FAT16/32
.Fat:	mov eax, [edx+File_Reserved]
	mov bl, [edx+File_Partition]
	mov esi, [edx+File_Directory]
	;call FAT_File_Expand
	popad
	ret


;-----------------------------------------------------------------------------------------