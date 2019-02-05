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
; @Id: GraphSystem.inc  - 2008-02-12, fb-toledo & cb-toledo
; Graphic mode video routines
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------
;
; OBS:
; O sistema grafico funciona com descritores, esses descritores fornecem informaçoes sobre
; determinada imagem, como tamanho, tipo, endereco dela. Para manter generealizar as rotinas
; o video tambem foi transfomado em um bitmap.

; Descritor de imagem
;    IAss	  	equ 0x0    ; dw - Assinatura
;    IType      	equ 0x2    ; db - Tipo
;    IBpp	  	equ 0x3    ; db - Profundidade da cor
;    IHeight    	equ 0x4	 ; dw - tamanho X da imagem
;    IWidth     	equ 0x6	 ; dw - tamanho Y da imagem
;    IBytesPerLine  equ 0x8    ; dw - quantidade de bytes por linha
;    Ireserved	equ 0xa	 ; dw - reservado
;    IPointer 	equ 0xc	 ; dd - Ponteiro da memoria da imagem
; tamanho total = 0x10 bytes

; Descritor de Quadro 
;    QsizeX	equ 0x10	; dw - Tamanho X do quadro
;    QsizeY	equ 0x12  ; dw - Tamanho Y do quadro
;    QqtdeX	equ 0x14  ; dw - Quantidade de quadro no eixo X 
;    QqtdeY 	equ 0x16	; dw - Quantidade de quadro no eixo Y			 


;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================
align 4

OS_Window		dd 0

; Estruruta do descritor do Heap - O mesmo que o padrao
Heap_Ass		equ 0	; dd - Assinatura do descritor ('DEHE') (DEscritor HEap)
Heap_Ini		equ 4	; dd - Ponteiro do inicio do Heap
Heap_Size		equ 8	; dd - Tamanho do heap

Heap_AssinaturaDE	equ "DEHE"	; Assinatura do descritor do heap
Heap_Assinatura	equ "HEAP"	; Assinatura do heap

; Heap do video
Video_Heap:
	dd Heap_AssinaturaDE
	dd 0
	dd 0

; Tabela de rotinas para put pixel e getpixel
putPixel8_Table:			; 8 BPP
	dd 0
	dd putPixel8		; (VESA20.inc)

putPixel24_Table:			; 24 BPP
	dd 0			; put Pixel for System (VESA 1.2 or VESA 2.0)
	dd Vesa20_putPixel242	; put Pixel in bitmap

putPixel24Neg_Table:		; 24 BPP - negativo (inverte cor)
	dd Vesa20_putPixel24_Neg	; put Pixel for System (VESA 1.2 or VESA 2.0)
	dd Vesa20_putPixel242_Neg	; put Pixel in bitmap

putPixel32_Table:			; 32 BPP
	dd 0		          ; put Pixel for System (VESA 1.2 or VESA 2.0)
	dd Vesa20_putPixel322         ; put Pixel in bitmap (baixo para cima)

putPixel32Neg_Table:		; 32 BPP - negativo (inverte cor)
	dd Vesa20_putPixel32_Neg	; put Pixel for System (VESA 1.2 or VESA 2.0)
	dd Vesa20_putPixel322_Neg     ; put Pixel in bitmap


getPixel8_Table:			; 8 BPP
	dd 0
	dd getPixel8		; (VESA20.inc)

getPixel24_Table:			; 24 BPP
	dd 0                          ; get Pixel for System (VESA 1.2 or VESA 2.0)
	dd Vesa20_getPixel242         ; get Pixel in bitmap

getPixel32_Table:			; 32 BPP
	dd 0                          ; get Pixel for System (VESA 1.2 or VESA 2.0)
	dd Vesa20_getPixel322         ; get Pixel in bitmap



;-----------------------------------------------------------------------------------------



;-----------------------------------------------------------------------------------------

; Rotinas de Inicializacao de Video

;-----------------------------------------------------------------------------------------

;======================== F U N C T I O N 1 ==============================================
; Inicia o modo grafico
; IN:
;	Sistema de memoria iniciada
; OUT:
;	nothing

align 4
init_GraphSystem:

	mov byte [Screen+IType], 0

; Coloca o tamanho do video
	mov ax, [Vesa_Desc+Vesa_Height]
	mov word [Screen+IHeight], ax
	mov ax, [Vesa_Desc+Vesa_Width]
	mov word [Screen+IWidth], ax
	mov ax, [Vesa_Desc+Vesa_BytePerLine]
	mov word [Screen+IBytesPerLine], ax

; Coloca o BPP do video (24 ou 32)
	mov al, [Vesa_Desc+Vesa_BPP]
	mov byte [Screen+IBpp], al
	cmp al, 24
	jne .32
	mov dword [putPixel24_Table], Vesa20_putPixel24
	mov dword [getPixel24_Table], Vesa20_getPixel24
.32:	mov dword [putPixel32_Table], Vesa20_putPixel32
	mov dword [getPixel32_Table], Vesa20_getPixel32

; Coloca o endereco da memoria de video
	mov esi, [Vesa_Desc+Vesa_LFBAddress]		; posicao da memoria de video
	mov dword [Screen+IPointer], esi

; Marca como pagina ocupada
	push esi
	mov al, 0x87
	mov edi, esi
	shr edi, 20	; divide por (4mb*4byte)
	mov ebx, OS_PageTable
	call marcar_Page
	add edi, 4
	add esi, 0x400000
	call marcar_Page
	pop esi

; Cria o heap de video
	add esi, 0x300004
	mov [Video_Heap+Heap_Ini], esi
	mov dword [Video_Heap+Heap_Size], 0x500000
	mov dword [esi-4], Heap_Assinatura
	mov dword [esi], 0x500000



	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 2 ==============================================
; Finaliza o modo grafico
; Funcionamento:
; IN:
; OUT:

align 4
deinit_GraphSystem:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 3 ==============================================
; Inicia o grafico do sistema (Papel de parede, icones, janela do SO,...)
; IN:
;	Sistema de memoria iniciada
;	Manipulador de arquivos iniciado
; OUT: nothing
; Altera: eax, ebx, ecx, edi


