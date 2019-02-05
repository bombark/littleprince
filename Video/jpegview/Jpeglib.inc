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
; @Id: jpeglib.inc - 2008-02-22 - cb-toledo
; jpeg library routines & services
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------


iniciomemoria	equ 0x80250000

;405 412 586
;
; Definitions for block of parameters
	bufer_size =	1024*16+2
	fichero		equ 4
	mcu_ptr		equ 8
	color_ptr		equ 0xc
	estado		equ 0x10	;by
	color_c		equ 0x11	;by
	nbits=color_c
	idct		equ 0x14	;dd
	tmp_bits		equ 0x18	;dd
	actable		equ 0x2c	;dd
	matriz_limit	equ 0x20	;dd
	sourcebits=	36
	sourcebits_index=	40
	sourcebits_limit=	44
	qt_ptrs=		48
	ht_dc_ptrs=	64
	ht_ac_ptrs=	80
	matrices=		96
	tmp_bufer=100
	x_org=	104
	y_org=	108
	x_mcu=	112
	y_mcu=	116
	x_size=	120
	y_size=	124
	x_org2=	128
	y_org2=	132
	x_mcu2=	136
	y_mcu2=	140
	x_size2=	144
	y_size2=	148
	q_ptr=	152
	dc=	164
	position=	204
	draw_ptr=	208
	struct_size=	212



;-----------------------------------------------------------------------------------------
align 4
jpeg_display:
	test ebp,ebp		; ebp deve apontar o bloco de parametros
	jnz .inicio
	ret

  .inicio:
	pushad
	mov [ebp],esp
	mov eax,[ebp+fichero]
	mov edx,[ebp+position]
	call lseek		; mov [eax+file_handler.position],edx
	mov edi,[ebp+sourcebits]	;****************************************?
	add edi,bufer_size		;****************************************?
	mov [ebp+sourcebits_index],edi
	sub edi,2
	mov [ebp+sourcebits_limit],edi
	mov edi,[ebp+matrices]	;****************************************?
	mov [ebp+matriz_limit],edi
	xor eax,eax
	mov [esp+8],eax
	mov [ebp+estado],eax
	mov [ebp+tmp_bits],eax
	mov [ebp+dc],eax
	mov [ebp+dc+16],eax
	mov [ebp+dc+32],eax

	mov eax,[ebp+y_mcu]
	mov ecx,[ebp+y_org]
	sub ecx,eax
	mov [ebp+y_org2],ecx
	mov [ebp+y_mcu2],eax
	push dword [ebp+y_size]
	pop dword [ebp+y_size2]
  .l3:	push dword [ebp+x_org]
	pop dword [ebp+x_org2]
	push dword [ebp+x_mcu]
	pop dword [ebp+x_mcu2]
	push dword [ebp+x_size]
	pop dword [ebp+x_size2]
	mov eax,[ebp+y_mcu2]
	add [ebp+y_org2],eax
	sub [ebp+y_size2],eax
	jnc .l4
	add eax,[ebp+y_size2]
	jnz .cont
	mov [esp+8],ebp
	popad
	ret

  .cont:
	mov dword [ebp+y_size2],0
	mov [ebp+y_mcu2],eax
  .l4:
	mov eax,[ebp+x_mcu2]	;*****************************??
	sub [ebp+x_size2],eax
	jnc .l5
	add eax,[ebp+x_size2]
	jz .l3
	mov dword [ebp+x_size2],0
	mov [ebp+x_mcu2],eax
	call dword [ebp+mcu_ptr]
	mov eax,[ebp+x_mcu]		;*****************************??
	mov ecx,[ebp+x_mcu2]
	mov edx,[ebp+y_mcu2]
	call recortar
	jmp .l6

  .l5:
	call dword [ebp+mcu_ptr]
	mov ecx,[ebp+x_mcu2]
	mov edx,[ebp+y_mcu2]
  .l6:
	mov eax,[ebp+x_org2]
	mov ebx,[ebp+y_org2]
	call dword [ebp+draw_ptr]
	add [ebp+x_org2],ecx
	mov ax,[ebp+estado]
	test al,15
	jz .l4
	cmp ah,8
	jnc .l4
	xor edx,edx
	mov [ebp+tmp_bits],edx
	mov [ebp+dc],edx
	mov [ebp+dc+16],edx
	mov [ebp+dc+32],edx
	add dword [ebp+sourcebits_index],2
	and word [ebp+estado],0c0h
	test al,32
	jz .l4
	jmp .l3

color100:
	push edi
  .l1:	lodsw
	mov dl,ah
	mov ah,al
	stosw
	mov ah,dl
	stosb
	mov al,dl
	stosb
	stosw
	dec cl
	jnz .l1
	pop edi
	ret

color111:
	push edi
  .l1:	lodsw
	mov bx,[esi+62]
	mov dx,[esi+126]
	xchg ah,bh
	xchg ah,dl
	xchg ah,bl
	stosw
	mov ax,bx
	stosw
	mov ax,dx
	stosw
	dec cl
	jnz .l1
	pop edi
	mov ecx,64*3
	jmp ybr_bgr		;-------------------->

color411:
	push ebp
	push edi
	lea ebp,[esi+ecx*8]
  .l1:	push ecx
	mov ax,[esi]
	mov cx,[ebp]
	mov dx,[ebp+64]
	add ebp,2
	xchg ch,dl
	mov bx,ax
	mov ah,cl
	mov bl,ch
	mov [edi],ax
	mov [edi+2],bx
	mov [edi+4],cx
	mov ax,[esi+8]
	mov bh,ah
	mov ah,cl
	mov [edi+48],ax
	mov [edi+48+2],bx
	mov [edi+48+4],cx
	mov ax,[esi+2]
	mov bx,ax
	mov ah,dl
	mov bl,dh
	mov [edi+6],ax
	mov [edi+2+6],bx
	mov [edi+4+6],dx
	mov ax,[esi+8+2]
	mov bh,ah
	mov ah,dl
	mov [edi+48+6],ax
	mov [edi+48+2+6],bx
	mov [edi+48+4+6],dx
	pop ecx
	add edi,12
	dec ecx
	add esi,4
	test cl,1
	jnz .l1
	add esi,64-8
	test cl,2
	jnz .l1
	sub esi,128-16
	add edi,48
	test cl,15
	jnz .l1
	add esi,64
	test cl,cl
	jnz .l1
	pop edi
	pop ebp
	mov ecx,64*4*3
	jmp ybr_bgr		;----------------->

