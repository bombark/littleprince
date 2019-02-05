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
; @Id: Start.inc - 2006-11-26, fb-toledo
; Protected and paging mode start
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------

BufferTemp	equ 0x0c00

align 4
GDT_temp:
	dw end_GDT_Table-GDT_Table+8	; size of GDT
	dd GDT_Table-OS_BASE-8	; position of GDT

use16
inicio16:
; detecta unidades de disco
	;call busca_DiscosMR	; detecta unidades FD e HD e monta as tabelas     OK
	cli
.a1:	in al, 0x64
	test al, 2
	jnz .a1
	mov al, 0xD1
	out 0x64, al
.a2:	in al, 0x64
	test al, 2
	jnz .a2
	mov al, 0xDF
	out 0x60, al

; posiciona modo texto 3
;       mov ax, 0x1003
;       xor bx, bx
;       int 0x10

; inicia o modo grafico
  ; pega informacoes
	mov ax, 0x4f01
	mov cx, 0x4118
	mov di, Vesa_Desc;-OS_BASE
	int 0x10

  ; inicia o modo
	mov ax, 0x4f02
	mov bx, 0x4118
	mov di, Vesa_Desc
	int 0x10

; posiciona gdt temporaria
	lgdt [GDT_temp] ; ponteiro da gdt...trava nesta instrução

; entra no modo protegido
	mov eax,cr0
	or al, CR0_PE
	mov cr0, eax
	jmp far pword segCode:Protected_Mode

use32
align 4
Protected_Mode:

; inicia os descritores (modo flat)
	mov ax, segData
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, 0x9fffe

; Configura a memoria paginada
	mov dword [OS_PageTable],0x87
	mov dword [OS_PageTable+0x800], 0x0000187
	mov dword [OS_PageTable+0x804], 0x0400187
	mov dword [OS_PageTable+0x808], 0x0800187
	mov dword [OS_PageTable+0x80c], 0x0c00187
	mov dword [OS_PageTable+0x810], 0x1000187	; imagem
	mov dword [OS_PageTable+0x814], 0x1400187	; imagem

; CR4 -> Ativa pagina de 4mb e paginas globais
	mov eax, cr4
	or eax, CR4_PSE
	or eax, CR4_PGE
	mov cr4, eax

; CR0 -> modo paginado
	mov eax, OS_PageTable
	mov cr3, eax

; carrega a nova GDT e a IDT (passa o ponteiro da GDT para posicao 0x80000000+base_GDT)
	mov eax, cr0
	or eax, CR0_PG + CR0_WP	;    *para desativar o modo paginado
	mov cr0, eax		;    *apagar as instruções marcadas
	lgdt [aponta_GDT]		; ponteiro da gdt
	jmp far pword segCode:Paging_Mode ; Pula pro arquivo INICIO_Paginado.inc

org $ + OS_BASE 		;                                       *com asteriscos
align 4
Paging_Mode:
	add esp, OS_BASE		; fix esp
	mov byte [Timer_flags], 1	; Desativa a troca de tarefa

; inicializa o PIC

	mov al, 0x11	; ou 11
	out  0x20,al
	mov al, 0x20
	out  0x21,al
	mov al, 0x04
	out  0x21,al
	mov al, 0xd1	; 01
	out  0x21,al
	mov al, 0x11	; ou 11
	out  0xa0,al
	mov al, 0x28
	out  0xa1,al
	mov al, 0x02
	out  0xa1,al
	mov al, 0xd1	; 01
	out  0xa1,al

	lidt [aponta_IDT]

; Inicia a Tarefa do sistema
;.now:
	mov ax, segTSS_OS
	ltr ax
	sti		; tem que ficar aqui, senao DetectaPartição32 nao funciona

	mov dword [CharTable], Font_Table	; opção fonte imbutida no kernel

; inicia os sistemas
	call init_Memory	; inicia a memoria
	call init_GraphSystem	; inicia o sistema grafico
	call init_TaskSystem	; inicia o sistema de tarefa
	call Object_Init	; inicia o sistema de objetos
	call init_Keyboard	; inicia o teclado
	call HD_infoSeek       	; monta as tabelas HD (substitue rotina #16 busca_DiscosMR)
	call DetectaPartiçao32 	; monta a tabela de partiçoes
	call InitStartDisk      ; load initial folders
	call init_FileManager	; Inicia o manipulador de arquivos
	call load_fonts		; carrega fonte padrao para video (tipo bmp)
	call init_OSGraph 	; Coloca o papel de parede
	call init_BIOS32	; verifica a existencia bios32
	call init_Mouse 	; inicia o mouse
	call SC_Init		; Inicia a instrucao SysEnter e SysCall

	;call ExecuteMM		; Dispara Mostra memoria como tarefa N3 (provisorio)

	;call LPFStest
	;call testFS		; testes do sistema de arquivos


	;call VideoTest
	;call V2

	mov byte [Timer_flags], 0	; Ativa a troca de tarefa

; Cria uma imagem
;mov eax, 0x00300030
;mov ebx, 0x00800080
;mov ecx, 0x00040000
;mov edx, [OS_Window]
;call Object_Create
;mov dword [edi+0x30], Icone_Desc
;call Object_Draw

; Cria um Icone
;mov eax, 0x00100010
;mov ebx, 0x00700050
;mov ecx, 0x00070001
;mov edx, [OS_Window]
;call Object_Create
;call Object_Draw

	jmp Teste_Main
	;jmp Exp_Main
	;jmp Mm		; mostra a memoria
	;jmp Calc_Main	; calculadora


OS_loop:
	jmp $
	call Mouse_Click
	;call SWN_Event_Run

	call SWN_Event_Get
	or cx, cx
	jz OS_loop
;int 1
	cmp cx, 1
	jne OS_loop
	mov esi, texto01
	call File_Execute

	;call SWN_Event_Run		; Executa eventos pre-programados
	jmp OS_loop



;-----------------------------------------------------------------------------------------

align 8
aponta_GDT:
	dw end_GDT_Table-GDT_Table+8-1	; size of GDT
	dd GDT_Table-8			; position of GDT

aponta_IDT:
	dw IDT_end-IDT_Table-1		; size of IDT
	dd IDT_Table			; position of IDT

texto01	db "d:\calc.bin",0
texto02	db "d:\icone.bmp",0
texto03	db "Calculad",0
texto04	db "d:\",0
icone01   dd 0x00200020,0x00550050,0x00080005,0x00050008,0x00400040,0x00450000,0x00100050
icone02   dd 0x00600020,0x00400040,0x00080005,0x00050010,0x00200020,0x00280008,0x00100020
botao01	dd 0xff000002,0x00200020,0x00200020,0x00020001
	dd 0xff000002,0x00200050,0x00200020,0x00020002
	db 0x00

Icone_Desc  rb 0x20

;-----------------------------------------------------------------------------------------



