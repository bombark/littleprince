;-----------------------------------------------------------------------------------------

; Manipulacao de Diretorio na FAT

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Cria uma pasta
; IN:
;	eax= Cluster do diretorio onde será criado a pasta
;	edx= Nome Padronizado
;	esi= Data Block Memory Base
; OUT:
;	eax= Cluster da pasta
;	cy= 0	-> OK
;	cy= 1	-> Error, acabou o diretorio
; OBS: A pasta onde será criado a pasta deverá estar aberta no Data Block

align 4
FAT_Folder_Create0:
	mov ecx, eax
	mov al, 0x10
int 1	
	call FAT_Item_Create
int 1	
	
  ; Cria um espaco para o diretorio	
	mov eax, 0x2
int 1	
	call FAT_GetFree
int 1	
	mov [esi+edx+BD_Folder+FAT32_HWC], eax
	mov word [esi+edx+BD_Folder+FAT32_HWC], 0
	mov [esi+edx+BD_Folder+FAT16_C], ax
	
  ; Salva o diretorio
  	call FAT_Write
int 1  	
  	call FAT_Folder_Write
int 1  		
	 	
  ; Abre o diretorio
  	call FAT_Folder_OpenCluster	
int 1	
  	
  ; Grava o primeiro setor do diretorio
  	push eax
  	push ecx
 	xor eax, eax
 	mov ecx, 0x200/4
 	lea edi, [esi+BD_Folder]
  	rep stosd
  	pop ecx
  	pop eax
  	
    ; Cria o item '.'
  	lea edi, [esi+BD_Folder]
  	mov word [edi], '. '
  	mov dword [edi+2], '    '
  	mov dword [edi+6], '    '
  	mov byte [edi+10], ' '
  	mov byte [edi+FileAttr], 0x10
  	mov [edi+FAT32_HWC], eax
  	mov word [edi+FAT32_HWC], 0
  	mov [edi+FAT16_C], ax
  
    ; Cria o item '..'	
  	add edi, 0x20
  	mov word [edi], '..'
  	mov dword [edi+2], '    '
  	mov dword [edi+6], '    '
  	mov byte [edi+10], ' '
  	mov byte [edi+FileAttr], 0x10
  	mov [edi+FAT32_HWC], eax
  	mov word [edi+FAT32_HWC], 0
  	mov [edi+FAT16_C], ax	
	
    ; Salva o diretorio	
	call FAT_Folder_Write
int 1	
  ; Zera os setores restantes do cluster	
	sub edi, 0x20
	push edi
	xor eax, eax
	mov ecx, 0x10
	rep stosd
	pop edi
	movzx ecx, byte [esi+BD_FAT_SPC]
	dec ecx
	jz .end
.a:	inc dword [esi+BD_Folder_Lba]
	call FAT_Folder_Write
int 1	
	loop .a
	
.end:	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Apaga uma pasta
; IN:
;	eax= Cluster
;	esi= Data Block Memory Base
; OUT:
;	cy= 0	-> OK
;	cy= 1	-> Error, acabou o diretorio

align 4
FAT_Folder_Destroy:
	
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Abre um novo cluster de um diretorio (Use essa rotinas antes do Folder_Read)
; IN:
;	eax= Cluster
;	esi= Data Block Memory Base
; OUT:
;	cy= 0	-> OK
;	cy= 1	-> Error, acabou o diretorio

align 4
FAT_Folder_OpenCluster:
	push eax
	push edx
	push edi
	push ebp

  ; Se (eax> Ultimo cluster do HD)  -> Sai 	
	cmp eax, [esi+BD_HD_LastCluster]   
	ja .error    
	
  ; Se (eax<2)  -> Pega o cluster do root 
	cmp eax,2
	jae .calc
	mov eax, [esi+BD_Root_Cluster]

  ; Calcula o lba a partir do cluster
.calc:	mov [esi+BD_Folder_Cluster], eax	; salva o novo cluster
	mov byte [esi+BD_Folder_Rlba], 0
	sub eax,2
	movzx ebp, byte [esi+BD_FAT_SPC]
	imul eax,ebp
	add eax, [esi+BD_HD_Lba]

  ; Lê o diretorio
	lea edi, [esi+BD_Folder]
	mov edx, edi
	cmp eax, [esi+BD_Folder_Lba]	; compara com a lba existente na memoria
	je .end
	mov [esi+BD_Folder_Lba], eax	; marca a nova lba
	call ReadSector0	
	jb .error
	
  ; Fim 	
.end:	pop ebp
	pop edi
	pop edx
	pop eax
	ret

  ; Error - Cluster invalido ou Erro na leitura
.error:	stc
	pop ebp
	pop edi
	pop edx
	pop eax
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Lê um diretorio em modo sequencial 
; IN:
;	 al= Direcao (0-> lê o proximo setor, 1->lê o setor anterior)
;	 bl= Particao
;	esi= Data Block Memory Base
; OUT:
;	[esi+BD_Folder_Cluster], [esi+BD_Folder_Lba] 
;	e [esi+BD_Folder_Rlba]  são atualizadas
;	z= 0	-> O diretorio ainda nao terminou
;	z= 1	-> O diretorio terminou
;	cy= 0	-> OK
;	cy= 1	-> Error - Leitura, Cluster invalido
; OBS: Ainda nao está feito a direcao de leitura

align 4
FAT_Folder_Read:
	push eax
	push edi	
	
  ; Se (lba relativo>=BD_FAT_SPC)  -> incrementa o cluster	
	inc byte [esi+BD_Folder_Rlba]
	mov al, [esi+BD_Folder_Rlba]
	cmp al, [esi+BD_FAT_SPC]
	jb .senao
	
    ; Pega o proximo cluster	
	mov byte [esi+BD_Folder_Rlba], 0	
	mov eax, [esi+BD_Folder_Cluster]
	call FAT_Get
	cmp eax, 2
	jb .end2
	cmp eax, FATreserved 
	je .end2
	call FAT_Folder_OpenCluster
	jmp .end
	
  ; Senao
.senao:  	inc dword [esi+BD_Folder_Lba]
  	mov eax, [esi+BD_Folder_Lba]
  	lea edi, [esi+BD_Folder]
  	call ReadSector0		
	jc .end2		;error
	
  ; Fim - O Diretorio ainda nao terminou	
.end:	pop edi
	pop eax
	ret

  ; Fim - Diretorio terminou  - Falta marcar o flag 
.end2:	pop edi
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Apenas grava a parte do diretorio aberto no Data Block
; IN:
;	 bl= Particao
;	esi= Data Block Memory Base
; OUT:
;	cy= 0	-> OK
;	cy= 1	-> Error, acabou o diretorio

align 4
FAT_Folder_Write:
	push eax
	push edi	
	
 	mov eax, [esi+BD_Folder_Lba]
  	lea edi, [esi+BD_Folder]
  	call WriteSector0		
		
	pop edi
	pop eax
	ret

;-----------------------------------------------------------------------------------------