color211:
	push ebp
	push edi
	lea ebp,[esi+ecx*4]
  .l1:	push ecx
	mov ax,[esi]
	mov cx,[ebp]
	mov dx,[ebp+64]
	add ebp,2
	xchg ch,dl
	mov bx,ax
	mov ah,cl
	mov bl,ch
	mov [edi],ax
	mov [edi+2],bx
	mov [edi+4],cx
	mov ax,[esi+2]
	mov bx,ax
	mov ah,dl
	mov bl,dh
	mov [edi+6],ax
	mov [edi+2+6],bx
	mov [edi+4+6],dx
	pop ecx
	add edi,12
	dec cl
	add esi,4
	test cl,1
	jnz .l1
	add esi,64-8
	test cl,2
	jnz .l1
	sub esi,128-8
	test cl,cl
	jnz .l1
	pop edi
	pop ebp
	mov ecx,64*3*2
	jmp ybr_bgr		;-------------------->


mcu411:	lea ebx,[ebp+q_ptr]
	call hufdecode
	lea ebx,[ebp+q_ptr]
	call hufdecode
mcu211:	lea ebx,[ebp+q_ptr]
	call hufdecode
mcu111:	lea ebx,[ebp+q_ptr]
	call hufdecode
	lea ebx,[ebp+q_ptr+16]
	call hufdecode
mcu100:	lea ebx,[ebp+q_ptr+32]
	call hufdecode
	mov esi,[ebp+matrices]
	mov dword [ebp+matriz_limit],esi
	mov ecx,32
	lea edi,[esi+64*6]
	jmp dword [ebp+color_ptr]


;-----------------------------------------------------------------------------------------
align 4
load_bits:	;edx=bits,	cl=nbits,
		;bp=data struct
		;cr: cl,edx,eax,si
		;ncr bx,bp,di,ch

	mov esi,[ebp+sourcebits_index]
	cmp esi,[ebp+sourcebits_limit]
	jnc .l6
	movzx eax,byte [esi]
	inc esi
	add cl,8
	cmp al,-1
	je .l2
	mov ah,al
	lodsb
	add cl,8
	cmp al,-1
	je .l2
  .l1:	ror eax,cl
	or edx,eax
	mov [ebp+sourcebits_index],esi
	ret
  .l2:	lodsb
	test al,al
	jnz .l3
	mov al,-1
	call .l1
	cmp cl,16
	jc load_bits
	ret
  .l3:	sub esi,2
	sub cl,8
	sub al,0d0h
	cmp al,8
	jc .l4
	sub al,9
	mov al,63
	jz .l4
	mov al,127
  .l4:	inc al
	or [ebp+estado],al
	movzx eax,ah
	jmp .l1
  .l5:	mov [ebp+sourcebits_limit],edi
	mov word [edi],0d9ffh
	popad
	jmp load_bits
  .l6:				;read file
	pushad
	mov ecx,bufer_size-2
	mov edx,[ebp+sourcebits_limit]
	mov edi,[ebp+sourcebits]
	mov ax,[edx]
	sub edx,edi
	stosw
	sub esi,edx
	mov [ebp+sourcebits_index],esi
	cmp edx,ecx
	jne .l5
	mov eax,[ebp+fichero]
	; call read
	lea ecx,[edi+ecx-2]
	mov [ebp+sourcebits_limit],ecx
	popad
	jmp load_bits


;-----------------------------------------------------------------------------------------
align 4
hufdecode:	;si->dctable [bp+20]->actable di->outbufer edx->bits cl->bits en edx
	;[bp+24]->sourcebits
	;[bp+22]=outbufer+128
	;[bx]       q ptr para aa&n
	;[bx+2]     a ptr
	;[bx+4]     d ptr
	;[bx+8]     dc componente

	fninit
	push dword [ebx]
	mov cl,[ebp+nbits]
	mov edx,[ebp+tmp_bits]
	cmp cl,16
	jnc .l1
	call load_bits
  .l1:	mov eax,[ebx+4]
	mov esi,[ebx+8]
	mov [ebp+actable],eax
	movzx eax,word [esi]
	add esi,2
  .l2: 	add edx,edx
	jnc .l3
	add esi,eax
  .l3: 	lodsw
	test ax,ax
	jns .l2
;codigo encontrado
	and ax,7f0fh
	mov edi,[ebp+matriz_limit]	;arrays
	sub cl,ah
	jns .l4
	fldz
  .error:	xor ecx,ecx
	or byte [ebp+estado],32
	jmp .l12
  .l4:	cmp cl,al
	jnc .l5
	push eax
	call load_bits
	pop eax
   .l5:	sub cl,al
	mov ch,cl
	mov cl,al
	mov eax,edx
	shl edx,cl
	sar eax,17
	xor ax,8000h
	xor cl,15
	sar ax,cl
	mov cl,ch
	mov ch,2
	add ax,8000h 		;incrementar si negativo
	adc ax,8000h
	add [ebx+12],ax
	fild word [ebx+12]
	push ecx
	mov ecx,64
	xor eax,eax
	add [ebp+matriz_limit],ecx
	rep stosd
	pop ecx
	sub edi,64*4
	mov ebx,[esp]
	fmul dword [ebx]
  .l6:	cmp cl,16
	jnc .l7
	call load_bits
  .l7:	mov esi,[ebp+actable]
	movzx eax,word[esi]
	add esi,2
  .l8:	add edx,edx
	jnc .l9
	add esi,eax
   .l9:	lodsw
	test ax,ax
	jns .l8
