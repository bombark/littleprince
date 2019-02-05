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
; @Id: FAT_Ini.inc - 2008-02-09 - cb-toledo
; The fat units have not any protection and can be open by any user
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------
; Inicializa a partição tipo FAT alocando um bloco de carga do setor de boot (2 setores)
; in:
;	bl= partição
;
; out:
; 	edx= aponta a base do bloco na memoria
;	cy=1 	erro.

align 4
Fat_Ini:			; rotina usada para abrir a unidade			OK
	push eax edi ecx esi
	xor ecx, ecx
	and ebx,0xf
	push ebx
	dec bl
	shl ebx,5
	lea esi,[ebx+TabelaPart]
	pop ebx

; alocação do bloco no heap
	push esi
	mov eax, 0x400		;
	call aloca_HB		; aloca bloco no heap
	jb .erro1
	mov edi, esi
 	mov edx, esi 		; ebp= base do bloco na memoria
 	pop esi
	mov [esi+Part_DMpos], edi	; salva a base do bloco de memoria boot da unidade

; buscar o setor de boot (2 setores)- bl= partição
	mov eax, 0		;
	mov cl, 2
	call ReadSeg0		; altera eax, ebx
.fim:	pop esi ecx edi eax
	ret

.erro1:	pop esi		; neste caso liberar o bloco....ou tentar aumentar o
	jmp .fim		; tamanho do heap

; OBS: abre um bloco e carrega o boot, a 1a e 2a fat e o diretorio root da unidade
; estrutura: 	000-200: boot
;		200-400: 2o setor boot + variaveis


;-----------------------------------------------------------------------------------------