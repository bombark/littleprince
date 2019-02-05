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
; @Id: Boot.inc - 2006-11-26, fb-toledo
; Boot for floppy Disk
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------

;OS_BASE_F equ 0x2000
; Definitions:

use16
FDaux_k16		equ 0x0c00
FDseg_k16		equ 0x0c00+2
FDaux_k32		equ 0x0c00+4

FDbase_16		equ OS_BASE_F 		;OS_BASE_F equ 0x2000
FDseg_16		equ 0x0000
FDbase_32		equ OS_BASE_F

; ponto de entrada no sistema, para o modo real
seg_ent		equ 0x0000
enter_point	equ inicio16 	; (see start.inc)

; posiçao onde o boot deve ser executado
FDnova_base 	equ 0x600

FDimagem_HDboot:
jmp FDinicio_boot

;------------------------------------------------------------------------------------------

; DADOS

; os dados zerados sao configurados na instalacao
FDunid_cab	db 0 	; 0x80 (c:), 0x81 (d:), 0x82 (e:) ou 0x83 (f:) ou 00 (a:)	2
FDcab		db 0	;						3

; setor inicial do kernel posicionado na compilacao usado no modo CHS
FDsetor_i	db 2 	; setor B						4
FDres		db 0	;						5
FDt_ini_kernel	dw 0	; trilha inicial					6

; extensao do kernel em setores posicionada na compilação
FDext_kernel  	dw ((sistema_fim-sistema_ini+0x1ff)/0x200) and 0xffff  ;		8

; quantidade de setores por trilha posicionada na instalaçao
FDq_setores	db 0x12			;				a

; quantidade de cabeças por cilindro posicionada na instalaçao
FDq_cabecas	db 2			;				b


;-----------------------------------------------------------------------------------------

FDinicio_boot :

	xor ax,ax		;
  	mov es,ax		;

; usa pilha padrao
  	mov ax, 0x9000
  	mov ss, ax
  	mov sp, 0xfffe

; obtem o valor de IP
  	push cs			; de 0x800 a 0xfffe
  	pop  ds			; ds=cs
.teste:	call FDpega_ip		; pega endereço-base do boot anterior e poe em SI

; transfere o boot para a posiçao 600
	sub si,$-FDimagem_HDboot	; si aponta o inicio do bloco de boot
	mov di, FDnova_base		; posicão do boot na memoria
	mov cx, 0x100			; extensão= 1 setor
	repne movsw

; pula para a próxima instruçao, no endereço 0:063b
	jmp 0:FDnova_base+FDp1-FDimagem_HDboot

; inicia o carregamento do kernel
FDp1:	mov dx,word [FDnova_base+FDunid_cab-FDimagem_HDboot]     ; dh=cabeca, dl=unidade

; carregamento do boot pelo modo CHS (Hd antigos e disquetes)
	mov word [FDaux_k16], FDbase_16   ; prepara buffer com base e seg para modo real
	mov word [FDseg_k16], FDseg_16
	mov dword [FDaux_k32], FDbase_32  ; idem para endereçamento 32
	mov ax, seg_ent
	mov ds, ax
	movzx ax,byte [FDnova_base+FDsetor_i-FDimagem_HDboot]    ; setor inicial do kernel
	mov si,ax
	mov cx,[FDnova_base+FDext_kernel-FDimagem_HDboot]	       ; extensao
	mov ax,[FDnova_base+FDt_ini_kernel-FDimagem_HDboot]      ; trilha inicial

; carrega bloco de setores (segmento de arquivo)
; ax= trilha
; cx=extensao em setores
; dx cab / unidade
; si=setor inicial (byte superior indiferente)
; es:bx=ponteiro de memoria

.2a0:	mov bx, [FDaux_k16]		; posiçao de memoria que recebe os DADOS
	mov es, [FDseg_k16]		; segmento
   	push ax
  	xchg cx,si
   	and cl,0x3f
  	ror ah,2
  	and ah,0xc0
  	mov ch,al
  	or cl,ah

; int13- ah=2 leitura, al=quant setores, cx=trilha, dl=unidade, dh=cab, bx= aponta memoria
  	mov ax, 0x0201
  	int 0x13
  	jb .opa	       ; erro .28c, trava...melhor mostrar na tela o erro se acontecer

; ajusta o endereço
  	push dx
  	mov eax,dword 0x200
  	add [FDaux_k32],eax     ; monta o descritor para o sistema
  	add [FDaux_k16],ax
  	mov ax,word [FDaux_k32+0x2]
  	mov bx,word 0x1000
  	mul bx
  	mov [FDseg_k16],ax
  	pop dx
  	pop ax
  	and cl,byte 0x3f			       ; maximo permitido: 63 setores
  	inc cl
  	cmp cl,[FDnova_base+FDq_setores-FDimagem_HDboot]     ; excedeu a quantidade de setores
  	ja .2e9
.2e4:	xchg cx,si
	loop .2a0

; desliga motor se boot foi feito atraves de floppy disk
	mov al,0xc
	mov dx,0x3f2
	out dx,al			; a rotina termina com es=0x0, ds=0x0

; pula para executar o sistema
.fim:	jmp seg_ent:enter_point 	; execucao do kernel (verificado...OK)

; a rotina termina com es=0x0, ds=0x0
.opa:	mov edi, alert
	mov edi, 0xb8000
.m:	mov [edi],al
	add di, 2
	or al, al
	jnz .m
	jmp $

;-----------------------------------------------------------------------------------------

align 2
.2e9:	mov cl,byte 0x01		; comeca de 1
  	inc dh
  	cmp dh,[FDnova_base+FDq_cabecas-FDimagem_HDboot]     ; excedeu a quantidade de cabecas
  	je .2f5
  	jmp .2e4
.2f5:	xor dh,dh			; zera a cabeca
  	inc ax			; incrementa a trilha
  	jmp .2e4

;-----------------------------------------------------------------------------------------

align 2
FDpega_ip:	pop si		; si= IP (ponteiro de instruçao)
		push si
		ret

alert	db 'xiiiii.......problems', 0

;-----------------------------------------------------------------------------------------

;db '06/05/2006 14:25'
times (FDimagem_HDboot+0x1fe-$)  db 00h
db 0x55,0xaa			;boot signature

;-----------------------------------------------------------------------------------------

use32














