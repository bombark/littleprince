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
; @Id: idt.inc  - 2008-1-20, fb-toledo
; Interruption table definitions
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------

align 4
IDT_Table:
; exceções

; int 0 - divisão por zero
	p_int0		dw DPexc00 and 0xffff
	rseg_i0		dw segCode	; code_seg substituido por s_exe16
	byte_i0		db 0
	desc_i0		db 0x8f 		; trap #32 bits... terminar com iretd
	ext_i0		dw DPexc00 shr 0x10

; int 1 - depuração passo a passo
	p_int1		dw DPint1 and 0xffff 	; entra com cor verde para as instr
	rseg_i1		dw segCode 	; code_seg substituido por s_exe16
	byte_i1		db 0
	desc_i1		db 0x8f 		; trap #16 bits... terminar com iret
	ext_i1		dw DPint1 shr 0x10
; int 2 - int não mascarável
	p_int2		dw r_iretd and 0xffff
	rseg_i2		dw segCode
	byte_i2		db 0
	desc_i2		db 0x8f ;
	ext_i2		dw r_iretd shr 0x10
; int 3 - breakpoint
	p_int3		dw DPexc03 and 0xffff		; int_01		;r_excDP
	rseg_i3		dw segCode
	byte_i3		db 0
	desc_i3		db 0x8f ;
	ext_i3		dw DPexc03 shr 0x10
; int 4 - overflow
	p_int4		dw DPexc04 and 0xffff
	rseg_i4		dw segCode
	byte_i4		db 0
	desc_i4		db 0x8f ;
	ext_i4		dw DPexc04 shr 0x10
; int 5 -
	p_int5		dw DPexc05 and 0xffff
	rseg_i5		dw segCode
	byte_i5		db 0
	desc_i5		db 0x8f ;
	ext_i5		dw DPexc05 shr 0x10

; int 6 -
	p_int6		dw DPexc06 and 0xffff
	rseg_i6		dw segCode
	byte_i6		db 0
	desc_i6		db 0x8f ;
	ext_i6		dw DPexc06 shr 0x10

; int 7 -
	p_int7		dw DPexc07 and 0xffff
	rseg_i7		dw segCode
	byte_i7		db 0
	desc_i7		db 0x8f ;
	ext_i7		dw DPexc07 shr 0x10

; int 8 -
	p_int8		dw DPexc08 and 0xffff
	rseg_i8		dw segCode
	byte_i8		db 0
	desc_i8		db 0x8f ;
	ext_i8		dw DPexc08 shr 0x10

; int 9 -
	p_int9		dw r_iretd and 0xffff
	rseg_i9		dw segCode
	byte_i9		db 0
	desc_i9		db 0x8f ;
	ext_i9		dw r_iretd shr 0x10

; int a -
	p_inta		dw DPexc0a and 0xffff
	rseg_ia		dw segCode
	byte_ia		db 0
	desc_ia		db 0x8f ;
	ext_ia		dw DPexc0a shr 0x10

; int b -
	p_intb		dw DPexc0b and 0xffff
	rseg_ib		dw segCode
	byte_ib		db 0
	desc_ib		db 0x8f ;
	ext_ib		dw DPexc0b shr 0x10

; int c -
	p_intc		dw DPexc0c and 0xffff
	rseg_ic		dw segCode
	byte_ic		db 0
	desc_ic		db 0x8f
	ext_ic		dw DPexc0c shr 0x10

; int d -
	p_intd		dw DPexc0d and 0xffff
	rseg_id		dw segCode
	byte_id		db 0
	desc_id		db 0x8f
	ext_id		dw DPexc0d shr 0x10		; o retorno deve ser iretd

; int e -
	p_inte		dw DPexc0e and 0xffff
	rseg_ie		dw segCode
	byte_ie		db 0
	desc_ie		db 0x8f ;
	ext_ie		dw DPexc0e shr 0x10

; int f -
	p_intf		dw r_iretd and 0xffff
	rseg_if		dw segCode
	byte_if		db 0
	desc_if		db 0x8f ;
	ext_if		dw r_iretd shr 0x10

; int 10 -
	p_int10		dw r_iretd and 0xffff
	rseg_i10		dw segCode
	byte_i10		db 0
	desc_i10		db 0x8f ;
	ext_i10   	dw r_iretd shr 0x10

