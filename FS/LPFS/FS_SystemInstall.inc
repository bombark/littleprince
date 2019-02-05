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
; @Id: InstalaSistema.inc  - 2008-1-20, fb-toledo
; Install system into a HD unit formatted with a C4 partitition (incomplete)
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------

;BufferTemp2	equ 0xe00		; ver definiçao em FS/boot.inc
kernelAddr	equ BufferTemp2+8
kernelExt		equ BufferTemp2+0xc
kernel_Sector	equ BufferTemp2+4
kernel_track	equ BufferTemp2+6
kernel_head	equ BufferTemp2+3
kernel_unit	equ BufferTemp2+2

; verificar se a pasta sistema existe. criar se nao existe
; deve instalar o sistema na pasta de sistema
; deve verificar se o arquivo já existe. neste caso atualizar a copia no mesmo endereço
;
; IN: 	bl= partição
;	ecx= extOS
;	edi= baseOS na memoria
; OUT:	cy=0  nada...instala o sistema na partiçao
;	cy=1  sistema não é instalado

align 4
InstalaSistema:
;int 1
	mov edi, OS_BASE_F or OS_BASE
; extensao do kernel em setores
	mov ecx, (sistema_fim-(sistema_ini + OS_BASE))
.a:	call LoadBoot		; busca boot da unidade

; verifica se o disco está formatado para o sistema...senao formata
;int 1
	push esi
; busca dados sobre DM
	or bl, bl
	jz .erro1
	and ebx, 0x1f
	dec bl
	shl ebx, 5
	mov ebx, [TabelaPart+ebx+Part_DMpos]	; verifica se DM se encontra na memoria
	or ebx, ebx		; ebp= base mem DM
	jz .erro1			; pode-se carregar futuramente
; pesquisar existencia da pasta do sistema...ebx= base DM
	mov esi, PSys_nome		; pesquisa pasta Sistema
	mov al, Kn1-Kn0;

	push ecx
	push edi
;int 1	; ebx=base dir; esi=string; ax=ext string
	call pesquisa_diretorio	; altera edi
	jb .b			; a pasta não existe...criar uma e instalar o sistema
;int 1
; encontrou...pesquisa item (kernel)	(carrega na memoria se existe) ebx: aponta o item
.erro_:	mov esi, K_Nome
	mov eax, Kn2-Kn1
	mov cl, byte 4
	movzx ecx, cl
	call procura_item		; edi= ponteiro do item se cy=0
	;jb .c			; edi=0 se cy=1 -> instalar o kernel

; o item ja existe...pode-se atualizar o kernel

	;call atualiza arquivo	;regrava no mesmo item..o tamanho pode mudar
	pop edi
	pop ecx
	pop esi
	ret

; a pasta não existe...criar uma e alocar o sistema
; ebx= 	base DM
; esi=	base Nome da pasta
; ax=  	ext. nome

.b:
;int 1
	mov ecx,eax	; esi= base do nome
	mov al, 1		; tipo: pasta
	call cria_item	;						OK
;int 1
	jb .erro

	mov esi, edi

	call cria_diretorio_n0	; ebx= aponta DM (altera DM )
	mov ebx, edi		; edi= aponta novo diretório
	pop edi			; edi= base arquivo na memoria
	pop ecx			; ecx= extensao do arquivo
;int 1
; instala o kernel...
	mov esi, K_Nome	; nome do item
	mov al, Kn2-Kn1	; ext nome

.c:	call InstalaArquivo		; bl= partição
int 1
; atualiza boot com a posição do kernel....fazer outro boot específico para este SA

	;mov ebx, edi		; usar edi diretamente
	mov esi, [ebx+esi+I_continue]	;
; extensao
	mov eax, [ebx+esi]		; +F_ext=0
	mov [kernelExt], eax	; ver FS/boot.inc
; base
	mov eax, [ebx+esi+F_lba1]
	mov [kernelAddr], eax
	mov bl, [ebx+DM_part]
;int 1
; salva cabeça trilha e setor inicial
	push ebx
	and ebx, 0x1f
	dec bl
	shl ebx, 5		; multiplica por 0x20
	mov bl, [TabelaPart+ebx+ Part_unidade]
	mov [kernel_unit], bl
	and ebx, 0x3
	shl ebx, 5
	lea esi, [baseHDD+ebx]	;
;int 1
	call calculaTCS		; ver Devices/HD_dev.inc
;int 1
	mov [kernel_Sector], al
	shr eax, 0x10
	mov [kernel_track], ax
	shr eax, 8
	mov [kernel_head], al
	pop ebx
;int 1
	call SaveBoot
;int 1
	pop esi
	ret

.erro1:		; carregar DM da unidade se ela existe
.erro:	pop esi
	stc
	ret

Kn0:
PSys_nome:	db 'Sistema'
Kn1:
K_Nome:		db 'FGB_CoreSys001'
Kn2:

;-----------------------------------------------------------------------------------------

; Busca setor de boot
; IN: bl= partiçao
LoadBoot:
	push esi
	push eax
	push edi
	mov edi, BufferTemp2
	mov eax, 0x0
	call ReadSector0
	pop edi
	pop eax
	pop esi
	ret


; Busca setor de boot
; IN: bl= partiçao
SaveBoot:
	push esi
	push eax
	push edi
	mov edi, BufferTemp2	; edi= 0x80000e00
	xor eax, eax		; address= 0x0
	call WriteSector0
	pop edi
	pop eax
	pop esi
	ret


;-----------------------------------------------------------------------------------------

