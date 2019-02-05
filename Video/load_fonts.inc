;-----------------------------------------------------------------------------------------
;
; LITTLE PRINCE Operational System
; Copyright (C) 2008 to 2011 - LP Development Team
;
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
; @Id: load_fonts.inc - 2008-02-22 - fb-toledo
; load defalut font for video operation
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------

; Entrada:
; 	ax: TIPO
;	ebx: diretorio inicial
;	esi: string (diretorio/diretorio/arquivo.ext)
;	cx: string extension
; Saida:
;	ebx: o diretorio onde está o arquivo
;	esi: aponta o arquivo
;	edi:
; Altera:	eax

align 4
load_fonts:

	mov dword [CharTable], Font_Table	; opção fonte imbutida no kernel
	ret			;


	mov ax, 0x4	; tipo	; opção carrega fonte do hd
	mov ebx, [SystemFolderBase]
	mov cx, Nf-Ni	; ext nome
	mov esi, PName	; pont. nome
	call LoadFileInSysMemory		; carrega arquivo no heap
; validaçao do arquivo de fonte:

	cmp dword [edi], 'LP02'
	jnz .error
	add edi, [edi+0x4]
	mov [CharTable], edi
	ret
.error:	stc
	ret

Ni:
PName	db 'f_bmp7x9m'
Nf:

;-----------------------------------------------------------------------------------------


ExecuteMM:
	mov esi, arquivo
	call File_Execute
	ret

arquivo	db "MMexx", 0


;-----------------------------------------------------------------------------------------