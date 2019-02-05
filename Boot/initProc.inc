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
; @Id: kernel.asm  - 2008-1-20, modified and adapted by cb-toledo
; initial procedures
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------

;cpu_vendor        equ 0x0414 ;rd 3
;cpu_sign          equ 0x0420 ;rd 1
;cpu_info          equ 0x0424 ;rd 1
;cpu_caps          equ 0x0428 ;rd 4

;cpu_type   	equ 0x0438 ;db
;cpu_id		equ 0x0439 ;db
;cpu_vendor	equ 0x043a ;db
;cpu_free		equ 0x043b ;db

;bios32_entry	equ 0x043c ;dd
;pci_code_32	equ 0x0440 ;dd
;pci_data_32	equ 0x0444 ;dd
;pci_bios_entry	equ 0x0448 ;dd


;-----------------------------------------------------------------------------------------


;cpu type:
;CPU_386        equ 3		(see const.inc)
;CPU_486        equ 4
;CPU_PENTIUM    equ 5
;CPU_P6         equ 6
;CPU_PENTIUM4   equ 0x0F


align 4
init_BasicTests:			; test_cpu, protected mode
;int 1
           mov [cpu_type], byte 0
           xor eax, eax
           mov [cpu_caps], eax
           mov [cpu_caps+4], eax
;int 1
; 386
           pushfd
           pop eax
           mov ecx, eax
           xor eax, 0x40000
           push eax
           popfd
           pushfd
           pop eax
           xor eax, ecx
           mov [cpu_type], byte CPU_386
           jz .end_cpuid
           push ecx
           popfd
; 486
           mov [cpu_type], byte CPU_486
           mov eax, ecx
           xor eax, 0x200000
           push eax
           popfd
           pushfd
           pop eax
           xor eax, ecx
           je .end_cpuid
           mov [cpu_id], byte 1
; cpuID
           xor eax, eax
           cpuid

           mov [cpu_vendor], ebx
           mov [cpu_vendor+4], edx
           mov [cpu_vendor+8], ecx
           cmp ebx, dword [intel_str]
           jne .check_AMD
           cmp edx, dword [intel_str+4]
           jne .check_AMD
           cmp ecx, dword [intel_str+8]
           jne .check_AMD
           mov [cpu_vendor], byte 1		; 1-

	cmp eax, 1
           jl .end_cpuid
;
  .a:	mov eax, 1
           cpuid
           mov [cpu_sign], eax
           mov [cpu_info],  ebx
           mov [cpu_caps],  edx
           mov [cpu_caps+4],ecx
           shr eax, 8
           and eax, 0x0f
           jmp SysEnterDetection

.end_cpuid:
           movzx eax, byte [cpu_type]
           ret

.check_AMD:
           cmp ebx, dword [AMD_str]
           jne .a
           cmp edx, dword [AMD_str+4]
           jne .a
           cmp ecx, dword [AMD_str+8]
           jne .a
           mov [cpu_vendor], byte 2
           jmp .a


intel_str      db "GenuineIntel",0
AMD_str        db "AuthenticAMD",0


;-----------------------------------------------------------------------------------------

SYSENTER_CS	equ	0x174
SYSENTER_ESP	equ	0x175
SYSENTER_EIP	equ	0x176


; Fast System Call init SYSENTER/SYSEXIT
SysEnterDetection:
          bt dword [cpu_caps], CAPS_SEP
          jnc .fim   			; SysEnter not Present
          xor edx, edx
          mov ecx, SYSENTER_CS
          mov eax, segCode
          wrmsr
          mov ecx, SYSENTER_ESP
          mov eax, segCode			;sysenter_stack ; Check it
          xor eax, eax
          wrmsr
          mov ecx, SYSENTER_EIP
          ;mov eax, sysenter_entry
          wrmsr
  	;ret
.fim:

;-----------------------------------------------------------------------------------------


align 4
init_BIOS32:			; procura pela existencia do serviço

          mov edi, 0xE0000
.pcibios_nxt:
          cmp dword [edi], '_32_' 	; "magic" word
          je .BIOS32_found
.pcibios_nxt2:
          add edi, 0x10
          cmp edi, 0xFFFF0
          jne .pcibios_nxt
          stc
	ret
.BIOS32_found:			; magic word found, check control summ
          movzx ecx, byte[edi + 9]
          shl ecx, 4
          mov esi, edi
          xor eax, eax
	cld
@@:	lodsb
          add ah, al
          loop @b
          jnz .pcibios_nxt2 		; control summ must be zero

  ; BIOS32 service found !
          mov ebp, [edi + 4]
          or ebp, 0x80000000
          mov [bios32_entry], ebp	; 0x8000043c

  ; check PCI BIOS present
          mov eax, '$PCI'
          xor ebx, ebx
          push cs  			; special for 'ret far' from  BIOS
          call ebp		;ebp
          test al, al
          jnz .error		; PCI_BIOS32_not_found


  ; PCI BIOS
          or edx, 0x800f0000
          mov [pci_bios_entry], edx	; 0x80000448

  ; construçao dos descritores de código e dados
          ;add ebx, OS_BASE
          ;dec ecx
  ; prepara descritores gdt
          ;mov [pci_code_32], cx    	;limit 0-15	- 0x80000440
          ;mov [pci_data_32], cx    	;limit 0-15	- 0x80000444

         ; mov [pci_code_32+2], bx  	;base  0-15
         ; mov [pci_data_32+2], bx  	;base  0-15

          ;shr ebx, 16
          ;mov [pci_code_32+4], bl  	;base  16-23
          ;mov [pci_data_32+4], bl  	;base  16-23

          ;shr ecx, 16
          ;and cl, 0x0F
          ;mov ch, bh
          ;add cx, 0x40 		; D32 - 32bit segment
          ;mov [pci_code_32+6], cx  	;lim   16-19 &
          ;mov [pci_data_32+6], cx  	;base  24-31
          ;;or edx, 0x80000000
          ;;mov [pci_bios_entry], edx	; 0x80000448

	;mov al,2		; usado para teste...
	;pushfd		; idem
	;jmp 0x800fb922	; idem

	ret


.error:				;PCI_BIOS32_not_found:
	;  pci_emu_dat
	stc
	ret

; obs:
; Se bios #32 existe 0x8000043c é diferente de zero
; no modo flat nao é necessário a construçao dos descritores de codigo e dados

;-----------------------------------------------------------------------------------------