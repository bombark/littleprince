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
; @Id: FormatDisk.inc  - 2005-04-03, cb-toledo
; Make a C4 partitition over another chosen existing partitition and fixed DM and Admin
; folders
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------


; BufferTemp		equ 0xc00	(see OS_variables.inc)


; in:	al= partição 	(0, 1, 2 ou 3)
;   	bh= unidade	(80, 81, 82, ou 83)

align 4
FormatDisk :

	push eax
	push edi
	push ecx
	push esi
	movzx eax, al
	shl eax, 4
	lea esi, [eax+0x1be]
	mov edi, BufferTemp
	mov ecx,0x1
.b:	mov eax, 0		; lba
	mov bl, 1			; leitura

	push edi
	call discoHDFD		; altera ecx
	pop edi			; esi = base da tabela (1be, 1ce, 1de ou 1fe)
	jb .erro

	cmp word [edi+0x1fe], 0xaa55	; verifica a existencia do boot
	jne .erro

; verifica a existencia da partição desejada
	test byte [edi+esi+4], 0xff
	jz .erro			; partição nao existe

; ajusta base lba para o boot
	xor eax, eax
	add eax, [edi+esi+0x8]	;
	push eax			;
	push ecx
; PREPARA VARIAVEIS DO BOOT

; unidade mestre+cabeça
	mov bl, [edi+esi+0x1]
	xchg bh,bl
	mov word [unid_cab], bx	; unidade + cabeça do boot

	xchg bh,bl

; quantidade de cabeças
; Nas unidades grandes, t_ini_part = t_final_part e q_cabecas > 0x0f, portanto,
; deve-se desconsiderar a indicacao de trilha, cabeca e setor por trilha...
	mov al, byte[edi+esi+0x5]
	;inc al
	mov [q_cabecas], al		; al= qtde cabeças

; quantidade de setores
	mov al, byte [edi+esi+6]
	and al, 0x3f
	mov [q_setores], al		; al= qtde setores

; trilha final da partiçao
	movzx eax, word [edi+esi+6]
	and al, 0xc0
	ror al, 0x6
	ror ax, 0x8
	mov [t_final_part], ax

; trilha inicial da partiçao
	mov ax, word [edi+esi+2]
	and al, 0xc0
	ror al, 0x6
	ror ax, 0x8
	mov [t_ini_part], ax	; inicio da partiçao

; trilha inicial do kernel
;	mov [t_ini_kernel], ax	; inicio do kernel (Não definir ainda)

; posição do diretório mestre		; pré-definida em 2, com extensão igual a DM_tam
;	and edx, 0xff
;	mul edx
;	movzx edx, byte [unid_cab + 1]
;	add eax,edx
;	mul ecx
;	add eax, 2		; (trilha*cabeças+cab_inicial)*setores/trilha+2
;	mov [pos_Dir], eax		; ver boot
;	pop edx			; dx=cabeca/unidade

; quantidade total de setores da partiçao
	mov eax, [edi+esi+0x0c]
	mov [q_setores_part], eax


; PREPARA VARIAVEIS DO DIRETORIO MESTRE PARA HD

; define extensao do espaco livre no diretório (jul/2006)
; eax = quantidade de setores da partição
	sub eax, (DM_tam/0x200)*4 + 2	; subtrair o tamanho do diretório mestre
	;sub eax, DM_tam		; subtrair o tamanho da pasta publica
	mov [HDextEV], eax		; 2 = boot + setor 1

	mov eax, [edi+esi+0x8]
	mov [q_setores_anteriores], eax

; posiciona lba diretório...depende da trilha inicial da particao (setor 3 por definição)
	xor eax, eax
	add eax, 2
;	mov [pos_Dir], eax		; ver boot.inc

; define base do espaco livre na partiçao de instalaçao (jul/2006)
	add eax, (DM_tam/0x200)*4
	mov [base_EV], eax
;	mov word [LBA_ext],0

; define base/ext do kernel no diretório (jul/2006) (Não definir agora)
; todos os enderecos sao sempre referenciados a base, por isso podem ser definidos na
; compilacao
;	;mov [lba_i], 3+ExtDirMestre	; base
;	;mov word [lba_ei], 0
;	;movzx eax, [ext_kernel]	; extensao
;	;mov [extArq], eax
;
; define a localizacao do programa console (Não definir agora)
;
; define data instalaçao do kernel no diretório (jul/2006) (Não definir agora)
;
; define soma dos bytes do kernel no diretório (jul/2006) (Não definir agora)
;
;
; GRAVA BOOT E DIRETÓRIO MESTRE

.SalvaBoot:
	pop ecx
	pop eax

	mov bl, 2
	push edi
	mov edi, HDboot_img
	call HDD_Enter		; grava boot
	jb .erro1

	push ecx
	mov ecx, (DM_tam/0x200)*3	; pode variar conforme o tamanho de DM
	mov bl, 2			; base + 3
	mov edi, baseHD		; aponta DM e pasta publica
	add eax, 2
	call HDD_Enter		; grava DM
	jb .erro2
	add edi, DM_tam		; edi= memoria
	add eax, DM_tam/0x200	; eax= lba
	mov bl, 4			; 4 - gravação de segmento
	call HDD_Enter		; grava pasta publica, admin, publica e lixeira_ADMIN

	pop ecx
	pop edi
	xor eax, eax		; lba=0
	mov [edi+esi+4], byte 0xc4
	mov bl, 2			; serviço gravação
	call HDD_Enter		; grava tabela de partições
	jb .erro

.saida:	pop esi
	pop ecx
	pop edi
	pop eax
	ret

.erro1:	pop edi
.erro2:	pop ecx
.erro:	pop esi
	pop ecx
	pop edi
	pop eax
	stc
	ret



;=========================================================================================