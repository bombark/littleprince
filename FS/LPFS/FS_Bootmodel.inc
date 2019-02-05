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
; @Id: FS_Bootmodel.inc - 2006-11-26, fb-toledo
; Universal boot
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------

; Definitions:
use16

;DM_tam		equ 0x400
;p_ide0     	equ 0x01f0
;p_ide1     	equ 0x0170
;OS_BASE_F		equ 0x2000



seg_16	equ 0		;
base_16	equ OS_BASE_F	;
base_32	equ OS_BASE_F	;

; Enter point
IniSys equ inicio16 	; (ver inicio.inc)

; Segmento para o ponto de entrada no sistema, para o modo real
seg_ent equ 0

; buffer temporario para uso do boot
BufferBoot	equ 0x0c00
Buffer_lba	equ BufferBoot+0x1f0
aux_k16		equ BufferBoot+0x1f0	; usa o fundo do buffer
seg_k16		equ BufferBoot+0x1f2
aux_k32		equ BufferBoot+0x1f4

; posiçao onde o boot deve ser executado
nova_base 	equ 0x600
; nao importa onde o boot mestre carrega o boot do sistema...

;-----------------------------------------------------------------------------------------


align 16
HDboot_img:
	jmp HDboot and 0xffff


;-----------------------------------------------------------------------------------------

; DADOS

; configurar na instalacao
unid_cab		db 0 	; 0x80 (c:), 0x81 (d:), 0x82 (e:) ou 0x83 (f:) ou 00 (a:)  	2
cab		db 0	;						3
; setor inicial do kernel posicionado na compilacao usado no modo CHS
setor_i		db 0	; (definir na instalaçao do sistema )			4 *
b_res	      	db 0	; livre						5
t_ini_kernel	dw 0	; (definir na instalaçao do sistema )			6 *

; endereço linear do kernel
end_kernel	dd 0	; (definir na instalaçao do sistema )			8 *

; extensao do kernel em setores
ext_kernel  	dw 0	; (((sistema_fim-sistema_ini/512)) ) and 0xfffe	;	c

; trilha inicial da partição. Inicializar na instalaçao				e
t_ini_part	dw 0

; trilha final da partição. Inicializar na instalaçao				10
t_final_part	dw 0			;

; quantidade de setores por trilha. Inicializar na instalaçao			12
q_setores		db 0			;

; quantidade de cabeças por cilindro. Inicializar na instalaçao
q_cabecas		db 0			;				13

; quantidade de setores existentes antes da partição. Inicializar na instalaçao
; indica a posicao do boot
q_setores_anteriores 	dd 0		;				14

; quantidade total de setores dentro da partição. Inicializar na instalaçao
q_setores_part		dd 0		;				18


;-----------------------------------------------------------------------------------------
;
; Permite localizar a posiçao do diretório da unidade de partida (base+0x10)

Tabela_DM:
	; Primeira particao
pos_Dir 	dd 2			; posicao do diretorio (setor 3)		1c
extDirSet dd DM_tam/0x200		; extensao do diretorio (ver HD.inc)		20


;-----------------------------------------------------------------------------------------

HDboot:

	xor ax,ax		;
  	mov es,ax		;
; usa pilha padrao
  	mov ax, 0x9000
  	mov ss, ax
  	mov sp, 0xfffe

; obtem o valor de IP
  	push cs			; de 0x800 a 0xfffe
  	pop  ds			; ds=cs
	call pega_ip and 0xffff	; pega endereço-base do boot anterior e poe em SI

; transfere o boot para a posiçao 600
	sub si,$-HDboot_img		; si aponta o inicio do bloco de boot
	mov di, nova_base		; posicão do boot na memoria
	mov cx, 0x100		; extensão= 1 setor
	repne movsw

; pula para a próxima instruçao, no endereço 0:063b
	jmp 0:nova_base+p1-HDboot_img

; inicia o carregamento do kernel
p1:	mov bx,word [nova_base+unid_cab-HDboot_img]; dh=cabeca, dl=unidade

	test bl, 0x80
	jz .b and 0xffff		; modo CHS se for disquete

	test bl, 2
	jnz .c and 0xffff
	mov dx, p_ide0		; posiciona porta básica
	jmp .d and 0xffff
.c:	mov dx, p_ide1

.d:	mov cx, 0x1000
	add dx, 7			; aguarda DRDY=1
	in al, dx
	test al, 0x40
	jz opa and 0xffff

	dec dx			; dx=porta básica + 6
	mov al, bl
	and al, 3
	shl al, 4
	mov di, BufferBoot		; carrega a base do buffer (BufferTemporario)
	push edi
	call hd_id and 0xffff
	pop edi
	jc .f41 and 0xffff		; testa função 0x41 da int13
	;jmp .aa

;mov [0xb8000], 0x41

.aa:	test byte [di+2*49+1], 2	; verifica suporte ao modo lba
	jnz .mlba	and 0xffff	;jnz .mlba

; carregamento do boot pelo modo CHS (Hd antigos e disquetes)
.b:	mov word [aux_k16], base_16	; prepara buffer com base e seg para modo real
	mov word [seg_k16], seg_16
	mov dword [aux_k32], base_32  ; idem para endereçamento 32
	mov ax, seg_ent
	mov ds,ax					; DS= 0000
	mov dx, bx
	movzx ax, byte[nova_base+setor_i-HDboot_img]	; setor inicial do kernel
	mov si,ax
	mov cx,[nova_base+ext_kernel-HDboot_img]	; extensao
	mov ax,[nova_base+t_ini_kernel-HDboot_img]	; trilha inicial

; carrega bloco de setores (segmento de arquivo)
		; ax= trilha
		; cx=extensao em setores
		; dx cab / unidade
		; si=setor inicial (byte superior indiferente)
		; es:bx=ponteiro de memoria

.2a0:	mov bx, [aux_k16]		; posiçao de memoria que recebe os DADOS
	mov es, [seg_k16]		; segmento
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
  	jb opa and 0xffff	       ; erro .28c, trava...melhor mostrar na tela o erro se acontecer

; ajusta o endereço
  	push dx
  	mov eax,dword 0x200
  	add [aux_k32],eax     	; monta o descritor para o sistema
  	add [aux_k16],ax
  	mov ax,word [aux_k32+0x2]
  	mov bx,word 0x1000
  	mul bx
  	mov [seg_k16],ax
  	pop dx
  	pop ax
  	and cl,byte 0x3f			       ; maximo permitido: 63 setores
  	inc cl
  	cmp cl,[nova_base+q_setores-HDboot_img]     ; excedeu a quantidade de setores
  	ja .2e9 and 0xffff
.2e4:	xchg cx,si
	loop .2a0 and 0xffff

; desliga motor se boot foi feito atraves de floppy disk
	cmp dl, 0x80
	jae .fim and 0xffff
	mov al,0x0c
	mov dx,0x3f2
	out dx,al			; a rotina termina com es=0x0, ds=0x0

.fim:

;jmp $ and 0xffff

	jmp seg_ent:IniSys 		; execucao do kernel (verificado...OK)


;-----------------------------------------------------------------------------------------

align 2
.2e9:	mov cl,byte 0x01		; comeca de 1
  	inc dh
  	cmp dh,[nova_base+q_cabecas-HDboot_img]     ; excedeu a quantidade de cabecas
  	je .2f5 and 0xffff
  	jmp .2e4 and 0xffff
.2f5:	xor dh,dh			; zera a cabeca
  	inc ax			; incrementa a trilha
  	jmp .2e4 and 0xffff


;-----------------------------------------------------------------------------------------

; testa funcao 41 da int13
.f41:	mov dx, bx		; dl = unidade
	mov bx, 0x55aa
	mov ah, 0x41
	int 0x13
	mov al, 0xcc
	jnb .mlba3 and 0xffff	;jnb .mlba3
	jmp opa and 0xffff


; carrega kernel pelo modo lba
.mlba:	mov dx, bx		; recupera DL= unidade
.mlba3:	mov si, Buffer_lba		; di=BufferBoot
	mov word [si], 0x10		; extensao do bloco de dados p/int 0x42
;prepara a base lba para acessar o kernel no disco
	mov eax, [nova_base+q_setores_anteriores-HDboot_img]	; Buffer+8
	add eax, [nova_base+end_kernel-HDboot_img] 	; base lba do kernel no HD
	mov [si+0x8], eax
	xor eax,eax
	mov [si+0xc], eax		; lba estendido = zero
	mov ax, word seg_16
	mov [si+0x6], ax
	mov ax, base_16
	mov [si+0x4], ax		; posicao na memoria
	movzx ecx, word [nova_base+ext_kernel-HDboot_img]

;jmp $ and 0xffff

	mov ah, 0x42
.mlba2:	cmp cx, 0x7f
	jbe .mlba1 and 0xffff
	mov word [si+0x2], 0x7f
	sub cx, 0x7f		; verificar se eh 80 ou 7f
	int 0x13
	jb opa and 0xffff
	add dword [si+0x8], 0x7f	; idem
	jmp .mlba2 and 0xffff
.mlba1:	or ecx,ecx
	jz .fim and 0xffff			; nao tem mais nada a buscar
	mov al, 0xbb		; ?????
	xor al,al			; usado no teste
	mov [si+0x2], ecx		; posiciona a quantidade de setores (max: 127)

;jmp $ and 0xffff
; VIRTUALBOX: FALHA NA LEITURA USANDO O SERVIÇO 0X42 DA INT 0X13
	int 0x13			; Nao funciona em todos os BIOS
	jnb .fim and 0xffff
opa:	mov dx, bobeira and 0xffff
	mov ah,9
	int 0x21
	jmp $ and 0xffff		; trava se ocorre erro


;-----------------------------------------------------------------------------------------

; rotinas auxiliares

align 2
pega_ip:	pop si		; si= IP (ponteiro de instruçao)
	push si
	ret

; verifica suporte ao modo lba... pesquisa bit #2[base+63]
align 2
hd_id :	out dx, al		; dx = porta basica + 6, envia unidade
	inc dx
	mov al,0xec
	out dx, al		; envia comando ec
        	mov cx,0xffff
.a1:	jcxz .a2 and 0xffff
        	in al,dx
        	test al,80h   		; BSY- ocupado
        	loopnz .a1 and 0xffff
        	test al,1     		; ERRO
        	jnz .a2 and 0xffff
        	test al,08h   		; DRQ
        	loopz .a1 and 0xffff
          mov cx,0x0100
          and dl,0xf0		; coleta dx=1x0
          cld
          cli
          repe insw
          retn
.a2:	stc
	ret


;-----------------------------------------------------------------------------------------

; Avisos:
bobeira	db 'f',0xd,0xa,'$'


;-----------------------------------------------------------------------------------------

;db '06/05/2006 14:25'
times (HDboot_img + 0x1fe - $)  db 0
db 0x55,0xaa			;boot signature

;-----------------------------------------------------------------------------------------
use32