; int 11 -
	p_int11		dw r_iretd and 0xffff
	rseg_i11		dw segCode
	byte_i11		db 0
	desc_i11		db 0x8f ;
	ext_i11		dw r_iretd shr 0x10

; int 12 -
	p_int12		dw r_iretd and 0xffff
	rseg_i12		dw segCode
	byte_i12		db 0
	desc_i12		db 0x8f ;
	ext_i12		dw r_iretd shr 0x10

; int 13 -
	p_int13		dw r_iretd and 0xffff
	rseg_i13		dw segCode
	byte_i13		db 0
	desc_i13		db 0x8f ;
	ext_i13		dw r_iretd shr 0x10

; int 14 -
	p_int14		dw r_iretd and 0xffff
	rseg_i14		dw segCode
	byte_i14		db 0
	desc_i14		db 0x8f ;
	ext_i14		dw r_iretd shr 0x10

; int 15 -
	p_int15		dw r_iretd and 0xffff
	rseg_i15		dw segCode
	byte_i15		db 0
	desc_i15		db 0x8f ;
	ext_i15		dw r_iretd shr 0x10

; int 16 -
	p_int16		dw r_iretd and 0xffff
	rseg_i16		dw segCode
	byte_i16		db 0
	desc_i16		db 0x8f ;
	ext_i16		dw r_iretd shr 0x10

; int 17 -
	p_int17		dw r_iretd and 0xffff
	rseg_i17		dw segCode
	byte_i17		db 0
	desc_i17		db 0x8f ;
	ext_i17		dw r_iretd shr 0x10

; int 18 -
	p_int18		dw r_iretd and 0xffff
	rseg_i18		dw segCode
	byte_i18		db 0
	desc_i18		db 0x8f ;
	ext_i18		dw r_iretd shr 0x10

; int 19 -
	p_int19		dw r_iretd and 0xffff
	rseg_i19		dw segCode
	byte_i19		db 0
	desc_i19		db 0x8f ;
	ext_i19		dw r_iretd shr 0x10

; int 1a -
	p_int1a		dw r_iretd and 0xffff
	rseg_i1a		dw segCode
	byte_i1a		db 0
	desc_i1a		db 0x8f ;
	ext_i1a		dw r_iretd shr 0x10

; int 1b -
	p_int1b		dw r_iretd and 0xffff
	rseg_i1b		dw segCode
	byte_i1b		db 0
	desc_i1b		db 0x8f ;
	ext_i1b		dw r_iretd shr 0x10

; int 1c -
	p_int1c		dw r_iretd and 0xffff
	rseg_i1c		dw segCode
	byte_i1c		db 0
	desc_i1c		db 0x8f ;
	ext_i1c		dw r_iretd shr 0x10

; int 1d -
	p_int1d		dw r_iretd and 0xffff
	rseg_i1d		dw segCode
	byte_i1d		db 0
	desc_i1d		db 0x8f ;
	ext_i1d		dw r_iretd shr 0x10

; int 1e -
	p_int1e		dw r_iretd and 0xffff
	rseg_i1e		dw segCode
	byte_i1e		db 0
	desc_i1e		db 0x8f ;
	ext_i1e		dw r_iretd shr 0x10

; int 1f -
	p_int1f		dw r_iretd and 0xffff
	rseg_i1f		dw segCode
	byte_i1f		db 0
	desc_i1f		db 0x8f ;
	ext_i1f		dw r_iretd shr 0x10


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; interrupções de hardware
base_int_pic:	; desabilitadas pela instruçao CLI

; int20- interrupção do timer
	timer_irq		dw int_Timer and 0xffff
	rseg0		dw segCode
	byte_m0		db 0
	desc_int0		db 0x8f 		; rotina presente, tipo: trap exe#32
	ext_int0		dw int_Timer shr 0x10
; int21- interrupçao do teclado
	teclado_irq	dw Tec_int and 0xffff	;int_Keyboard
	rseg1		dw segCode
	byte_m1		db 0
	desc_int1		db 0x8f
	ext_int1		dw Tec_int shr 0x10
; int22-
	irq_2		dw Int_ret and 0xffff	; interrupçao de hardware...(ver timer)
	rseg_2		dw segCode
	byte_m2		db 0
	desc_int2		db 0x8f
	ext_int2		dw Int_ret shr 0x10
