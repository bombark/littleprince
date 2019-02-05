use32
align 4

; header

dd 'LP01'
dd Calc_Main
dd 0x800
rb 0x200-0xc


;======================== V A R I A V E I S ==============================================

; Scripts dos botoes
Calc_Script:
	db "SCP1"
	;dd 0xff000003,0x00180020,0x00180080,0x00030000,0xff0000,visor
	
       ; Numeros de 0 a 9	
	dd 0xff000002,0x00380020,0x00200020,0x00020007,bt7
	dd 0xff000002,0x00380050,0x00200020,0x00020008,bt8
	dd 0xff000002,0x00380080,0x00200020,0x00020009,bt9
	dd 0xff000002,0x00600020,0x00200020,0x00020004,bt4
	dd 0xff000002,0x00600050,0x00200020,0x00020005,bt5
	dd 0xff000002,0x00600080,0x00200020,0x00020006,bt6
	dd 0xff000002,0x00880020,0x00200020,0x00020001,bt1
	dd 0xff000002,0x00880050,0x00200020,0x00020002,bt2
	dd 0xff000002,0x00880080,0x00200020,0x00020003,bt3
	dd 0xff000002,0x00b00020,0x00200020,0x0002000a,bt0
	
       ; Operacoes (+,-,*,/) 	
	dd 0xff000002,0x006000b0,0x00200020,0x0002000b,btsum
	dd 0xff000002,0x008800b0,0x00200020,0x0002000c,btsub
	dd 0xff000002,0x006000e0,0x00200020,0x0002000d,btmul
	dd 0xff000002,0x008800e0,0x00200020,0x0002000e,btdiv
	dd 0xff000002,0x00b000b0,0x00200050,0x0002000f,btequ
	dd 0x0


; Textos dos botoes
bt1	db "1",0
bt2	db "2",0
bt3	db "3",0
bt4	db "4",0
bt5	db "5",0
bt6	db "6",0
bt7	db "7",0
bt8	db "8",0
bt9	db "9",0
bt0	db "0",0
btsum	db "+",0
btsub	db "-",0
btmul	db "*",0
btdiv	db "/",0
btequ	db "=",0

; Tabela de Rotinas indexadas nos botoes
CalcFunction_Table:
	dd Calc_Num1
	dd Calc_Num2
	dd Calc_Num3
	dd Calc_Num4
	dd Calc_Num5
	dd Calc_Num6
	dd Calc_Num7
	dd Calc_Num8
	dd Calc_Num9
	dd Calc_Num10
	dd Calc_Sum
	dd Calc_Sub
	dd Calc_Div
	dd Calc_Mul
	dd Calc_Equ

; Variaveis
visor	rb 0x20		; visor da calculadora
num3	rb 0x20		; 1 numero digitado
Pwrite	db 0		; ponteiro da posicao da escrita no visor
Operation db 0		; Operacao a ser realizada
Flag	db 0		; Flag - marca se será inserido outro numero

num1	dd 0
num2	dd 0
Calc_Window	dd 0
Calc_Visor	dd 0


;-----------------------------------------------------------------------------------------


;======================== P R I N C I P A L ==============================================

Calc_Main:
	call Calc_Init

.loop:	call Mouse_Click
	mov ebp, 4
	int 0x42
	mov ebp, 3
	int 0x42
	or cx, cx
	jz .loop
	and ecx, 0xffff
	cmp cx, 0xffff
	je .end
	dec ecx
	call dword [CalcFunction_Table+ecx*4]
	jmp .loop

.end:	mov ebp, 0x11
	int 0x42
	jmp $

;-----------------------------------------------------------------------------------------



;======================== F U N C T I O N  1 =============================================

Calc_Init:
; Cria uma janela
	mov eax, 0x00200020		;pos
	mov ebx, 0x01000120		;size
	mov ebp, 1
	int 0x42
	mov [Calc_Window], edi

; Zera as variaveis
	xor eax, eax
	mov [num1], eax
	mov [Pwrite], al
	mov [Operation], al
	mov [Flag], al

; Cria o visor
	mov eax, 0x00180020
	mov ebx, 0x00180080
	mov ecx, 0x00030000
	mov edx, [Calc_Window]
	mov ebp, 5
	int 0x42
	mov [Calc_Visor], edi
	mov byte [edi+0x30], 0x2		; aling: right
	mov dword [edi+0x38], 0xff0000	; color; red
	mov dword [edi+0x3c], visor		; pointer: visor
	mov word [visor], 0x0030
	mov ebp, 7
	int 0x42

; Executa o Scripts
	mov edx, [Calc_Window]
	mov esi, Calc_Script
	mov ebp, 0xf
	int 0x42	

; Fim
	ret


;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  2 =======================================
; Converte
; IN:
;	nothing
; OUT:
;	nothing
; Altera:
;	Tudo

CalcVisor_Copy:
; Salva o numero no visor
	mov esi, visor
	call Calc_AtoI
	mov [num1], eax
	ret

;-----------------------------------------------------------------------------------------



Calc_Num1:
	movzx esi, byte [Pwrite]
	add esi, visor
	mov byte [esi], '1'
	inc [Pwrite]
	mov byte [esi+1], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Num2:
	movzx esi, byte [Pwrite]
	add esi, visor
	mov byte [esi], '2'
	inc [Pwrite]
	mov byte [esi+1], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Num3:
	movzx esi, byte [Pwrite]
	add esi, visor
	mov byte [esi], '3'
	inc [Pwrite]
	mov byte [esi+1], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Num4:
	movzx esi, byte [Pwrite]
	add esi, visor
	mov byte [esi], '4'
	inc [Pwrite]
	mov byte [esi+1], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Num5:
	movzx esi, byte [Pwrite]
	add esi, visor
	mov byte [esi], '5'
	inc [Pwrite]
	mov byte [esi+1], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Num6:
	movzx esi, byte [Pwrite]
	add esi, visor
	mov byte [esi], '6'
	inc [Pwrite]
	mov byte [esi+1], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Num7:
	movzx esi, byte [Pwrite]
	add esi, visor
	mov byte [esi], '7'
	inc [Pwrite]
	mov byte [esi+1], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Num8:
	movzx esi, byte [Pwrite]
	add esi, visor
	mov byte [esi], '8'
	inc [Pwrite]
	mov byte [esi+1], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Num9:
	movzx esi, byte [Pwrite]
	add esi, visor
	mov byte [esi], '9'
	inc [Pwrite]
	mov byte [esi+1], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Num10:
	movzx esi, byte [Pwrite]
	add esi, visor
	mov byte [esi], '0'
	inc [Pwrite]
	mov byte [esi+1], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Sum:
	mov [Operation], '+'
	mov esi, visor
	call Calc_AtoI
	mov [num1], eax
	mov [Flag], 0x1
	mov word [visor], 0x0030
	mov byte [Pwrite], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Sub:
	mov [Operation], '-'
	mov esi, visor
	call Calc_AtoI
	mov [num1], eax
	mov [Flag], 0x1
	mov word [visor], 0x0030
	mov byte [Pwrite], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Div:
	mov [Operation], '/'
	mov esi, visor
	call Calc_AtoI
	mov [num1], eax
	mov [Flag], 0x1
	mov word [visor], 0x0030
	mov byte [Pwrite], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Mul:
	mov [Operation], '*'
	mov esi, visor
	call Calc_AtoI
	mov [num1], eax
	mov [Flag], 0x1
	mov word [visor], 0x0030
	mov byte [Pwrite], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	ret

Calc_Equ:	
	mov esi, visor
	call Calc_AtoI	
	mov [num2], eax

  ; Se operacao == '+'  -> Soma	
	mov bl, [Operation]
	cmp bl, '+'	
	jne .sub
	add eax, [num1]
	jmp .a
	
  ; Se operacao == '-'  -> Soma	
.sub:	cmp bl, '-'	
	jne .mul
	sub [num1], eax
	mov eax, [num1]
	jmp .a	
	
  ; Se operacao == '+'  -> Soma	
.mul:	cmp bl, '*'	
	jne .div
	xor edx, edx
	imul eax, [num1]
	jmp .a
	
  ; Se operacao == '+'  -> Soma	
.div:	cmp bl, '/'	
	jne .nop
	or eax, eax
	jz .nop	
	jmp .a	
	
.a:	mov byte [visor], 0x00
	mov byte [Pwrite], 0
	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
		
	mov cx, 0x130a
	mov esi, visor
	mov ebp, 0x12
	int 0x41	
	
	cmp [Flag], 0
	jz .nop
	mov eax, [num2]
	mov [num1], eax
	mov [Flag], 0
	
.nop:	mov edi, [Calc_Visor]
	mov ebp, 7		; Object_Draw
	int 0x42
	
	ret


;=========================================================================================
; Transforma ascii em numero decimal
; IN:
;	esi: string
; OUT:
;	eax: numero

Calc_AtoI:
	xor eax, eax
	xor ebx, ebx

  ; Enquanto (bl!=0)  	
.b:	mov bl, [esi]
	or bl, bl
	jz .end

     ; eax= eax*10
	mov ecx, eax
	shl ecx, 1
	shl eax, 3
	add eax, ecx

     ; eax = eax + bl-0x30
	sub bl, 0x30		; transforma de ascii para numero
	add eax, ebx
	inc esi
	jmp .b
	
  ; Fim	
.end:	ret

;-----------------------------------------------------------------------------------------