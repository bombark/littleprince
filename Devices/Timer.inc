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
; @Id: Timer.inc  - 2008-1-20, fb-toledo
; Timer interruption routine
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------


;======================== I N T E R R U P T I O N  R O U T I N E =========================
;
; Entrada da interrupcao
; Funcionamento:
; IN: nothing
; OUT: nothing
align 4
int_Timer:

	pushad	
	;call Mouse_Click
.b:	dec dword [r47c]
	jnz .a
	;call rot_vigia		; ativa proteção de tela e retorna
.a:	cmp byte [Timer_flags], 0
	jnz .fim
	dec word [Timer_cont]
	mov esi, [Task_Current]	
	call Task_Change
.fim:	mov al, 0x20
	out 0x20, al
	popad
	iretd

;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N 1 ========================================
; Troca a tarefa
; IN:
;	esi: current_Task
; OUT:
;	nothing

Task_Change:
	cmp word [Timer_cont],0
	jz .troca
	ret

  ; Busca outra tarefa ativa
.troca:	mov ebx, Task_List_Ini	
	call List_NextItem

    ; Se a tarefa está ativa  -> troca de tarefa
	or edi, edi
	jnz @f
	int 1
@@:	cmp byte [edi+Task_Status], Task_Actived
	je .ok

    ; Se a tarefa está pausada -> Passa para a proxima tarefa
	cmp byte [edi+Task_Status], Task_Paused
	je .troca

    ; Se a tarefa está condenada  -> Executa a sentença. hehehehehheheh	
	cmp byte [edi+Task_Status], Task_Condenada
	jne .ok		
	push esi
	mov esi, edi		
	call Task_Destroy
	pop esi
	jmp .troca
	
  ; Troca de tarefa	
.ok:	mov al, [edi+Task_Cont]
	mov [Timer_cont], al
	cmp esi, edi			; a tarefa eh a mesma?
	je .fim

    ; Verifica a integridade da tabela de pagina - Trava o sistema se for invalida
	movzx eax, byte [edi+Task_PageTable]
	shl eax, 0x0c		; multiplica por 4kb
	add eax, PageTable
	cmp dword [OS_BASE+eax], 0
	jnz .blz
	int 1		; error - Tabela invalida

    ; Troca de pilha n0
.blz:	mov [esi+Task_Pilha_n0], esp
	mov esp, [edi+Task_Pilha_n0]

    ; Troca de tabela	
	mov cr3, eax

    ; Marca como trocando tarefa em cr0  	
	mov eax, cr0
	or eax, CR0_TS			;set task switch flag
	mov cr0, eax
.fim:	ret

;-----------------------------------------------------------------------------------------




;======================== F U N C T I O N  2 =============================================
; Interrupcao invalida
; IN:
;	nothing
; OUT:
;	nothing

Int_ret:
	mov al, 0x20
	out 0x20, al
	iretd


;-----------------------------------------------------------------------------------------




;-----------------------------------------------------------------------------------------

; Timer Sub-Task

;-----------------------------------------------------------------------------------------


;======================== C O M E N T A R I O ============================================
; Lista das subtarefas
;     4b    2b      2b      4b
; | Prox | Cont  | Cont2 | Rotina |



;=========================================================================================


;======================== V A R I A V E I S ==============================================
SubTask_List	dd 0
SubTask_List_Size	equ 0xc


;=========================================================================================


;======================== F U N C T I O N  1 =============================================
; Cria uma sub-tarefa no timer
; IN:
; OUT:

Timer_SubTask_Create:
	push eax
	push edi

; Aloca um bloco
	mov eax, SubTask_List_Size
	call aloca_HB

; Coloca o bloco na lista
  ; Procura o final da lista
	mov eax, SubTask_List
.a:	mov edi, [SubTask_List]
	or edi, edi
	jz .end
	mov eax, edi
	mov edi, [edi]
	jmp .a

  ; Coloca o ponteiro e sai
.end:	mov [eax], esi
	pop edi
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Cria uma sub-tarefa no timer
; IN:
; OUT:

Timer_SubTask_Destroy:
; Tira o bloco da lista
; desaloca o bloco
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Executa as sub-tarefas do timer
; IN:
;	nothing
; OUT:
;	nothing

Timer_SubTask_Run:
; Enquanto nao chega no fim da lista
  ; Diminui contador
  ; Se chegou a 0, Executa e destroi ou renova a rotina
	ret

;-----------------------------------------------------------------------------------------