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
; @Id: HC_info.inc - 2008-1-20, fb-toledo
; Alternative disk unit detect for protected mode - replace DiskDetect_16.inc (Doesn't work well yet)
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------


p_ide0     equ 0x01f0
p_ide1     equ 0x0170

DrvWaitTime equ 0x1000

; busca dados complementares das unidades IDE1. substitui a rotina pesquisa HD em Detecta
; Discos_prg.cb.

align 4
;FDHD_info:		; unidades físicas indicadas na CMOS
;	mov al,0x10
;	out 0x70,al
;	mov  cx,0xff
;.a:	dec  cx		; loop espera
;	cmp cx,0
;	jne .a
;	in  al,0x71
;	;mov [DRIVE_DATA],al
;	and al, 0xc0
;	shr al, 6
;	mov esi, baseFDD
;.b:	mov byte [esi+FDqTrilha],0x80	; salva trilhas da unidade, (sempre 80,- 0x4f)
;	mov dword [esi+FDbps],0xff1b1202	; parametros 1.44MB/3,5"
;	add esi, 0x20
;	dec al
;	jnz .b

; Esta rotina so detecta as unidades físicas indicadas na memoria CMOS. Para obter a
; quantidade de unidades virtuais, usar o serviço da int13 no modo real

align 4
HD_infoSeek:
	push edi
	push esi
	push edx
	push ebx
	push ecx
	xor ebx, ebx
	mov dx, p_ide0		; p_ide0=0x1f0+6

	mov esi, baseHDD		;

	mov al, 0xa0
	call recolhe_dados
	mov [esi+0x1d], bl		; coloca a unidade
	jb .a			; no caso de erro passa para a unidade seguinte
	or byte [esi+0x1c], 0xa0
	;mov [esi+0x1e], bh
	;or byte [esi+0x1e], 0x80
	inc bh			; quantidade de unidades
;int 1
.a:	;mov [q_unidIDE+3], bh	;apenas para verificação
	clc
	add esi, 0x20
	mov al, 0xb0
	call recolhe_dados
	mov [esi+0x1d], bl
	jb .b
	or byte [esi+0x1c], 0xb0

	;mov [esi+0x1e], bh
	;or byte [esi+0x1e], 0x80
	inc bh
;int 1
.b:	;mov [q_unidIDE+2], bh	;apenas para verificação
	clc
	add esi, 0x20
	mov dx, p_ide1		; p_ide1=0x170+6
	mov al, 0xa0
	call DeviceReset1
	call recolhe_dados
	mov [esi+0x1d], bl
	jb .c
	or byte [esi+0x1c], 0xa0
	;mov [esi+0x1e], bh
	;or byte [esi+0x1e], 0x80
	inc bh
;int 1
.c:	;mov [q_unidIDE+1], bh	;apenas para verificação
	clc
	add esi, 0x20
	mov al, 0xb0
	call recolhe_dados
	mov [esi+0x1d], bl
	jb .fim
	or byte [esi+0x1c], 0xb0
	;mov [esi+0x1e], bh
	;or byte [esi+0x1e], 0x80
	inc bh

.fim:	mov [q_unidIDE], bh		; salva quantas unidades detectou
	pop ecx
	pop ebx
	pop edx
	pop esi
	pop edi
	clc
	ret



align 4		; in: al= 0xa0 ou 0xb0
recolhe_dados:
	mov edi, BufferTemp		; valor para edi
	call HD_identify		; dx= 1x0h
	jnb .d			; tenta CD

; procura por cd/dvd
 	and dl, 0xf0
	;call DeviceReset1
	;jb .end			; tentativa de verificar CD
	push esi
	and dl, 0xf0		; dx= 1x0h
	call CD_identify
	pop esi
	jb .b4
	clc
	mov [esi+0x1c], byte 0xcd
	xor bl,bl

; testa se o dispositivo eh ATA
.d:	test byte [BufferTemp+1], 0x80
	jnz .b2			; deve ser zero

; testa so modo IORDY eh suportado
	mov al, byte [BufferTemp+2*49+1]
	test al, 0x10
	jnz .b2			; desvia se #4[]=1

; testa so modo IORDY eh suportado
	test al, 8
	jz .b2			; #3[]=1 senao desvia para acionar modo dma

; estes dados sao obsoletos na versao 6. se o modo LBA é suportado, nao se necessita CHS
	test byte [BufferTemp+2*53], 1
	jz .a
	mov eax, dword [BufferTemp+2*54]
	mov [esi+0x14], eax
	mov al, byte [BufferTemp+2*56]
	mov [esi+0x17], al
	;mov eax, [2*57]		; capacidade em setores (obsoleto)

; testa suporte ao modo LBA
	test byte [BufferTemp+2*83+1], 4	; testa suporte ao modo LBA estendido
	jnz .c			; bit #3[]=1
	test byte [BufferTemp+2*49+1], 2
	jz .fim
	mov eax, dword [BufferTemp+2*60]
	mov [esi+0x18], eax
	or byte [si+0x1c], 0x40	; ativa bit modo lba
.fim: 	clc
	ret

; modo CHS (para unidades antigas). nao precisa fixar a capacidade.
.a:	mov ax, word [BufferTemp+2*1]
	mov [esi+0x14], ax
	mov al, byte [BufferTemp+2*3]
	mov [esi+0x16], al
	mov al, byte [BufferTemp+2*6]
	mov [esi+0x17], al
	and byte [esi+0x1c], 0xbf	; garante o modo CHS
	clc
	ret

; operacao DMA nao suportada por enquanto
.b2:	mov bl, 0xfe
.b:	stc
	ret

; fixa modo lba estendido
.c:	mov eax, dword [BufferTemp+2*100]
	mov [esi+0x18], eax		; despreza 2 ultimos bytes
	or byte [esi+6], 2		; ativa LBA estendido
	or byte [esi+0x1c], 0x40	; ativa bit modo lba
	clc
	ret

align 4
.b4:	and dl, 0xf0		; erro vem das rotinas
	call DeviceReset1
	stc
	ret


; outros recursos como modo lba estendido poderao serem adicionados futuramente



;-----------------------------------------------------------------------------------------
;
; Entrada: 	al= 0xa0 ou 0xb0
;		dx= porta básica
;

align 4
DeviceReset1:
	push eax
	or dl, 6
	out dx,al		; al-> 1x6
	mov al,08h
	inc dx		; dx= 1x7
	out dx,al
	mov ecx,0x1fff
.a:	dec ecx		; rotina de espera
	cmp ecx,0
	je .erro1
	in al,dx		; BSY
	test al,80h
	jnz .a
	clc
	pop eax
	ret

.erro1:	mov bl,0xa1		; Unidade não respondeu
	pop eax
	stc
	ret



;in:	al= 0xa0 ou 0xb0
;	dx= porta basica

align 4
CD_identify:
	push eax
	or dl, 6			; dx=1x6
	out dx, al
	inc edx
	mov al,0xa1		; comando 0XA1
	out dx, al
        	mov cx,0x8000		; para unidades fisicas colocar 0x8000*****************
.a:	jcxz .erro1		; rotina de espera
        	in al,dx
        	test al,0x80   		; BSY- ocupado
        	loopnz .a
        	test al,1     		; ERRO
        	jnz .erroI
        	test al,0x08   		; DRQ
        	jz .a
          mov ecx,0x100		; possivel fazer coleta em #32
          and dl,0xf0		; coleta dx=1x0
          cld
          repe insw
          xor bl,bl
          pop eax
          retn
.erro1:  	mov bl,10
          stc
	pop eax
.b:    	retn
.erroI:	mov bl,0x11		; erro a classificar
	stc
	pop eax
	ret



align 4
HD_identify:
	push eax
	or dl, 6			; in: dx=1x0, al=0xa0 ou 0xb0
	out dx, al
	inc edx
	mov al,0xec		; comando 0XEC
	out dx, al

        	mov cx,0x8000		; em unidades fisicas colocar 0x8000*********************
.a:	jcxz .erro1		; rotina de espera
        	in al,dx
        	test al,0x80   		; BSY- ocupado
        	loopnz .a
        	test al,1     		; ERRO
        	jnz .erroI

        	test al,0x08   		; DRQ
        	jz .a
          mov ecx,0x100		; possivel fazer coleta em #32
          and dl,0xf0		; coleta dx=1x0
          cld
          repe insw
          xor bl,bl
          pop eax
          retn

.erro1:  	mov bl,1
          stc
	pop eax
.b:    	retn

.erroI:	mov bl,0xff		; erro a classificar
	stc
	pop eax
	ret



;erros:
;	01-	terminou o tempo de espera por resposta - time out
;	ff-	erro na unidade
;	10-	time out em unidade de cd/dvd
;	11-	erro em unidade cd/dvd
;	a1-	a unidade nao respondeu ao reset