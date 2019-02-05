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
; @Id: FonteSystem.inv  - 2008-02-06, fb-toledo
; Font manager in graphic mode
; For compilation use Flat Assembler (http://flatassembler.net)<font color="#708090"></font>
;
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 ==============================================
; Inicia o sistema de fonte
; IN:
; OUT:

align 4
FontSystem_init:

	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 2 ==============================================
; Carrega uma fonte
; IN:
; OUT:

align 4
Font_Load:

	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 3 ==============================================
; Destroi uma fonte
; IN:
; OUT:

align 4
Font_Free:

	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
; Desenha um caracter
; IN:
;	eax= [y|x] coordenate
;	ebx= | tamanho | fonte | caracter |
;	ecx= cor da fonte
;	edx= tamanho da fonte
;	edi= descritor da imagem
; OUT:
;	eax= nova [y|x] coordenate

; OBS:
; a fonte tem 2 partes, descritor da fonte e o os dados da fonte
; no descritor contem o tamanho X e Y da fonte e a quantidade de vetor para o dados da fonte
; O dados da fonte eh um vetor com N vetores. Cada vetor contem Inicio X e Y da reta e direcao
; sendo entao, 4 bytes para cada indice de vetor

align 4
draw_Char:

	push ebx
	push edx
	push esi

; Busca o ponteiro do bitmap
	movzx ebx, bl
	shl ebx, 1
	movzx esi, word [esi+ebx+4]
	or esi, esi
	jz .error

	push esi

; Pega o proxima linha
.loop:	add esi, 4
	mov ebp, [esi]
	or ebp, ebp
	jz .end

; Posiciona o lugar onde desenhar
	push eax
	push eax
	push edx
	movzx ax, byte [esi]
	shl eax, 0x10
	movzx ax, byte [esi+1]
	mul edx
	mov ebx, eax
	pop edx
	pop eax
	add eax, ebx

	; Pega o tamanho da linha
	push eax
	push edx
	movzx ebx, byte [esi+3]
	mov eax, ebx
	mul edx
	add eax, edx
	mov ebx, eax
	pop edx
	pop eax

	cmp byte [esi+2], 1
	jne .HLine
	push edx
.a1:	call draw_VLine
	inc eax
	dec dl
	jnz .a1
	pop edx
	pop eax
	jmp .loop

.HLine:	cmp byte [esi+2], 2
	jne .D1Line
	push edx
.a2:	call draw_HLine
	add eax, 0x00010000		; incrementa a linha
	dec dl
	jnz .a2
	pop edx
	pop eax
	jmp .loop

.D1Line:	cmp byte [esi+2], 3
	jne .D2Line
	push edx
.a3:	;call draw_D1Line
	add eax, 0x00010000
	dec dl
	jnz .a3
	pop edx
	pop eax
	jmp .loop

.D2Line:	cmp byte [esi+2], 4
	jne .error2
	push edx
.a4:	;call draw_D2Line
	add eax, 0x00010000
	dec dl
	jnz .a4
	pop edx
	pop eax
	jmp .loop

.end:	pop esi

; calcula o nova coordenada [Y|X]
	push eax
	movzx eax, byte [esi+1]
	mul edx
	mov edx, eax
	pop eax
	add eax, edx

.error:	pop esi
	pop edx
	pop ebx
	ret

.error2:	pop eax
	pop edx
	pop ebx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
; Desenha um caracter 8x16 (Rotina Provisoria)
; IN:
;	eax= [Y|X] position
;	ebx= fonte
;	ecx= cor do caracter
;	  0x0FRRGGBB	- se F=1, ele nao desenha o fundo e ebp é desprezado
;	dl= caracter
;	ebp= cor do fundo
;	edi= descritor de imagem
; OUT:
;	eax= new [Y|X] position

align 4
draw_Char2:

	push ebx
	push edx
	push esi

; Pega na tabela o caracter
	and edx, 0xff
	mov esi, [CharTable];Font1_Table
	shl edx, 0x4
	add esi, edx

	xor ebx,ebx
.a1:	push ax
	mov dh, [esi+ebx]
	mov dl, 8
.a2:	shl dh, 1
	jnc .background
	call SDW_PutPixel
.b1:	inc ax
	dec dl
	jnz .a2
	pop ax
	add eax, 0x10000
	inc bl
	cmp bl, 0x10
	jae .end
	jmp .a1

.background:
	test ecx, 0x02000000
	jnz .b1
	xchg ecx, ebp
	call SDW_PutPixel
	xchg ebp, ecx
	jmp .b1

; Ajusta a nova coordenada
.end:	add eax, 0x8
	sub eax, 0x100000

	pop esi
	pop edx
	pop ebx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Desenha uma string
; IN:
;	eax= [y|x] coordenate
;	ebx= fonte
;	ecx= cor da fonte
;	  0x0FRRGGBB	- se F=2, ele nao desenha o fundo e ebp é desprezado
;	ebp= cor do fundo
;	esi= string
;	edi= descritor da imagem
; OUT:
;	nothing
; Codigo da rotina eh ainda provisorio

align 4
draw_String:

	push edx
	push esi
.a:	mov dl, [esi]
	or dl, dl
	jz .end
	call draw_Char2
	inc dl
	inc esi
	jmp .a
.end:	pop esi
	pop edx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 6 ==============================================
; Desenha um caracter 8x16 (Rotina Provisoria)
; IN:
;	eax= [Y|X] position
;	ebx= fonte/tamanho/caracter 
;	ecx= cor do caracter
;	  0x0FRRGGBB	- se F=1, ele nao desenha o fundo e ebp é desprezado
;	edx= objeto
;	ebp= cor do fundo
;	edi= descritor de imagem
; OUT:
;	eax= new [Y|X] position

   ; variaveis ====================
       dc_linha	equ esp+3		; db- linha  
       dc_cont	equ esp+2		; db- contador
       dc_x  	equ esp		; dw- posX
   ;-------------------------------

align 4
draw_Char23:
	push ebx
	push edx
	push esi
	sub esp, 0x4
	
	mov [dc_x], ax
	mov byte [dc_cont], 8
	mov byte [dc_linha], 0
	
; Pega na tabela o caracter
	push ebx
	and ebx, 0xff
	mov esi, [CharTable]
	shl ebx, 0x4
	add esi, ebx
	pop ebx

  ; Desenha um linha do caracter
  	xor ebx, ebx
.linha:	mov ax, [dc_x]
	mov bl, [dc_linha]
	cmp bl, 0x10
	jae .end
	mov bl, [esi+ebx]
.loop:	shl bl, 1
	jnc .back 
	
    ; Desenha a letra	
	call putPixel	
.inc:    	inc ax
	dec byte [dc_cont]
    	jnz .loop
    
    ; incrementa a linha   	
	add eax, 0x00010000
	mov byte [dc_cont], 8
	inc byte [dc_linha]
	jmp .linha	
	
    ; Desenha o fundo	
.back:	test ecx, 0x02000000
	jnz .inc 
	push ecx
	mov ecx, ebp
	call putPixel
	pop ecx
	jmp .inc

; Ajusta a nova coordenada
.end:	add eax, 0x8
	sub eax, 0x100000

	add esp, 4
	pop esi
	pop edx
	pop ebx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 7 ==============================================
; Desenha uma string
; IN:
;	eax= [y|x] coordenate
;	ebx= fonte
;	ecx= cor da fonte
;	  0x0FRRGGBB	- se F=2, ele nao desenha o fundo e ebp é desprezado
;	edx= Objeto
;	ebp= cor do fundo
;	esi= string
;	edi= descritor da imagem
; OUT:
;	nothing
; Codigo da rotina eh ainda provisorio

align 4
draw_String23:

	push ebx
	push esi
.a:	mov bl, [esi]
	or bl, bl
	jz .end
	call draw_Char23
	inc esi
	jmp .a
.end:	pop esi
	pop ebx
	ret


;-----------------------------------------------------------------------------------------