; int23
	irq_3		dw Int_ret and 0xffff
	rseg_3		dw segCode
	byte_m3		db 0
	desc_int3		db 0x8f
	ext_int3		dw Int_ret shr 0x10
; int24
	irq_4		dw Int_ret and 0xffff
	rseg_4		dw segCode
	byte_m4		db 0
	desc_int4		db 0x8f
	ext_int4		dw Int_ret shr 0x10
; int25
	irq_5		dw Int_ret and 0xffff
	rseg_5		dw segCode
	byte_m5		db 0
	desc_int5		db 0x8f
	ext_int5		dw Int_ret shr 0x10
; int26 - floppy disk
	fdc_irq		dw Int_ret and 0xffff		; FDC
	rseg_6		dw segCode
	byte_m6		db 0
	desc_int6		db 0x8f
	ext_int6		dw Int_ret shr 0x10
; int27
	irq_7		dw Int_ret and 0xffff
	rseg7		dw segCode
	byte_m7		db 0
	desc_int7		db 0x8f
	ext_int7		dw Int_ret shr 0x10
; int28
	irq_8		dw Int_ret and 0xffff
	rseg_8		dw segCode
	byte_m8		db 0
	desc_int8		db 0x8f
	ext_int8		dw Int_ret shr 0x10
; int29
	irq_9		dw Int_ret and 0xffff
	rseg_9		dw segCode
	byte_m9		db 0
	desc_int9	db 0x8f
	ext_int9	dw Int_ret shr 0x10
; int2a
	irq_a		dw Int_ret and 0xffff
	rseg_a		dw segCode
	byte_ma		db 0
	desc_inta	db 0x8f
	ext_inta	dw Int_ret shr 0x10
; int2b
	irq_b		dw Int_ret and 0xffff
	rseg_b		dw segCode
	byte_mb		db 0
	desc_intb	db 0x8f
	ext_intb	dw Int_ret shr 0x10
; int2c - interrupcao do mouse
	irq_c		dw Mouse_Int and 0xffff
	rseg_c		dw segCode
	byte_mc		db 0
	desc_intc	db 0x8f
	ext_intc	dw Mouse_Int shr 0x10
; int2d
	irq_d		dw Int_ret and 0xffff
	rseg_d		dw segCode
	byte_md		db 0
	desc_intd	db 0x8f
	ext_intd	dw Int_ret shr 0x10
; int2e
	irq_e		dw IntIDE1 and 0xffff	; 14-IntIDE1
	rseg_e		dw segCode
	byte_me		db 0
	desc_inte	db 0x8f
	ext_inte	dw Int_ret shr 0x10
; int2f - HD, ide1
	HDC1_irq	dw IntIDE2 and 0xffff	; 15-IntIDE2
	rseg_f		dw segCode
	byte_mf		db 0
	desc_intf	db 0x8f
	ext_intf	dw Int_ret shr 0x10

; int30 a 0x40
	rb 8*0x11

; int41 - Servico de Memoria
	SMM_irq		dw SMM_Enter and 0xffff
	rseg_int41	dw segCode
	byte_int41	db 0
	desc_int41	db 0x8f
	ext_int41	dw SMM_Enter shr 0x10

; int42 - Servico de Janela
	SWN_irq		dw SWN_Enter and 0xffff
	rseg_int42	dw segCode
	byte_int42	db 0
	desc_int42	db 0x8f
	ext_int42	dw SWN_Enter shr 0x10

; int43 - Servico de Desenho
	SDW_irq		dw SDW_Enter and 0xffff
	rseg_int43	dw segCode
	byte_int43	db 0
	desc_int43	db 0x8f
	ext_int43	dw SDW_Enter shr 0x10

; int44 - Serviço de dispositivos
	SDD_irq		dw SDD_Enter and 0xffff
          rseg_int44	dw segCode
	byte_int44	db 0
          desc_int44	db 0x8f
          ext_int44	dw SDD_Enter shr 0x10

; int45 - Serviço de arquivos
	SFS_irq		dw SFS_Enter and 0xffff
          rseg_int45	dw segCode
	byte_int45	db 0
          desc_int45	db 0x8f
          ext_int45		dw SFS_Enter shr 0x10

IDT_end: