;-----------------------------------------------------------------------------------------
;
; LITTLE PRINCE Operational System
; Copyright (C) KolibriOS team 2004-2007
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
; @Id: const.inc - 2008-1-20, modified and adapted by fb-toledo
; Constant equate definitions
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------


OS_BASE_F 	equ 0x1000		; Posicao fisica do sistema
OS_BASE 	equ 0x80000000		; Posicao do sistema no modo paginado
OS_PageTable	equ 0x100000		; dd - Ponteiro da tabela de pagina do sistema
OS_PageNumberTable	equ 0x00	; db - Numero da Tabela de pagina do sistema

;-----------------------------------------------------------------------------------------


;========================== P R O C E S S A D O R ========================================

CPU_386        equ 3
CPU_486        equ 4
CPU_PENTIUM    equ 5
CPU_P6         equ 6
CPU_PENTIUM4   equ 0x0F

CR0_PE         equ    0x00000001   ;protected mode
CR0_MP         equ    0x00000002   ;monitor fpu
CR0_EM         equ    0x00000004   ;fpu emulation
CR0_TS         equ    0x00000008   ;task switch
CR0_ET         equ    0x00000010   ;extension type hardcoded to 1
CR0_NE         equ    0x00000020   ;numeric error
CR0_WP         equ    0x00010000   ;write protect
CR0_AM         equ    0x00040000   ;alignment check
CR0_NW         equ    0x20000000   ;not write-through
CR0_CD         equ    0x40000000   ;cache disable
CR0_PG         equ    0x80000000   ;paging

CR4_VME        equ    0x0001
CR4_PVI        equ    0x0002
CR4_TSD        equ    0x0004
CR4_DE         equ    0x0008	     ;
CR4_PSE        equ    0x0010	     ; size of page
CR4_PAE        equ    0x0020	     ; mode translation (normal,Pae paging)
CR4_MCE        equ    0x0040
CR4_PGE        equ    0x0080
CR4_PCE        equ    0x0100
CR4_OSFXSR     equ    0x0200
CR4_OSXMMEXPT  equ    0x0400

CAPS_FPU       equ    00 ;on-chip x87 floating point unit
CAPS_VME       equ    01 ;virtual-mode enhancements
CAPS_DE        equ    02 ;debugging extensions
CAPS_PSE       equ    03 ;page-size extensions
CAPS_TSC       equ    04 ;time stamp counter
CAPS_MSR       equ    05 ;model-specific registers
CAPS_PAE       equ    06 ;physical-address extensions
CAPS_MCE       equ    07 ;machine check exception
CAPS_CX8       equ    08 ;CMPXCHG8B instruction
CAPS_APIC      equ    09 ;on-chip advanced programmable interrupt controller
;                     10 ;unused
CAPS_SEP       equ    11 ;SYSENTER and SYSEXIT instructions
CAPS_MTRR      equ    12 ;memory-type range registers
CAPS_PGE       equ    13 ;page global extension
CAPS_MCA       equ    14 ;machine check architecture
CAPS_CMOV      equ    15 ;conditional move instructions
CAPS_PAT       equ    16 ;page attribute table
CAPS_PSE36     equ    17 ;page-size extensions
CAPS_PSN       equ    18 ;processor serial number
CAPS_CLFLUSH   equ    19 ;CLFUSH instruction
;		  20 ;unused
CAPS_DS        equ    21 ;debug store
CAPS_ACPI      equ    22 ;thermal monitor and software controlled clock supported
CAPS_MMX       equ    23 ;MMX instructions
CAPS_FXSR      equ    24 ;FXSAVE and FXRSTOR instructions
CAPS_SSE       equ    25 ;SSE instructions
CAPS_SSE2      equ    26 ;SSE2 instructions
CAPS_SS        equ    27 ;self-snoop
CAPS_HTT       equ    28 ;hyper-threading technology
CAPS_TM        equ    29 ;thermal monitor supported
CAPS_IA64      equ    30 ;IA64 capabilities
CAPS_PBE       equ    31 ;pending break enable
CAPS_SSE3      equ    32 ;SSE3 instructions
;                     33
;                     34
CAPS_MONITOR   equ    35 ;MONITOR/MWAIT instructions
CAPS_DS_CPL    equ    36 ;
CAPS_VMX       equ    37 ;virtual mode extensions
;                     38 ;
CAPS_EST       equ    39 ;enhansed speed step
CAPS_TM2       equ    40 ;thermal monitor2 supported
;                     41
CAPS_CID       equ    42 ;
;                     43
;                     44
CAPS_CX16      equ    45 ;CMPXCHG16B instruction
CAPS_xTPR      equ    46 ;
;reserved
;
;ext edx /ecx
CAPS_SYSCAL    equ    64 ;
CAPS_XD        equ    65 ;execution disable
CAPS_FFXSR     equ    66 ;
CAPS_RDTSCP    equ    67 ;
CAPS_X64       equ    68 ;
CAPS_3DNOW     equ    69 ;
CAPS_3DNOWEXT  equ    70 ;
CAPS_LAHF      equ    71 ;
CAPS_CMP_LEG   equ    72 ;
CAPS_SVM       equ    73 ;secure virual machine
CAPS_ALTMOVCR8 equ    74 ;

; CPU MSR names
MSR_SYSENTER_CS		equ	0x174
MSR_SYSENTER_ESP	equ	0x175
MSR_SYSENTER_EIP	equ	0x176
MSR_AMD_EFER	equ	0xC0000080	; Extended Feature Enable Register
MSR_AMD_STAR	equ	0xC0000081	; SYSCALL/SYSRET Target Address Register

SSE_IE         equ    0x0001
SSE_DE         equ    0x0002
SSE_ZE         equ    0x0004
SSE_OE         equ    0x0008
SSE_UE         equ    0x0010
SSE_PE         equ    0x0020
SSE_DAZ        equ    0x0040
SSE_IM         equ    0x0080
SSE_DM         equ    0x0100
SSE_ZM         equ    0x0200
SSE_OM         equ    0x0400
SSE_UM         equ    0x0800
SSE_PM         equ    0x1000
SSE_FZ         equ    0x8000

SSE_INIT equ (SSE_IM+SSE_DM+SSE_ZM+SSE_OM+SSE_UM+SSE_PM)

;-----------------------------------------------------------------------------------------


;=================================== M E M O R Y =========================================

PageTable	equ 0x100000	; base das tabelas (1mb) - endereço físico
PageTable_Size 	equ 0x1000	; tamanho das tabelas (4kb)
PageTable_QTDE	equ 255		; Quantidade de tabelas de paginas

TSS_OS		equ 0x300		; rb 0x100
TSS_AP1		equ 0xe00

   ;==== TSS VARIAVEL ==========
   TSS_reserved1   equ 0x0
   TSS_ESP0	   equ 0x4
   TSS_SS0	   equ 0x8
   TSS_ESP1	   equ 0xc
   TSS_SS1	   equ 0x10
   TSS_ESP2	   equ 0x14
   TSS_SS2	   equ 0x18
   TSS_CR3	   equ 0x1c
   TSS_EIP	   equ 0x20
   TSS_EFLAGS	   equ 0x24
   TSS_EAX	   equ 0x28
   TSS_ECX	   equ 0x2c
   TSS_EDX	   equ 0x30
   TSS_EBX	   equ 0x34
   TSS_ESP	   equ 0x38
   TSS_EBP	   equ 0x3c
   TSS_ESI	   equ 0x40
   TSS_EDI	   equ 0x44
   TSS_ES	   equ 0x48
   TSS_CS	   equ 0x4c
   TSS_SS	   equ 0x50
   TSS_DS	   equ 0x54
   TSS_FS	   equ 0x58
   TSS_GS	   equ 0x5c
   TSS_LDT	   equ 0x60
   TSS_BYTE	   equ 0x64
   ;----------------------------

;-----------------------------------------------------------------------------------------