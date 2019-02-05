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
; @Id: FS_variables.inc  - 2008-1-20, fb-toledo
; File system constant equate definition
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------


;===================  S I S T E M A  D E  A R Q U I V O S  L P F S  ======================

DM_tam 	equ 0x400		; Tamanho padrao dos diretorios
assDM	equ 0xd0d0	; assinatura para DM
assD	equ 0xd1d1	; assinatura para Diretorios normais

;-----------------------------------------------------------------------------------------


; ========================== D I R E T O R I O  M E S T R E ==============================

; OBS:
; no arquivo DetectaDiscos_prg.cb tem a rotina que carrega diretorio mestre a ser usado
; A base do diretório mestre e a sua extensão são guardadas no bloco de boot
; O diretório é carregado na memória e sua posição é guardada numa variavel fixa do sistema
; (DM_pos)
; O diretório é colocado no heap como um heap local. Isto é, usa o endereço da base do bloco
; como base de busca, apontado por ebx.

D_extB 		equ -4	; Indica o tamanho do bloco, multiplos de 4 kb
DM_ass 		equ 0	; dd - Assinatura do diretorio
DM_part 		equ 2	; dd - Particao do diretorio mestre
DM_anterior	equ 4	; dd - 0
DM_itemDA		equ 8	; dd - 0
DM_X		equ 0xc	; dd - 0

; tamanho do bloco, multiplos de 4 kb
DM_ini_heap	equ 0x10	; dd - Ponteiro do inicio do heap
DM_topo_heap	equ 0x14	; dd - Ponteiro do ultimo item do heap (topo)
DM_fim_heap	equ 0x18	; dd - Ponteiro do fim do heap

; Os itens guardam os usuários
DM_ini_item 	equ 0x1c	; dd - Ponteiro do primeiro item
DM_atual_item 	equ 0x20	; dd - Ponteiro do item atual
DM_topo_item 	equ 0x24	; dd - Ponteiro do ultimo item

; Endereço na unidade e extensão (multiplo de 4 kb)
DMlba1 		equ 0x28	; dd - lba1 - usado para regravação....pegar na tabela de partições
DMlba2 		equ 0x2c	; dd - lba2
DMext 		equ 0x30	; dd - extensao do diretorio mestre

DM_res		equ 0x34	; rw

DM_cor_nome 	equ 0x36	; db - Cor do nome
DM_ext_nome 	equ 0x37	; db - Extensão do nome do diretorio mestre
DM_nome 		equ 0x38	; dd - Nome

; Espaço vazio
DM_ini_EV 	equ 0x3c	; dd - Ponteiro do inicial do espaco vazio
DM_atual_EV 	equ 0x40	; dd - Ponteiro do atual do espaco vazio
DM_topo_EV 	equ 0x44 	; dd - Ponteiro do ultimo do espaco vazio


; Extensão= 0x48
;-----------------------------------------------------------------------------------------



;============= Constantes do Diretorio ===================================================

D_extB 		equ -4		; Extensao do diretorio
D_ass 		equ 0		; dw - Assinatura do diretorio (0xc1c2)
D_part 		equ 2		; dw - Particao do diretorio
D_anterior 	equ 4		; dd - Diretorio anterior
D_itemDA		equ 8		; dd - Ponteiro deste item no diretorio anterior
D_X		equ 0xc		; dd - free

D_ini_heap 	equ 0x10		; dd - Ponteiro do incio do heap
D_topo_heap 	equ 0x14		; dd - Ponteiro do topo do heap
D_tam_heap 	equ 0x18		; dd - Ponteiro do fim do heap

D_ini_item 	equ 0x1c	 	; dd - Ponteiro do primeiro item
D_atual_item 	equ 0x20		; dd - Ponteiro do item atual
D_topo_item 	equ 0x24		; dd - Ponteiro do ultimo item

D_lba1 		equ 0x28		; dd - lba
D_lba2 		equ 0x2c		; dd - lba extendido
D_ext 		equ 0x30		; dd - extensao do diretorio

D_res		equ 0x34		; dw - free

D_cor_nome 	equ 0x36		; db - Cor do nome
D_ext_nome 	equ 0x37		; db - Extensão do nome do diretorio mestre
D_nome 		equ 0x38		; dd - base do Nome

; Extensão total= 0x3c



;-----------------------------------------------------------------------------------------


; Estrutura do Item ======================================================================

; Nao mude
I_extB  		equ -4		; dd - Extensão do item
I_prox 		equ 0		; dd - Ponteiro do proximo item
I_ant 		equ 4		; dd - Ponteiro do item anterior
I_continue 	equ 8		; dd - Ponteiro do item fragmentado do arquivo
I_diretorio 	equ 0xc		; dd - Ponteiro utilizado apenas quando o item for uma pasta, esse vai apontar aonde a pasta está localizada na memoria

I_tipo 		equ 0x10	  	; dw - tipo do arquivo (0xAABB-> AA-alto,BB-baixo)
I_attrib	 	equ 0x12		; dw - atributos
I_comentario   	equ 0x14		; dd - Ponteiro do comentario do arquivo

I_datacriacao 	equ 0x18		; dd - Data e hora de criação
I_dataacesso 	equ 0x1c		; dd - Data e hora do ultimo acesso
I_datamod 	equ 0x20		; dd - Data e hora da ultima modificaçao
I_reserved5	equ 0x24		; dd - Free

;I_extensaoS 	equ 0x28 		; dd - Extensão total do arquivo em bytes
I_SizeSectores	equ 0x28		; dd - Extensao do arquivo em setores
I_SizeByte	equ 0x2c		; dw - Extensao do ultimo setor
I_reserved4	equ 0x2e		; dw - Free
I_soma 		equ 0x30		; dd - Soma dos bytes do arquivo

I_reserved2 	equ 0x34		; dd - Free
I_reserved3 	equ 0x38		; dw - Free

I_cor_nome 	equ 0x3a      	; db - Cor do nome
I_ext_nome 	equ 0x3b		; db - Extensão do arquivo   	e
I_nome 		equ 0x3c		; dd - Ponteiro do Nome do item

extItem 		equ 0x40


;-----------------------------------------------------------------------------------------


; Constantes do Item fragmentado =========================================================
; apontado pela variavel I_continue

F_ext  		equ 0x0		; dd - extensao do arquivo
F_lba1 		equ 0x4		; dd - Ponteiro do lba do arquivo
F_continue 	equ 0x08		; dd - Ponteiro do item fragmentado do arquivo
F_lba2 		equ 0xc		; dd - Ponteiro do lba2 do arquivo (48bits)

extFrag 		equ 0x10


;-----------------------------------------------------------------------------------------


; Constantes do Item do Espaco livre =====================================================

EV_prox equ 0				; dd - Ponteiro para o proximo Item
EV_ant equ 0x4				; dd - Ponteiro para o Item anterior
EV_lba1 equ 0x8				; dd - Ponteiro do lba
EV_lba2 equ 0xc				; dd - Ponteiro do lba2
EV_ext  equ 0x10				; dd - Extensão do espaço livre

extEV equ 0x14


;-----------------------------------------------------------------------------------------