;codigo encontrado
	and ah,127
	xor ebx,ebx
	sub cl,ah
	js .error
	or bl,al
	jz .l12
	and al,0f0h
	shr al,3
	add ch,al
	js .error
	and bl,0fh
	jz .l11
	cmp cl,bl
	jnc .l10
	call load_bits
  .l10:	sub cl,bl
	xchg bl,cl
	mov eax,edx
	shl edx,cl
	sar eax,17
	xor cl,15
	xor ax,8000h
	sar ax,cl
	add ax,8000h 		;incrementar si negativo
	adc ax,8000h
	mov cl,bl
	mov bl,ch
	mov [ebp+tmp_bits],ax
	mov ax,[ebx+zigzag]
	mov ebx,[esp]
	fild word [ebp+tmp_bits]
	or [ebp+idct],ax
	and eax,11111100b
	fmul dword [ebx+eax]
	fstp dword [edi+eax]
  .l11:	add ch,2
	jns .l6
  .l12:	mov [ebp+nbits],cl
	mov [ebp+tmp_bits],edx
	xor ebx,ebx
	add esp,4
	xchg ebx,[ebp+idct]
	cmp ch,2
	je idctf1
	fstp dword [edi]
	test bh,0feh
	jnz idctf3
idctf2a:	test bh,1
	mov esi,edi
	jz .l1
	test bl,1
	jnz idctf3
	push idctf2b
	jmp idctf3b
  .l1: 	call idctf3a
	mov cl,4
	call limit
	mov eax,[edi-8]
	mov edx,[edi-4]
	mov cl,7
  .l2:	mov [edi],eax
	mov [edi+4],edx
	add edi,8
	dec cl
	jnz .l2
	ret


idctf1:
	fistp word[edi+64]
	mov ax,128
	add ax,[edi+64]
	jz .l2
	test ah,ah
	jz .l1
	mov al,-1
	js .l2
  .l1: 	mov ah,al
	stosw
	stosw
	mov eax,[edi-4]
	mov ecx,15
	rep stosd
  .l2: 	ret


idctf3:
	mov bl,8
	mov esi,edi
  .l1:	 rcr bh,1
	jc .l3
	mov eax,[esi]
	test eax,eax
	jz .l4
	mov cl,7
  .l2:	 add esi,32
	mov [esi],eax
	dec cl
	jnz .l2
	sub esi,32*7-4
	dec bl
	jnz .l1
	jmp .l5
  .l3:	call idctf3b
  .l4:	add esi,4
	dec bl
	jnz .l1
  .l5:	mov esi,edi
	mov cl,8
  .l6:	call idctf3a
	add esi,32
	add edi,16
	dec cl
	jnz .l6
	sub edi,128
	mov esi,edi
	mov cl,32
limit:	mov dx,[esi]
	mov bx,[esi+2]
	add esi,4
	add dx,128
	add bx,128
	test dh,dh
	mov ax,dx
	jz .l1
	mov al,0
	js .l1
	mov al,-1
  .l1:	test bh,bh
	mov ah,bl
	jz .l2
	mov ah,0
	js .l2
	mov ah,-1
  .l2:	stosw
	dec cl
	jnz limit
	ret


idctf2b:
	mov dl,8
  .l1:	fld dword[esi]
	add esi,32
	mov ax,128
	fistp word [edi]
	add ax,[edi]
	test ah,ah
	jz .l2
	mov al,0
	js .l2
	mov al,-1
  .l2:	mov ah,al
	stosw
	stosw
	stosw
	stosw
	dec dl
	jnz .l1
	ret


;-----------------------------------------------------------------------------------------
align 4
idctf3a:			;si(d float),di(w int) ncr
	fld dword[esi+1*4]		;f1	;t21=f1+f7
	fld st0
	fld dword[esi+7*4]		;f7
	fadd st2,st0
	fsubp st1,st0		;t22=f1-f7
	fld dword[esi+5*4]
	fld st0			;f5	;t23=f5+f3
	fld dword[esi+3*4]		;f3
	fadd st2,st0
	fsubp st1,st0		;t20=f5-f3
	fld st0
	fadd st0,st3		;t25=(t20+t22)*k2
	fmul dword[k+4]		;k2 ;t25,t20,t23,t22,t21
	fld st4			;t7=t21+t23
	fadd st0,st3		;t7,t25,t20,t23,t22,t21
	fld dword[k+12]		;k4	;t6=k4*t20+t25-t7
	fmulp st3,st0
	fsub st2,st0
	fld st1
	faddp st3,st0		;t7,t25,t6,t23,t22,t21
	fld st5			;t5=(t21-t23)*k1-t6
	fsub st0,st4
	fmul dword[k]		;k1
	fsub st0,st3
	fstp st6			;t7,t25,t6,t23,t22,t5
	fstp st3			 ;t25,t6,t7,t22,t5
	fxch st3
	fmul dword[k+8]		;k3	;t4=k3*t22-t25+t5
	fadd st0,st4		;t22*k3+t5,t6,t7,t25,t5
	fsubrp st3,st0		;t6,t7,t4,t5
	fld dword[esi]		;f0	;t10=f0+f4
	fst st5			;f0,t4,t5,t6,t7,f0
	fld dword[esi+4*4]		;f4
	fsub st6,st0		;t11=f0-f4
	faddp st1,st0
	fld st0			;t10,t10,t6,t7,t4,t5,t11
	fld dword[esi+2*4]		;f2	;t13=f2+f6
	fadd dword[esi+6*4]		;f6	;t13,t10,t10,t6,t7,t4,t5,t11
	fadd st2,st0		;t13,t10,t0,t6,t7,t4,t5,t11 ;t0=t10+t13
	fsubp st1,st0		;t3,t0,t6,t7,t4,t5,t11 ;t3=t10-t13
	fld st0			;p3=t3-t4
	fsub st0,st5
	fistp word [edi+3*2]	;p3
	fadd st0,st4		;p4=t3+t4
	fld dword[esi+2*4]		;f2
	fstp st5
	fistp word [edi+4*2]	;p4	;t0,t6,t7,f2,t5,t11
	fld st0			;p0=t0+t7
	fsub st0,st3
	fistp word [edi+7*2]	;p7
	fadd st0,st2		;p7=t0-t7
	fistp word [edi]		;p0	;t6,t7,f2,t5,t11
	fld st2			;f2	;f2,t6,t7,f2,t5,t11	;t12=(f2-f6)*k1-t13
	fld dword[esi+6*4]		;f6
	fadd st4,st0		;f6,f2,t6,t7,t13,t5,t11
	fsubp st1,st0
	fmul dword[k]		;k1
	fsub st0,st3
	fst st3			;t12,t6,t7,t12,t5,t11
	fadd st0,st5		;t1=t11+t12
	fst st2			;t1,t6,t1,t12,t5,t11
	fadd st0,st1		;p1=t1+t6
	fistp word [edi+2]		;p1	;t6,t1,t12,t5,t11
	fsubp st1,st0		;p6=t1-t6
	fistp word [edi+6*2]	;p6	;t12,t5,t11
	fsubp st2,st0		;t2=t11-t12	;t5,t2
	fld st0
	fadd st0,st2		;p2=t2+t5
	fistp word [edi+2*2]	;p2
	fsubp st1,st0		;p5=t2-t5	;t5,t2
	fistp word [edi+5*2]
	ret			;p5


