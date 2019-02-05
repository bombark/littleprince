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
; @Id: DeviceTable.inc  - 2006-6-20, fb-toledo & cb-toledo
; Build partition table of devices (all storage units)
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------



; TABELA DE PARTICOES (definições):
; 0 - 4   = linear address of the start of partition
; 4 - 8   = particao size in sectores of 512 bytes
; 8 - 12  = master directory of the partition
; 12 - 14 = size in sectores with 512 bytes of the main partition
; 14 	= storage unit
; 15 	= type (C4, FAT, ...)
;	0xf0 - partition type: 0-not formated, 1-LPFS, 2- ?, 3-FAT, 4-other
;	0x0f - driver type (floppy= 1, hd=2, cdrom=3, pendrive=4)
; 16 - 20 = Point to the master directorio (DM) memory position
;
;-----------------------------------------------------------------------------------------
;
; TabelaPart:
;  Part_ini	equ 0	; Inicio da particao		4 bytes
;  Part_ext	equ 4	; Extensao da particao		4 bytes
;  Part_lbaDM	equ 8	; LBA do diretorio mestre		4 bytes
;  Part_extDM	equ 0xc	; DM size				2 bytes
;  Part_unidade	equ 0xe	; Unit number (01,80,81,...)		1 byte
;  Part_tipo	equ 0xf	; Tipo (C4, FAT,...)		1 byte
;  Part_DMpos	equ 0x10	; Point to DM folder in memory	4 bytes
;  Part_Padmin	equ 0x14	; point to admin folder base in memory	4
;  Part_Ppublica	equ 0x18	; point to public folder base in memory	4
;  Part_res	equ 0x1c	; free				4
;
;-----------------------------------------------------------------------------------------
use32	; Don't remove


; IN:
;	null
; OUT:
;	null
align 4
DetectaPartiçao32:		; OK
;int 1
	xor ebx, ebx
	mov edx, TabelaPart + 2*0x20 	; edx = base da tabela
	mov ecx, 1	; 1 setor		; esi= base boot no buffer
	mov bh, 0x80

.a:	mov eax, 0
	xor esi,esi
	mov bl, 1
	mov edi, BufferTemp
;int 1
	push ebx
	call HDD_Enter				; busca DM
	pop ebx
;int 1
	jb .erro
; busca a base e extensão da partição
.b:	cmp byte [esi+BufferTemp+0x1be+4 ], 0xc4	; partition type verify
	jne .continue1				; ...can merge others searchs

	mov eax, [esi+BufferTemp+0x1be+12 ]		; partition size
	mov [edx+4], eax
	mov eax, [esi+BufferTemp+0x1be+Part_lbaDM ]	; partition LBA start
	mov [edx], eax

; buscar a base e a extensão do diretório mestre
	; edi - OK 	BufferTemp+0x200
	; bh  - OK
	; ecx - OK	1
	mov bl, 1
;int 1
	push ebx
	call HDD_Enter		; read boot setor
	pop ebx
;int 1
	jb .erro
	cmp word [BufferTemp2+0x1fe], 0xaa55	; verify if is a boot
	jne .continue

; define parâmetros
	mov eax, [BufferTemp2+0x1c ]	; DM base (pos_Dir)
	mov [edx+Part_lbaDM ], eax
	mov ax, [BufferTemp2+0x1c+4 ]	; DM size (extDirSet)
	mov [edx+12], ax
	mov [edx+14], bh		; fixed unit number

	cmp dword [BufferTemp2+0x50 ],0xba057502 ; if boot changes, look here -----------
	jz .p

; marca unidade não formatada
	mov byte [edx+15], 0x02		; fixa o tipo (HD) nao formatado
	jmp .nova

.p:	mov byte [edx+15], 0x12		; fixa o tipo (HD) C4

; nova partição
.nova:	add edx, 0x20

.continue:
	add esi, 0x10
	cmp esi, 0x40
	jb .b

; nova unidade ou erro
.erro:

	inc bh
	cmp bh, 0x84
	jb .a
	ret

