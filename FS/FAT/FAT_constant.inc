;-----------------------------------------------------------------------------------------

; FAT Constant

;-----------------------------------------------------------------------------------------


;======================== D E F I N I C O E S ============================================

	FATreserved         equ 0x0FFFFFF6	; Reservado
	FATbad              equ 0x0FFFFFF7	; Cluster estragado
	FATend              equ 0x0FFFFFF8	; 
	FATmask             equ 0x0FFFFFFF	;
	FATeof		equ 0x0FFFFFFF	; Fim de arquivo

;-----------------------------------------------------------------------------------------


;======================== B O O T ========================================================
		
	OEM		equ 3			;char[8]	"OEM"
	Bytes_per_sector	equ 0xb			; dw
	Sectors_per_cluster	equ 0xd			; db
	Reserved_sectors	equ 0xe			; dw
	Number_of_FATs	equ 0x10  		; db
	Root_entries 	equ 0x11  ; (unused)	; dw
	Sectors 		equ 0x13  		; dw
	Media_descriptor	equ 0x15  ; Media descriptor  ; db
	Sectors_per_FAT 	equ 0x16  ; (small vol.)      ; dw  = 0 on FAT32
	Sectors_per_track	equ 0x18  		; dw
	Heads		equ 0x1a  		; dw
	Hidden_sectors	equ 0x1c  		; dd
	Sectors 		equ 0x20  ; (on large volumes); dd

  ; section	"FAT32 Section"
	Sectors_per_FAT32	equ 0x24  		; dd
	flags_		equ 0x28  		; dw
	Version		equ 0x2a  		; dw
	Root_dir_1cluster	equ 0x2c  		; dd
	FSInfo_sector	equ 0x30  		; dw
	Backup_boot_sector	equ 0x32  		; dw
	hex_12 		equ 0x34  ; (read-only Reserved)

	drive		equ 0x40  ; BIOS drive (HD=8x); db
	readonly 		equ 0x41  ; (Unused)	; db
	sign		equ 0x42  ; boot signature    ; (29h)
	volume_d		equ 0x43  ; dec serial number ; dd

	volume_h		equ 0x43  ; hex serial number ; dd
	label		equ 0x47  ; Volume label	; rb 11
	label2		equ 0x52	; File system	; rb
	
;-----------------------------------------------------------------------------------------


;======================== F O L D E R ====================================================

  ; Descritor de um item no diretorio ============
	FileName 		equ 0	; rb 8 - (blank-padded)" if #0= 0xE5....erased file
	FileExtension 	equ 8	; rb 3 - (blank-padded)"
	FileAttr 		equ 0xb	; db   = ( - -a-dir-vol-s-h-r)"
	EntryReserved	equ 0x0c
	CrtimeR		equ 0x0d	; db - Cr. time refinement in 10-ms units"
	CrDateTime	equ 0x0e	; dd - Creation date & time
	AccessDate	equ 0x10	; dw
	Entry_		equ 0x12	; dw
	FAT32_HWC 	equ 0x12	; dd - (FAT 32) High word of cluster # (0x14)
	FAT32_HC		equ 0x14  ; dw
	UpTime		equ 0x16	; dw - "Access time"
	UpDate		equ 0x18	; dw - "Access date"
	FAT16_C		equ 0x1a	; dw - 16-bit cluster #
	Dir_FileSize 	equ 0x1c	; dd - (zero for a directory)

  ;-----------------------------------------------

;-----------------------------------------------------------------------------------------


;======================== B L O C O   D E   D A D O S ====================================
	
	BD_ASS		equ 0x0		; dd - Assinatura
	BD_FAT_Base	equ 0x4		; dd - Lba inicial da fat
	BD_FAT_Lba	equ 0x8  		; dd - Lba atual da fat
	BD_FAT_SPF	equ 0x0c 		; dd - Setores por fat
	BD_FAT_SPC	equ 0x10 		; db - Setores por cluster
	BD_FAT_QTDE	equ 0x11		; db - Quantidade de tabelas fats
	BD_FAT_Type	equ 0x12		; db - Tipo da fat (16/32) (0=16 e 1=32)
	BD_FAT_Changed	equ 0x13 		; db - Marca se houve escrita na fat
	
	BD_HD_Lba		equ 0x14		; dd - Guarda o lba base da area de armazenamento no disco
	BD_HD_LastCluster	equ 0x18		; dd - Ultimo cluster do HD
	
	BD_Root_Cluster	equ 0x1c		; dd - Cluster do root
	BD_Folder_Cluster	equ 0x20		; dd - Cluster do diretorio
	BD_Folder_Lba	equ 0x24		; dd - Lba atual do diretorio
	BD_Folder_Rlba	equ 0x28		; db - Lba atual relativo (0 a BD_FAT_SPC)

	
	BD_Buffer		equ 0x50	       	; rb 0x100 - Buffer para normalizar o nome	
	
	BD_Fat		equ 0x400      	; fat memory base
	BD_Folder		equ 0x600      	; root memory base
	
bd_ass equ 'BFAT'	
	
;-----------------------------------------------------------------------------------------