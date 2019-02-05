;-----------------------------------------------------------------------------------------

; Syscall & SysEnter

;-----------------------------------------------------------------------------------------


;======================== C O M E N T A R I O ============================================
;   Existem 3 formas do programa se comunicar com o sistema operacional. Pela interrupcao,
; suportado por todos os processadores acima do 386, ou pela instrucoes sysenter, suportado
; pelos processadores acima do pentium 2, ou syscall, parece-me que eh um padrao da AMD,
; mas nao tenho certeza. 
;   Como sao instrucoes executadas pelo programa é necessario, caso use as instrucoes, 
; verificar se o CPU suporta as instrucoes, pelo menos uma delas. Para verificar, existe 
; um byte dentro da memoria do programa que terá a informacao

; Posicao : 0x???? 		- FAZER ESSA PARTE AINDA
;   #1 = 1 -> Suporta sysenter
;   #2 = 1 -> Suporta syscall

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

SysCall_Table:
	dd SMM_Table
	dd SWN_Table
	dd SDW_Table
	dd 0
	dd SFS_Table

SysCall_Size_Table:		; Quantidade de funcoes na biblioteca
	dw SMM_Table_Qtde
	dw SWN_Table_Qtde
	dw SDW_Table_Qtde
	dw 0
	dw SFS_Table_Qtde

SC_Service_Qtde	equ 5			; 5 Bibliotecas de servicos 


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Intel SYSENTER/SYSEXIT (AMD CPU support it too) - KOLIBRIOS COPYRIGHT
; IN:
;	nothing
; OUT:
;	nothing
; Altera:
;	eax, ecx
; Requisitos:
;	nothing

SC_Init:
  ; Verifica se o CPU suporta a instrucao SYSENTER
	;bt [cpu_caps], CAPS_SEP
	;jnc .scall   	; SysEnter not Present

  ; Configura o registrador da instrucao SYSENTER
	xor edx, edx
	mov ecx, MSR_SYSENTER_CS
	mov eax, segCode
	wrmsr
	mov ecx, MSR_SYSENTER_ESP
	mov eax, 0x800d0000	; Check it
	;xor eax, eax
	wrmsr
	mov ecx, MSR_SYSENTER_EIP
	mov eax, SC_Enter
	wrmsr
	
  ; Verifica se o CPU suporta a instrucao SYSCALL
.scall:	;bt [cpu_caps], CAPS_SEP
	;jnc .end   	; SysEnter not Present
	
  ; Configura o registrador da instrucao SYSCALL
	
.end:	ret


;-----------------------------------------------------------------------------------------


;======================== S Y S E N T E R ================================================
; Entrada do sysenter
; IN:
;	ebp: funcao
;	 0 - 15 -> funcao do Servico
;	 0 - 31 -> Numero da Biblioteca (Interrupcao)
; OUT:
;	nothing
; OBS: está com problemas na saida

SC_Enter:	
	push edi
	
  ; Chama a Rotina		 
	;movzx edi, bp
	;shr ebp, 0x10
	;cmp bp, SC_Service_Qtde		; Verifica se a biblioteca é válida
	;ja .error
	;cmp di, [SysCall_Size_Table+ebp*2]	; Verifica se a funcao é válida
	;ja .error
	;mov ebp, [SysCall_Table+ebp*4]		; Pega o ponteiro da biblioteca
	;mov ebp, [ebp+edi*4]			; Pega o ponteiro da funcao
	;pop edi
	;call dword ebp				; Executa a funcao	
	;
	;xor ebp, ebp
	
  ; Configura a Saida	
	mov ecx, esp			; ecx = esp antigo
	mov edx, 0;atemp		; edx = ponteiro da antiga rotina
	sysexit
	
.error:	xor ebp, ebp
	inc ebp
	sysexit

;-----------------------------------------------------------------------------------------


;======================== S Y S C A L L ==================================================
; Entrada do sysenter
; IN:
;	ebp: funcao
;	 0 - 15 -> funcao do Servico
;	 0 - 31 -> Numero da Biblioteca (Interrupcao)
; OUT:
;	nothing
;
;SC_Call:	
;	push edi
;		 
;	movzx edi, bp
;	shr ebp, 0x10
;	cmp bp, SC_Service_Qtde			; Verifica se a biblioteca é válida
;	ja .error
;	cmp di, [SysCall_Size_Table+ebp]	; Verifica se a funcao é válida
;	ja .error
;	mov ebp, [SysCall_Table+ebp]		; Pega o ponteiro da biblioteca
;	mov ebp, [ebp+edi]			; Pega o ponteiro da funcao
;	pop edi
;	call dword cs:ebp			; Executa a funcao	
;	
;	xor ebp, ebp
;	sysret
;	
;.error:	xor ebp, ebp
;	inc ebp
;	sysret
;
;-----------------------------------------------------------------------------------------



