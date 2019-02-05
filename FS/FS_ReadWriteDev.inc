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
; @Id: ReadWriteDev.inc  - 2008-1-20, fb-toledo
; Partitition read and write routines
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------
;
; Info: O tipo de dispositivo é buscado na tabela de partições e com ele é
; buscado o driver correspondente.
;
;-----------------------------------------------------------------------------------------
;
align 4
DeviceTable:
	dd leia.return	; FLOPPY		; leitura/escrita em disquete
	dd HDD_Enter	; HD		; leitura/escrita em hd
	dd leia.return	; CDROM		; leitura/escrita em cdrom
	dd leia.return	; PENDRIVE	; leitura/escrita em pendrive

;-----------------------------------------------------------------------------------------
;
; Entrada:
;	eax: lba
;	ecx: extensao do bloco
;	bl: particao (0 a 32)
;      	edi: buffer
; Saida:
;	altera eax, al =codigo de erro se cy=1
align 4
ReadSeg:
leia:

	push ebp
	push ebx
	push esi
	push edi

	or bl, bl
	jz .fim
	and ebx, 0x1f
	dec ebx
	;mov ebp, ebx
	;shl ebx, 4	; Multiplica o bx por 20
	;shl ebp, 2
	;add ebx, ebp
	shl ebx, 5	; Multiplica o bx por 32
	lea esi, [ebx+TabelaPart ]		; esi= base da tabela de partiçoes
	add eax, [TabelaPart+ebx+Part_ini ]
	movzx ebp, byte [TabelaPart+ebx+15]	; tipo de unidade
	mov bh, [TabelaPart+ebx+14 ]		; unidade
	mov bl, 3				; serviço de leitura de segmentos
	and ebp, 0x0f
	shl ebp, 2
	call dword [cs:DeviceTable + ebp - 4 ]

.fim:	pop edi
	pop esi
	pop ebx
	pop ebp
.return:	ret


;-----------------------------------------------------------------------------------------
;
; Entrada:
;	eax: lba
;	ecx: extensao do bloco
;	bl: particao
;         edi: buffer (o que vai ser gravado)
; Saida:
;	edi atualizado
;	nada
;	Colocar para que em ax sai a quantidade de setores e o nnumero de erro
align 4
WriteSeg:
grave:

	push ebp
	push ebx
	push esi
	push edi

	or bl, bl
	jz .fim
	and ebx, 0x1f
	dec ebx
	;mov ebp, ebx
	;shl ebx, 4	; Multiplica o bx por 0x14
	;shl ebp, 2
	;add ebx, ebp
	shl ebx, 5	; Multiplica o bx por 0x20
	lea esi, [ebx+TabelaPart ]
	add eax, [TabelaPart+ebx+Part_ini ]	; setores anteriores
	movzx ebp, byte [TabelaPart+ebx+15 ]
	and ebp, 0x0f
	mov bh, [TabelaPart+ebx+14 ]		; unidade
	mov bl, 4				; serviço de gravaçao de segmento
	shl ebp, 2
	call dword [cs:DeviceTable + ebp - 4 ]

.fim:	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;-----------------------------------------------------------------------------------------
;
; Entrada:
;	eax: lba
;	ecx: extensao do bloco
;	bl: particao (0 a 32)
;      	edi: buffer
; Saida:
;	altera eax, al =codigo de erro se cy=1
align 4
ReadSector:

	push ebp
	push ebx
	push edx
	push esi
	push edi

	mov edx, 20
	or ecx, ecx
	jnz .ok
	inc ecx
.ok:	or bl, bl
	jz .fim

.a:	push eax
	push ebx
	and ebx, 0x1f
	dec ebx
	;mov ebp, ebx
	;shl ebx, 4	; Multiplica o bx por 20
	;shl ebp, 2
	;add ebx, ebp
	shl ebx, 5	; Multiplica o bx por 32
	lea esi, [ebx+TabelaPart]		; esi= base da tabela de partiçoes
	add eax, [TabelaPart+ebx+Part_ini ]
	movzx ebp, byte [TabelaPart+ebx+15]	; tipo de unidade
	mov bh, [TabelaPart+ebx+14 ]		; unidade
	mov bl, 1				; serviço de leitura de setor
	and ebp, 0x0f
	shl ebp, 2
	call dword [cs:DeviceTable + ebp - 4 ]
	pop ebx
	pop eax
	jc .error

