;-----------------------------------------------------------------------------------------

; CMOS DRIVER

;-----------------------------------------------------------------------------------------
; OBS:
; CMOS, rotinas diversas
; autor: C. Bombardeli, maio, 2005
; Usa: Fasm Assembler

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

; constantes:
dias_semana:
	db 'Domingo',0
	db 'Segunda-feira',0
	db 'Terça-feira',0
	db 'Quarta-feira',0
	db 'Quinta-feira',0
	db 'Sexta-feira',0
	db 'Sabado',0

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 ==============================================
;
; leitura da CMOS
; IN:
; 	edi= ponteiro para colocacao dos dados
; 	eax= endereco de leitura para a CMOS
; 	ecx= quantidade de dados desejados, ate 0xff
; OUT:
; 	ecx= 0, eax= ecx+eax, edi= edi+ecx

align 4
readcmos:
	push edi
.a:	push eax
	out 0x70, al
	in al, 0x71
	mov [edi], al
	inc edi
	pop eax
	inc al
	loop .a
	pop edi
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 2 ==============================================
;
; leitura da CMOS
; IN:
; OUT:

align 4
readcmosDT:	push edi
		push ecx
		mov al, 0
		mov ecx, 0xa		; 10 bytes
		mov edi,0x500		; buffer, rever
.a:		push eax
		out 0x70, al
		in al, 0x71
		mov [edi], al
		inc edi
		pop eax
		inc al
		loop .a
		pop ecx
		pop edi
		ret

; 	results:
; 	fs:0x500	segundos
; 	fs:0x502	minutos
; 	fs:0x504	horas
; 	fs:0x506	dia da semana
;	fs:0x507 	dia
;	fs:0x508	mes
;	fs:0x509	ano
; 	fs:0x50a	seculo


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 3 ==============================================
;
; IN:
; OUT:
align 4
CMOS_Time_Read:
	cli
	mov	al,0
	out	0x70,al
	jmp .a
.a:	in	al,0x71
	mov	[segundos],al
	mov	al,2
	out	70h,al
	jmp .b
.b:	in	al,71h
	mov	[minutos],al
	mov	al,4
	out	0x70,al
	jmp .c
.c:	in	al,0x71
	mov	[horas],al
	sti
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
;
; IN:
; OUT:
align 4
CMOS_Date_Read:

	mov	al,6
	out	70h,al
	jmp .a
.a:	in	al,71h
	mov	[dia_semana],al
	mov	al,7
	out	70h,al
	jmp .b
.b:	in	al,71h
	mov	[dia],al
	mov	al,8
	out	70h,al
	jmp .c
.c:	in	al,71h
	mov	[mes],al
	mov	al,9
	out	70h,al
	jmp .d
.d:	in	al,71h
	mov	[ano],al
	mov	al,32h
	out	70h,al
	jmp .e
.e:	in	al,71h
	mov	[seculo],al
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N X ==============================================
; leitura da CMOS
; IN:
; OUT:

CMOS_Time:
;	USES	ebx
;; refazer.....
;	call	CMOS_Time_Read
;	call	Unpack_BCD STDCALL,[cmos_hour]
;	mov	edx,3600
;	mul	edx
;	mov	ebx,eax
;	call	Unpack_BCD STDCALL,[cmos_min]
;	mov	edx,60
;	mul	edx
;	add	ebx,eax
;	call	Unpack_BCD STDCALL,[cmos_sec]
;	add	eax,ebx
ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
;
; IN:
; OUT:

; setting date,time,clock and alarm-clock
; add sys_settime at servetable as for ex. 22 fcn:
; 22 - SETTING DATE TIME, CLOCK AND ALARM-CLOCK
; ebx =0 - set time ecx - 00SSMMHH
; ebx =1 - set date ecx=00DDMMYY
; ebx =2 - set day of week ecx- 1-7
; ebx =3 - set alarm-clock ecx - 00SSMMHH
; out: 0:Ok - 1:wrong format - 2:battery low
sys_settime:
      	mov  ecx,eax
      	cli
      	mov  al,0x0d
      	out  0x70,al
      	in   al,0x71
      	bt   ax,7
      	jnc  bat_low
      	cmp  ecx,2    		;day of week
      	jne  nosetweek
      	test  ebx,ebx         	;test day of week
      	je   wrongtime
      	cmp  ebx,7
      	ja   wrongtime
      	mov  dx,0x70
      	call startstopclk
      	dec  edx
      	mov  al,6
      	out  dx,al
      	inc  edx
      	mov  al,bl
      	out  dx,al
      	jmp  endsettime