; 1. busca a tabela de partições no setor 1 da trilha 0;
; 2. busca o setor de boot com base nos setores anteriores da partição
; 3. VIRTUAL BOX NAO ACEITA SE A UNIDADE NAO EXISTIR
; 4.

;-----------------------------------------------------------------------------------------

; posiciona unidades FAT16 / FAT32 (tipo 3)

.continue1:
;int 1
	cmp byte [esi+BufferTemp+0x1be+4], 0x07	; partition type verify
	je .ntfs				;
	cmp byte [esi+BufferTemp+0x1be+4], 0x80	; partition type verify
	je .ntfs				; provisorio
	cmp byte [esi+BufferTemp+0x1be+4], 0x82	; partition type verify
	je .ntfs				; provisório
	cmp byte [esi+BufferTemp+0x1be+4], 0x00		; partition type verify
	je .continue				; ...can merge others searchs

.Fat32:	mov eax, [esi+BufferTemp+0x1be+12 ]		; partition size
	mov [edx+4], eax
	mov eax, [esi+BufferTemp+0x1be+Part_lbaDM ]	; partition LBA start
	mov [edx], eax

; buscar a base e a extensão do diretório mestre
	mov bl, 1
;int 1
	call HDD_Enter		; read boot setor
;int 1
	jb .erro
	cmp word [BufferTemp2+0x1fe], 0xaa55	; verify if is a boot
	jne .continue

	cmp dword [BufferTemp2+0x37 ],dword 0x36315441	; confirma FAT16
	jz .f16
	cmp dword [BufferTemp2+0x53 ],dword 0x32335441	; confirma FAT32
	jnz .nf
	mov byte [edx+15], 0x32		; fixa o tipo (HD) FAT32: 32
				; continua
; define parâmetros (diretorio)
	mov eax, [BufferTemp2+0x24 ]		; setores por fat
.par:	mov cl, [BufferTemp2+0x10 ]		; numero de fats
	shr cl, 2
	shl eax, cl			; x2
	movzx ecx, word [BufferTemp2+0xe ]	; setores reservados
	add eax,ecx
	mov [edx+Part_lbaDM], eax		; base diretorio
	movzx ax, byte [BufferTemp2+0xd ]	; Sectors_per_cluster
	mov [edx+Part_extDM], ax		; extensao diretorio (1 cluster)
	mov [edx+14], bh

; neste ponto bh = unidade (0x80, 0x81.. 0x83)
; inicializa fat (equ. Fat_Ini)-----------------------------------------------------------
	push esi
	push edx
	sub edx, TabelaPart
	shr edx, 0x5
	mov bl, dl
	inc bl
	mov eax, 0x400		;

	call aloca_HB		; aloca bloco no heap
;int 1
	mov edi, esi
	pop edx
	pop esi
	jb .nf			; provisorio -..............deve aumentar o heap
; buscar o setor de boot (2 setores)- bl= partição
	mov eax, 0		;
	mov cl, 2
	call ReadSeg0		; altera eax, ebx
	jb .nf			; provisorio -..............unidade com defeito no setor
	mov [edx+Part_DMpos], edi	; salva a base do bloco de memoria boot da unidade
;-----------------------------------------------------------------------------------------
	jmp .nova


.nf:	mov byte [edx+15], 0x02		; fixa o tipo (HD) nao formatado
	jmp .nova


.f16:	mov byte [edx+15], 0x22		; fixa o tipo (HD) FAT16: 22
	mov eax, [BufferTemp2+0x0d]		; setores por fat
	or eax, eax
	jnz .par
	mov eax, [BufferTemp2+0x20]		; setores por fat
	jmp .par				; continua


.ntfs:	jmp .continue			; a fazer


.continue2:				; novos FS


;-----------------------------------------------------------------------------------------