arquivo1	db "c:\wallpaper",0
arquivo2	db "d:\icone01.bmp",0
Icones	rb 0x18

Graph_Script:
	db "OBJ1"
	;dd 0xff000006,0x00000000,0x00000000,0x00060005,arquivo1
	dd 0xff000003,0x02300002,0x00200020,0x00020001
	dd 0x0


align 4
init_OSGraph:
	
	
; Carrega os icones
	xor ebx, ebx
	mov ecx, 0x00400040
	mov edx, 0x00040004
	mov esi, texto02
	mov edi, Icone_Desc	
	;call ImagemQuadro_Load

; Cria o objeto Imagem (Wallpaper)
	mov eax, 0
	mov ebx, [Screen+IHeight]
	shl ebx, 0x10
	mov bx, [Screen+IWidth]
	mov ecx, 0x00040001
	xor edx, edx
	call Object_Create
	mov [OS_Window], edi
	mov edx, edi
	
  ; Carrega o papel de parede	
	push edi
	mov ebx, Video_Heap
	mov esi, arquivo1
	mov edi, 0x80000600
	call load_Image
	mov [edx+0x30], edi
	pop edi

  ; Desenha o papel de parede
	call Object_Draw

	ret

;-----------------------------------------------------------------------------------------



;-----------------------------------------------------------------------------------------

; Rotinas de Desenho de Primitivas

;-----------------------------------------------------------------------------------------

;======================== F U N C T I O N 1 ==============================================
; Pega um pixel da tela
; Funcionamento:
; IN:
;	eax: [Y|X] coordenada
;	esi= descritor da imagem
; OUT:
;	ecx: color
; Fazer ainda

align 4
getPixel:
	push eax
	push ebx
	push edx
	push edi
	mov ebx, eax
	and eax, 0xffff
	shr ebx, 0x10

; Verifica se esta dentro da imagem
	cmp ax, [esi+IWidth]
	ja .error
	cmp bx, [esi+IHeight]
	ja .error

; Pixel 24 BPP
	movzx edx, word [esi+IType]	; move dl o tipo e dh o bpp
	cmp dh, 24
	jne .pixel32
	xor dh, dh		; retira o BPP de dx
	shl edx, 2		; edx*4 = posicao na tabela
	call dword [getPixel24_Table+edx]
	jmp .end

; Pixel 32 BPP
.pixel32:	cmp dh, 32
	jne .pixel8
	xor dh, dh		; retira o BPP de dx
	shl edx, 2		; edx*4 = posicao na tabela
	call dword [getPixel32_Table+edx]
	jmp .end

; Pixel 8 BPP
.pixel8:	cmp dh, 8
	jne .end
	xor dh, dh		; retira o BPP de dx
	shl edx, 2		; edx*4 = posicao na tabela
	call dword [getPixel8_Table+edx]

.end:	pop edi
	pop edx
	pop ebx
	pop eax
	ret

.error:	stc
	pop edi
	pop edx
	pop ebx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 2 ==============================================
; Coloca um pixel na tela
; Funcionamento:
;	variaveis:
;	   eax= coordenada X, ebx= coordenada Y, ecx= cor, edi= descritor da imagem
;	se pixel dentro do quadro
;	  compara o tipo
;	    busca a tabela de acordo com o BPP (24,32) e o Tipo (bmp,jpeg,...)
;	FIM

; IN:
;	eax= [Y|X] coordenada
;	ecx= color
;	ecx= 0xFFRRGGBB
;	      FF= 01 -> negativo
;	      RRGGBB= red,green,blue
;	edx= Objeto
;	edi= descritor da imagem
; OUT:
;	nothing
; Verificar pq nao funciona o mov ebx,[Screen_BytesPerLine]
; terminar rotina

align 4
putPixel:
	pushad

; Ajusta as coordenadas (eax= X, ebx= Y)
	mov ebx, eax
	and eax, 0xffff
	shr ebx, 0x10

; Verifica está dentro do bitmap
	cmp ax, [edi+IWidth]
	jae .error
	cmp bx, [edi+IHeight]
	jae .error

; Verifica se está dentro da janela
	cmp edi, Screen
	jne .pula
	or edx, edx
	jz .pula
	call Object_CheckPixel
	jc .end

; Coloca pixel de 24BPP
.pula:	movzx edx, word [edi+IType]	; move dl o tipo e dh o bpp
	cmp dh, 24
	jne .pixel32
	xor dh, dh		; retira o BPP de dx
	shl edx, 2		; edx*4 = posicao na tabela
	test ecx, 0x01000000
	jnz .neg24
	and ecx, 0xffffff
	call dword [putPixel24_Table+edx]
	jmp .end
.neg24:	call dword [putPixel24Neg_Table+edx]
	jmp .end

; Coloca pixel 32 BPP
.pixel32:	cmp dh, 32
	jne .pixel8
	xor dh, dh		; retira o BPP de dx
	shl edx, 2		; edx*4 = posicao na tabela
	test ecx, 0x01000000
	jnz .neg32
	call dword [putPixel32_Table+edx]
	jmp .end
.neg32:	call dword [putPixel32Neg_Table+edx]
	jmp .end

; Coloca pixel 8 BPP
.pixel8:	cmp dh, 8
	jne .end
	xor dh, dh
	shl edx, 2
	and ecx, 0xff
	call dword [putPixel8_Table+edx]

.end:	clc
	popad
	ret

.error:	stc
	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 3 ==============================================
;
; Funcionamento:
; IN:
;	eax: [Y|X] coordenada 1
;	ebx: [Y|X] coordenada 2
;	ecx: cor
;	edx= Objeto
;	edi= descritor da imagem
; OUT:
;	nothing

   ; variaveis ====================
       dl_y1 equ esp+0xc		; dw- posY1
       dl_x1 equ esp+0xa		; dw- posX1
       dl_y2 equ esp+8		; dw- posY2
       dl_x2 equ esp+6		; dw- posX2
       dl_fl equ esp+4		; dw- flags (00-> sem sinal, 10-> um sinal, 01->um sinal, 11 2 sinais)
       dl_dx equ esp+2		; dw- (X2-X1) -> gera sinal
       dl_dy equ esp+0		; dw- (Y2-Y1) -> gera sinal
   ;-------------------------------