.fim:	pop edi
	pop esi
	pop edx
	pop ebx
	pop ebp
	ret

.error:	dec edx
	stc
	jz .fim
	jmp .a

;-----------------------------------------------------------------------------------------
;
; Entrada:
;	eax: lba
;	ecx: extensao do bloco
;	bl: particao
;         edi: buffer (o que vai ser gravado)
; Saida:
;	nada
;	Colocar para que em ax sai a quantidade de setores e o nnumero de erro
align 4
WriteSector:

	push ebp
	push ebx
	push esi
	push edi

	or bl, bl
	jz .fim
	and ebx, 0x1f
	dec ebx
	;mov ebp, ebx
	;shl ebx, 4	; Multiplica o bx por 0x14
	;shl ebp, 2
	;add ebx, ebp
	shl ebx, 5	; Multiplica o bx por 0x20
	lea esi, [ebx+TabelaPart ]
	add eax, [TabelaPart+ebx+Part_ini ]	; setores anteriores
	movzx ebp, byte [TabelaPart+ebx+15 ]
	and ebp, 0x0f
	mov bh, [TabelaPart+ebx+14 ]		; unidade
	mov bl, 2				; serviço de gravaçao de segmento
	shl ebp, 2
	call dword [cs:DeviceTable + ebp - 4 ]

.fim:	pop edi
	pop esi
	pop ebx
	pop ebp
	ret



;-----------------------------------------------------------------------------------------
;
; Entrada:
;	eax: lba
;	ecx: extensao do bloco
;	bl: particao (0 a 32)
;      	edi: buffer
; Saida:
;	altera eax, al =codigo de erro se cy=1
align 4
ReadSegA:

	push ebp
	push edx
	push esi
	;push edi

	mov edx, 20	; 5 tentativas
.a:	push eax
	push ebx
	or bl, bl
	jz .fim
	and ebx, 0x1f
	dec ebx
	;mov ebp, ebx
	;shl ebx, 4	; Multiplica o bx por 20
	;shl ebp, 2
	;add ebx, ebp
	shl ebx, 5	; Multiplica o bx por 32
	lea esi, [ebx+TabelaPart ]		; esi= base da tabela de partiçoes
	add eax, [TabelaPart+ebx+Part_ini ]
	movzx ebp, byte [TabelaPart+ebx+15 ]	; tipo de unidade
	mov bh, [TabelaPart+ebx+14 ]		; unidade
	mov bl, 3				; serviço de leitura de segmentos
	and ebp, 0x0f
	shl ebp, 2
	call dword [cs:DeviceTable + ebp - 4 ]
	pop ebx
	pop eax
	jc .error

.fim:	;pop edi
	pop esi
	pop edx
	pop ebp
	ret

.error:	dec edx
	stc
	jz .fim
	jmp .a

;-----------------------------------------------------------------------------------------
;
; Entrada:
;	eax: lba
;	ecx: extensao do bloco
;	bl: particao
;         edi: buffer (o que vai ser gravado)
; Saida:
;	edi atualizado
;	nada
;	Colocar para que em ax sai a quantidade de setores e o nnumero de erro
align 4
WriteSegA:

	push ebp
	push ebx
	push esi
	;push edi

	or bl, bl
	jz .fim
	and ebx, 0x1f
	dec ebx
	;mov ebp, ebx
	;shl ebx, 4	; Multiplica o bx por 0x14
	;shl ebp, 2
	;add ebx, ebp
	shl ebx, 5	; Multiplica o bx por 0x20
	lea esi, [ebx+TabelaPart ]
	add eax, [TabelaPart+ebx+Part_ini ]	; setores anteriores
	movzx ebp, byte [TabelaPart+ebx+15]
	and ebp, 0x0f
	mov bh, [TabelaPart+ebx+14 ]		; unidade
	mov bl, 4				; serviço de gravaçao de segmento
	shl ebp, 2
	call dword [cs:DeviceTable + ebp - 4 ]

.fim:	;pop edi
	pop esi
	pop ebx
	pop ebp
	ret


;-----------------------------------------------------------------------------------------
;