;-----------------------------------------------------------------------------------------
align 4
idctf3b:				;si ncr
	fld dword[esi+1*32]
	fld st0			;f1	;t21=f1+f7
	fld dword[esi+7*32]
	fadd st2,st0		;f7
	fsubp st1,st0		;t22=f1-f7
	fld dword[esi+5*32]
	fld st0			;f5	;t23=f5+f3
	fld dword[esi+3*32]		;f3
	fadd st2,st0
	fsubp st1,st0
	fld st0			;t20=f5-f3
	fadd st0,st3		;t25=(t20+t22)*k2
	fmul dword[k+4]		;k2	;t25,t20,t23,t22,t21
	fld st4			;t7=t21+t23
	fadd st0,st3		;t7,t25,t20,t23,t22,t21
	fld dword[k+12]		;k4	;t6=k4*t20+t25-t7
	fmulp st3,st0
	fsub st2,st0
	fld st1
	faddp st3,st0		;t7,t25,t6,t23,t22,t21
	fld st5			;t5=(t21-t23)*k1-t6
	fsub st0,st4
	fmul dword[k]		;k1
	fsub st0,st3
	fstp st6			;t7,t25,t6,t23,t22,t5
	fstp st3
	fxch st3			;t25,t6,t7,t22,t5
	fmul dword[k+8]		;k3	;t4=k3*t22-t25+t5
	fadd st0,st4		;t22*k3+t5,t6,t7,t25,t5
	fsubrp st3,st0		;t6,t7,t4,t5
	fld dword[esi]		;f0	;t10=f0+f4
	fst st5			;f0,t4,t5,t6,t7,f0
	fld dword[esi+4*32]		;f4
	fsub st6,st0		;t11=f0-f4
	faddp st1,st0
	fld st0			;t10,t10,t6,t7,t4,t5,t11
	fld dword[esi+2*32]		;f2	;t13=f2+f6
	fadd dword[esi+6*32]	;f6	;t13,t10,t10,t6,t7,t4,t5,t11
	fadd st2,st0		;t13,t10,t0,t6,t7,t4,t5,t11	;t0=t10+t13
	fsubp st1,st0		;t3,t0,t6,t7,t4,t5,t11	;t3=t10-t13
	fld st0			;p3=t3-t4
	fsub st0,st5
	fstp dword[esi+3*32]	;p3
	fadd st0,st4		;p4=t3+t4
	fld dword[esi+2*32]		;f2
	fstp st5
	fstp dword[esi+4*32]	;p4	;t0,t6,t7,f2,t5,t11
	fld st0
	fsub st0,st3		;p0=t0+t7
	fstp dword[esi+7*32]	;p7
	fadd st0,st2		;p7=t0-t7
	fstp dword[esi]		;p0	;t6,t7,f2,t5,t11
	fld st2			;f2	;f2,t6,t7,f2,t5,t11	;t12=(f2-f6)*k1-t13
	fld dword[esi+6*32]		;f6
	fadd st4,st0		;f6,f2,t6,t7,t13,t5,t11
	fsubp st1,st0
	fmul dword[k]		;k1
	fsub st0,st3
	fst st3			;t12,t6,t7,t12,t5,t11
	fadd st0,st5		;t1=t11+t12
	fst st2			;t1,t6,t1,t12,t5,t11
	fadd st0,st1		;p1=t1+t6
	fstp dword[esi+1*32]	;p1	;t6,t1,t12,t5,t11
	fsubp st1,st0		;p6=t1-t6
	fstp dword[esi+6*32]	;p6	;t12,t5,t11
	fsubp st2,st0
	fld st0			;t2=t11-t12	;t5,t2
	fadd st0,st2		;p2=t2+t5
	fstp dword[esi+2*32]	;p2
	fsubp st1,st0		;p5=t2-t5		;t5,t2
	fstp dword[esi+5*32]
	ret			;p5


;-----------------------------------------------------------------------------------------

align 4
ybr_bgr:		;edi=bmp ecx=n_BYTES, 	retorna edi+=ecx

	pushad
	mov esi,edi
	add edi,ecx
	push edi
	mov edi,[ColorTable]
  .l1:	lodsw
	movzx ebx,ah
	movzx ebp,al
	movzx eax,al
	movzx ecx,byte[esi]
	lea ebx,[ebx*4+edi+1024]
	lea ecx,[ecx*4+edi]
	add eax,[ebx] 	;cb   ;solo se usan 16 bits
	mov edx,[ebx+2]     ;pero el codigo de 32 bits es mas rapido
	mov ebx,[ecx] 	;cr
	add eax,[ecx+2]
	add ebx,ebp 	;b
	add edx,ebp 	;r
	test ah,ah
	jz .l2
	mov al,0
	js .l2
	mov al,-1
  .l2:	test dh,dh
	jz .l3
	mov dl,0
	js .l3
	mov dl,-1
  .l3:	test bh,bh
	mov dh,al
	jz .l4
	mov bl,0
	js .l4
	mov bl,-1
  .l4:	mov [esi-2],dx
	mov [esi],bl
	inc esi
	cmp esi,[esp]
	jc .l1
	pop edi
	popad
	ret


;-----------------------------------------------------------------------------------------

;R = Y                    + 1.402  *(Cr-128)
;G = Y - 0.34414*(Cb-128) - 0.71414*(Cr-128)
;B = Y + 1.772  *(Cb-128)

