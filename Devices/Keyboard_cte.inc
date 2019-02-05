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
; @Id: Keyboard_cte.inc  - 2005-3-07, cb-toledo
; Constant definitions for keyboard interruption routine
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------
;
; INFORMAÇÕES:

; INICIALIZAÇÃO:
; o video sempre carrega a tabela 860 para qualquer teclado
; as variáveis r472 e r474 devem serem inicializadas na partida.
; provisoriamente são definidas com ret (não faz nada)
; definir o tempo de espera padrão na variável fs: rdbe

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; DEFINIÇOES:
;
;variaveis do teclado
;r470	db 0	; flags	#7- modo 1 - usada pela tecla ctrl-dir para comandos
;		;	#6- E1 pressionado
;		;	#5- E0 pressionado
;		;	#4- alt pressionado
;		;	#3- shift pressionado
;		;	#2- caps lock pressionado
;		;	#1- num lock pressionado
;		;	#0- scroll lock pressionado
;
;r471	db 0	; flags 	#7- comando válido
;		;	#6- ctrl dir pressionada
;		;	#5- enter numerico pressionada
;		;	#4- alt direito pressionada
;		;	#3- shift direito pressionada
;		;	#2- #2, #1, #0 - tipo de acento:
;		;	#1-
;		;	#0-
;
;r47a	rb 1	; CARACTERE PRESSIONADO
;r47b	rb 1	; #7=1= digitação válida. colocada antes da tecla ser liberada.
; o programa que usa a digitação pode ficar aguardando a liberação da tecla analisando o bit #6.
;		; #6=1 digitação pressionada, =0 tecla liberada.
;		; #4,5=reservado
;		; #0,1,2,3= indice da tecla
;
;; op teclado
;r472	rw 1	; ponteiro para rotina de escrita
;r474	rw 1	; ponteiro para rotina de comando (padrão)
;
;r476	rw 1	; ponteiro do caractere pressionado
;r478	rw 1	; ponteiro do indice da ultima tecla pressionada
;
;r47c	rd 1	; reservado para o contador descendente de tempo de espera do teclado
;
;r47e	rb 1	; controle dos leds			#7- caractere digitado
;		;				#6- X
;		;				#5- teclado não respondeu
;		;				#4- FE recebido
;		;				#3- FA recebido
;		;				#2-
;		;				#1-
;		;				#0- #2, #1, #0 - espelho leds
;
;r47f	rb 1	; salva a resposta do teclado
;
;
; códigos para os comandos tabela de tradução usada nas tabelas de comandos
; especiais 0, 1 e 2.
; 00- nada	01- esc		02- backscape	03- tab		04- enter
; 05- s_cima	06- s_esquerda	07- s_direira	08- s_baixo	09- home
; 0a- end		0b- pg_up		0c- pg_dn		0d- insert	0e- delete
; 0f- sh esc	10- sh BS		11- alt esc	12- alt BS	13- alt tab
; 14- alt enter	15- alt s_cima	16- alt s_esq	17- alt s_dir	18- alt s_baixo
; 19- alt home	1a- alt end	1b- alt pg_up	1c- alt pg_dn	1d- alt insert
; 1e- alt delete	1f- sh tab	20- sh enter	21- F1		22- F2
; 23- F3		24- F4		25- F5		26- F6		27- F7
; 28- F8		29- F9		2a- F10		2b- F11		2c- F12
; 2d- S F1	2e- S F2		2f- S F3		30- S F4		31- S F5
; 32- S F6	33- S F7		34- S F8		35- S F9		36- S F10
; 37- S F11	38- S F12		39- A F1		3a- A F2		3b- A F3
; 3c- A F4	3d- A F5		3e- A F6 		3f- A F7		40- A F8
; 41- A F9	42- A F10		43- A F11		44- A F12		45- res
; 46- res		47- res		48- res		49- reservado	4a- reservado
; 4b- reservado	4c- reservado	4d- reservado	4e- reservado	4f- reservado
;
; observaçoes: qualquer digitaçao valida (comando ou caractere) sempre seta #7[r47b]=1
; A tecla ctrl esq tem uso dedicado. A tecla ctrl dir aciona as rotinas predefinidas
; e programáveis.

; alt-sh (kp /, *, -, +): imprimem somente caractere


;----------------------------------------------------------------------------------------------
;
; TABELAS DO TECLADO: tabela 860 (portugues)
; id_tec 	db 'tec portugues-set01-002(05.2005)'

ini_teclado:
align 16
TabInd:

; tabela de indice normal, primeiro byte: modo normal, segundo byte: modo comando
; formato do indice:
; #7=1 normal, aceita comando ( pode ser imprime caractere, ou marca acento)
; #7=0 rotina de controle

;              ;00- Para pegar o código certo deve-se usar a base da tabela-1
     db 0x8c   ;01-*1  tec esc 81
     db 0x8a   ;02-  tec 1
     db 0x8a   ;03-  tec 2
     db 0x8a   ;04-  tec 3
     db 0x8a   ;05-  tec 4
     db 0x8a   ;06-  tec 5
     db 0x8b   ;07-  tec 6   com shift tem acento trema
     db 0x8a   ;08-  tec 7
     db 0x8a   ;09-  tec 8
     db 0x8a   ;0A-  tec 9
     db 0x8a   ;0B-  tec 0
     db 0x8a   ;0C-  tec -
     db 0x8a   ;0D-  tec =
     db 0x8c   ;0E-*2  tec backspace 82
     db 0x8c   ;0F-*3  tec tab       83
     db 0x8a   ;10-  tec q
     db 0x8a   ;11-  tec w
     db 0x8a   ;12-  tec e
     db 0x8a   ;13-  tec r
     db 0x8a   ;14-  tec t
     db 0x8a   ;15-  tec y
     db 0x8a   ;16-  tec u
     db 0x8a   ;17-  tec i
     db 0x8a   ;18-  tec o
     db 0x8a   ;19-  tec p
     db 0x8b   ;1A-  tec ?  marca acento
     db 0x8a   ;1B-  tec [/{
     db 0x8c   ;1C-*4  tec enter	84
     db 0x8c   ;1D-**5  tec L_ctrl
     db 0x8a   ;1E-  tec a
     db 0x8a   ;1F-  tec s
     db 0x8a   ;20-  tec d	* (rotina predefinida 1...o indice deve ser 0x8d)
     db 0x8a   ;21-  tec f
     db 0x8a   ;22-  tec g
     db 0x8a   ;23-  tec h
     db 0x8a   ;24-  tec j
     db 0x8a   ;25-  tec k
     db 0x8a   ;26-  tec l
     db 0x8a   ;27-  tec ç
     db 0x8a   ;28-  tec ~ marca acento
; as teclas de acento, tem na tabela de caracteres o valor dos acentos: 01´, 02`, 03~, 04^, 05¨
     db 0x8a   ;29-  tec `
     db 0x01   ;2A-  tec L_shift, no break code executa a rotina 7
     db 0x8b   ;2B-  tec \  marca acento
     db 0x8a   ;2C-  tec z
     db 0x8a   ;2D-  tec x
     db 0x8a   ;2E-  tec c
     db 0x8a   ;2F-  tec v
     db 0x8a   ;30-  tec b
     db 0x8a   ;31-  tec n
     db 0x8a   ;32-  tec m
     db 0x8a   ;33-  tec ,
     db 0x8a   ;34-  tec .
     db 0x8a   ;35-  tec /
     db 0x02   ;36-  tec R_shift, no break code executa a rotina 8
     db 0x8a   ;37-  tec
     db 0x03   ;38-  tec L_alt, no break code executa a rotina 9
     db 0x8a   ;39-  tec space
     db 0x06   ;3A-  tec Caps Lock*******************************
     db 0x8c   ;3B-*6  tec f1 	85
     db 0x8c   ;3C-*7  tec f2		86
     db 0x8c   ;3D-*8  tec f3		87
     db 0x8c   ;3E-*9  tec f4		88
     db 0x8c   ;3F-*10  tec f5	89
     db 0x8c   ;40-*11  tec f6	8a
     db 0x8c   ;41-*12  tec f7	8b
     db 0x8c   ;42-*13  tec f8	8c
     db 0x8c   ;43-*14  tec f9	8d
     db 0x8c   ;44-*15  tec f10	8e
     db 0x05   ;45-  tec Num Locl********************************
     db 0x04   ;46-  tec Scroll Lock*****************************
; teclas numericas (kp)
     db 0x8a   ;47-  tec kp7
     db 0x8a   ;48-  tec kp8
     db 0x8a   ;49-  tec kp9
     db 0x8a   ;4A-  tec ?
     db 0x8a   ;4B-  tec kp4
     db 0x8a   ;4C-  tec kp5
     db 0x8a   ;4D-  tec kp6
     db 0x8a   ;4E-  tec ?
     db 0x8a   ;4F-  tec kp1
     db 0x8a   ;50-  tec kp2
     db 0x8a   ;51-  tec kp3
     db 0x8a   ;52-  tec kp0
     db 0x8a   ;53-  tec kp.
     db 0x8c   ;54-*16  tec SysRq (alt-PrintScreen)...pode ter uma rotina especial para ela.
     db 0x00   ;55-  tec
     db 0x00   ;56-  tec ?
     db 0x8c   ;57-*17  tec f11	8f
     db 0x8c   ;58-*18  tec f12	90


align 16
;; tabelas de caracteres (teclado PORTUGUES)
;;      CL=0           CL=1
;;      SH=0 SH=1 ALT  SH=1 SH=0
TabCar:
     db 0x01,0x00,0x00,0x00,0x00   ; 	01-  tec esc
     db 0x31,0x21,0xfb,0x21,0x31   ; 	02-  tec 1	1,!,¹,!,1
     db 0x32,0x22,0xfd,0x22,0x32   ; 	03-  tec 2	2,@,²,@,2
     db 0x33,0x23,0xfc,0x23,0x33   ; 	04-  tec 3	3,#,³,#,3
     db 0x34,0x24,0x9C,0x24,0x34   ;  	05-  tec 4	4,$,£,$,4
     db 0x35,0x25,0xbd,0x25,0x35   ;  	06-  tec 5	5,%,¢,%,5
     db 0x36,0x05,0x00,0x36,0x05   ;  	07-  tec 6	6,¨, ,¨,6    ¨:f9 trocado por 05
     db 0x37,0x26,0x00,0x37,0x26   ;  	08-  tec 7	7,&, ,&,7
     db 0x38,0x2A,0x00,0x38,0x2A   ;  	09-  tec 8	8,*, ,*,8    *:2a
     db 0x39,0x28,0x00,0x39,0x28   ;  	0A-  tec 9	9,(, ,(,9	   (:28
     db 0x30,0x29,0x9d,0x30,0x29   ;  	0B-  tec 0	0,), ,),0    ):29
     db 0x2E,0x5F,0xf2,0x2E,0x5F   ;  	0C-  tec -/_	-,_, ,_,-	   -:2e/_:5f
     db 0x3D,0x2B,0xf5,0x3D,0x2B   ;  	0D-  tec =/+	=,+, ,+,=    =:3d/+:2b
     db 0x00,0x00,0x00,0x00,0x00   ;  	0E-  tec backspace   , , , ,
     db 0x00,0x00,0x00,0x00,0x00   ;  	0F-  tec tab         , , , ,
     db 0x71,0x51,0x2f,0x51,0x51   ;  	10-  tec q	q,Q, ,Q,Q
     db 0x77,0x57,0x3f,0x57,0x57   ; 	11-  tec w	w,W, ,W,W
     db 0x65,0x45,0xf8,0x45,0x45   ;  	12-  tec e	e,E, ,E,E
     db 0x72,0x52,0x00,0x52,0x52   ;  	13-  tec r	r,R, ,R,R
     db 0x74,0x54,0x00,0x54,0x54   ;  	14-  tec t	t,T, ,T,T
     db 0x79,0x59,0x00,0x59,0x59   ;  	15-  tec y	y,Y, ,Y,Y
     db 0x75,0x55,0x00,0x55,0x55   ;  	16-  tec u	u,U, ,U,U
     db 0x69,0x49,0x00,0x49,0x49   ;  	17-  tec i	i,I, ,I,I
     db 0x6F,0x4F,0x9b,0x4F,0x4F   ;  	18-  tec o	o,O, ,O,O
     db 0x70,0x50,0x00,0x50,0x50   ;  	19-  tec p	p,P, ,P,P
     db 0x01,0x02,0x00,0x01,0x02   ;  	1A-  tec ´/`	´,`, ,´,`  mesmo com CL=1 shift deve ser pressionado sh para `
     db 0x5B,0x7B,0xa6,0x5B,0x7B   ;  	1B-  tec [/{	[,{, ,[,{
     db 0x00,0x00,0x00,0x00,0x00   ;  	1C-  tec enter	 , , , ,
     db 0x00,0x00,0x00,0x00,0x00   ;  	1D-  tec L_ctrl	 , , , ,
     db 0x61,0x41,0x00,0x41,0x41   ;  	1E-  tec a 	a,A, ,A,A
     db 0x73,0x53,0x00,0x53,0x53   ;  	1F-  tec s	s,S, ,S,S
     db 0x64,0x44,0x00,0x44,0x44   ;  	20-  tec d	d,D, ,D,D
     db 0x66,0x46,0x9f,0x46,0x46   ;  	21-  tec f	f,F, ,F,F
     db 0x67,0x47,0x00,0x47,0x47   ;  	22-  tec g	g,G, ,G,G
     db 0x68,0x48,0x00,0x48,0x48   ;  	23-  tec h	h,H, ,H,H
     db 0x6A,0x4A,0x00,0x4A,0x4A   ;  	24-  tec j	j,J, ,J,J
     db 0x6B,0x4B,0x00,0x4B,0x4B   ;  	25-  tec k	k,K, ,K,K
     db 0x6C,0x4C,0x00,0x4C,0x4C   ;  	26-  tec l	l,L, ,L,L
     db 0x87,0x80,0x00,0x80,0x80   ;  	27-  tec ç	ç,Ç, ,Ç,Ç
     db 0x03,0x04,0x00,0x03,0x04   ;  	28-  tec ~/^	~,^, ,~,^ mesmo com CL=1 deve ser pressionado sh para ^
     db 0x5D,0x7D,0xa7,0x5D,0x7D   ;  	29-  tec ]/}	],}, ,],}
     db 0x00,0x00,0x00,0x00,0x00   ;  	2A-  tec L_shift	 , , , ,
     db 0x5C,0x7C,0xdd,0x5C,0x7C   ;  	2B-  tec \/|	\,|, ,\,|
     db 0x7A,0x5A,0x00,0x5A,0x5A   ;  	2C-  tec z	z,Z, ,Z,Z
     db 0x78,0x58,0x9e,0x58,0x58   ;  	2D-  tec x	x,X, ,X,X
     db 0x63,0x43,0xb8,0x43,0x43   ;  	2E-  tec c	c,C, ,C,C
     db 0x76,0x56,0xe7,0x56,0x56   ;  	2F-  tec v	v,V, ,V,V
     db 0x62,0x42,0xe8,0x42,0x42   ;  	30-  tec b	b,B, ,B,B
     db 0x6E,0x4E,0x00,0x4E,0x4E   ;  	31-  tec n	n,N, ,N,N
     db 0x6D,0x4D,0xe6,0x4D,0x4D   ;  	32-  tec m	m,M, ,M,M
     db 0x2C,0x3C,0xae,0x3C,0x2C   ;  	33-  tec ,<	,,<, ,,,<
     db 0x2E,0x3E,0xaf,0x3E,0x2E   ;  	34-  tec .>	.,>, ,.,>
     db 0x3B,0x3A,0xa8,0x3B,0x3A   ;  	35-  tec ;/:	;,:, ,;,:
     db 0x00,0x00,0x00,0x00,0x00   ;  	36-  tec R_shift	 , , , ,
     db 0x2F,0x3F,0xf8,0x2F,0x3F   ;  	37-  tec ?/	/,?, ,?,/
     db 0x00,0x00,0x00,0x00,0x00   ;  	38-  tec L_alt	 , , , ,
     db 0x20,0xB1,0xB0,0x20,0xB2   ;  	39-  tec space	 , , , ,


align 16
TabNum:
;       0    1    2    3    4    5    6    7
;       NL=0	        NL=1
;       CL=0      CL=1      CL=0      CL=1
;       SH=0 SH=1 SH=0 SH=1 SH=0 SH=1 SH=0 SH=1
     db 0x07,0x00,0x00,0x37,0x07,0xC9,0x03,0xDA   ;47-  tec kp7 0
     db 0x0F,0x00,0x00,0x38,0x1E,0xCB,0x18,0xC2   ;48-  tec kp8 1
     db 0x08,0x00,0x00,0x39,0x08,0xBB,0x04,0xBF   ;49-  tec kp9 2
     db 0x2D,0x00,0x00,0x2D,0x2D,0x2D,0x2D,0x2D   ;4A-  tec -   3
     db 0xDD,0x00,0x00,0x34,0x11,0xCC,0x1B,0xC3   ;4B-  tec kp4 4
     db 0xFE,0x00,0x00,0x35,0x01,0xCE,0x0F,0xC5   ;4C-  tec kp5 5
     db 0xDE,0x00,0x00,0x36,0x10,0xB9,0x1A,0xB4   ;4D-  tec kp6 6
     db 0x2B,0x00,0x00,0x2B,0x2B,0x2B,0x2B,0x2B   ;4E-  tec +   7
     db 0x09,0x00,0x00,0x31,0x09,0xC8,0x05,0xC0   ;4F-  tec kp1 8
     db 0xDC,0x00,0x00,0x32,0x1F,0xCA,0x19,0xC1   ;50-  tec kp2 9
     db 0x0A,0x00,0x00,0x33,0x0A,0xBC,0x06,0xD9   ;51-  tec kp3 a
     db 0x0B,0x00,0x00,0x30,0x0B,0xCD,0x0E,0xC4   ;52-  tec kp0 b
     db 0x2E,0x00,0x00,0x2C,0x0C,0xBA,0x0D,0xB3   ;53-  tec kp. c
; podem ser desprezados
;     db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   ;54-  tec    d
;     db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   ;55-  tec    e
;     db 0x01,0x3E,0x3C,0x3C,0x3E,0x3C,0x01,0x3E   ;56-  tec ?  f



; tabelas de comando
align 16
TabCom:	 ; ALT  SH=1 SH=0      ; 00- Para pegar o código certo deve-se usar a base da tabela-1
	db 0x11,0x0F,0x01      ;*01-  tec esc
	db 0x00,0x00,0x00      ; 02-  tec 1
	db 0x00,0x00,0x00      ; 03-  tec 2
	db 0x00,0x00,0x00      ; 04-  tec 3
	db 0x00,0x00,0x00      ; 05-  tec 4
	db 0x00,0x00,0x00      ; 06-  tec 5
	db 0x00,0x00,0x00      ; 07-  tec 6
	db 0x00,0x00,0x00      ; 08-  tec 7
	db 0x00,0x00,0x00      ; 09-  tec 8
	db 0x00,0x00,0x00      ; 0A-  tec 9
	db 0x00,0x00,0x00      ; 0B-  tec 0
	db 0x00,0x00,0x00      ; 0C-  tec -
	db 0x00,0x00,0x00      ; 0D-  tec =
	db 0x12,0x10,0x02      ;*0E-  tec backspace
	db 0x13,0x1F,0x03      ;*0F-  tec tab
	db 0xD2,0x00,0x00      ; 10-  tec q...* (rot 02)- ctrldir+alt+q
	db 0x00,0x00,0x00      ; 11-  tec w
	db 0x00,0x00,0x00      ; 12-  tec e
	db 0xD6,0x00,0x00      ; 13-  tec r...* (rot 06)- ctrldir+alt+r
	db 0xD1,0x00,0x00      ; 14-  tec t...* (rot 01)- ctrldir+alt+t
	db 0x00,0x00,0x00      ; 15-  tec y
	db 0x00,0x00,0x00      ; 16-  tec u
	db 0x00,0x00,0x00      ; 17-  tec i
	db 0x00,0x00,0x00      ; 18-  tec o
	db 0x00,0x00,0x00      ; 19-  tec p
	db 0x00,0x00,0x00      ; 1A-  tec ?  marca acento
	db 0x00,0x00,0x00      ; 1B-  tec ?  marca acento
	db 0x14,0x20,0x04      ;*1C-  tec enter (inclui enter numerico)
	db 0xD5,0x00,0x00      ; 1D-  tec L_ctrl.....(rot 05)- ctrldir+alt+l............
	db 0x00,0x00,0x00      ; 1E-  tec a
	db 0x00,0x00,0x00      ; 1F-  tec s
	db 0xD4,0x00,0xD3      ; 20-  tec d...* (rot 04)- ctrldir+alt+d.................
	db 0xD3,0x00,0x00      ; 21-  tec f...* (rot 03)- ctrldir+alt+f
	db 0x00,0x00,0x00      ; 22-  tec g
	db 0x00,0x00,0x00      ; 23-  tec h
	db 0x00,0x00,0x00      ; 24-  tec j
	db 0x00,0x00,0x00      ; 25-  tec k
	db 0x00,0x00,0x00      ; 26-  tec l
	db 0x00,0x00,0x00      ; 27-  tec ç
	db 0x00,0x00,0x00      ; 28-  tec ?
	db 0x00,0x00,0x00      ; 29-  tec `
	db 0x00,0x00,0x00      ; 2A-  tec L_shift****contem codigo para KP enter
	db 0x00,0x00,0x00      ; 2B-  tec \  marca acento
	db 0x00,0x00,0x00      ; 2C-  tec z
	db 0x00,0x00,0x00      ; 2D-  tec x
	db 0x00,0x00,0x00      ; 2E-  tec c
	db 0x00,0x00,0x00      ; 2F-  tec v
	db 0x00,0x00,0x00      ; 30-  tec b
	db 0x00,0x00,0x00      ; 31-  tec n
	db 0x00,0x00,0x00      ; 32-  tec m
	db 0x00,0x00,0x00      ; 33-  tec ,
	db 0x00,0x00,0x00      ; 34-  tec .
	db 0x00,0x00,0x00      ; 35-  tec /
	db 0x00,0x00,0x00      ; 36-  tec R_shift****contem codigo para ShiftPrintScreen
	db 0x00,0x00,0x00      ; 37-  tec
	db 0x00,0x00,0x00      ; 38-  tec L_alt******contem codigo para KP /
	db 0x00,0x00,0x45      ; 39-  tec space
	db 0x00,0x00,0x00      ; 3A-  tec Caps Lock*******************************
	db 0x39,0x2D,0x21      ;*3B-  tec f1
	db 0x3A,0x2E,0x22      ;*3C-  tec f2
	db 0x3B,0x2F,0x23      ;*3D-  tec f3		23- F3	2f- S F3  3b- A F3
	db 0x3C,0x30,0x24      ;*3E-  tec f4		24- F4	30- S F4  3c- A F4
	db 0x3D,0x31,0x25      ;*3F-  tec f5
	db 0x3E,0x32,0x26      ;*40-  tec f6
	db 0x3F,0x33,0x27      ;*41-  tec f7
	db 0x40,0x34,0x28      ;*42-  tec f8
	db 0x41,0x35,0x29      ;*43-  tec f9
	db 0x42,0x36,0x2A      ;*44-  tec f10
	db 0x00,0x00,0x00      ; 45-  tec Num Locl********************************
	db 0x00,0x00,0x00      ; 46-  tec Scroll Lock*****************************
	db 0x00,0x00,0x00      ; 47-  tec kp7
	db 0x00,0x00,0x00      ; 48-  tec kp8
	db 0x00,0x00,0x00      ; 49-  tec kp9
	db 0x00,0x00,0x00      ; 4A-  tec ?
	db 0x00,0x00,0x00      ; 4B-  tec kp4
	db 0x00,0x00,0x00      ; 4C-  tec kp5
	db 0x00,0x00,0x00      ; 4D-  tec kp6
	db 0x00,0x00,0x00      ; 4E-  tec ?
	db 0x00,0x00,0x00      ; 4F-  tec kp1
	db 0x00,0x00,0x00      ; 50-  tec kp2
	db 0x00,0x00,0x00      ; 51-  tec kp3
	db 0x00,0x00,0x00      ; 52-  tec kp0
	db 0x00,0x00,0x00      ; 53-  tec kp.
	db 0x00,0x00,0x00      ; 54-  tec  (alt-PrintScreen: SysRq)
	db 0x00,0x00,0x00      ; 55-  tec
	db 0x00,0x00,0x00      ; 56-  tec ?
	db 0x43,0x37,0x2B      ; 57-  tec f11
	db 0x44,0x38,0x2C      ; 58-  tec f12

; Observaçao: modificar para otimizar o uso da tabela. separar comandos dos ponteiros de
; rotinas predefinidas

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; os comandos especiais são disponíveis apenas no set 01 e no set 02
TabCom_esp:
;	existem códigos diferentes quando NL=1,
; para o modo normal ou shift-tecla quando NL=0 o código tem dois bytes: e0 xx
; para NL=1 o scancode tem 4 bytes: e0 2a e0 xx. 2a sempre aciona o bit de shift pressionado
; com shift esquerdo pressionado, o scancode tem 4 bytes: e0 aa e0 xx
; com shift direito pressionado, o scancode tem 4 bytes: e0 b6 e0 xx
	;  a=0  a=1
	db 0x09,0x19 ; 47 - home
	db 0x05,0x15 ; 48 - up arrow
	db 0x0B,0x1B ; 49 - pg up
	db 0x00,0x00 ; 4a - X
	db 0x06,0x16 ; 4b - left arrow
	db 0x00,0x00 ; 4c - X
	db 0x07,0x17 ; 4d - right arrow
	db 0x00,0x00 ; 4e - X
	db 0x0A,0x1A ; 4f - end
	db 0x08,0x18 ; 50 - down arrow
	db 0x0C,0x1C ; 51 - pg dn
	db 0x0D,0x1D ; 52 - ins
	db 0x0E,0x1E ; 53 - del


; tabelas de acentos
tab_acent: ; ´    `    ~    ^    ¨    °
	db 0xb5,0xb7,0xc7,0xb6,0x8e;,0x8f  ; A
	db 0x90,0xd4,0x00,0xd2,0xd3;,0x00  ; E
	db 0xd6,0xde,0x00,0xd7,0xd8;,0x00  ; I
	db 0xe0,0xa3,0xe5,0xe2,0x99;,0x00  ; O
	db 0xe9,0xeb,0x00,0xea,0x9a;,0x00  ; U
	db 0xed,0x00,0xa5,0x00,0x00;,0x00  ; Y, ,Ñ, , , ,
;          db 0x80,0x00,0xa5,0x00,0x0;0,0x00  ; Ç, ,Ñ, , , ,

	db 0xa0,0x85,0xc6,0x83,0x84;,0x86  ; a
	db 0x82,0x8a,0x00,0x88,0x89;,0x00  ; e
	db 0xa1,0x8d,0x00,0x8c,0x8b;,0x00  ; i
	db 0xa2,0x95,0xe4,0x93,0x94;,0x00  ; o
	db 0xa3,0x97,0x00,0x96,0x81;,0x00  ; u
          db 0x3c,0x00,0xa4,0x00,0x98;,0x00  ; y, ,ñ, ,ÿ, ,
;          db 0x87,0x00,0xa4,0x00,0x0;0,0x00  ; ç, ,ñ, , , ,
                                     ;
b_acent:
          db 0x27,0x60,0x7e,0x5e,0x7f,0x00  ; especiais:
;*****************************************************************************************
;
; Os programas apontados aqui são executados diretamente por toque na tecla correspondente
; as rotinas dedicadas devem ser inicializadas na partida.
align 16
b_rot_ded:
	RotDed0	dd rot_t0	;
	RotDed1 	dd DPexcD
	RotDed2	dd rot_t0
	RotDed3 	dd rot_t0
	RotDed4	dd rot_t0
	RotDed5 	dd rot_t0
;	RotDed6	dd	rot_t0
;	RotDed7 	dd	rot_t0
;	RotDed8	dd	rot_t0
;	RotDed9 	dd	rot_t0
;	RotDeda	dd	rot_t0
;	RotDedb 	dd	rot_t0
;	RotDedc	dd	rot_t0
;	RotDedd 	dd	rot_t0
;	RotDede	dd	rot_t0
;	RotDedf 	dd	rot_t0


key_wait_time		dd 0x15600    	; tempo basico de espera do teclado
rot_vigia			dd rot_t0		; aponta inicialmente "ret"

align 16
base_rt:
	dd rot_t0	; despreza. não faz nada além de um ret
	dd rot_t1	; shift esquerdo
	dd rot_t2	; shift direito
	dd rot_t3	; alt esquerdo
	dd rot_t4	; SL
	dd rot_t5	; NL
	dd rot_t6	; CL
	dd rot_t7	; desmarca shifh esquerdo
	dd rot_t8	; desmarca shift direito
	dd rot_t9	; desmarca alt
	dd rot_ta ; imprime caractere em r47a (caractere digitado)
	dd rot_tb	; marca acento
	dd rot_tc	; busca comando nas tabelas de comandos 0, 1 e 2.
          dd rot_t0 ; livre
	dd rot_t0 ; livre
	dd rot_t0 ; livre


;-----------------------------------------------------------------------------------------

; rotinas pre-definidas. Executadas através da combinaçao CTRL-DIR + tecla (ver comandos)
;
r_predef_tc:
 	dd rot_t0		; D0
	dd rot_t0 	; D1 - ativa bit T na pilha para acionar passo a passo
	dd rot_t0	; D2 - dispara modo depurador
	dd rot_t0	; D3 - desativa flag de depuracao
	dd rot_t0	; D4 - ativa flag de depuracao
	dd rot_t0		; D5
	dd rot_t0		; D6
	dd rot_t0	; D7 - dispara bloco iniciar	;


;-----------------------------------------------------------------------------------------

; Novas idéias: adotar uma tabela de indice que:
; numeros menores de oitenta representam rotinas como as atuais.
; numeros maiores que oitenta representam diretamente os comandos, retirando apenas o bit7
; isto permite 80 comandos diretosx3 (alt, sh e nsh) e 7f rotinas internas.
; com o numeros do indice busca-se na tabCom o código final
; por enquanto usar os espaços zerados de TabCom para posicionar as rotinas pre-definidas
; como está tudo que é controle na TabCom é repetido com a tecla ctrl dir
; as alteraçoes permitem definir ambas.
;-----------------------------------------------------------------------------------------