align 4
draw_Line:
	pushad
	sub esp, 0xe
	mov [dl_x1], eax		; salva y1 e x1
	mov [dl_x2], ebx		; salva y2 e x2
	mov byte [dl_fl], 0

; Calcula dl_dx e dl_dy
   ; Calcula (X2-X1)
	sub bx, ax
	mov [dl_dx], bx
	jns @f			; x1 eh menor que x2
	neg bx
	mov [dl_dx], bx
	or byte [dl_fl], 1

   ; Calcula (Y2-Y1)
@@:	mov bx, [dl_y2]
	shr eax, 0x10		; passa y1 para o bx
	sub bx, ax
	mov [dl_dy], bx
	jns .lv
	neg bx
	mov [dl_dy], bx
	or byte [dl_fl], 2

; Verifica se sao linhas horizontal ou vertical
   ; Linha vertical
.lv:	mov ax, [dl_dx]
	or ax, ax
	jnz .lh			; nao eh vertical
	mov eax, [dl_x1]
	cmp eax, [dl_x2]		; verifica se Y1<Y2
	jb @f
	mov eax, [dl_x2]
@@:	xor ebx, ebx
	mov bx, [dl_dy]
	call draw_VLine
	jmp .end2

   ; Linha horizontal
.lh:	mov bx, [dl_dy]
	or bx, bx
	jnz .nvh			; nao eh vertical e nem horizontal
	mov eax, [dl_x1]
	cmp ax, [dl_x2]		; verifica se x1<x2
	jb @f
	mov ax, [dl_x2]
@@:	xor ebx, ebx
	mov bx, [dl_dx]
	call draw_HLine
	jmp .end2

; Desenha a Linha
   ; Verifica se o angulo eh menor que 45 ou maior
.nvh:	and ebx, 0xffff
	and eax, 0xffff
@@:	cmp ax, bx
	jb .a45

   ; angulo<45
      ; Calcula a tangente
      	push edx
      	xor edx, edx
	xchg eax, ebx
	shl eax, 0x10
	div ebx
	mov ebp, eax
	pop edx
	cmp byte [dl_fl], 0		; ajusta o sinal ebp (quadrante 2 e 4 devem ser negativos)
	jz .blz1
	cmp byte [dl_fl], 3
	jz .blz1
	neg ebp

      ; Verifica as regras (X1<X2)
.blz1:   	mov eax, [dl_x1]
	mov ebx, [dl_x2]
	cmp ax, bx
	jb .blz12
	xchg eax, ebx
	mov [dl_x1], eax		; inverte dl_x1 com dl_x2
	mov [dl_x2], ebx

      ; Desenha a linha
.blz12:   mov esi, eax
	and esi, 0xffff0000
@@:	call putPixel
	push ax
	add esi, ebp
	mov eax, esi
	pop ax
	inc ax
	cmp ax, bx
	ja .end
	jmp @b

   ; angulo>45
      ; Calcula a cotangente
.a45:	push edx
      	xor edx, edx
	shl eax, 0x10
	div ebx
	mov ebp, eax
	pop edx
	cmp byte [dl_fl], 0		; ajusta o sinal ebp (quadrante 2 e 4 devem ser negativos)
	jz .blz2
	cmp byte [dl_fl], 3
	jz .blz2
	neg ebp

      ; Verifica as regras (Y1<Y2)
.blz2:   	mov eax, [dl_x1]
	mov ebx, [dl_x2]
	cmp eax, ebx
	jb .blz22
	xchg eax, ebx
	mov [dl_x1], eax		; inverte dl_x1 com dl_x2
	mov [dl_x2], ebx

      ; Desenha a linha
.blz22:  	mov esi, eax
	shl esi, 0x10
@@:	mov eax, [dl_x1]
	call putPixel
	add esi, ebp
	mov eax, esi
	shr eax, 0x10
	mov [dl_x1], ax
	inc word [dl_y1]
	mov ax, [dl_y1]
	shl eax, 0x10
	cmp eax, ebx
	ja .end
	jmp @b

; Fim
.end:	mov eax, [dl_x2]
	call putPixel		; desenha o ultimo ponto
.end2:	add esp, 0xe
	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
; Desenha uma linha na vertical
; Funcionamento:
; IN:
;	eax: [Y|X] coordenada 1
;	ebx: size Y
;	ecx: cor
;	edx= Objeto
;	edi= descritor da imagem
; OUT:
;	nothing
; Funcionando  -  Tentar otimizar depois

align 4
draw_VLine:
	push eax
	push ebx
.a:	call putPixel
	;jc .end
	add eax, 0x00010000
	dec ebx
	jnz .a
.end:	pop ebx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Desenha uma linha na horizontal
; Funcionamento:
;	Coloca o pixel
;	se o pixel fora do quadro
;	   fim
;	incrementa coordenada X
;	decrementa o tamanho
;	se terminou (ebx==0)
;	   fim
; IN:
;	eax: [Y|X] coordenada
;	ebx: size Y
;	ecx: cor
;	edx= Objeto
;	edi= descritor da imagem
; OUT:
;	nothing
; Funcionando  -  Tentar otimizar depois

align 4
draw_HLine:
	push eax
	push ebx
.a:	call putPixel
	;jc .end
	inc eax
	dec ebx
	jnz .a
.end:	pop ebx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 6 ==============================================
;
; Funcionamento:
; IN:
;	eax: [Y|X] coordenada
;	ebx: [Y|X] size
;	ecx: color
;	edx= Objeto
;	edi= descritor da imagem
; OUT:
;	nothing
; Funcionando

align 4
draw_Rect:
	pushad
	mov ebp, ebx

; Desenha 1° linha
	shr ebx, 0x10
	call draw_VLine

; Desenha 2° linha
	mov ebx, ebp
	and ebx, 0xffff
	call draw_HLine

; Desenha 3° linha
	push eax
	mov ebx, ebp
	and ebx, 0xffff
	add eax, ebx
	dec eax
	mov ebx, ebp
	shr ebx, 0x10
	call draw_VLine
	pop eax

; Desenha 4° linha
	mov ebx, ebp
	and ebp, 0xffff0000
	add eax, ebp
	sub eax, 0x00010000
	and ebx, 0xffff
	call draw_HLine

; FIM
	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 7 ==============================================
; Desenha um retangulo preenchido
; Funcionamento:
; IN:
;	eax: [Y|X] coordenada
;	ebx: [Y|X] size
;	ecx= color
;	edx= Objeto
;	edi= descritor da imagem
; OUT:
;	nothing
; Funcionando
; OBS: ele trava quando Y size=0  - Arrumado

align 4
draw_RectFill:
	push eax
	push ebx
	push ebp

	mov ebp, ebx
	shr ebp, 0x10
	and ebx, 0xffff

	or bp, bp
	jnz .a
	inc bp

.a:	call draw_HLine
	add eax, 0x00010000
	dec ebp
	jne .a

	pop ebp
	pop ebx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 8 ==============================================
; Desenha um retangulo preenchido em degrade-up
; Funcionamento:
; IN:
;	eax: [Y|X] coordenada
;	ebx: [Y|X] size
;	ecx= color
;	 dh= Intensidade do degrade
;	edx= Objeto
;	edi= descritor da imagem
; OUT:
;	nothing
; Funcionando - (movimentacao da janela - trava)

; IN:
;	eax= [Y|X] coordenada
;	ebx= [Y|X] size
;	ecx= color
;	 dl= Intensidade do degrade
;	 dh= Repeticao 
;	esi= Objeto
;	edi= descritor da imagem
; OUT:
;	nothing

   ;==== variaveis =========	
      DRDU_red 	equ esp+4	 ; db - Cor vermelha	
      DRDU_blue 	equ esp+3  ; db - Cor azul	
      DRDU_green	equ esp+2  ; db - Cor verde	
      DRDU_int 	equ esp+1  ; db - Intensidade do degrade	
      DRDU_rep 	equ esp	 ; db - Repeticao	

   ;------------------------

align 4
draw_RectDegradeUp:
	pushad
	sub esp, 6

	mov ebp, ebx
	shr ebp, 0x10
	and ebx, 0xffff

	mov [DRDU_green], ecx
	mov [DRDU_rep], dx
	mov edx, esi 

	or bp, bp
	jnz .a
	inc bp

.a:	mov ecx, [DRDU_green]	
	call draw_HLine	
	add eax, 0x00010000

	mov cl, [DRDU_int]
	add cl, [DRDU_red] 
	jnc @f
	mov cl, 0xff
@@:	mov [DRDU_red], cl
 	
	mov cl, [DRDU_int]
	add cl, [DRDU_green] 
	jnc @f
	mov cl, 0xff
@@:	mov [DRDU_green], cl

	mov cl, [DRDU_int]
	add cl, [DRDU_blue] 
	jnc @f
	mov cl, 0xff
@@:	mov [DRDU_blue], cl

	dec ebp
	jne .a
	
	add esp, 6
	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 9 ==============================================
; Desenha um retangulo preenchido em degrade-down
; IN:
;	eax= [Y|X] coordenada
;	ebx= [Y|X] size
;	ecx= color
;	 dl= Intensidade do degrade
;	 dh= Repeticao 
;	esi= Objeto
;	edi= descritor da imagem
; OUT:
;	nothing

   ;==== variaveis =========	
      DRDD_red 	equ esp+4	 ; db - Cor vermelha	
      DRDD_blue 	equ esp+3  ; db - Cor azul	
      DRDD_green	equ esp+2  ; db - Cor verde	
      DRDD_int 	equ esp+1  ; db - Intensidade do degrade	
      DRDD_rep 	equ esp	 ; db - Repeticao	

   ;------------------------

align 4
draw_RectDegradeDown:
	pushad
	sub esp, 6

	mov ebp, ebx
	shr ebp, 0x10
	and ebx, 0xffff

	mov [DRDD_green], ecx
	mov [DRDD_rep], dx
	mov edx, esi 

	or bp, bp
	jnz .a
	inc bp

.a:	mov ecx, [DRDD_green]	
	call draw_HLine
	add eax, 0x00010000

	mov cl, [DRDD_red]
	sub cl, [DRDD_int]
@@:	mov [DRDD_red], cl
 	
	mov cl, [DRDD_green]
	sub cl, [DRDD_int] 
@@:	mov [DRDD_green], cl

	mov cl, [DRDD_blue]
	sub cl, [DRDD_int] 
@@:	mov [DRDD_blue], cl

	dec ebp
	jne .a
	
	add esp, 6
	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 0 ============================================
; Desenha um triangulo
; Funcionamento:
; IN:
;	edx= Objeto	
;	esi= matriz de pontos
;	edi= descritor da imagem
; OUT:
;	nothing
; OBS: matriz de pontos = posX1,posY1,posX2,posY2,posX3,posY3,cor

draw_Triangulo:
	push eax
	push ebx
	push ecx
	mov ecx, [esi+0xc]

; Desenha a primeira linha
	mov eax, [esi]
	mov ebx, [esi+4]
	call draw_Line

; Desenha a segunda linha
	mov eax, [esi+8]
	call draw_Line

; Desenha a terceira linha
	mov ebx,[esi]
	call draw_Line

	pop ecx
	pop ebx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 1 ============================================
; Desenha uma elipse ou circulo
; Funcionamento:
; IN:
;	eax= [Y|X] coordenada
;	ebx= distorcao/tamanho do raio
;	ecx= cor
;	edx= Objeto
;	edi= descritor da imagem
; OUT:
;	nothing

draw_Elipse:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 2 ============================================
; Desenha um poligono
; Funcionamento:
; IN:
;	edx= Objeto
;	esi= matriz de pontos
;	edi= descritor da imagem
; OUT:
;	nothing

draw_Polygon:
	ret

;-----------------------------------------------------------------------------------------



;-----------------------------------------------------------------------------------------

; Rotinas de Desenho de Bitmap

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Cria um bitmap
; IN:
;	eax= X size
;	ebx= Y size
;	 cl= BPP (24 ou 32)
;	edx= descritor do Heap
;	;esi= Paleta (Nao usado)
;	edi= descritor da imagem
; OUT:
;	nothing
; OBS: o padrao é 32BPP

align 4
create_Bitmap:
	push eax
	push ebx
	push esi

  ; Coloca as variaveis no descritor
	mov dword [edi+IAss], Image_Ass
	mov byte [edi+IType], 0x1
	mov [edi+IWidth], ax
	mov [edi+IHeight], bx
	mov byte [edi+IBpp], cl
	push eax
	xor eax, eax
	mov [edi+QsizeX], eax
	mov [edi+QqtdeX], eax
	mov [edi+IFree], eax
	mov [edi+IPaleta], eax
	pop eax
	
    ; Se (cl==24) -> calcula numero de pixel para 24 Bpp
    	cmp cl, 24
    	jne .b32
	lea eax, [eax+eax*2]	; eax = eax * 3	 
	mov [edi+IBytesPerLine], ax
    	jmp .aloca
    
    ; Senao
      ; Se (cl==32) -> calcula numero de pixel para 32 Bpp
.b32:	cmp cl, 32
	jne .error		; Sai com erro se nao for 24 ou 32 Bpp
	shl eax, 2		; eax = eax * 4	 
	mov [edi+IBytesPerLine], ax
    
  ; Aloca o bloco de memoria para a imagem
.aloca:	mul ebx
	or edx, edx
	jnz .local
	
    ; Se (edx==0)  -> Aloca no heap padrao do programa	
	call SMM_MainHeap_Alloc		
	jc .error
	mov [edi+IPointer], esi
	jmp .end

    ; Senao   -> Aloca no heap apontado por edx
.local:	mov ebx, edx
	call SMM_Heap_Alloc	
	jc .error
	mov [edi+IPointer], esi

  ; Fim
.end:	pop esi
	pop ebx
	pop eax
	ret
	
  ; Error - Falta de espaco no heap	
.error:	stc
	xor eax, eax
	mov [edi+IAss], ax	
	pop esi
	pop ebx
	pop eax
	ret
	
	
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Carrega uma imagem
; Funcionamento:
; IN:
;	ebx= descritor do Heap
;	   ebx = 0 -> bloco aloca no heap principal
;	   ebx!= 0 -> bloco aloca no heap apontado por ebx
;	esi= nome do arquivo
;	edi= descritor da imagem
; OUT:
;	edi= descritor da imagem
;	ebp= retorna o erro
;	  0- sem erro
;	  1- Arquivo nao encontrado
;	  2- O arquivo nao eh uma imagem
; OBS: Carrega apenas bitmap, por enquanto
;      Está com problemas quando o heap é invalido	

align 4
load_Image:
	pushad

; Carrega a imagem na memoria	
	push edi
	call File_Open	
	jc .error2
	call File_GetSize

   ; Aloca um bloco para a imagem
	or ebx, ebx
	jnz @f
	call SMM_MainHeap_Alloc	; Aloca no heap principal
	jc .error3
	jmp .a
@@:	call SMM_Heap_Alloc		; Aloca num heap apontado por ebx
	jc .error3
.a:	mov edi, esi

	;call File_AllRead		; Verificar pq o allread tah lendo mais do que devia

   ; Lê a image -   Temporario, ateh o File_AllRead funcionar OK
.read:	mov ecx, 0x200
	call File_Read
	jc .end
	add edi, 0x200
	jmp .read
.end:	call File_Close
	pop edi

; pega as variaveis
   ; Verifica se é um bitmap
	;mov eax, [esi]
	;shr eax, 0x10	- colocar ror
	;cmp eax, 'BM'
	;jne .error

   ; Coloca as variaveis constantes
	mov dword [edi+IAss], Image_Ass
	xor eax, eax
	mov [edi+QsizeX], eax
	mov [edi+QqtdeX], eax
	mov [edi+IFree], eax
	mov [edi+IPaleta], eax

   ; Pega o tamanho da imagem
   	mov bx, [esi+0x12]		; depois mudar para dword
	mov word [edi+IWidth], bx
	mov ax, [esi+0x16]
	dec ax
	mov word [edi+IHeight], ax
	mov al, [esi+0x1c]
	mov [edi+IBpp], al

   ; Calcula a quantidade de bytes por linha
	and ebx, 0xffff
	shr al, 3			; divide por 8 (24/8 ou 32/8)
	and ax, 0x0f
	imul bx, ax
	add ebx, 3
	and bl, 0xfc

	mov dword [edi+IBytesPerLine], ebx
	mov byte [edi+IType], 1

   ; Ajusta o buffer
   	mov eax, [esi+0xa]
	add esi, eax
	mov [edi+IPointer], esi
; Fim
	popad
	xor ebp, ebp
	ret

; Error - Arquivo nao eh bmp
.error:	popad
	mov ebp, 0x1
	ret

; Error - Arquivo nao encontrado
.error2:	pop edi
	popad
	mov ebp, 0x2
	ret

; Error - Falta de memoria
.error3:	call File_Close
	mov ebp, 0x3
	popad
	ret



;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Destroi um bitmap
; IN:
;	edi= bitmap a ser destruido
;	edx= descritor do heap
; OUT:
;	nothing
; testar

align 4
destroy_Bitmap:
; verifica se é uma imagem
	cmp dword [edi+IAss], Image_Ass
	jne .error

; Libera a imagem
	push esi

   ; Desaloca o bloco da imagem
	mov esi, [edi+IPointer]
	call SMM_Heap_Free

   ; Desaloca o descritor de imagem
	mov esi, edi
	call SMM_Heap_Free

	pop esi
	ret

.error:	stc
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; desenha uma imagem
; IN:
;	eax: [Y|X]coordenada
;	edx= Objeto
;	esi: bitmap fonte
;	edi: bitmap destino
; OBS:
;	2- tipo | 2- image_heigth | 2- image_width | 2- image_bpp | 4- *image

; OUT:
;	nothing

align 4
draw_Image:
	pushad

; verifica se sao imagens
	;cmp word [esi], "BM"
	;jc .end
	;cmp word [edi], "BM"
	;jc .end

	xor ebx, ebx
	mov ebp, eax
.loop:	push eax
	mov eax, ebx
	call getPixel
	pop eax
	call putPixel
	inc bx
	cmp bx, [esi+IWidth]
	jae .incLinha
	inc ax
	jmp .loop

.incLinha:
	shr ebx, 0x10
	inc bx
	cmp bx, [esi+IHeight]
	jae .end
	shl ebx, 0x10
	add eax, 0x00010000
	mov ax, bp
	jmp .loop

.end:	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; desenha um bitmap mascarado (a cor rosa (0xff00ff) nao eh desenhado)
; Funcionamento:
;	pega pixel do bitmap fonte
;	coloca o pixel no bitmap destino
;
; IN:
;	eax: [Y|X]coordenada
;	 dl= numero da janela
;	esi: bitmap fonte
;	edi: bitmap destino
; OUT:
;	nothing

align 4
draw_MaskedImage:

	pushad
	xor ebx, ebx
	mov ebp, eax
.loop:	push eax
	mov eax, ebx
	call getPixel
	pop eax
	and ecx, 0xffffff		; Colocar essa instrucao no getPixel
	cmp ecx, 0xff00ff
	je .pula
	call putPixel
.pula:	inc bx
	cmp bx, [esi+IWidth]
	jae .incLinha
	inc ax
	jmp .loop

.incLinha:
	shr ebx, 0x10
	inc bx
	cmp bx, [esi+IHeight]
	jae .end
	shl ebx, 0x10
	add eax, 0x00010000
	mov ax, bp
	jmp .loop

.end:	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Desenha parte de uma imagem
; IN:
;	eax= [Y|X] coordenada destino
;	ebx= [Y|X] coordenada fonte
;	ecx= [Y|X] tamanho
;	edx= Objeto
;	esi= bitmap fonte
;	edi= bitmap destino
; OUT:
;	nothing

;==== variaveis =========

      BI_Fix equ esp+0xe	; dw - salva a primeira coordenada X passado por bx		
      BI_Dix equ esp+0xc	; dw - salva a primeira coordenada X passado por ax 
      BI_Ty equ esp+0xa	; dw - Tamanho Y do quadro
      BI_Tx equ esp+8	; dw - Tamanho X do quadro
      BI_Dy equ esp+6	; dw - coordenada Y da imagem destino
      BI_Dx equ esp+4	; dw - coordenada X da imagem destino
      BI_Fy equ esp+2	; dw - coordenada Y da imagem fonte
      BI_Fx equ esp	; dw - coordenada X da imagem fonte	

   ;------------------------

align 4
blit_Image:
	pushad
	add esp, 0xe	
	
  ; Salva as variaveis na pilha
  	mov [BI_Fix], bx
  	mov [BI_Dix], ax
  	mov [BI_Dx], eax	
	mov [BI_Fx], ebx
	add ecx, ebx		; ecx = coordenada fonte + tamanho
	mov [BI_Tx], ecx
		
.loop:	mov eax, [BI_Fx]
	call getPixel		
	mov eax, [BI_Dx]
	call putPixel
	inc word [BI_Fx]
	mov ax, [BI_Fx]
	cmp ax, [BI_Tx]
	jae .incLinha
	inc word [BI_Dx]
	jmp .loop

.incLinha:
	mov ax, [BI_Fy]
	inc ax
	cmp ax, [BI_Ty]
	jae .end
	mov [BI_Fy], ax
	mov ax, [BI_Fix]
	mov [BI_Fx], ax
	mov ax, [BI_Dix]
	mov [BI_Dx], ax
	inc word [BI_Dy]
	jmp .loop

.end:	sub esp, 0xe
	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Desenha parte de uma imagem
; IN:
;	eax= [Y|X] coordenada destino
;	ebx= [Y|X] coordenada fonte
;	ecx= [Y|X] tamanho
;	 dl= numero da janela
;	esi= bitmap fonte
;	edi= bitmap destino
; OUT:
;	nothing

;-----
; Funcionamento:
;	variaveis:
;	   eax= [Y|X] coordenada destino
;	   ebx= [Y|X] coordenada fonte
;	   ecx= [Y|X] tamanho
;	   dx= X coordenada destino inicial
;	   bp= X coordenada fonte inicial
;	   esi= bitmap fonte
;	   edi= bitmap destino
;	FIM

   ;==== variaveis =========

      BMI_Fix equ esp+0xe	; dw - salva a primeira coordenada X passado por bx		
      BMI_Dix equ esp+0xc	; dw - salva a primeira coordenada X passado por ax 
      BMI_Ty equ esp+0xa	; dw - Tamanho Y do quadro
      BMI_Tx equ esp+8	; dw - Tamanho X do quadro
      BMI_Dy equ esp+6	; dw - coordenada X da imagem destino
      BMI_Dx equ esp+4	; dw - coordenada Y da imagem destino
      BMI_Fy equ esp+2	; dw - coordenada Y da imagem fonte
      BMI_Fx equ esp	; dw - coordenada X da imagem fonte	

   ;------------------------

align 4
blit_MaskedImage:
	pushad
	add esp, 0xe	
	
  ; Salva as variaveis na pilha
  	mov [BMI_Dix], ax
  	mov [BMI_Fix], bx
  	mov [BMI_Dx], eax	
	mov [BMI_Fx], ebx
	add ecx, ebx		; ecx = coordenada fonte + tamanho
	mov [BMI_Tx], ecx
		
.loop:	mov eax, [BMI_Fx]
	call getPixel			
	and ecx, 0xffffff		; Colocar essa instrucao no getPixel
	cmp ecx, 0xff00ff
	je .pula
	mov eax, [BMI_Dx]
	call putPixel

.pula:	
	inc word [BMI_Fx]
	mov ax, [BMI_Fx]
	cmp ax, [BMI_Tx]
	jae .incLinha
	inc word [BMI_Dx]
	jmp .loop

.incLinha:
	mov ax, [BMI_Fy]
	inc ax
	cmp ax, [BMI_Ty]
	jae .end
	mov [BMI_Fy], ax
	mov ax, [BMI_Fix]
	mov [BMI_Fx], ax
	mov ax, [BMI_Dix]
	mov [BMI_Dx], ax
	inc word [BMI_Dy]
	jmp .loop

.end:	sub esp, 0xe
	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; Aumenta ou diminui uma imagem (bitmap)
; IN:
;	   eax= [Y|X] coordenada destino
;	   ecx= [Y|X] novo tamanho
;	   edx= Objeto
;	   esi= bitmap fonte
;	   edi= bitmap destino
; OUT:
;	nothing

   ;==== variaveis =========
      SI_Object equ esp+0x14	; dd - guarda o objeto
      SI_Dy equ esp+0x12	; dw - coordenada Y da imagem destino
      SI_Dx equ esp+0x10	; dw - coordenada X da imagem destino
      SI_Fx equ esp+0xc		; dd - coordenada X da imagem fonte
      SI_Fy equ esp+8		; dd - coordenada Y da imagem fonte
      SI_dx equ esp+4		; dd - Novo tamanho X / tamanho original X
      SI_dy equ esp		; dd - Novo tamanho Y / tamanho original Y
      ; ebp			; dd - Salva o parametro passado por EAX

   ;------------------------

align 4
Stretch_Image:
	pushad
	sub esp, 0x18

	mov ebp, eax		; salva eax em ebp
	shr ebp, 0x10


; Inicia as variaveis
	mov [SI_Dx], eax		; salva a coordenada destino
	mov [SI_Object], edx	; salva o numero da janela
	xor edx, edx		; necessario para calcular o x/tx
	mov dword [SI_Fx], edx	; zera as variaveis
	mov dword [SI_Fy], edx

; Calcula  Tamanho Original de X / Novo Tamanho de X
	movzx eax, word [esi+IWidth]
	shl eax, 0x10
	mov ebx, ecx
	and ebx, 0xffff
	or ebx, ebx		; verifica se nao eh 0
	jz .end
	div ebx
	mov [SI_dx], eax

; Calcula  tamanho original de Y / novo tamanho de Y
	xor edx, edx
	movzx eax, word [esi+IHeight]
	shl eax, 0x10
	mov ebx, ecx
	shr ebx, 0x10
	div ebx
	mov [SI_dy], eax

; Desenha a imagem
	mov edx, [SI_Object]	; pega o numero da janela

   ; pega o pixel
.loop:  mov eax, [SI_Fy]
   	mov ax, [SI_Fx+2]	; pega a parte inteira de Fx
   	call getPixel

   ; coloca o pixel
   	mov ax, [SI_Dy]
   	shl eax, 0x10
   	mov ax, [SI_Dx]	
   	call putPixel

   ; Incrementa coluna
   	mov eax, [SI_dx]
   	mov ebx, [SI_Fx]
   	add ebx, eax
   	mov [SI_Fx], ebx
   	shr ebx, 0x10
   	cmp bx, [esi+IWidth]
   	jae .incLinha
   	inc word [SI_Dx]
   	jmp .loop

   ; Incrementa linha
.incLinha:
	mov dword [SI_Fx], 0
   	mov [SI_Dx], bp
	mov eax, [SI_dy]
	mov ebx, [SI_Fy]
	add ebx, eax
	mov [SI_Fy], ebx
	shr ebx, 0x10
	cmp bx, [esi+IHeight]
	jae .end
	inc word [SI_Dy]
	jmp .loop

; FIM
.end:	add esp, 0x18
	popad
	ret


;-----------------------------------------------------------------------------------------




;======================== O T I M I Z A C A O ============================================


;======================== F U N C T I O N  1 =============================================
; Desenha um retangulo preenchido
; IN:
;	eax: [Y|X] coordenada
;	ebx: [Y|X] size
;	ecx= color
;	 dl = numero da janela
; OUT:
;	nothing
; OBS= deixar sempre dl a janela

Screen_DrawRectFill24:
	pushad

; ebp= Y e edx= X
	mov esi, ebx
	mov ebp, ebx
	shr ebp, 0x10
	and esi, 0xffff

; Verifica se Y=0
	or bp, bp
	jnz .a
	inc bp

; Calcula o endereco base
.a:	mov ebx, eax
	shr ebx, 0x10
	and eax, 0xffff
	push eax
	push ebx
	imul ebx, [Screen+IBytesPerLine]
	lea edi, [eax+eax*2]
	add edi, ebx
	add edi, [Screen+IPointer]
	pop ebx
	pop eax

; Coloca uma linha na horizontal
.newLine:	push eax
	push esi
	push edi

.put:	cmp ax, [Screen+IWidth]		; verifica se X<Screen_X
	jae .incLine
	call Object_CheckPixel
	jc .incX

; Desenha o pixel e incremento o X
	push ecx
	mov [edi], cx
	shr ecx, 8
	mov [edi+2], ch
	pop ecx
.incX:	dec esi
	jz .incLine
	add edi, 3
	inc eax
	jmp .put

; Incrementa o Y
.incLine:	pop edi
	pop esi
	pop eax
	add edi, [Screen+IBytesPerLine]
	inc ebx
	cmp bx, [Screen+IHeight]		; Verifica se Y<Screen_Y
	jae .end
	dec bp
	jnz .newLine

.end:	popad
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Desenha um retangulo preenchido
; IN:
;	eax: [Y|X] coordenada
;	ebx: [Y|X] size
;	ecx= color
;	 dl= numero da janela
; OUT:
;	nothing

Screen_DrawRectFill32:
	pushad

; ebp= Y e edx= X
	mov esi, ebx
	mov ebp, ebx
	shr ebp, 0x10
	and esi, 0xffff

; Verifica se Y=0
	or bp, bp
	jnz .a
	inc bp

; Calcula o endereco base
.a:	mov ebx, eax
	shr ebx, 0x10
	and eax, 0xffff
	push eax
	push ebx
	imul ebx, [Screen+IBytesPerLine]
	lea edi, [ebx+eax*4]
	add edi, [Screen+IPointer]
	pop ebx
	pop eax

; Coloca uma linha na horizontal
.newLine:	push eax
	push esi
	push edi

.put:	cmp ax, [Screen+IWidth]		; verifica se X<Screen_X
	jae .incLine
	call Object_CheckPixel
	jc .incX

; Desenha o pixel e incremento o X
	mov [edi], ecx
.incX:	dec esi
	jz .incLine
	add edi, 4
	inc eax
	jmp .put

; Incrementa o Y
.incLine:	pop edi
	pop esi
	pop eax
	add edi, [Screen+IBytesPerLine]
	inc ebx
	cmp bx, [Screen+IHeight]		; Verifica se Y<Screen_Y
	jae .end
	dec bp
	jnz .newLine

.end:	popad
	ret

;-----------------------------------------------------------------------------------------







;-----------------------------------------------------------------------------------------

; Imagem Quadro

;-----------------------------------------------------------------------------------------

;======================== F U N C T I O N  1 =============================================
; Desenha parte de uma imagem dividida em frames. Exemplo: Imagem com movimentos de um
; personagem num jogo 2d
; IN:
;	ebx: [Y|X] size
; OUT:
;	nada

;ImagemQuadro_Create:
;	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Desenha parte de uma imagem dividida em frames. Exemplo: Imagem com movimentos de um
; personagem num jogo 2d
; IN:
;	ebx: descritor de memoria
;	ecx: [Y|X] size do quadro
;	edx: [Y|X] qtde de quadro 
;	esi: Arquivo
;	edi: Descritor de Imagem
; OUT:
;	nada

ImagemQuadro_Load:	
	call load_Image	
	jc .end
	mov [edi+QsizeX], ecx
	mov [edi+QqtdeX], edx	
.end:	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Destroi uma imagem tipo quadro
; IN:
;	esi: imagem fonte
; OUT:
;	nada
; utilize a rotina Imagem_Free
;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Desenha parte de uma imagem dividida em frames. Exemplo: Imagem com movimentos de um
; personagem num jogo 2d
; IN:
;	eax: [Y|X] coordenada destino
;	ecx: numero do quadro
;	edx: Objeto
;	esi: imagem quadro fonte
;	edi: imagem destino
; OUT:
;	nada
; OBS: A quantidade de quadros comeca do 1 e o numero do quadro comeca do 0

ImagemQuadro_Draw:	
	push ecx
	push ebp
	mov ebp, ecx
	shr ecx, 0x10

  ; Se (bp>Quantidade de quadro no eixo X)  -> corrige e pega uma nova linha
	cmp bp, [esi+QqtdeX]
	jb .ok
	push eax
	push edx
	xor edx, edx
	mov ax, bp
	mov cx, [esi+QqtdeX]
	or cx, cx
	jz .error
	div cx
	mov cx, ax	; cx = numero do quadro Y corrigido
	mov bp, dx	; bp = numero do quadro X corrigido
	pop edx
	pop eax

  ; Se (cx>Quatidade de quadro no eixo Y)  -> Sai da rotina
.ok:  	cmp cx, [esi+QqtdeY]
	jae .end

  ; Calcula a coordenada Y do descritor da imagem fonte
	imul cx, [esi+QsizeY]
	mov ebx, ecx
	shl ebx, 0x10  	
  
  ; Calcula a coordenada X do descritor da imagem fonte
	imul bp, [esi+QsizeX]
	mov bx, bp	

  ; Desenha
	mov ecx, [esi+QsizeX]
	call blit_MaskedImage

  ; Fim
.end:	pop ebp
	pop ecx
	ret

  ; Error - Divisao por 0
.error:
	pop edx
	pop eax
	pop ebp
	pop ecx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Insere um quadro de uma imagem normal numa imagem tipo quadro
; IN:
;	eax: [Y|X] coordenada
;	ecx: numero do quadro
;	esi: imagem fonte
;	edi: imagem destino
; OUT:
;	nada

;ImagemQuadro_Insert:
;	ret

;-----------------------------------------------------------------------------------------


;-----------------------------------------------------------------------------------------



;-----------------------------------------------------------------------------------------

; Engine 2d - Para jogos 2D planos

;-----------------------------------------------------------------------------------------

; Com double buffer e sistema de frames
;E2D_Init:
;E2D_Deinit:
;E2D_Object_Create:
;E2D_Object_Destroy:
;E2D_LargeObject_Create:		; Para mapas grandes-> nao carregar inteiro na memoria
;E2D_LargeObject_Destroy:
;E2D_ObjectAnim_Create:
;E2D_ObejctAnim_Destroy:
;E2D_GetColisao:
;E2D_Forca_Create:
;E2D_Forca_Destroy:
;E2D_Render:


;-----------------------------------------------------------------------------------------



;-----------------------------------------------------------------------------------------

; Engine 3d

;-----------------------------------------------------------------------------------------

; Com double buffer e sistema de frames
;E3D_Init:
;E3D_Deinit:
;E3D_Object_Create:
;E3D_Object_Destroy:
;E3D_Object_Load:
;E3D_Object_Rotate:
;E3D_Object_Translation:
;E3D_Camera_Create:
;E3D_Camera_Destroy:
;E3D_Camera_Move:
;E3D_Camera_Rotate:
;E3D_Particles_Create:
;E3D_Particles_Destroy:
;E3D_Light_Create:
;E3D_Light_Destroy:
;E3D_GetColisao:
;E3D_Force_Create:
;E3D_Force_Destroy:
;E3D_CampForce_Create:
;E3D_CampForce_Destroy:
;E3D_SetViewRange:
;E3D_GetFPS:
;E3D_Render:


;-----------------------------------------------------------------------------------------


;-----------------------------------------------------------------------------------------

; Ethernet

;-----------------------------------------------------------------------------------------

;Rede_Config:
;Rede_GetIP:
;Rede_Socket_Create:		//Protocolo, Porta, IP, Status,
;Rede_Socket_Destroy:
;Rede_Socket_Connection:
;Rede_Socket_Send:
;Rede_Socket_Received
;Rede_Porta_Create:
;Rede_Porta_Destroy:


;-----------------------------------------------------------------------------------------