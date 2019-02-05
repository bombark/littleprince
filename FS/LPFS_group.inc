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
; @Id: LPFS_group.inc - 2008-02-10 cb-toledo
; Constant definition for FAT file system
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------

; OK
include "/LPFS/FS_Constant.inc"
;include "/LPFS/FS_StartFolder.inc"		; Start folders model
include "/LPFS/FS_LoadStartFolders.inc"
include "/LPFS/FS_SystemInstall.inc"		; Install System routines
include "/LPFS/FS_Bootmodel.inc"		; boot model for system installations
;include "/LPFS/FS_FormatDisk.inc"		; format unit with LPFS

;
include "/LPFS/FS_EVproc.inc"			;
include "/LPFS/FS_DMproc.inc"			;
include "/LPFS/FS_Folderproc.inc"		;
include "/LPFS/FS_Fileproc.inc"		;
include "/LPFS/FS_Itemproc.inc"		;
;include "/LPFS/FS_FileAsciiZ.inc"		; manipula arquivos e pasta nominados por AsciiZ
include "/LPFS/LPFS_Services.inc"

		;