; FAT32_BootBase:
;	JMP instruction						;         0
;	OEM		equ FAT32_BootBase+3	;char[8]	"OEM"	;	3
;
; section	"BIOS Parameter Block"
;	Bytes_per_sector	equ FAT32_BootBase+0xb			; dw	b
;	Sectors_per_cluster	equ FAT32_BootBase+0xd			; db	d
;	Reserved_sectors	equ FAT32_BootBase+0xe			; dw	e
;	Number_of_FATs	equ FAT32_BootBase+0x10			; db
;	Root_entries 	equ FAT32_BootBase+0x11 	;(unused)		; dw
;	Sectors 		equ FAT32_BootBase+0x13			; dw
;	Media_descriptor	equ FAT32_BootBase+0x15	; 		; db
;	Sectors_per_FAT 	equ FAT32_BootBase+0x16	; (small vol.)      ; dw
;	Sectors_per_track	equ FAT32_BootBase+0x18			; dw
;	Heads		equ FAT32_BootBase+0x1a 			; dw
;	Hidden_sectors	equ FAT32_BootBase+0x1c			; dd
;	Sectors 		equ FAT32_BootBase+0x20	; (on large volumes); dd
;
; section	"FAT32 Section"
;	Sectors_per_FAT32	equ FAT32_BootBase+0x24			; dd
;	flags_		equ FAT32_BootBase+0x28			; dw
;	Version		equ FAT32_BootBase+0x2a			; dw
;	Root_dir_1cluster	equ FAT32_BootBase+0x2c			; dd
;	FSInfo_sector	equ FAT32_BootBase+0x30			; dw
;	Backup_boot_sector	equ FAT32_BootBase+0x32			; dw
;	hex_12 		equ FAT32_BootBase+0x34	; (read-only Reserved)
;
;	BIOS_drive	equ FAT32_BootBase+0x40	; BIOS drive (HD=8x); db
;	readonly 		equ FAT32_BootBase+0x41 	; (Unused)	; db
;	sign		equ FAT32_BootBase+0x42		; boot signature    ; (29h)
;	volume_d		equ FAT32_BootBase+0x43	;  dec serial number; dd
;	volume_h		equ FAT32_BootBase+0x43	; hex serial number ; dd
;	label		equ FAT32_BootBase+0x47	; Volume label	; rb 11
;	label2		equ FAT32_BootBase+0x52	; File system	; rb



; FAT12/FAT16 formatted logical drive.

;	JMP instruction							0
;	char[8]	"OEM"

; section	"BIOS Parameter Block"
;	Bytes per sector	equ FAT32_BootBase+0xb			; dw	b
;	Sectors_per_cluster	equ FAT32_BootBase+0xd			; db	d
;	Reserved_sectors	equ FAT32_BootBase+0xe			; dw	e
;	Number_of_FATs	equ FAT32_BootBase+0x10			; db	10
;	Root_entries	equ FAT32_BootBase+0x11			; dw	11
;	Sectors		equ FAT32_BootBase+0x13	; (under 32 MB)	; dw	13
;	Media_descriptor	equ FAT32_BootBase+0x15	; (hex)		; db	15
;	Sectors_per_FAT	equ FAT32_BootBase+0x16	;		; dw	16
;	Sectors_per_track	equ FAT32_BootBase+0x18			; dw
;	Heads		equ FAT32_BootBase+0x1a			; dw
;	Hidden_sectors	equ FAT32_BootBase+0x1c			; dd
;	Sectors 		equ FAT32_BootBase+0x20	; (over 32 MB)	; dd
;
;
;	BIOS_drive	equ FAT32_BootBase+0x24	; hex, HD=8x	; db
;	readonly 		equ FAT32_BootBase+0x25	; (Unused)	; db
;	sign	 	equ FAT32_BootBase+0x26	; (29h)		; db
;	volume_d		equ FAT32_BootBase+0x27	; (decimal)	; dd
;	volume_h		equ FAT32_BootBase+0x27	; (hex)		; dd
;	label		equ FAT32_BootBase+0x31	; 		; dd
;	label2		equ FAT32_BootBase+0xb	; 		; dd


;-----------------------------------------------------------------------------------------													; ...can merge others searchs