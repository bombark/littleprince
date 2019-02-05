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
; @Id: FS_Open.inc - 2008-02-22 - fb-toledo
; For operation with files
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------


;======================== C O M E N T A R I O S ==========================================



;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

; Buffer do Diretorio
   LPDir_Lba	equ 0	; dd -
   LPDir_LbaBase	equ 4	; dd -
   LPDirAnt_Lba	equ 8	; dd -
   LPDir_LbaExt	equ 0xc	; dw -
   LPDirAnt_LbaExt	equ 0xe	; dw -
   LPDir_LbaExtBase	equ 0x10 	; dw -
   LPDir_Status	equ 0x12	; db - Status do buffer do diretorio
   LPDir_Rs	equ 0x13	; db - free

LPDir_Start	equ 0x14
LPDir_Size	equ 0x214


;-----------------------------------------------------------------------------------------


;======================== F U C T I O N   1 ==============================================
; LPFS Open
; IN:
;	nothing
; OUT:
;	nothing

LPFS_Open:
	push eax
	mov eax, LPDir_Size
	call aloca_HB
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U C T I O N   1 ==============================================
; LPFS Open
; IN:
;	edx= nome do arquivo
;	esi= buffer
; OUT:
;	nothing

LPFS_GetFileInfo:

	ret

;-----------------------------------------------------------------------------------------