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
; @Id: HD.inc  - 2008-1-20, fb-toledo
; Folder estructure for starting system
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------



; More informations:
; Define as pastas que devem ser instaladas na formatação de uma unidade. São:
; diretório mestre, pasta do Administrador e Pasta Publica
; a pasta do Administrador tem já definida a pasta da lixeira

; tipo de itens:
;	0- x
;	1- pasta
;	2- atalho de pasta
;	3- atalho de arquivo
;	4 a 0xf-	arquivos do sistema
;	10 em diante- arquivos genéricos (a definir)
;

;-----------------------------------------------------------------------------------------

; Definições

DM_tam 	equ 0x400
assDM	equ 0xd0d0
assD	equ 0xd1d1

; por enquanto é carregado juntamente com o kernel
; as variaveis marcada por ? são inicializadas na instalação
; assinatura para o diretório mestre: c0c0, para os demais: c1c2

;=========================================================================================

; 		Diretório mestre da unidade (DM) - LBA base + 2

;=========================================================================================
;
align 0x10
baseHD:
	dw assDM   		; Assinatura do diretorio (c1c2c3cb)		0
	dw ?			; partição (dispositivo)			2

; link com diretório superior
	dd 0			; DM_anterior - diretorio superior		4
	dd 0			; DM_itemDA - aponta item no dir superior	8
	dd 0			; x - free (buscar na tabela de partições)				c

; tamanho do diretorio, sempre em multiplos de 4kB
	dd iniHeap-baseHD		; Ponteiro do inicio do heap			10
	dd fim_HD-baseHD-4		; Ponteiro do ultimo item do heap		14
	dd fimdd-nomeDM+4		; Ponteiro do fim do heap (alterado para o tamanho)

; bloco fixo da lista de espaço vazio
	dd itemDM1-baseHD		; Ponteiro do inicial do espaco vazio		1c
	dd itemDM1-baseHD		; Ponteiro do atual do espaco vazio		20
	dd itemDM2-baseHD		; Ponteiro do ultimo do espaco vazio		24

; Posição do diretório mestre dentro da unidade (usada na regravação)
	dd 2			; lba1					28
	dd 0			; lba2 (dw alto reservado)			2c
	dd (DM_tam/0x200)		; extensão				30

	dw 0xffff			; reservado				34

; Nome da pasta
	db 7			; cor nome (busca cor em tabela com 256 possibilidades)
	db 2			; ext nome
	dd nomeDM-baseHD		; ponteiro do nome				38

; bloco fixo dos itens do diretório
	dd EV1-baseHD	 	; Ponteiro do primeiro item			40
	dd EV1-baseHD  		; Ponteiro do atual item			44
	dd EV1-baseHD		; Ponteiro do ultimo item			48

align 4;----------------------------------------------------------------------------------

;inicio do espaço util do diretório     ; 0x48
iniHeap	dd 3			; marca o tamanho do bloco ocupado pelo nome	4c
nomeDM	db "C:"			; da unidade (partição)			50

;-----------------------------------------------------------------------------------------
;
; primeiro item do espaço vazio
	dd 0x15		; extensão do bloco + bit ocupado  (bloco do diretório mestre)
EV1:	dd 0		; ponteiro do próximo item
	dd 0		; ponteiro item anterior
base_EV	dd ?		; lba 1 (dd) - posição do diretório mestre na unidade
LBA_ext	dd 0		; lba 2 (dd)
HDextEV	dd ?		; ext: INICIALIZADO NA INSTALAÇÃO COM O TAMANHO DA PARTIÇÃO

; O espaço vazio é sempre decrementado na quantidade de setores a cada alocação. Apenas
; na liberação é que se cria novo bloco	amarrado ao bloco inicial.


;-----------------------------------------------------------------------------------------
;
; Primeiro item da lista - Pasta do administrador (proprietário)

	dd 0x41		; * (tamanho do bloco no heap)
itemDM1:
	dd itemDM2-baseHD	; ponteiro do proximo item
	dd 0x0		;
	dd fragDB1-baseHD	; Ponteiro do item fragmentado do arquivo 	4 bytes
	dd 0		; apenas para pasta, aponta onde a pasta está na memoria

	dd 0		; Ponteiro do icone (tabela de icone do sistema) 	2 bytes
	dd 0		; Ponteiro do comentario do arquivo              	4 bytes

	dd 0xffffffff	; Data de criação     	4 bytes
	dd 0xffffffff	; Hora de criação	 	4 bytes
	dd 0xffffffff	; Data de modificaçao 	4 bytes
	dd 0xffffffff	; Hora de modificaçao 	4 bytes

	dd (DM_tam/0x200)	; Extensão total do arquivo em setores  	4 bytes
	dw (DM_tam/0x200)*0x200	; Extensão total em bytes		2 bytes
	dw 1                ; Atributo (pasta)          			2 bytes
	dd 0xffffffff       ; Soma dos bytes do arquivo   		4 bytes

	dd 0		; Reservado 				4 bytes
	dw 0                ; Reservado 				2 bytes

	db 7                ; Cor do nome				1 byte
	db 13               ; Extensão do arquivo   			1 byte
	dd nomeI1-baseHD    ; Nome do item (pasta, arquivo, icone,...)  	4 bytes

;-----------------------------------------------------------------------------------------

	dd 15   		; * (tamanho do bloco no heap)
nomeI1: 	db "Administrador",0	; manter 14 espaços

;-----------------------------------------------------------------------------------------

	dd 0x11		; * (tamanho do bloco no heap)
fragDB1: 	dd (DM_tam/0x200)		; extensao
	dd (DM_tam/0x200)+2		; lba1
	dd 0		; Icontinue
	dd 0		; lba2

;-----------------------------------------------------------------------------------------

; Segundo item da lista - Pasta publica

	dd 0x41		; * (tamanho do bloco no heap)			0
itemDM2:
	dd 0x0		; ponteiro do proximo item				0
	dd itemDM1-baseHD		;						4
	dd fragDB2-baseHD	; Ponteiro do item fragmentado do arquivo 	4 bytes	8
	dd 0		; apenas para pasta, aponta onde a pasta está na memoria	c

	dd 0		; Ponteiro do icone (tabela de icone do sistema) 	2 bytes	10
	dd 0		; Ponteiro do comentario do arquivo              	4 bytes	14

	dd 0xffffffff	; Data de criação     	4 bytes			18
	dd 0xffffffff	; Hora de criação	 	4 bytes			1c
	dd 0xffffffff	; Data de modificaçao 	4 bytes			20
	dd 0xffffffff	; Hora de modificaçao 	4 bytes			24

	dd (DM_tam/0x200)	; Extensão total do arquivo em setores  	4 bytes	28
	dw (DM_tam/0x200)*0x200	; Extensão total em bytes		2 bytes	2c
	dw 2                ; Atributo (pasta)          			2 bytes	2e
	dd 0xffffffff       ; Soma dos bytes do arquivo   		4 bytes	30

	dd 0xffffffff	; Reservado 				4 bytes	34
	dw 0xffff             ; Reservado 				2 bytes	38

	db 7                ; Cor do nome				1 byte	3a
	db 13               ; Extensão do arquivo   			1 byte	3b
	dd nomeI2-baseHD    ; Nome do item (pasta, arquivo, icone,...)  	4 bytes	3c

;-----------------------------------------------------------------------------------------

	dd 14 or 1   		; * (tamanho do bloco no heap)
nomeI2: 	db "Pasta Publica",0	; manter 14 espaços

;-----------------------------------------------------------------------------------------

	dd 0x11		; * (tamanho do bloco no heap)
