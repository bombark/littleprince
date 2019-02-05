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
; @Id: DiskDetect_16.inc  - 2006-6-20, fb-toledo & cb-toledo
; Build table of detected disk units (FD & HD)
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------

TabelaPart	equ 0x800
base_CDD		equ 0x05c0

; DEFINIÇÕES da palavra de comando (para as unidades A e B)(variaveis)
; base_FDD only for 1,44MB, 3,5" floppy unit


use16
baseFDD		equ 0x500 	;(see OS_variables.inc)

DMA_com	equ 0	; comando para DMA
FDC_com	equ 1	; comando para a controladora FDC
FDu_c	equ 2	; cabeça + unidade
FDtrilha	equ 3	; trilha solicitada
FDcab	equ 4	; cabeça solicitada
FDseti	equ 5	; setor inicial solicitado
FDbps	equ 6	; bytes por setor       -ff
FDspt	equ 7	; setores por trilha    -12
FDp1	equ 8	; comprimento da lacuna -1B
FDp2	equ 9	; comprimento dos dados -02
FDST3	equ 0xa	; status sense
FDST0	equ 0x0b	; status A da controladora
FDST1	equ 0x0c	; status B da controladora
FDST2	equ 0x0d	; status C da controladora
FDts	equ 0x0e  ; próxima trilha
FDcs	equ 0x0f	; proxima cabeça
FDss	equ 0x10  ; proximo setor
FDtd	equ 0x11 	; tipo do proximo setor
FDqTrilha	equ 0x12	; q. trilhas....(sempre tem 80)...marca unidade como presente
FDerro	equ 0x13	; codigo de erro da unidade
FDcodID	equ 0x14	; código para decodificação (até si+19); 2 dd - inicializar na leitura
FDNSerie	equ 0x1c	; número de série do disco (ler do disco)
		; extensao do arquivo

	; total 2x1f


baseHDD		equ 0x540		;(see OS_variables.inc)

;	si+0     dd- eax- lba3, lba2, lba1, lba0	(setor, trilha, cabeça)
;	si+4     dw- X
;	si+6     db- flags de controle
;	si+7     db- ATAFeatures
;	si+8     dd- edi- end. mem
;	si+c     dw- si- seletor para es:...para definir o segmento
;	si+e     dw- tarefa solicitante...para definir a tarefa
;	si+10    dd- cx- quant. setores
;	si+14    dw- q. trilhas da unidade ------------cte-----------------------------
;	si+16    dw- q.setores/q. cabeças  ------------cte-----------------------------
;	si+18    dd- q. total de setores da unidade ---cte-----------------------------
;	si+1c    db- modo de endereçamento 1110 0000-flags de controle
;	si+1d    db- codigo de erro da unidade
;	si+1e    db- unidade
;	si+1f    db- serviço


; (si+6)
; #7[si+6]=1 ->
; #6[si+6]=1 ->
; #5[si+6]=1 -> x
; #4	   -> x
; #3[si+6]=1 -> x
; #2[si+6]=1 -> x
; #1[si+6]=1 -> x
; #0[si+6]=1 -> x
;
;

; variables: (defined in OS_variables.inc))

q_unidIDE		equ 0x04ff	; gets all IDE units in the system
intHDrec		equ 0x04ff	; for INT 14 e 15 use: #7-int15, #6-int14,

;	#0,1 = 	quantidade de unidades até 2
; 	#2,3,4,5= livres
;	#6,7 = 	1-> int14 e int15 received

; important
; The task descriptor save in this table allows IDE interruption retake the task (not used)
; The segment descriptor save allows multi segment operation
;
;
;-----------------------------------------------------------------------------------------

; search for disk units (fd/hd) and build its tables in real mode

align 2
busca_DiscosMR:

;	mov ah,8
;	xor dl,dl		; ah= 8, dl= un.
;	int 0x13
;	jb .fim		; dl= unit quantity, dh= heads, cl= sectors, ch= tracks
;	mov si, baseFDD 	;
;	xor dh,dh
;	mov bp,dx
;	xor dl,dl
;	mov di, TabelaPart
;.a:	mov ah,8
;	push di
;	int 0x13		; (changes edi)
;	pop di
;	jb .fim
;	mov al,cl
;
;; parameters in table for units
;	and al,0x3f
;	mov [si+07], al			; save sectors by tracks, (allways 18)
;	mov byte [si+0x12],0x80		; save tracks (allways 80,- 0x4f)
;	mov dword [si+0x6],0xff1b1202		; parameters for 1.44MB/3,5"
;	add si, 0x20			; memory base for other unit
;
;; parameters in table for devices
;	mov dword [di+8], 2		; position for main directory (DM)
;	mov word [di+12], 4		; extent of DM
;	mov [di+14], dl		; unit
;	mov byte [di+15], 0x11	; partition type (C5) + unit type (floppy disk)
;	add di, ext_part
;	mov dl, 1
;	dec bp
;	jnz .a			; repeat if there are two units
;
;; if unit B: it is not defined, copy data from unit A
;	cmp di, TabelaPart + ext_part*2
;	jae .d
;	mov eax, [di - ext_part]			;bochs don't work...?????
;	mov [di], eax
;	mov eax, [di - ext_part + 4]
;	mov [di+4], eax
;	mov eax, [di - ext_part + 8]
;	mov [di+0x8], eax
;	mov eax, [di - ext_part + 0xc]
;	mov [di+0xc], eax

; hd seek
.d:	mov bp,4			; 4 unit by hipothesys
	mov dl,0x80
	mov bh, dl
	mov ah,0x8
	int 0x13
	xor dh,dh
	mov [q_unidIDE], dl		; unit quantity

	mov ebp,edx		; dl= number of units
	mov dl,0x80
	mov si,baseHDD		; base of IDE unit tables

.c:	mov ah,0x8		; SERVICE 8, int 13
	int 0x13
	jb .fim
	mov [si+0x1e], bh
	and dh, 0x0f
	mov [si+0x16],dh		; heads quantity
	mov al, bh		; unit
	and al, 1
	ror al, 4
	or al, 0xa0 		;
	or [si+0x1c],al		; disc 0 or 1 (for 80/81 or 82/83)
	mov al,cl
	and al,0x3f
	mov [si+0x17], al		; sectores by track
	and cl,0xc0
	mov al,ch
	xor ch,ch
	shl ecx,2
	mov cl,al
	mov [si+0x14],cx		; track quantity
	cmp dh, 0xf
	jb .b			; CHS units doesn't need total sectores in the unit
	mov al,dh
	mul cx
	mov cl, [si+0x17]		; sectores by track
	and cx,0x3f
	mul cx
	mov[si+0x18],ax		; total sectores access by LBA mode address
	mov[si+0x1a],dx		; sectores 512 bytes
	or byte [si+0x1c],0x40	; enable LBA address mode
.b:	add si,0x20
	inc bh
	mov dl,bh
	dec bp
	jnz .c			; repeat if there are other units

.fim:	ret


; obs: 	executed in real mode
;
; CHS units -> 0xa0 ou 0Xb0 in (baseHDD+si+0x1c), LBA units -> 0xe0 or 0xf0
; This routine can be sustituted by HD_info routine for protected mode



;-----------------------------------------------------------------------------------------