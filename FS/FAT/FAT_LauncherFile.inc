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
; @Id: fat1632_execute.inc - 2008-02-10 cb-toledo
; Launcher file for FAT file system
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------

; Variaveis colocado na pilha por um int
;reg_EIP		equ 0x24
;reg_CS		equ 0x28
;reg_EFLAGS	equ 0x2c
;
;; Variaveis colocados na pilha por um pushad
;reg_EAX		equ 0x20
;reg_ECX		equ 0x1c
;reg_EDX		equ 0x18
;reg_EBX		equ 0x14
;reg_ESP		equ 0x10
;reg_EBP		equ 0xc
;reg_ESI		equ 0x8
;reg_EDI		equ 0x4

; lança o primeiro setor e verifica. se for executável instala no anel de execução
; se nao for apenas carrega o arquivo no endereço fornecido em edi, ou sai com
; indicação de erro.

;-----------------------------------------------------------------------------------------
;
; IN:
;        	al = 0 procura no path al=1 procura no diretorio aberto
;	edx= pointer to filename
; 	bl= partition (*)
;	esi= data block memory base (*0
; OUT:
;

align 4
FAT_execute:

; Variaveis
FE_file	equ 0x1c		; arquivo (eax)
FE_part	equ 0x10		; particao (bl)
FE_DBM	equ 0x04		; Data block memory (esi)

	pushad

; Pega informacoes do programa
  	mov eax, 0x200
	call aloca_HB
	jb .error			;-------------------------------------------------
	mov edi, esi
	push esi

;int 1
	mov eax, [esp+FE_file+4]	; filename pointer
	mov ebp, 0
	mov ecx, 0		; qtde de setores a desconsiderar
	mov edx, 1		; qtde de setores a ler
	mov esi, [esp+FE_DBM+4]
	push edi
	call FAT_PartialFileRead	; eax= cluster inicial da cadeia
	pop edi			; edx= ponteiro do item no diretorio
	pop esi
	jb .error2			;-------------------------------------------------
; Arredonda o tamanho dos dados
	;push edx
	push eax
; verifica assinatura
	cmp [edi], dword 'LP01'
	jne .error1		;-------------------------------------------------
	mov edx, [edi+0x4]
	mov eax, [edi+0x8]

	call lib_HB
	jb .error3		;-------------------------------------------------

	or eax, eax
	jnz .st
	inc eax
.st:	add eax, 0x3fffff
	and eax, 0xffc00000

; Cria a tarefa

	mov cl, 0x5
	mov esi, [esp+FE_file+4]
	call create_Task
	sub eax, 0x30
	mov ebp, eax
	pop eax
	;xchg edx, [esp]
	mov cr3, esi
;int 1
; Carrega o programa

	mov esi, [esp+FE_DBM]
	xor edi,edi		; endereço 0x20
	call FAT_GetDCCC		; carrega todo cluster
	;pop edx
	jb .error			;-------------------------------------------------

       	;call FAT_ReadClusterChain0


	mov byte [Timer_flags], 1	; desativa troca d tarefas

; Ajusta os registradores
	xor eax, eax
	mov dword [ebp+reg_RET], int_Timer.fim
	mov [ebp+reg_EDI], eax
	mov [ebp+reg_ESI], eax
	mov [ebp+reg_EBP], eax
	mov dword [ebp+reg_ESP], eax
	mov [ebp+reg_EBX], eax
	mov [ebp+reg_EDX], eax
	mov [ebp+reg_ECX], eax
	mov [ebp+reg_EAX], eax
	mov dword [ebp+reg_EFLAGS], 001000000010b	; 001100000010b (passoapasso)
	mov dword [ebp+reg_CS], segCode
	mov dword [ebp+reg_EIP], edx
	mov eax, OS_PageTable
	mov cr3, eax
	mov byte [Timer_flags], 0 		; ativa troca de tarefas

  .error:
  	popad
	ret


  .error2:
	call lib_HB
	jb @f

  .error1:
	mov ebp, 1
  .error3:
  	pop eax
   @@:	stc
	popad
	ret


;-----------------------------------------------------------------------------------------