colorprecalc:	;prepara a tabela para converter ycb a rgb - calculate colors	OK

	mov eax, 1024*2		;
	push esi
	call aloca_HB		; aloca bloco
	mov edi, esi
	pop esi
	jc .error
	mov [ColorTable], edi

	fninit
	fld dword [k3+4]
	fld dword [k3]
	mov dl,0
	call .a
	fld dword [k3+12]
	fld dword[k3+8]
  .a:	mov cx,-128
  .b:	mov [edi],ecx
	inc ecx
	fild word[edi]
	fld st0
	fmul st0,st2
	fistp word[edi]
	fmul st0,st2
	fistp word[edi+2]
	add edi,4
	inc dl
	jnz .b
.error:	ret

;ColorTable: dd 0			; ver variaveis.inc


;-----------------------------------------------------------------------------------------
;
; constantes

align 4

modes:
dd mcu100,color100,8,8  ;monocromo
dd mcu111,color111,8,8
dd mcu211,color211,16,8
dd mcu411,color411,16,16
zigzag:
db 0,0, 4+1,0, 32,1, 64,1, 36,2, 8+1,0, 12+1,0, 40,4
db 68,2, 96,1, 128,1, 100,2, 72,4, 44,8, 16+1,0, 20+1,0
db 48,16, 76,8, 104,4, 132,2, 160,1, 192,1, 164,2, 136,4
db 108,8, 80,16, 52,32, 24+1,0, 28+1,0, 56,64, 84,32, 112,16
db 140,8, 168,4, 196,2, 224,1, 228,2, 200,4, 172,8, 144,16
db 116,32, 88,64, 60,128, 92,128, 120,64, 148,32, 176,16, 204,8
db 232,4, 236,8, 208,16, 180,32, 152,64, 124,128, 156,128, 184,64
db 212,32, 240,16, 244,32, 216,64, 188,128, 220,128, 248,64, 252,128
k:
dd 1.41421,1.84776,1.08239,-2.6131
k2:
dd 0.3535534,0.49039264,0.46193953,0.415734806
dd 0.3535534,0.277785116,0.191341716,0.0975451609
k3:
dd 1.402,	-0.71414,	-0.34414,	+1.772
;  +0	+4	+8	+12

;-----------------------------------------------------------------------------------------

;boot_set_background:
;    mov     [drawing_mode],2
;.1:
;    mcall 18,16		;
;    cmp   eax,1024*2
;    jb    set_mono
;    mov     ecx,memsize-fin-stack_size  ; size
;    mov     edi,fin                     ; pointer
;    call    add_mem             ; mark memory from fin to 0x100000-1024 as free
;    call    colorprecalc        ; calculate colors
;    ;mov     esi,name_string	  ; abre arquivo imagem
;    ;call    open
;    ;test    eax,eax
;    ;jz      close_program
;    call    jpeg_info	; pega informaçoes
;    mov dword [jpeg_st],ebp
;    call    set_as_bgr2         ; set wallpaper
;    ;jmp     close_program       ; close the program right now
;
;******************************************************************************
;set_mono:
;    mov     eax, 15
;    mov     ebx, 1
;    mov     ecx, 1
;    mov     edx, 1
;    mcall
;
;    cmp     [PARAMS], dword "BOOT" ; received BOOT parameter -> goto handler
;    jne     @f
;.green:
;    mov     ecx,mono+6
;    jmp     .set
;@@:
;;    cmp     [PARAMS], word "CO" ; received BOOT parameter -> goto handler
;;    jne     .green
;    xor     ecx,ecx
;    mov     cl,[PARAMS+3]
;    sub     cl,0x30
;    cmp     ecx,0
;    jb      .green
;    cmp     ecx,9
;    ja      .green
;    imul    ecx,3
;    add     ecx,mono
;.set:
;    mcall   15,5, ,0,3
;
;    ; Stretch the image to fit
;    mov     eax, 15
;    mov     ebx, 4
;    mov     ecx, 1
;    mcall
;
;    mov     eax, 15
;    mov     ebx, 3
;    mcall
;    jmp     close_program
;
;mono:
;    db 0,0,0       ; black
;    db 255,255,255 ; white
;    db 128,128,0   ; green
;    db 240,202,166 ; lilas
;    db 192,192,192 ; grey
;    db 255,255,0   ; light-blue
;    db 255,0,0     ; blue
;    db 192,220,192 ; salad
;    db 255,0,255   ; pink
;    db 0,255,255   ; yellow
;******************************************************************************
;
;set_as_bgr2:
;    mov ebp,dword[jpeg_st]
;    test    ebp,ebp
;    jz      .end
;
;    mov     eax, 15
;    mov     ebx, 1
;    mov     ecx, [ebp + x_size]
;    mov     edx, [ebp + y_size]
;    mcall
;
;        mov     ebx, 6
;        mcall
;        test    eax, eax
;        jz      .end
;        mov     [ipc_mem_out], eax
;
;    mov     dword [ebp+draw_ptr],put_chunk_to_mem
;    call    jpeg_display
;
;        mov     eax, 15
;        mov     ebx, 7
;        mov     ecx, [ipc_mem_out]
;        mcall
;
;    ; Stretch the image to fit
;    mov     eax, 15
;    mov     ebx, 4
;    mov     ecx, [drawing_mode]  ;2
;    mcall
;
;    mov     eax, 15
;    mov     ebx, 3
;    mcall
;
;
; .end:
;    ret
;
;-----------------------------------------------------------------------------------------
align 4
recortar:		;edi= buffer, eax= largura (pixels), (ecx,edx)= tamanho desejado
	pushad
	dec edx
	jz .a
	lea ebx,[ecx*3]
	lea eax,[eax*3]
	lea esi,[edi+eax]
	add edi,ebx
	sub eax,ebx
  .b:	mov ecx,ebx
	call movedata	; ver
	add esi,eax
	dec edx
	jnz .b
  .a:	popad
	ret


;-----------------------------------------------------------------------------------------
;
; obtenção dos parâmentros jpeg
;
;-----------------------------------------------------------------------------------------
align 4
jpeg_info:	;in:  eax= ponteiro do arquivo
		;out: ebp=
;int 1
	;xor ebp,ebp
	pushad
	mov ebp,esp

	mov ecx,6
	sub esp,ecx	; frame de pilha
	mov edi,esp
	push dword [eax+2]

	;call read	; le 6 bytes primeiros e poe na pilha

	;pop dx
	;cmp dx,0d8ffh	; confere assinatura
	;je .11
	cmp word [eax], 0xd8ff
	je .l1
	mov esp,ebp
	popad
	ret

;......................................................
  .l1: 	mov edx, 2
  	push eax
        	mov ecx,struct_size
        	call malloc;z
;int 1
        	mov [edi],ebp		; [edi]= esp original
        	mov ebp,edi		; ebp= base do bloco de parametros
; poe na pilha
	pop dword [ebp+fichero]	;ebp= base buffer .....eax-> base+(fichero=4)
;int 1
        	pop ax			; 0xe0, 0xff
        	pop cx			; 0x10, 0x00
        	jmp .l3



;busca do arquivo jpg....?
; posiciona recursividade para decomposiçao
   .l2: 	mov ebx,[ebp+tmp_bufer]	;tmp_bufer=100
        	add ebx,[ebx-4]		;matrices
        	mov cx,[ebx-2]
        	mov ax,[ebx-4]
   .l3:
int 1
   	push .l2			; coloca ponteiro na pilha
        	xchg cl,ch
        	add cx,2
        	;cmp ch,3
        	;jnc .l4
        	cmp al,0ffh
        	jne eoi			; fim
        	;cmp ah,0dbh
        	;je dqt
        	;cmp ah,0c4h
        	;je dht
        	;cmp ah,0c0h
        	;je sof0
        	;cmp ah,0dah
        	;je sos
        	;cmp ah,0c2h
        	;je eoi			; fim
        	;cmp ah,0c9h
        	;je eoi			; fim
        	;cmp ah,0d9h
        	;je eoi			; fim
;int 1
   .l4: 	add edx,ecx		; ecx=1000
        	;sub edx, 4
        	mov ecx,4			; le 4 bytes...

        	call READ1		; le
int 1
        	cmp ecx,[edi-4]
        	jne eoi
  	ret			; ao dar o ret a rotina .12 é executada
 .error:	stc			; até encontrar eoi para sair
	ret


;-----------------------------------------------------------------------------------------

eoi:
      mov esp,[ebp]			; busca base da pilha
; do not close file - this will be done by caller
      and dword [ebp+fichero], 0
      call jpeg_close		; fecha arquivo e libera blocos:
      popad
      xor ebp,ebp			; saida
      ret

jpeg_close:
      test ebp,ebp
      jz .l2
      pushad
      mov eax,[ebp+fichero]		; fecha
      call close
      mov edi,[ebp+sourcebits]	; libera
      call free
      lea esi,[ebp+qt_ptrs]
      mov ecx,14
 .l1: mov edi,[esi]			; libera
      add esi,4
      call free
      loop .l1
      mov edi,ebp
      call free			; libera
      popad
 .l2: ret


;-----------------------------------------------------------------------------------------

dqt:
int 1
	call READ1
int 1
      mov esi,edi
      lea eax,[edi+ecx]
      push eax
 .l1: xor eax,eax
      lodsb
      cmp al,4
      jnc eoi
      lea ebx,[ebp+qt_ptrs+eax*4]
      test dword [ebx],-1
      jnz eoi
      mov ecx,64
      xor eax,eax
      sub esp,128
      mov edi,esp
 .l2: lodsb
      stosw
      loop .l2
      mov ecx,256
      call malloc			; aloca novo bloco  (0x200)
      mov [ebx],edi
      mov eax,esi
      mov esi,esp
      pushad
      mov ebp,zigzag
      fninit
      mov cl,64
      xor eax,eax
  .l3: fild word[esi]
      mov al,[ebp]
      and al,-2
      add ebp,2
      add esi,2
      mov ebx,eax
      and ebx,28
      fmul dword [ebx+k2]
      mov ebx,eax
      shr ebx,3
      and ebx,4+8+16
      fmul dword [ebx+k2]
      fstp dword [edi+eax]
      dec cl
      jnz .l3
      popad
      mov esi,eax
      add esp,128
      mov eax,[esp]
      sub eax,esi
      jc eoi
      cmp eax,4
      ja .l1
      jne eoi
      pop eax
      ret


sof0:  call READ
       cmp byte [edi],8
       jne eoi  			;precision
       mov ax,[edi+1]
       xchg al,ah
       mov [ebp+y_size],ax
       mov ax,[edi+3]
       xchg al,ah
       mov [ebp+x_size],ax
       mov al,[edi+5] 		;ncomponentes
       mov cl,al
       mov [ebp+color_c],al
       mov edx,modes
       dec al
       jz .l1
       dec al
       dec al
       jnz eoi
       mov al,[edi+10]
       mov ah,[edi+13]
       cmp ax,1111h
       jne eoi
       mov al,[edi+7]
       add edx,16
       cmp al,11h
       je .l1
       add edx,16
       cmp al,21h
       je .l1
       add edx,16
       cmp al,22h
       jne eoi
  .l1: lea ebx,[ebp+q_ptr]
       lea esi,[ebp+qt_ptrs]
       mov [ebp+mcu_ptr],edx
  .l2: movzx eax,byte [edi+8]
       add edi,3
       cmp al,4
       jnc eoi
       lea eax,[eax*4+esi]
       mov [ebx],eax
       add ebx,16
       dec cl
       jnz .l2
       ret


dht:    call READ
        mov esi,edi
        lea eax,[edi+ecx]
        push eax
   .l1: lodsb
        mov edi,esi
        mov ebx,3+16
        and bl,al
        cmp bl,al
        jne eoi
        shr bl,2
        and al,3
        or bl,al
        lea ebx,[ebp+ht_dc_ptrs+ebx*4]
        test dword [ebx],-1
        jnz eoi
        mov cl,15
        mov al,[edi]
   .l2: inc edi  ;calcular numero de codigos
        add al,[edi]
        jc eoi
        dec cl
        jnz .l2
        movzx ecx,al
        lea ecx,[ecx*4+2]
        call malloc			; novo bloco (ecx*4+2)
        mov [ebx],edi
        call arbol_hf
        mov eax,[esp]
        sub eax,ebx
        jc eoi
        mov esi,ebx
        cmp eax,4
        ja .l1
        jne eoi
        pop eax
        ret



arbol_hf:   ;esi=ht edi=memoria para el arbol
            ;retorna en ebx  el final de ht
            ;codigos: bits 0-3=nbits del siguiente numero
            ;         bits 4-7=numero de zeros
            ;         bits 8-14=longitud de este codigo o error si =127
            ;         bit 15=codigo/puntero
        push ebp
        lea ebx,[edi-2]
        add ebx,[ebx-2]
        mov word [ebx],-1 ;codigo de error si encontrado
        push ebx
        push esi
        lea ebx,[esi+16]
        mov ebp,esp
        xor ecx,ecx
        push edi
        push ecx
        add edi,2
        mov dx,1
        add dh,[esi]
        jz .l3
        jmp .l2
  .l1:  push edi
        push ecx
        add edi,2
  .l2:  inc cl
        cmp cl,dl
        jc .l1
        mov al,[ebx]
        inc ebx
        mov ah,128 ;marca de codigo
        or ah,dl
        cmp edi,[ebp+4]
        jnc .l5
        stosw
        cmp esp,ebp
        jnc .l5
        pop ecx
        pop esi
        lea eax,[edi-2]
        sub eax,esi
        mov [esi],ax
        dec dh
        jnz .l2 ;ncodigos
        mov esi,[ebp]
   .l3: inc esi
        inc dl
        cmp dl,17
        jnc .l4
        add dh,[esi]
        jz .l3
        mov [ebp],esi
        jmp .l2
   .l4: lea esp,[ebp+8]
        pop ebp
        ret
   .l5: mov ebp,[ebp+8]
        jmp eoi



sos:   sub ecx,4  ;a continuacion vienen los datos de la imagen
       call READ
       mov eax,[ebp+fichero]
       call ftell
       mov [ebp+position],edx
       mov esi,edi
       lea edi,[ebp+q_ptr]
       lodsb ;numero de componentes
       sub [ebp+color_c],al
       jnz eoi
       mov dh,al
  .l1: mov ebx,[edi]
       mov eax,[ebx]
       stosd
       lodsw
       mov cl,ah
       and eax,0f00h
       and ecx,0f0h
       shr eax,6
       shr ecx,2
       lea ebx,[ebp+ht_ac_ptrs+eax]
       mov eax,[ebx]
       lea ebx,[ebp+ht_dc_ptrs+ecx]
       mov ecx,[ebx]
       test eax,eax
       jz eoi
       test ecx,ecx
       jz eoi
       stosd
       mov eax,ecx
       stosd
       add edi,4
       dec dh
       jnz .l1
       mov edx,[ebp+mcu_ptr]
       cmp edx,modes
       jne .l2
       lea esi,[ebp+q_ptr]
       lea edi,[ebp+q_ptr+32]
       movsd
       movsd
       movsd
   .l2:
       mov esi,edx
       push dword [esi]
       pop dword [ebp+mcu_ptr]
       push dword [esi+4]
       pop dword[ebp+color_ptr]
       push dword [esi+12]
       pop dword [ebp+y_mcu]
       push dword [esi+8]
       pop dword [ebp+x_mcu]
       mov ecx,64*18
       call malloc				; novo bloco (64*18)
       mov [ebp+matrices],edi
       mov ecx,bufer_size
       call malloc				; novo bloco (1024*16+2)
       mov [ebp+sourcebits],edi
       mov esp,[ebp]
       mov [esp+8],ebp
       popad
       ret


READ1:  	mov eax,[ebp+fichero]
      	mov edi,[ebp+tmp_bufer]
       	movzx ecx,cx
int 1
       	call mresize			; redimensiona
int 1
       	mov [ebp+tmp_bufer],edi
.a:	mov eax, [eax+edx]	;a
	mov [edi], eax		;a



	ret

READ:  ;mov eax,[ebp+fichero]

      	mov edi,[ebp+tmp_bufer]
       	movzx ecx,cx
       	call mresize			; redimensiona
       	mov [ebp+tmp_bufer],edi
	ret				; adicionado provisoriamente
      ; jmp read			; nao é preciso...arquivo ja carregado inteiro


;-----------------------------------------------------------------------------------------
;
; manipulador de arquivos...(analisar)
;
;-----------------------------------------------------------------------------------------

file_handler:
 .operation=0
 .position=4
 .reserved=8
 .n_bytes=12
 .bufer=16
 .name=20
 .st_size=20+1024



open:		;esi=name_string
		;retorna eax
	pushad
	mov ecx,file_handler.st_size
	call mallocz
	mov [esp+28],edi
	push edi
	mov ecx,1024
	add edi,file_handler.name
	call movedata
	pop edi
; test if file exists
	lea ebx,[edi+file_handler.operation]
	mov byte[ebx],5
	mov dword[ebx+16],fileattr
	mov eax,70
	int 0x40
	cmp eax,2
	jz .virtual
	test eax,eax
	jnz close.b
  @@:	clc
	popad
	ret


  .virtual:
	mov byte [fileattr], 0x10
	jmp @b



close:
	pushad
  .b:	mov edi,[esp+28]
	call free
	popad
	xor eax,eax
	ret



read:		;(f,bufer,nbytes)  eax,edi,ecx ncr
		;retorna bytes leidos en ecx
	pushad
	lea ebx, [eax+file_handler.operation]
	mov byte [ebx], 0		; 0= read operation with long name
	mov [ebx+12], ecx		; bytes a ler
	mov [ebx+16], edi		; point to buffer
	mov eax, 70
	int 0x40
	cmp ebx, -1
	sbb ebx, -1
	mov eax, [esp+28]
	add [eax+file_handler.position], ebx
	mov [esp+24], ebx
	popad
	ret



ftell:	mov edx,[eax+file_handler.position]
	ret



lseek:		;eax=file edx=pos
	mov [eax+file_handler.position],edx
	ret



skip:		;eax=file edx=bytes to skip
	add [eax+file_handler.position],edx
	ret

;-----------------------------------------------------------------------------------------
;
; gerenciamento de memoria (comparar com o nosso sistema)
;
;-----------------------------------------------------------------------------------------
; inicializaçao:
;	mov dword [iniciomemoria+4], 0xc		; proximo bloco
;	mov dword [iniciomemoria+8], 0x80250014		; 1o bloco livre heap
;	mov dword [iniciomemoria+0xc], 0x80260000	; fim heap
;	mov dword [iniciomemoria+0x10], 0x100000-0x14	; extensao do proximo bloco



align 4
mallocz:			; ecx= tamanho do bloco a alocar
	call malloc
	pushad
	add ecx,3
	xor eax,eax
	shr ecx,2
	rep stosd
	popad
	ret


;-----------------------------------------------------------------------------------------
align 4
mresize1:	popad
	xor edi,edi
	stc
mresize2: ret

mresize:		; puntero en edi ncr retorna nuevo puntero en edi
	test edi,edi
	jz malloc
	cmp ecx,[edi-4]
	je mresize2
	call free

malloc:		; ecx=tamanho a alocar,
	mov edi,ecx
	jecxz mresize2		; sai sem erro
	pushad
	mov esi,iniciomemoria+4	;1o bloco
	lea ebx,[ecx+3]		; ebx=tamanho do bloco a alocar
	and ebx,-4 		;redondeo a 4
  .l1:	mov edi,esi
	add esi,[esi]		; esi aponta proximo bloco
	jc mresize1		; sai com erro
	lodsd
	cmp eax,ebx
	jc .l1
	cmp esi,[iniciomemoria+8]	;total de memoria no heap
	jc .l2
	jne mresize1		; sai com erro
	lea edx,[ebx+esi+4]		; endereço base = endereço registrado em iniciomemoria+8
	cmp edx,[iniciomemoria+12]
	jnc mresize1		; sai com erro
	mov [iniciomemoria+8],edx
  .l2:	pop dword [esi-4]
	push esi
	sub eax,ebx
	je .l3
	sub eax,4
	mov [esi+ebx],eax
	jz .l3
;fragmentar
	add ebx,4
	add [edi],ebx
	mov eax,[esi]
	sub eax,ebx
	mov [esi+ebx],eax
	popad
	ret

  .l3:	lodsd
	add eax,4
	add [edi],eax
	popad
	ret


;-----------------------------------------------------------------------------------------
align 4
realloc: test edi,edi
	jz malloc
	jecxz free
	pushad
	pop esi
	mov eax,[edi-4]
	call malloc
	push edi
	cmp ecx,eax
	jc .l1
	mov ecx,eax
  .l1:	push esi
	call movedata
	pop edi
	call free
	popad
  .l2:	ret


;-----------------------------------------------------------------------------------------
align 4
free:		;puntero en di
		;no se comprueban los punteros
		;retorna di=0 , ncr
	test edi,edi
	jz realloc.l2
	pushad
	pop edi
	mov ebp,[edi-4]
	dec ebp
	and ebp,-4	;redondeo a 4,dx=dx-4
	xor edx,edx
	push edx
	mov edx,iniciomemoria+4
	mov esi,edx
;buscar puntero libre anterior
  .l1:	mov ebx,esi
	lodsd
	add esi,eax
	cmp esi,edi
	jc .l1
;enlazar
	mov ecx,esi
	sub ecx,edi
	sub eax,ecx
	sub ecx,4
	mov [ebx],eax
;fusionar con el anterior
	cmp eax,[ebx-4]
	jne .l2
	cmp ebx,edx
	je .l2 		;no fusionar con el primero
	mov edi,ebx
	add eax,4
	add ecx,eax
	add ebp,eax
  .l2:	mov ebx,ebp 	;fusionar con bloques de tamanho 0
  .l3:	add ebx,4
	test dword [edi+ebx],-1
	jz .l3
	cmp ebx,ecx
	jne .l4
;fusionar con el siguiente
	add ebx,[esi-4]
	add ecx,[esi]
	add ebx,4
	add ecx,4
	cmp esi,[edx+4]
	jne .l4
	mov [edx+4],edi
  .l4:	mov [edi-4],ebx
	mov [edi],ecx
	popad
	ret


;-----------------------------------------------------------------------------------------
align 4
add_mem:	;edi,ecx ;el ultimo bloque libre debe ser >8 bytes para poder fragmentarlo
	cmp ecx,64
	jc .l1
	add ecx,edi
	add edi,3
	and edi,-4
	and ecx,-4
	mov eax,ecx
	sub ecx,edi		;redondeo
	xchg eax,[iniciomemoria+12]
	cmp edi,eax
	jna .l1
	lea esi,[edi+4]
	mov edx,esi
	xchg esi,[iniciomemoria+8]
	neg edx
	mov [edi],edx
	mov [edi+4],edx
	lea edx,[edi-4]
	sub edi,esi
	mov [esi],edi
	sub eax,4
	sub eax,esi
	mov [esi-4],eax
	add esi,eax
	sub edx,esi
	mov [esi],edx
  .l1:	ret


;-----------------------------------------------------------------------------------------
align 4
check_mem:	;busqueda de errores en la memoria
		;retorna edx nbloques o 0 si error,ecx memoria libre
		;ncr: ebp,ebx,eax
	mov edi,iniciomemoria
	mov esi,edi
	xor edx,edx
	mov ecx,[edi]
	neg ecx ;el primer bloque no cuenta
  .l1:	add ecx,[edi]
	add edi,4
	add edi,[edi]
  .l2:	inc edx
	add esi,[esi]
	jc .l4
	add esi,7
	jc .l3
	and esi,-4
	cmp esi,edi
	jc .l2
	je .l1
	jmp .l4
  .l3:	test edi,edi
	jnz .l4
	add ecx,[iniciomemoria+12]
	ret
  .l4:	xor edx,edx
	stc
	ret


;-----------------------------------------------------------------------------------------
align 4
movedata:
	push eax
	xor eax,eax
	sub eax,edi
	and eax,3
	xchg ecx,eax
	sub eax,ecx
	jle .a
	rep movsb
	mov ecx,eax
	shr ecx,2
	rep movsd
	and eax,3
  .a:	add ecx,eax
	rep  movsb
	pop eax
	ret


;-----------------------------------------------------------------------------------------
align 4
fileattr	dd 0
iniciomemoria	equ 0x80250000
		dd 0
		dd 0
		dd 0

;-----------------------------------------------------------------------------------------