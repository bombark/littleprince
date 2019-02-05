;-----------------------------------------------------------------------------------------
;
; LITTLE P=RINCE Operational System
; Copyright (C) 2008 to 207/7/2008 14:2011 - LP Development Team
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
; @Id: kernel.asm  - 2008-1-20, fb-toledo & cb-toledo
; This is the main compilable file that groups all others include files of the project
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------



; System constants, equate & variables definitions
include "const.inc"
include "variables.inc"


; BOOT
;include "Boot/Boot.inc"                ; simply boot
;include "Boot/BootFD_deb.inc"          ; step by step boot
include "Boot/BootFD.inc"		; default boot


org OS_BASE_F			; colocado em start.inc
sistema_ini:


; Start in real mode and change to protected paging mode
;include "Devices/DiskDetect_16.inc"	; all #16 code files must go first
include "Boot/start.inc"		;
include "Boot/initProc.inc"            ;


; (#32 Protected paging mode) ------------------------------------------------------------


; Tables (GDT,IDT)
include "gdt.inc"
include "idt.inc"

; View Memory
;include "Aplicativos/MM_g.inc"
include "Aplicativos/MMexx.inc"
include "Aplicativos/calculadora.asm"

; Memory manager routines
include "Memory/Ger_memory.inc"
include "Memory/Ger_Task.inc"
include "Memory/Conversao.inc"
include "Memory/String.inc"
include "Memory/Lista.inc"
include "Memory/Buffer_Circular.inc"

; Exception & debug routines
include "DEBUG/desa_cte32.cb"
;include "DEBUG/desa_prg32.cb"	; for video text mode only
;include "DEBUG/Excecoes32.inc"	; for video text mode only
include "DEBUG/Excecoes32G.inc"
include "DEBUG/desa_prg32G.cb"


; Timer
include "Devices/Timer.inc"
include "Devices/CMOS_dev.inc"


; Driver do Teclado
include "Devices/Keyboard_cte.inc"
include "Devices/Keyboard_dev.inc"


; Mouse driver
include "Devices/PS2Mouse_dev.inc"
include "Devices/Mouse_dev.inc"
;include "Devices/Mouse_cte.inc"


; Driver do Floppy (obsolet)


; HD driver
include "Devices/HD_info.inc"
include "Devices/HD_dev.inc"


; USB driver
;include "Devices/USB_dev.inc"


; CDrom driver
;include "Devices /CDROM_dev.inc"


; Video driver
include "Video/Vesa20.inc"
include "Video/Jpegview/Jpeglib.inc"
include "Video/VideoTest.inc"


; Graph System
;include "FONTS/fonte1.inc"
include "FONTS/f_bmp7x9m.inc"			; geração de fonte para gravação
include "Video/GraphSystem.inc"
include "Video/TextSystem.inc"
include "Video/FontSystem.inc"
include "Video/load_fonts.inc"
include "Video/Object/Object_System.inc"
include "Video/GuiSystem2.inc"
include "Video/RunEvento.inc"


; File System
include "FS/FS_Constant.inc"
include "FS/FS_ReadWrite.inc"			; read/write routines
;include "FS/FS_ReadWriteDev.inc"		; read/write routines (obsolet)
include "Devices/DevicesTable.inc"		; build device table
include "FS/LPFS_group.inc"			; LPFS group
include "FS/FAT_group.inc"			; FAT group
include "FS/FAT_testes.inc"
include "FS/FileManager.inc"
include "FS/LPFStest.inc"


; Syscall
include "Syscall/memory.inc"
include "Syscall/filesystem2.inc"
include "Syscall/window.inc"
include "Video/Object/Obj_Interface.inc"
include "Syscall/draw.inc"
;include "Syscall/sound.inc"
;include "Syscall/ethernet.inc"
;include "Syscall/math.inc"
;include "Syscall/draw3d.inc"
include "Syscall/keyboard.inc"
include "Syscall/syscall.inc"		; Instrucoes Syscall/Sysenter


;rb 0x1900		; Espaco utilizado pelo Virtual Box (0xe600 a 0x10000)

include "Aplicativos/Explorer.asm"
;include "aplicativos/Memoria.inc"
include "Aplicativos/Teste.inc"

sistema_end:

db 'fim'

align 0x200	;(don't remove)
sistema_fim: db 0 	;(don't remove)


