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



;-----------------------------------------------------------------------------------------
;
; os parametros de entrada vem da rotina de serviços FS
;
; in:
;	bl= partição
;
; out:
; 	esi= aponta a base do bloco na memoria
;	cy=1 	erro.

align 4
Fat_Open:			; rotina usada para abrir a unidade			OK
	push eax ebx edx edi ecx
	xor ecx, ecx
	cmp bl, 26		; 26 letras (a:,b:,c:,d:)
	ja .erro2
	and ebx,0x3f
	push ebx
	dec bl
	shl ebx,5
	lea esi,[ebx+TabelaPart]
	cmp dword [esi+Part_ini], 0
	jz .erro3
	mov esi, [esi+Part_DMpos]
	or esi,esi		; edi = base Boot
	pop ebx
	jz .erro2

; alocação do bloco no heap
	push esi
	mov eax, 0x800		;
	call aloca_HB		; aloca bloco no heap - esi= bloco alocado
	jb .erro1
 	mov edi, esi
 	pop esi

; Posiciona variaveis do boot no bloco



; leitura da Fat: criar um buffer parcial com 8 setores
	movzx eax, word [esi+Reserved_sectors]
	mov [edi+FATbase_lba], eax
	mov [edi+FAT_lba], eax		; lba da fat existente na memoria
	add edi, 0x400
	push eax
         	call ReadSector0
	jb .erro
	pop eax
	add edi, 0x200			; edi= root base

; leitura da root
	mov byte [edi+FAT_Type-0x600], 0
	mov dx, [esi+Sectors_per_FAT]		; deve ser 0 em fat32
	or dx, dx
	jnz .b
.a:	mov edx, [esi+Sectors_per_FAT32]
	mov byte [edi+FAT_Type-0x600], 1
	mov [edi+FAT_SPF-0x600], edx		; posiciona setores por fat no bloco
.b:	mov cl, [esi+Number_of_FATs]
	mov [edi+FAT_QTDE-0x600], cl
	shr cl, 1
	shl edx,cl
	add eax, edx			; edx= total de setores usados pela fat

	mov [edi+Root_lba-0x600], eax	; root lba
	mov [edi+Database_lba-0x600], eax	; 1cluster lba = data lba = root base (never changes)
	push eax
	call ReadSector0		; busca diretorio root ..carregar só um setor
	jb .erro
	pop eax

; calculo ultimo cluster
	mov edx, [esi+Sectors]	; quantidade total de setores na partição
	sub edx, eax
	movzx ebx, byte [esi+Sectors_per_cluster]
	mov [edi+FAT_SPC-0x600], bl
	mov eax, edx
	xor edx, edx
	div ebx
	mov [edi+Last_cluster-0x600], eax

; Directory base cluster
	mov eax, [esi+Root_dir_1cluster]
	mov [edi+FAT_RootCluster-0x600], eax
	mov [edi+Dirbase_clt-0x600], eax
	sub edi, 0x600
	mov esi, edi

.fim:	pop ecx edi edx ebx eax
	ret

.erro:
	add esp,4		; al retorna erro do driver HD
	jmp .fim

.erro1:	pop esi		; neste caso liberar o bloco....ou tentar aumentar o
	jmp .fim		; tamanho do heap

.erro2:	stc		; particao invalida
	pop ecx edi edx ebx eax
	ret

.erro3:	stc		; particao invalida
	pop ebx ecx edi edx ebx eax
	ret


; falta marcar quando a fat é do tipo FAT16 (feito pela rotina que monta as partições)
; quando for fat marcar tipo 2 (independente se for 16 ou 32 bits)

; OBS: abre um bloco e carrega o boot, a 1a e 2a fat e o diretorio root da unidade
; estrutura: 	000-200: boot
;		200-400: 2o setor boot + variaveis
;		400-600: fat buffer
;		600-800: root buffer


;-----------------------------------------------------------------------------------------