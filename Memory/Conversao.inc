;-----------------------------------------------------------------------------------------

; Rotinas de Conversao

;-----------------------------------------------------------------------------------------


;======================== C O M E N T A R I O  ===========================================
; Nomeclatura:
;   H -> hexa
;   to -> para
;   C -> Char
;   I -> Inteiro decimal


; Servicos
; Converte Int 16# para Ascii
; Converte Int 32# para Ascii
; Converte Int 16# para Ascii (com sinal)
; Converte Int 32# para Ascii (com sinal)
; Converte Ascii para Int

; Funcoes
; Converte Int 16# para Ascii Hexa
; Converte Int 32# para Ascii Hexa

; Converte Int 16# para Ascii Deci
; Converte Int 32# para Ascii Deci
; Converte Int 16# para Ascii Deci (com sinal)
; Converte Int 32# para Ascii Deci (com sinal)

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 ==============================================
;
; Converte Hexa para Ascii
; Entrada:
;	al: number
; Saida:
;	ah: char 1
;	al: char 2

align 4
HtoA:
	aam 0x10
	cmp al, 0x9
	jbe .a
	add al, 0x7
.a:	cmp ah, 0x9
	jbe .b
	add ah, 0x7
.b:	add ax, 0x3030
	ret


;-----------------------------------------------------------------------------------------




;;======================== F U N C T I O N 3 ==============================================
;;
;; CONVERTE STRINGS ASCII HEX EM INTEIROS #32
;; Entrada:
;;	ds:esi= ponteiro da string
;; Saida:
;;	eax= resultado
;;	esi= aponta final da string convertida + 1
;; OBS: sai no primeiro caractere invalido (diferente de 0 a f)
;
;align 4
;AtoH :
;	push ecx
;	xor ecx, ecx
;.329:	lodsb
;	cmp al, 0x30
;	jb .34d
;	cmp al, 0x39
;	jbe .33e
;	or al, 0x20
;	cmp al, 0x66
;	ja .34d
;	cmp al, 0x61
;	jb .34d
;	add al, 0x09
;.33e:	shl ecx, 0x04
;	and eax, 0xf
;	or ecx, eax
;	jmp .329
;.34d:	mov eax, ecx
;	pop ecx
;	ret
;
;
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
;
; 
; copyright: Kolibri
; Entrada:
;	al= BCD number
; Saida:
;	al= decimal number

align 4
bcd2bin:

	xor   ah,ah
	shl   ax,4
	shr   al,4
	aad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 6 ==============================================
;;
;; modificar para operar com a tabela de caracteres definida na partida
;; convert ANSI character in al to UNICODE character in ax, using cp866 encoding
;; copyright: Kolibri
;
;align 4
;ansi2uni_char:
;
;        mov     ah, 0	; 0x00-0x7F - trivial map
;        cmp     al, 0x80
;        jb      .exit
;        cmp     al, 0xB0	; 0x80-0xAF -> 0x410-0x43F
;        jae     @f
;        add     ax, 0x410-0x80
;  .exit:
;        ret
;
;  @@:
;        cmp     al, 0xE0		; 0xE0-0xEF -> 0x440-0x44F
;        jb      .unk
;        cmp     al, 0xF0
;        jae     @f
;        add     ax, 0x440-0xE0
;        ret
;
;  @@:
;        cmp     al, 'ð'
;        jz      .yo1
;        cmp     al, 'ñ'
;        jz      .yo2
;
;  .unk:
;        mov     al, '_'         	; ah=0
;        ret
;
;  .yo1:
;        mov     ax, 0x401		; 0xF0 -> 0x401
;        ret
;
;  .yo2:
;        mov     ax, 0x451		; 0xF1 -> 0x451
;        ret
;
;
;;-----------------------------------------------------------------------------------------
;
;
;;======================== F U N C T I O N 7 ==============================================
;;
;; modificar para operar com a tabela de caracteres definida na partida
;; convert UNICODE zero-terminated string to ASCII-string (codepage 866)....review to page 850
;; in: esi->source, edi->buffer (may be esi=edi)
;; destroys: eax,esi,edi
;align 4
;uni2ansi_str:
;
;        lodsw
;        test    ax, ax
;        jz      .done
;        cmp     ax, 0x80
;        jb      .ascii
;        cmp     ax, 0x401
;        jz      .yo1
;        cmp     ax, 0x451
;        jz      .yo2
;        cmp     ax, 0x410
;        jb      .unk
;        cmp     ax, 0x440
;        jb      .rus1
;        cmp     ax, 0x450
;        jb      .rus2
;.unk:
;        mov     al, '_'
;        jmp     .doit
;.yo1:
;        mov     al, 'ð'
;        jmp     .doit
;.yo2:
;        mov     al, 'ñ'
;        jmp     .doit
;.rus1:
;
;        add     al, 0x70	; 0x410-0x43F -> 0x80-0xAF
;        jmp     .doit
;.rus2:
;
;        add     al, 0xA0	; 0x440-0x44F -> 0xE0-0xEF
;.ascii:
;.doit:
;        stosb
;        jmp     uni2ansi_str
;.done:
;        mov     byte [edi], 0
;        ret
;
;
;-----------------------------------------------------------------------------------------






;======================== F U N C T I O N 4 ==============================================
;
; Converte ASCII decimal em um numero inteiro 32#
; Entrada:
;	esi= ponteiro da string no formato ASCII, aceita sinal e espacos
; Saida:
;	eax= numero inteiro
; OBS: nao emite nenhum aviso de estouro e termina no primeiro caractere invalido
; Funcionando

align 4
ADtoI:	
	push ebx
	push ecx
	push edx
	push esi
	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	
  ; Retira caracteres reservados (espacos,+,-)	
.a:	lodsb
	cmp al, ' '
	je .a
	cmp al, '+'
	je .b
	cmp al, '-'
	jne .b
	dec ecx		; ativa a flag de numero negativo
		
  ; Enquanto nao termina a string
.loop:	lodsb

    ; Se (numero invalido) || (Final da string)  -> Sai da rotina   
.b:	or al, al
	jz .end
	cmp al, '0'
	jb .end
	cmp al, '9'
	ja .end
	
    ; Senao -> num = (num * 10) + num	
	sub al, 0x30 
	mov edx, ebx
	shl ebx, 0x3
	shl edx, 0x1
	add ebx, edx
	add ebx, eax
	jmp .loop
		
  ; Fim	 	
.end:	mov eax, ebx
	jcxz .e
	neg eax
.e:	pop esi
	pop edx
	pop ecx
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
;
; Converte ASCII hexa em um numero inteiro 32#
; Entrada:
;	esi= ponteiro da string no formato ASCII, aceita sinal e espacos
; Saida:
;	eax= numero inteiro
; OBS: nao emite nenhum aviso de estouro e termina no primeiro caractere invalido

align 4
AHtoI:	
	ret


;-----------------------------------------------------------------------------------------





;======================== F U N C T I O N 4 ==============================================
;
; Converte um numero inteiro 32# em ASCII
; Entrada:
;	eax= numero inteiro
;	esi= buffer para receber a string
;	cx= base numerica
; Saida:
;	esi= endereco final da string
;	eax= tamanho da string

align 4
I8toA:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
;
; Converte um numero inteiro 16# em ASCII
; Entrada:
;	 ax= numero inteiro
;	esi= buffer para receber a string
;	 cx= base
; Saida:
;	ecx= tamanho da string
;	esi= recebe uma stringZ com os numeros

align 4
I16toA:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
;
; Converte um numero inteiro 32# em ASCII
; Entrada:
;	eax= numero inteiro
;	cx= base numerica
;	esi= buffer para receber a string
; Saida:
;	esi= numero convertido
; OBS: reservar 12 caracteres para o numero (sinal(1 byte),numero(10 bytes),0x00)
; Funcionando

  ;======= Variaveis =========
    buffer equ esp	; rb 0x0b
  ;---------------------------  

align 4
I32toA:
	push eax
	push ebx
	push edx
	push esi
	sub esp, 0x10
	xor ebx, ebx

  ; Converte o numero e coloca na pilha	
.loop:	cdq
	div ecx
	add dl, 0x30
	cmp dl, 0x39
	jle .b
	add dl, 0x07		; caso for hexa, corrige para A-F
.b:	mov [buffer+ebx], dl
	inc ebx
	or eax, eax
	jne .loop
	
  ; Copia a string da pilha e passa para o esi
  	dec ebx
.cpy:  	mov al, [buffer+ebx]
	mov [esi], al
	inc esi
	dec ebx
	jns .cpy
	
  ; Fim		
	mov byte [esi], 0
	add esp, 0x10
	pop esi
	pop edx
	pop ebx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
;
; Converte um numero inteiro 32# em ASCII
; Entrada:
;	eax= numero inteiro
;	cx= base numerica
;	esi= buffer para receber a string
; Saida:
;	esi= numero convertido
; OBS: reservar 12 caracteres para o numero (sinal(1 byte),numero(10 bytes),0x00)
; Funcionando

  ;======= Variaveis =========
    buffer equ esp+1	; rb 0x0b
    sinal equ esp 		; db - guarda o sinal
  ;---------------------------  

align 4
I32StoA:
	push eax
	push ebx
	push edx
	push esi
	sub esp, 0x10
	xor ebx, ebx
	
  ; Coloca o sinal	
	or eax, eax	
  	mov byte [sinal], 0	
	jns .loop
	neg eax
	mov byte [sinal], '-'

  ; Converte o numero e coloca na pilha	
.loop:	cdq
	div ecx
	add dl, 0x30
	cmp dl, 0x39
	jle .b
	add dl, 0x07		; caso for hexa, corrige para A-F
.b:	mov [buffer+ebx], dl
	inc ebx
	or eax, eax
	jne .loop
	
  ; Copia a string da pilha e passa para o esi
    ; Coloca o sinal	
	dec ebx
	cmp byte [sinal], 0
	jz .cpy
	mov byte [esi], '-'
	inc esi
	
    ; Copia a string	
.cpy:  	mov al, [buffer+ebx]
	mov [esi], al
	inc esi
	dec ebx
	jns .cpy
	
  ; Fim		
.end:	mov byte [esi], 0
	add esp, 0x10
	pop esi
	pop edx
	pop ebx
	pop eax
	ret


;-----------------------------------------------------------------------------------------



I64toA:
I8StoA:
I16StoA:
I64StoA:
	ret