fragDB2: 	dd DM_tam/0x200		; extensao
	dd (DM_tam/0x200)*2+2		; lba1
	dd 0		; Icontinue
	dd 0

 	dd (DM_tam-(fim_HD-baseHD)) and 0xfffe 		; fecha heap------------------

fim_HD:
	 rb DM_tam-(fim_HD-baseHD)

fimdd:			; fim do diretório mestre



;=========================================================================================-
;
; 			Pasta Admin - LBA base + (DM_tam/0x200)+2
;
;=========================================================================================
; abre somente para o adminstrador apos digitar sua senha de habilitação
; contem link para lixeira e para pasta pública
;
PA_baseD:
	dw assD		; assinatura para diretório				0
	dw 0		; partição					2
	dd 0		; ponteiro de memoria para diretorio anterior		4
	dd itemDM1-baseHD	; aponta item na pasta anterior			8
	dd 0		; x 						c

	dd PA_Inome2-PA_baseD-4	;					10
	dd PA_fimD-PA_baseD-4	;					14
	dd PA_fimdd-PA_Inome2+4	;					18

	dd PA_itemD-PA_baseD	;					1c
	dd PA_itemDM2-PA_baseD	;					20
	dd PA_itemD-PA_baseD	;					24

	dd (DM_tam/0x200)+2	; base= DM_ext = 2	;				28
	dd 0		;		;				2c
	dd (DM_tam/0x200)	;						30

	dw 0xffff		;						34

	db 7		;						38
	db 13		;						3c
	dd PA_Inome2-PA_baseD	;					40
align 4
	dd 14 or 1			; * (tamanho do bloco no heap)	44
PA_Inome2:	db "Administrador",0 	; manter 14 espaços

; espaço do heap local
; pasta lixeira - A lixeira é particular a cada usuário
	dd 0x41		; * (tamanho do bloco no heap)
PA_itemD:
	dd PA_itemDM2-PA_baseD
	dd 0x0
	dd PA_fragI-PA_baseD
	dd 0

	dd 0
	dd 0

	dd 0xffffffff	; Data de criação     			4 bytes
	dd 0xffffffff	; Hora de criação	 			4 bytes
	dd 0xffffffff	; Data de modificaçao 			4 bytes
	dd 0xffffffff		; Hora de modificaçao 		4 bytes

	dd (DM_tam/0x200)	; Extensão total do arquivo em setores  	4 bytes
	dw (DM_tam/0x200)*0x200	; Extensão total em bytes		2 bytes
	dw 1                ; Atributo (pasta)          			2 bytes
	dd 0xffffffff       ; Soma dos bytes do arquivo   		4 bytes

	dd 0xffffffff
	dw 0xffff

	db 7
	db 7
	dd PA_nomeI1-PA_baseD

	dd 8 or 1		; * (tamanho do bloco no heap)
PA_nomeI1: 	db "Lixeira",0	; manter 8 espaços

; item fragmentado
	dd 0x11		; * (tamanho do bloco no heap)
PA_fragI	dd DM_tam/0x200
	dd (DM_tam/0x200)*3+2
	dd 0
	dd 0

;-----------------------------------------------------------------------------------------

; Segundo item da lista - Pasta publica

	dd 0x41		; * (tamanho do bloco no heap)			0
PA_itemDM2:
	dd 0x0		; ponteiro do proximo item				0
	dd PA_itemD-PA_baseD		;						4
	dd PA_fragDB2-PA_baseD	; Ponteiro do item fragmentado do arquivo 	4 bytes	8
	dd 0		; apenas para pasta, aponta onde a pasta está na memoria	c

	dd 0		; Ponteiro do icone (tabela de icone do sistema) 	2 bytes	10
	dd 0		; Ponteiro do comentario do arquivo              	4 bytes	14

	dd 0xffffffff	; Data de criação     	4 bytes			18
	dd 0xffffffff	; Hora de criação	 	4 bytes			1c
	dd 0xffffffff	; Data de modificaçao 	4 bytes			20
	dd 0xffffffff	; Hora de modificaçao 	4 bytes			24

	dd (DM_tam/0x200)	; Extensão total do arquivo em setores  	4 bytes	28
	dw (DM_tam/0x200)*0x200	; Extensão total em bytes		2 bytes	2c
	dw 1                ; Atributo (pasta)          			2 bytes	2e
	dd 0xffffffff       ; Soma dos bytes do arquivo   		4 bytes	30

	dd 0xffffffff	; Reservado 				4 bytes	34
	dw 0xffff             ; Reservado 				2 bytes	38

	db 7                ; Cor do nome				1 byte	3a
	db 13               ; Extensão do arquivo   			1 byte	3b
	dd PA_nomeI2-PA_baseD    ; Nome do item (pasta, arquivo, icone,...)  	4 bytes	3c

;-----------------------------------------------------------------------------------------

	dd 14 or 1   		; * (tamanho do bloco no heap)
PA_nomeI2: 	db "Pasta Publica",0	; manter 14 espaços

;-----------------------------------------------------------------------------------------

		dd 0x11			; * (tamanho do bloco no heap)
PA_fragDB2: 	dd DM_tam/0x200		; extensao
		dd (DM_tam/0x200)*2+2	; lba1
		dd 0			; Icontinue
		dd 0


	dd (DM_tam - (PA_fimD-PA_baseD)) and 0xfffe 	; fecha heap

PA_fimD:
	rb DM_tam - (PA_fimD-PA_baseD)

PA_fimdd:


;=========================================================================================
;
; 		Pasta Publica (vazia) - LBA_base + 3 + (DM_tam/0x200)*2
;
;=========================================================================================
;é a unica que abre para usuários anônimos
;todo o seu conteúdo é publico a todos os usários inclusive internet

PP_baseD:
	dw assD			; assinatura para diretório
	dw 0
	dd 0
	dd itemDM2-baseHD		; aponta item na pasta anterior
	dd 0			; x

	dd PP_Inome2-PP_baseD-4
	dd PP_fimD-PP_baseD-4
	dd PP_fimdd-PP_Inome2+4

	dd 0			; pasta vazia
	dd 0
	dd 0

	dd (DM_tam/0x200)*2+2
	dd 0
	dd (DM_tam/0x200)		; ext

	dw 0xffff

	db 7
	db 13
	dd PP_Inome2-PP_baseD
align 4
	dd 14 or 1			; *
PP_Inome2:	db "Pasta Publica",0	; manter 14 espaços

	dd (DM_tam - (PP_fimD-PP_baseD)) and 0xfffe 	; fecha heap

PP_fimD:
	rb DM_tam - (PP_fimD-PP_baseD)

PP_fimdd:

;=========================================================================================
;
; 	Pasta Lixeira do Administrador (vazia) - LBA_base + (DM_tam/0x200)*3 +2
;
;=========================================================================================
;
LA_baseD:
	dw assD			; D_ass - assinatura para diretório
	dw 0			; res
	dd 0			; D_part - partição (dispositivo)
	dd 0			; D_anterior - pasta anterior
	dd 0			; x

	dd LA_Inome2-LA_baseD-4
	dd LA_fimD-LA_baseD-4	;
	dd LA_fimdd-LA_Inome2+4

	dd 0			; pasta vazia
	dd 0
	dd 0

	dd (DM_tam/0x200)*3+2	; base LBA
	dd 0
	dd (DM_tam/0x200)		; ext

	dw 0xffff

	db 7
	db 7
	dd LA_Inome2-LA_baseD
align 4
	dd 8 or 1			; *
LA_Inome2:	db "Lixeira",0	; manter 8 espaços

	dd (DM_tam - (LA_fimD-LA_baseD)) and 0xfffe 	; fecha heap
LA_fimD:
	rb DM_tam - (LA_fimD-LA_baseD)

LA_fimdd:

;=========================================================================================