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
; @Id: gdt.inc  - 2008-1-20, fb-toledo
; Global descriptors table definitions
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------

align 4
GDT_Table:

; para apontar a gdt subtrair 8 a GDT_Table

; segmentos de codigo
segCode = 0x8
        dw 0xffff
	dw 0
	db 0
	db 10011010b		;0x9a
	db 0xcf
	db 0x0

; segmento de dados
segData = 0x10
        dw 0xffff
	dw 0x0
	db 0
	db 10010010b		;0x92
	db 0xcf
	db 0x0

segPilha	= 0x18
	dw 0xffff-0x1000 		; a pilha vai de bfff até ffff.(a0000-9bfff)
	dw 0x0000
	db 0x09			; base: 90000 (fixa)
	db 0x96 			; dados, permite leitura e escrita, n0
	db 0x0			; granularidade 1K
	db 0x0

segTSS_OS	= 0x20
	dw 103			; extensao:  ? K
	dw TSS_OS and 0xffff
	db TSS_OS shr 0x10 and 0xff			; base: 30000
	db 0x89 		 	; presente, dpl=00,
	db 0x0			; granularidade 1K
	db (TSS_OS or OS_BASE) shr 0x18

segTSS_AP1= 0x28
	dw 103			; extensao:  ? K
	dw TSS_AP1 and 0xffff
	db TSS_AP1 shr 0x10 and 0xff			; base: 30000
	db 0x89 		 	; presente, dpl=00,
	db 0x0			; granularidade 1K
	db (TSS_AP1 or OS_BASE) shr 0x18

UsegCode = 0x30
        dw 0xffff
	dw 0
	db 0
	db 11111010b		;0xfa
	db 0xcf
	db 0

; segmento de dados
UsegData = 0x38
        dw 0xffff
	dw 0x0
	db 0
	db 11110010b		;0xf2
	db 0xcf
	db 0


end_GDT_Table: