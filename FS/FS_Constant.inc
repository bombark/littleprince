;-----------------------------------------------------------------------------------------

; File System Constants

;-----------------------------------------------------------------------------------------


;======================== F O L D E R ====================================================

  ; Descritor da Pasta ===========================
	FD_List_Ini  	equ 0	; dd - Ponteiro para o inicio da lista
	FD_List_End	equ 4	; dd - Ponteiro para o fim da lista 
	FD_List_Atual	equ 8	; dd - Ponteiro para o item atual da lista
	FD_List_Size	equ 0xc	; dd - Tamanho do item da lista
	FD_Item_Qtde	equ 0x10	; dd - Quantidade de itens da pasta
	FD_Item_Size	equ 0x14	; dd - Tamanho ocupado pelos arquivos da pasta
	FD_Item_Lba	equ 0x18	; dd - Lba ou cluster do diretorio
	FD_Partition	equ 0x1c	; db - Numero da Particao
	FD_Item_Free	equ 0x1d	; db - Free
	FD_Ass		equ 0x1e	; dw - Assinatura 

FD_ItemSize	equ 0x20
FD_Assinatura	equ "FD"

  ;-----------------------------------------------


  ; Estrutura da lista de arquivos de uma pasta
	FL_Prox	   equ 0x0	; dd - ponteiro para proximo item
	FL_Ant	   equ 0x4	; dd - ponteiro para item anterior
	FL_Ass	   equ 0x8	; dw - assinatura
	FL_Data	   equ 0xa	; dw - data de criacao
	FL_Hora	   equ 0xc	; dw - hora de criacao
	FL_Data2     equ 0xe	; dw - data de modificacao
	FL_Hora2     equ 0x10	; dw - hora de modificacao
	FL_SubTipo   equ 0x12	; db - subtipo do item (jpeg,bmp,mp3,mpeg,...)
	FL_Tipo      equ 0x13	; db - tipo do item (pasta,texto,musica,...)
	FL_Rs1	   equ 0x14	; dw - free
	Fl_Rs2	   equ 0x16	; db - free
	FL_Color     equ 0x17	; db - cor do nome
	FL_Comment   equ 0x18	; dd - ponteiro para o comentario
	FL_Name	   equ 0x1c	; dd - ponteiro para o nome

FL_ItemSize equ 0x20
FL_Assinatura equ "FL"

  ;-----------------------------------------------
  
;-----------------------------------------------------------------------------------------


;======================== E R R O R S ====================================================

	ERROR_SUCCESS        = 0
	ERROR_DISK_BASE      = 1
	ERROR_UNSUPPORTED_FS = 2
	ERROR_UNKNOWN_FS     = 3
	ERROR_PARTITION      = 4
	ERROR_FILE_NOT_FOUND = 5
	ERROR_END_OF_FILE    = 6
	ERROR_MEMORY_POINTER = 7
	ERROR_DISK_FULL      = 8
	ERROR_FAT_TABLE      = 9
	ERROR_ACCESS_DENIED  = 10
	ERROR_UNIT_CLOSED	 = 11
	ERROR_SERVICE	 = 12
	ERROR_BAD_NAME	 = 13
	ERROR_NOT_FORMATTED	 = 14
	ERROR_EXIST_FILE	 = 15
	ERROR_PATH_INCORRECT = 16

;-----------------------------------------------------------------------------------------