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
; @Id: variables.inc  - 2008-1-20, fb-toledo & cb-toledo
; OS variables equate definitions
; For compilation use Flat Assembler (http://flatassembler.net)
;
;-----------------------------------------------------------------------------------------


;=========================== K E Y B O A R D =============================================0x0000

r470	equ (OS_BASE+0x0000);db -; flags #7- mode 1 - used by ctrl_dir key for commands
r471	equ (OS_BASE+0x0001);db -; flags #7- válid command
KB_char	equ (OS_BASE+0x0002);db -; last pressioned key
r47b	equ (OS_BASE+0x0003);db -; #7=1= válid alfanumeric key. (come first to key liberation)
r472	equ (OS_BASE+0x0004);rd -; point to keyboard write procedure
;r474	equ 0x6		;rw -; point to routine that writes key into command buffer

; escrever na posiçao 8 provoca exc D no virtual box


r47c	equ (OS_BASE+0x000c);rd 	; keyboard decrescent counter

KB_leds	equ (OS_BASE+0x0010);rb 	; leds control	- #7- caractere digitado

r47f	equ (OS_BASE+0x0011);rb	; keyboard

r476	equ (OS_BASE+0x0012);rw 	; caracter point of the last pressioned key

scancode	equ (OS_BASE+0x0014);rb	; main scancode from keyboard

kb_res	equ (OS_BASE+0x0015);rb	; free

r478	equ (OS_BASE+0x0016);rw 	; index point of the last pressioned key

;-----------------------------------------------------------------------------------------


;======================== M E M O R Y ====================================================

mapPage		equ (OS_BASE+0x0018); dd - Page bitmap point
size_mapPage	equ (OS_BASE+0x001c); dd - size of page bitmap

; Heap definido em Ger_memory.inc (init_Memory)

ini_Heap		equ (OS_BASE+0x0020); dd - heap start - constant
size_Heap		equ (OS_BASE+0x0024); dd - heap size


;-----------------------------------------------------------------------------------------


;======================== T A S K   S W I T C H ==========================================


Timer_cont	equ (OS_BASE+0x0028); dw - timer counter ticks

Timer_flags	equ (OS_BASE+0x002a)
m_counter		equ (OS_BASE+0x002b); db - waiting counter

  ; Descritor da lista de tarefas ================
    Task_List_Ini	equ (OS_BASE+0x002c) ; dd - start of task list
    Task_List_End	equ (OS_BASE+0x0030) ; dd - end of task list
    Task_Current	equ (OS_BASE+0x0034) ; dd - current task
    QtdeBytes_List	equ (OS_BASE+0x0038) ; dd - Quantidade de bytes reservados
 
  ;-----------------------------------------------
 
  ;====== Item task list definition ======

      Task_Prox	 	equ 0x0	; dd - Ponteiro para a proxima tarefa
      Task_Ant	 	equ 0x4	; dd - Ponteiro para a tarefa anterior
      Task_Cont	 	equ 0x8	; dw - Quanto tempo fica sendo executado
      Task_Reserved		equ 0xa	; dw - free
      Task_Pilha_n0	 	equ 0xc	; dd - ponteiro da pilha
      Task_PageTable 	equ 0x10	; db - Ponteiro para a tabela de paginas (memoria)
				; rb 3 - free
				
    ; Heap padrao do programa
      Task_Heap_Base	equ 0x14	; dd - Inicio do heap
      Task_Heap_Size	equ 0x18	; dd - Tamanho do heap

    ; Buffer circular de eventos para o sistema (Memory/Buffer_Circular.inc)
      Task_EventoSPR	equ 0x1c	; db - Ponteiro de leitura do buffer de evento
      Task_EventoSPW	equ 0x1d	; db - Ponteiro de escrita do buffer do teclado
      Task_EventoSSize	equ 0x1e	; db - Tamanho do buffer
      Task_EventoSStatus	equ 0x27  ; db - Status do buffer (0= livre, 1=ocupado)
      Task_EventoSBuffer	equ 0x20	; dd - Ponteiro do buffer de evento do sistema

    ; Buffer circular de eventos para o usuario (Memory/Buffer_Circular.inc)
      Task_EventoPR		equ 0x24	; db - Ponteiro de leitura do buffer de evento
      Task_EventoPW		equ 0x25	; db - Ponteiro de escrita do buffer de evento
      Task_EventoSize	equ 0x26	; db - Tamanho do buffer
      Task_EventoStatus	equ 0x27  ; db - Status do buffer (0= livre, 1=ocupado)
      Task_EventoBuffer	equ 0x28	; dd - Ponteiro do buffer de evento

    ; Para o sistema de arquivo
      Task_Path		equ 0x2c	; dd - ponteiro para uma string contendo todo path
      Task_PathLba		equ 0x30	; dd - Lba ou cluster do diretorio 
      Task_Directory	equ 0x34	; dd - ponteiro do buffer de diretorio aberto

      Task_Ass		equ 0x38	; dd - Assinatura do programa 'TRF1'
      Task_Status	 	equ 0x3c	; db - status do programa (ativo,parado,...)
      Task_Attr 		equ 0x3d	; db - Atributos do programa
      Task_Free3		equ 0x3e	; db - free
      Task_ExtNome	 	equ 0x3f	; db - extensao do nome do programa
      Task_Nome	 	equ 0x40	; rb ? (0-255)

  ;-----------------------------------------------
  
    Size_DescTask	    equ 0x40
    Size_CicleBuffer    equ 0x30	
  
  ; Atributos de Task_Attr =======================
    Task_Attr_Sistema   equ 0x01	; Se a tarefa é do sistema 	 	
    Task_Attr_Pausavel  equ 0x02	; Se a tarefa é pausavel
    Task_Attr_Destroy   equ 0x80	; Se a tarefa nao pode ser destruida		
  ;-----------------------------------------------  

;-----------------------------------------------------------------------------------------


;======================== M O U S E ======================================================

mouse_X 		equ (OS_BASE+0x003c); dw - X position - Deixe sempre na frente mouse_Y
mouse_Y 		equ (OS_BASE+0x003e); dw - Y position - Deixe sempre atras do mouse_X
mouse_Z 		equ (OS_BASE+0x0044); db - Z position
mouse_Button	equ (OS_BASE+0x0045); db - 0xXY = X->direction Y->button pressioned
mouse_Type 	equ (OS_BASE+0x0046); db - type: (PS2, PS2 ext, COM, ComExtended)
;res		equ (OS_BASE+0x0047); db -
ColorTable	equ (OS_BASE+0x004C); dd - point to converter ycb to rgb table

;-----------------------------------------------------------------------------------------


; provisorios.....
BufferNomeArquivo	equ (OS_BASE+0x0058)   	; deslocamento
onde		equ (OS_BASE+0x005c)		; aplicativo
;pasta_ativa			; aplicativo
;item_ativo			;

pasta_origem	equ (OS_BASE+0x0060)
item_origem	equ (OS_BASE+0x0064)




; ponteiro de comando do teclado
r474		equ (OS_BASE+0x006c)	;operador de comando do teclado


;-----------------------------------------------------------------------------------------


;======================== V I D E O ======================================================

; Screen

Screen 		equ (OS_BASE+0x0070)	; rb 0x20 - Descritor de imagem

Screen_Vesa 	equ (OS_BASE+0x0090)	; db - Versao do Vesa utilizada
Screen_Reserved2	equ (OS_BASE+0x0091)	; dw - reservado

Screen_Address 	equ (OS_BASE+0x0094)	; dd - Endereco da memoria flat de video (VESA 2.0)
Screen_Window	equ (OS_BASE+0x0098)	; dd - Endereco do mapa de byte

; Vesa 1.2
NumberOfPlanes	equ (OS_BASE+0x009c)	; db
NumberOfBanks	equ (OS_BASE+0x009d)	; db
BankSize		equ (OS_BASE+0x009e)	; db
Reserved		equ (OS_BASE+0x009f)	; db
Bank_Atual	equ (OS_BASE+0x00a0)	; db

Vesa_Desc		equ 0x700		;**************************rever

   ; Descritor do Vesa =================================================

      Vesa_BytePerLine equ 0x10	; dw
      Vesa_Width	   equ 0x12	; dw
      Vesa_Height	   equ 0x14	; dw
      Vesa_BPP	   equ 0x19	; db
      Vesa_LFBAddress  equ 0x28	; dd

   ;--------------------------------------------------------------------


   ; Descritor de Imagem ===============================================

       IAss	  equ 0x0    ; dd - Assinatura
       IType      	  equ 0x4    ; db - Tipo
       IBpp	  equ 0x5    ; db - Profundidade da cor
       IHeight    	  equ 0x6	   ; dw - tamanho X da imagem
       IWidth     	  equ 0x8	   ; dw - tamanho Y da imagem
       QsizeX	  equ 0xa    ; dw - Tamanho X do quadro
       QsizeY	  equ 0xc    ; dw - Tamanho Y do quadro
       QqtdeX	  equ 0xe    ; dw - Quantidade de quadro no eixo X
       QqtdeY 	  equ 0x10   ; dw - Quantidade de quadro no eixo Y
       IFree	  equ 0x12   ; dw - free
       IPaleta	  equ 0x14   ; dd - Ponteiro da paleta
       IBytesPerLine  equ 0x18   ; dd - quantidade de bytes por linha
       IPointer 	  equ 0x1c   ; dd - Ponteiro da memoria da imagem

     Image_Ass	equ 'IMG1'
     I_Size	equ 0x20
   ;-------------------------------------------------------------------

;-----------------------------------------------------------------------------------------


;================================ D E P U R A D O R  =====================================0x0110

DP_opcode		equ 0x100 or OS_BASE	;
regDp0		equ 0x104 or OS_BASE	;
DP_seg		equ 0x108 or OS_BASE	;
regDp7		equ 0x10c or OS_BASE	 ;

opcode_cont	equ (OS_BASE+0x0110); db	; opcode counter
buffer_opcode	equ (OS_BASE+0x0111); rb 15	; hexadecimal opcode

; reservado até 0x11f


;-----------------------------------------------------------------------------------------


;======================== O B J E C T S Y S T E M ========================================

ObjList_Ini	equ (OS_BASE+0x0120); dd - primeiro objeto da lista
ObjList_Atual	equ (OS_BASE+0x0128); dd - objeto atual da lista
ObjList_End	equ (OS_BASE+0x0124); dd - ultimo objeto da lista
Obj_Active	equ (OS_BASE+0x012c); dd - Objeto ativo
ObjMatriz_Desc 	equ (OS_BASE+0x0130); rb 0x10 - Descritor de imagem dos objetos


; reservado até 0x240

;-----------------------------------------------------------------------------------------


;============================= F R E E  ==================================================0x0240

pasta_aberta	equ (OS_BASE+0x240)		; provisorio
item_atual	equ (OS_BASE+0x244)		; provisorio
UserActivePartition	equ (OS_BASE+0x250)		; provisório


timer_ticks	equ (OS_BASE+0x248)
tab_comm		equ (OS_BASE+0x24c)



;-----------------------------------------------------------------------------------------


;======================= B I O S  V A R I A B L E S ======================================

; 0x400 a 0x40f  -  reservado - portas seriais e paralelas

RegConfBasica	equ (OS_BASE+0x0410)
; #0-	disco flexivel presente
; #1- 	coprocessador matemático presente
; #2,3-
; #4,5- 	modo de video inicial
; #6,7-  	quantidades de unidades de discos flexíveis presentes
; #8- 	auxiliar para #6 e #7
; #9,a,b-	quantidade de portas seriais
; #c-	porta de jogos presente
; #d-
; #e,f-	quantidade de impressoras instaladas




;CPUID information
cpu_vendor	equ (OS_BASE+0x0414) ;rd 3
cpu_sign		equ (OS_BASE+0x0420) ;rd 1
cpu_info		equ (OS_BASE+0x0424) ;rd 1
cpu_caps		equ (OS_BASE+0x0428) ;rd 4

cpu_type   	equ (OS_BASE+0x0438) ;db
cpu_id		equ (OS_BASE+0x0439) ;db
cpu_Intel		equ (OS_BASE+0x043a) ;db
cpu_AMD		equ (OS_BASE+0x043b) ;db

bios32_entry	equ (OS_BASE+0x043c) ;dd
pci_bios_entry	equ (OS_BASE+0x0440) ;dd

;free		equ (OS_BASE+0x0444) ;dd
;free		equ (OS_BASE+0x0448) ;dd

horas		equ (OS_BASE+0x044c) ;db
minutos		equ (OS_BASE+0x044d) ;db
segundos		equ (OS_BASE+0x044e) ;db

dia_semana	equ (OS_BASE+0x044f) ;db
dia		equ (OS_BASE+0x0450) ;db
mes		equ (OS_BASE+0x0451) ;db
ano		equ (OS_BASE+0x0452) ;db
seculo		equ (OS_BASE+0x0453) ;db

CharTable		equ (OS_BASE+0x0454) ;dd

;pci_code_32	equ (OS_BASE+0x0460) ;dd**********************************************
;pci_data_32	equ (OS_BASE+0x0468) ;dd**********transferir para gdt???????????????


; provisorios.....



;-----------------------------------------------------------------------------------------

;======================== H A R D  D I S K ===============================================all occuped henceforth

; ponteiros fundamentais:

ActivePartitionBase	equ (OS_BASE+0x04e8)

; point to system folder get in start
SystemFolderBase	equ (OS_BASE+0x04ec)	; sistema directory base point

; point to administrator folder of boot partition
BootAdmFolderBase	equ (OS_BASE+0x04f0)	; administrador directory base point

FileTable		equ (OS_BASE+0x004f4)	; file table base point

; point to base of boot partition
BootPartitionBase	equ (OS_BASE+0x004f8)	; boot partitition base point

Res001		equ (OS_BASE+0x004fc)	; dw

; boot partition
BootPartition	equ (OS_BASE+0x04fe)	; byte

; marca recebimento das int 14 e 15
intHDrec		equ (OS_BASE+0x04ff)	; 1 byte coincide com a variavel seguinte
q_unidIDE		equ (OS_BASE+0x04ff)	; 1 byte

; tabelas das unidades FD
baseFDD		equ (OS_BASE+0x0500)	; reservar 0x40 bytes para as tabelas FDD

; tabelas das unidades HD
baseHDD		equ (OS_BASE+0x0500+0x40)	; reservar 0x80 bytes para as tabelas HDD

; tabelas das unidades CD/DVD
base_CDD		equ (OS_BASE+0x05c0)	; reservar 0x40 bytes para até duas unidades


;-----------------------------------------------------------------------------------------


;======================== O C U P A D O ==================================================

; 0x600 a 7ff - system boot copy (can be eliminate)

;-----------------------------------------------------------------------------------------

;======================== F I L E S Y S T E M ============================================0x0800

; Estrutura dos diretórios (ver variaveis_FS.inc)- ocupa todo espaço entre 0x800 a 0xfff


; tabela de dispositivos (contem informações partições)
; reservado 0x800 a 0xbff (max. 32 partições de dispositivos)

TabelaPart	equ (OS_BASE+0x0800)

     ;======== Partição ===========
     Part_ini	equ 0	; Inicio da particao			4 bytes
     Part_ext	equ 4	; Extensao da particao			4 bytes
     Part_lbaDM	equ 8	; base DM					4 bytes
     Part_extDM	equ 0xc	; extensao DM				2 bytes
     Part_unidade	equ 0xe	; Unidade (01,80,81,...)			1 byte
     Part_tipo	equ 0xf	; FS/Tipo (C5, FAT,..., Floppy,HD,...)		1 byte
     Part_DMpos	equ 0x10	; posição DM na memoria  (unid. aberta qdo dif 0) 4 bytes
     Part_Padmin	equ 0x14	; free
     Part_res2	equ 0x18	; free
     Part_res3	equ 0x1c	; free

     ext_part	equ 0x20
     ;-----------------------------

; reservado para operações básicas com os dispositivos
BufferTemp	equ (OS_BASE+0x0c00); reservar 0xc00 a 0xdff para buffer temporário
BufferTemp2	equ (OS_BASE+0x0e00); usar somente durante a fase #16 (partida)


;-----------------------------------------------------------------------------------------0x1fff