nosetweek:           		;set date
      	cmp  ecx,1
      	jne  nosetdate
      	cmp  bl,0x99      		;test year
      	ja   wrongtime
      	shl  ebx,4
      	cmp  bl,0x90
      	ja   wrongtime
      	cmp  bh,0x99      		;test month
      	ja   wrongtime
      	shr  ebx,4
      	test  bh,bh
      	je   wrongtime
      	cmp  bh,0x12
      	ja   wrongtime
      	shl  ebx,8
      	bswap ebx       		;ebx=00YYMMDD
      	test bl,bl       		;test day
      	je   wrongtime
      	shl  ebx,4
      	cmp  bl,0x90
      	ja   wrongtime
      	shr  ebx,4
      	cmp  bh,2       		;February
      	jne  testday
      	cmp  bl,0x29
      	ja   wrongtime
      	jmp  setdate
testday:
      	cmp  bh,8
      	jb   testday1    		;Aug-Dec
      	bt   bx,8
      	jnc  days31
      	jmp  days30
testday1:
      	bt   bx,8       		;Jan-Jul ex.Feb
      	jnc  days30
days31:
      	cmp  bl,0x31
      	ja   wrongtime
      	jmp  setdate
days30:
      	cmp  bl,0x30
      	ja   wrongtime
setdate:
      	mov  dx,0x70
      	call startstopclk
      	dec  edx
      	mov  al,7          		;set days
      	out  dx,al
      	inc  edx
      	mov  al,bl
      	out  dx,al
      	dec  edx
      	mov  al,8          		;set months
      	out  dx,al
      	inc  edx
      	mov  al,bh
      	out  dx,al
      	dec  edx
      	mov  al,9          		;set years
      	out  dx,al
      	inc  edx
      	shr  ebx,8
      	mov  al,bh
      	out  dx,al
      	jmp  endsettime
nosetdate:       ;set time or alarm-clock
      	cmp  ecx,3
      	ja   wrongtime
      	cmp  bl,0x23
      	ja   wrongtime
      	cmp  bh,0x59
      	ja   wrongtime
      	shl  ebx,4
      	cmp  bl,0x90
      	ja   wrongtime
      	cmp  bh,0x92
      	ja   wrongtime
      	shl  ebx,4
      	bswap ebx        		;00HHMMSS
      	cmp  bl,0x59
      	ja   wrongtime
      	shl  ebx,4
      	cmp  bl,0x90
      	ja   wrongtime
      	shr  ebx,4
      	mov  dx,0x70
      	call startstopclk
      	dec  edx
      	cmp  ecx,3
      	je   setalarm
      	xor  eax,eax   		;al=0-set seconds
      	out  dx,al
      	inc  edx
      	mov  al,bl
      	out  dx,al
      	dec  edx
      	mov  al,2       		;set minutes
      	out  dx,al
      	inc  edx
      	mov  al,bh
      	out  dx,al
      	dec  edx
      	mov  al,4       		;set hours
      	out  dx,al
      	inc  edx
      	shr  ebx,8
      	mov  al,bh
      	out  dx,al
      	jmp  endsettime
setalarm:
      	mov  al,1   		;set seconds for al.
      	out  dx,al
      	inc  edx
      	mov  al,bl
      	out  dx,al
      	dec  edx
      	mov  al,3    		;set minutes for al.
      	out  dx,al
      	inc  edx
      	mov  al,bh
      	out  dx,al
      	dec  edx
      	mov  al,5    		;set hours for al.
      	out  dx,al
      	inc  edx
      	shr  ebx,8
      	mov  al,bh
      	out  dx,al
      	dec  edx
      	mov  al,0x0b  		;enable irq's
      	out  dx,al
      	inc  dx
      	in   al,dx
      	bts  ax,5     		;set bit 5
      	out  dx,al
endsettime:
      	dec  edx
      	call startstopclk
wrongtime:
bat_low:
      	sti
      	ret


startstopclk:
    	mov  al,0x0b
    	out  dx,al
    	inc  dx
    	in   al,dx
    	btc  ax,7
    	out  dx,al
    	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 6 ==============================================
;
; IN:
; OUT:

align 4
sys_clock:
        	cli
@@:     	mov   al, 10
        	out   0x70, al
        	in    al, 0x71
        	test  al, al
        	jns   @f
        	mov   esi, 1
        	jmp   @b
@@:      	xor   al,al         ; seconds
        	out   0x70,al
        	in    al,0x71
        	movzx ecx,al
        	mov   al,02         ; minutes
        	shl   ecx,16
        	out   0x70,al
        	in    al,0x71
        	movzx edx,al
        	mov   al,04         ; hours
        	shl   edx,8
        	out   0x70,al
        	in    al,0x71
        	add   ecx,edx
        	movzx edx,al
        	add   ecx,edx
        	sti		; ecx= hora:min:seg
        	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 7 ==============================================
;
; IN:
; OUT:

align 4
sys_date:
        	cli
        	mov     al,6            ; day of week
        	out     0x70,al
        	in      al,0x71
        	mov     ch,al
        	mov     al,7            ; date
        	out     0x70,al
        	in      al,0x71
        	mov     cl,al
        	mov     al,8            ; month
        	shl     ecx,16
        	out     0x70,al
        	in      al,0x71
        	mov     ch,al
        	mov     al,9            ; year
        	out     0x70,al
        	in      al,0x71
        	mov     cl,al
        	sti
        	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 8 ==============================================
;
; IN:
; OUT:

;align 2   	; dx= pos x,y	, ah = cor
;ImprimeTimeModoTexto:
;	push di
;	call end_linear_tela	; calcula endereço linear
;	push ecx
;	call sys_clock
;;	cmp cl, 9
;;	ja .b
;;	mov al, '0'
;;	call imp_bytehex
;.b:	mov al, cl
;	call imp_bytehex
;	mov al , 0x3a
;	call Imp_al_vtexto
;;	cmp ch, 9
;;	ja .a
;;	mov al, '0'
;;	call imp_bytehex
;.a:	mov al, ch
;	call imp_bytehex
;	pop ecx
;	pop di
;	ret
;

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 8 ==============================================
;
; Get date from CMOS and pack day,month,year in AX
; IN: 	nothing
; OUT:	DATE   	bits  0..4   : day of month 0..31
;              	5..8   : month of year 1..12
;              	9..15  : count of years from 1980
align 4

get_date_for_file:

	mov   al,0x7        ;day
	out   0x70,al
	in    al,0x71
	call  bcd2bin
	ror   eax,5

	mov   al,0x8        ;month
	out   0x70,al
	in    al,0x71
	call  bcd2bin
	ror   eax,4

	mov   al,0x9        ;year
	out   0x70,al
	in    al,0x71
	call  bcd2bin
	add   ax,20         ;because CMOS return only the two last
	                    ;digit (eg. 2000 -> 00 , 2001 -> 01) and we
	rol   eax,9         ;need the difference with 1980 (eg. 2001-1980)
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 8 ==============================================
;
; Get time from CMOS and pack hour,minute,second in AX
; IN:	nothing
; OUT:	TIME   	bits  0..4   : second (the low bit is lost)
;              	5..10  : minute 0..59
;              	11..15 : hour 0..23
align 4
get_time_for_file:

	mov   al,0x0        ;second
	out   0x70,al
	in    al,0x71
	call  bcd2bin
	ror   eax,6

	mov   al,0x2        ;minute
	out   0x70,al
	in    al,0x71
	call  bcd2bin
	ror   eax,6

	mov   al,0x4        ;hour
	out   0x70,al
	in    al,0x71
	call  bcd2bin
	rol   eax,11
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 8 ==============================================
;
; IN:
; OUT:





