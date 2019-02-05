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
; @Id: Boot.inc - 2007-11-26, fb-toledo
; Simple boot for floppy disk
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------

;OS_BASE equ 0x2000

use16
boot_inicio:

; Carrega os setores do sistema
	xor dx, dx
          mov ah, 2
          mov al, ((((sistema_fim-sistema_ini)/512)+1) and 0xfffe)
          mov bx, OS_BASE			; ver OS_const.inc
          mov es, dx
          mov ds, dx
          mov cx, 2
          int 0x13

          mov dx, 0x3f2			; motor off
          in al, 0
          out dx, al


; Passa para o sistema
          jmp 0:inicio16


boot_fim:

rb 0x1fe-boot_fim
dw 0xaa55


;-----------------------------------------------------------------------------------------

; For use this boot it is need the follow kernel.asm structure

;; System variables
;include "const.inc"
;include "variaveis.inc"
;
;; BOOT
;include "Boot/boot.inc"
;
;
;org $+OS_BASE
;
;; Tables (GDT,IDT)
;include "gdt.inc"
;include "idt.inc"
;
;
;; Start
;include "Boot/inicio.inc"
; ...continue the includes

use32
