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
; @Id: ReadWriteDev.inc  - 2008-1-20, fb-toledo
; Partitition read and write routines
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------
;
; Info: O tipo de dispositivo é buscado na tabela de partições e com ele é
; buscado o driver correspondente.
;
;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

TabelaPart	equ OS_BASE + 0x800		; see variables.inc

align 4
Device_Table:
	dd R_null		; FLOPPY		; leitura/escrita em disquete
	dd HDD_Enter	; HD		; leitura/escrita em hd
	dd R_null		; CDROM		; leitura/escrita em cdrom
	dd R_null		; PENDRIVE	; leitura/escrita em pendrive
; maximum of 0xf devices

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N   ==============================================
; read ecx sectors
; IN:								OK
;	eax: lba
;	bl: partition (0..32)
;	ecx: block extension
;      	edi: buffer
; OUT:
;	cy = 0 OK -> ebp = 0, eax= eax + setores anteriores da partiçao
;	cy = 1 error on ebp
align 4
ReadSeg0:
	push ebx
	push esi
	push edi					; mantem edi
	or ecx, ecx
	jz .fim
	or bl, bl
	jz .fim
	and ebx, 0x1f
	dec ebx
	shl ebx, 5			; ebx=ebx*0x20
	lea esi, [ebx+TabelaPart]		; esi= base da tabela de partiçoes
	add eax, [TabelaPart+ebx+Part_ini]
	movzx ebp, byte [TabelaPart+ebx+Part_tipo]	; tipo de unidade
	mov bh, [TabelaPart+ebx+0xe]		; unidade
	mov bl, 3				; leitura de segmentos
	and ebp, 0x0f
	shl ebp, 2
	call dword [Device_Table + ebp - 4]
.fim:	pop edi
	pop esi
	pop ebx
R_null:	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N   ==============================================
; Write ecx sectors
; IN:
;	eax: lba
;	ecx: block extension
;	bl: partition (0..32)
;         edi: buffer (point to data memory base)
; OUT:
;	cy = 0 OK -> ebp = 0, eax= eax + setores anteriores
;	cy = 1 error on ebp
align 4
WriteSeg0:

	or ecx, ecx
	jnz .a
	inc ecx
.a:	push ebx
	push esi
	push edi
	or bl, bl
	jz .fim
	and ebx, 0x1f
	dec ebx
	shl ebx, 5
	lea esi, [ebx+TabelaPart ]
	add eax, [TabelaPart+ebx+Part_ini ]	; setores anteriores
	movzx ebp, byte [TabelaPart+ebx+0xf ]
	and ebp, 0x0f
	mov bh, [TabelaPart+ebx+0xe ]		; unidade
	mov bl, 4				; serviço de gravaçao de segmento
	shl ebp, 2
	call dword [Device_Table + ebp - 4 ]
.fim:	pop edi
	pop esi
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N   ==============================================
; Lê um setor
; IN:
;	eax: lba
;	bl: partition (0..32)
;         edi: buffer (point to data memory base)
; OUT:
;	cy = 0 OK -> ebp = 0, eax= eax+setores anteriores
;	cy = 1 error on ebp
align 4
ReadSector0:

	pushad
	or bl, bl
	jz .fim
	and ebx, 0x1f
	dec ebx
	shl ebx, 5
	lea esi, [ebx+TabelaPart ]		; esi= base da tabela de partiçoes
	add eax, [TabelaPart+ebx+Part_ini ]
	movzx ebp, byte [TabelaPart+ebx+0xf ]	; tipo de unidade
	mov bh, [TabelaPart+ebx+0xe ]		; unidade
	mov bl, 1				; serviço de leitura de setor
	and ebp, 0x0f
	shl ebp, 2
	call dword [Device_Table + ebp - 4 ]
.fim:	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N   ==============================================
; Escreve um setor
; IN:
;	eax: lba
;	bl: partition (0..32)
;         edi: buffer (point to data memory base)
; OUT:
;	cy = 0 OK -> ebp = 0, eax= eax+setores anteriores
;	cy = 1 error on ebp
align 4
WriteSector0:

	pushad
	or bl, bl
	jz .fim
	and ebx, 0x1f
	dec ebx
	shl ebx, 5			;
	lea esi, [ebx+TabelaPart ]
	add eax, [TabelaPart+ebx+Part_ini ]	; setores anteriores
	movzx ebp, byte [TabelaPart+ebx+0xf ]
	mov bh, [TabelaPart+ebx+0xe]		; unidade
	mov bl, 2				; serviço de gravaçao de segmento
	and ebp, 0x0f
	shl ebp, 2
	call dword [Device_Table + ebp - 4 ]
.fim:	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N   ==============================================
; The same of ReadSeg0 but with EDI update
; IN:
;	eax: lba
;	ecx: block extension
;	bl: partition (0..32)
;         edi: buffer (point to data memory base)
; OUT:
;	cy = 0 OK -> edi updated, ebp = 0, eax= eax+setores anteriores
;	cy = 1 error on ebp
align 4
ReadSegEdiUp:

	push ebx
	push esi
	or bl, bl
	jz .fim
	and ebx, 0x1f
	dec ebx
	shl ebx, 5			;
	lea esi, [ebx+TabelaPart ]		; esi= base da tabela de partiçoes
	add eax, [TabelaPart+ebx+Part_ini ]
	movzx ebp, byte [TabelaPart+ebx+0xf ]	; tipo de unidade
	mov bh, [TabelaPart+ebx+0xe ]		; unidade
	mov bl, 3				; leitura de segmentos
	and ebp, 0x0f			; quantidade de serviços
	shl ebp, 2
	call dword [Device_Table + ebp - 4 ]
.fim:	pop esi
	pop ebx
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N   ==============================================
; The same of WriteSeg0 but with EDI update
; IN:
;	eax: lba
;	ecx: block extension
;	bl: partition (0..32)
;         edi: buffer (point to data memory base)
; OUT:
;	cy = 0 OK -> edi updated, ebp = 0, eax= eax+setores anteriores
;	cy = 1 error on ebp
align 4
WriteSegEdiUp:

	push ebx
	push esi
	or bl, bl
	jz .fim
	and ebx, 0x1f
	dec ebx
	shl ebx, 5			; ebx= ebx*0x20
	lea esi, [ebx+TabelaPart ]
	add eax, [TabelaPart+ebx+Part_ini ]	; setores anteriores
	movzx ebp, byte [TabelaPart+ebx+0xf ]
	mov bh, [TabelaPart+ebx+0xe]		; unidade
	mov bl, 4				; gravaçao de segmento
	and ebp, 0x0f
	shl ebp, 2
	call dword [Device_Table + ebp - 4 ]
.fim:	pop esi
	pop ebx
	ret


;-----------------------------------------------------------------------------------------




