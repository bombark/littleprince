;-----------------------------------------------------------------------------------------

; FAT 

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Abre um bloco de dados
; IN:
;         bl= partição
; OUT:
;         esi= aponta a base do bloco na memoria
;         cy= 0  -> OK
;         cy= 1  -> Error
; OBS: Necessario para utilizar todas as rotinas da fat

align 4
FAT_Open:
          push eax ebx edx edi ecx
          xor ecx, ecx
          
  ; (bl!=0) && (bl<=26)	
          or bl, bl
          jz .erro2
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
          
  ; leitura da Fat: criar um buffer parcial com 8 setores
          movzx eax, word [esi+Reserved_sectors]
          mov [edi+BD_FAT_Base], eax
          mov [edi+BD_FAT_Lba], eax		; lba da fat existente na memoria
          add edi, 0x400
          push eax
          call ReadSector0      	
          jb .erro
          pop eax
          add edi, 0x200			; edi= root base
          
  ; leitura da root
          mov byte [edi+BD_FAT_Type-0x600], 0
          mov dx, [esi+Sectors_per_FAT]		; deve ser 0 em fat32
          or dx, dx
          jnz .b
.a:       mov edx, [esi+Sectors_per_FAT32]
          mov byte [edi+BD_FAT_Type-0x600], 1
          mov [edi+BD_FAT_SPF-0x600], edx	; posiciona setores por fat no bloco
.b:       mov cl, [esi+Number_of_FATs]
          mov [edi+BD_FAT_QTDE-0x600], cl
          shr cl, 1
          shl edx,cl
          add eax, edx			; edx= total de setores usados pela fat

          mov [edi+BD_Folder_Lba-0x600], eax	; folder lba
          mov byte [edi+BD_Folder_Rlba-0x600], 0
          mov [edi+BD_HD_Lba-0x600], eax	; 1cluster lba = data lba = root base (never changes)
          push eax
          call ReadSector0		; busca diretorio root ..carregar só um setor
          jb .erro
          pop eax

  ; calculo ultimo cluster
          mov edx, [esi+Sectors]	; quantidade total de setores na partição
          sub edx, eax
          movzx ebx, byte [esi+Sectors_per_cluster]
          mov [edi+BD_FAT_SPC-0x600], bl
          mov eax, edx
          xor edx, edx
          div ebx
          mov [edi+BD_HD_LastCluster-0x600], eax
          
; Directory base cluster
          mov eax, [esi+Root_dir_1cluster]
          mov [edi+BD_Root_Cluster-0x600], eax
          mov [edi+BD_Folder_Cluster-0x600], eax
          mov byte [edi+BD_FAT_Changed-0x600], 0
          mov dword [edi+BD_ASS-0x600], bd_ass
          sub edi, 0x600
          mov esi, edi
          
.fim:     pop ecx edi edx ebx eax
          ret

.erro:
          add esp,4		; al retorna erro do driver HD
          jmp .fim

.erro1:   pop esi		; neste caso liberar o bloco....ou tentar aumentar o
          jmp .fim		; tamanho do heap

.erro2:   stc		; particao invalida
          pop ecx edi edx ebx eax
          ret

.erro3:   stc		; particao invalida
          pop ebx ecx edi edx ebx eax
          ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Fecha um bloco de dados
; IN:
;         esi= Bloco de dados
; OUT:
;         nothing

align 4
FAT_Close:
          mov dword [esi+BD_ASS], 0
          call lib_HB
          ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Carrega a fat onde está localizado o cluster e devolve o proximo - KolibriOS Copyright
; IN:
;         EAX = cluster
;         esi= unit data block
;         bl= partition
; OUT:
;         EAX = next cluster

align 4
FAT_Get:
          push edi		;ebp
          lea edi, [esi+BD_FAT_Base]		; edi= base fat_buffer
          
          test byte [esi+BD_FAT_Type], 0x01	; confirma FAT16
          jz @F				; Se for 16 pula
          add eax,eax
@@:       add eax,eax		; cluster = bloco de setores.

  ; Calcula o lba a partir do cluster
          mov ebp,0x200-1
          and ebp,eax		; esi = position in fat sector
          shr eax,9			; eax = fat sector
          add eax,[esi+BD_FAT_Base]	; salva a lba da fat em memoria
          
  ; Se (eax!=BD_FAT_Lba)  ->  lê nova posicao da fat e grava se necessario 	
          cmp eax,[esi+BD_FAT_Lba]	; is fat sector already in memory?  (FAT_lba)
          je .end
          
    ; Se (Fat mudou)  -> Grava
          cmp byte [esi+BD_FAT_Changed],0	; is fat changed?
          je .notread			; no
          call FAT_Write			; yes. write it into disk
          jb .error1
          
    ; Lê a fat
.notread: mov [esi+BD_FAT_Lba], eax	; eax = lba da fat
          call ReadSector0		; altera eax
          jb .error1
          
  ; Pega o cluster
.end:     mov eax,[edi+ebp]		; obtem cluster
          and eax,FATmask		
.error1:	pop edi 
          ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Escreve o cluster na fat - KolibriOS Copyright
; IN:
;         eax = cluster
;         edx = value to save
;         bl= partition (*)
;         esi= unit data block (*)
; OUT:
;         EDX = old value

align 4
FAT_Set:
          push eax 
          push ebx
          lea edi,[esi+BD_FAT_Base]
          cmp eax,2
          jb .error    		; cluster deve ser maior que 2
          cmp eax,[esi+BD_HD_LastCluster]
          ja .error
          
          test byte [esi+BD_FAT_Type], 0x01	; confirma FAT16
          jz @F				; Se for 16 pula
          add eax,eax
          
@@:       add eax,eax
          mov ebp,511
          and ebp,eax		; esi = position in fat sector
          shr eax,9			; eax = fat sector
          add eax,[esi+BD_FAT_Base]
          cmp eax,[esi+BD_FAT_Lba]	; is fat sector already in memory?
          je .fat_loaded		; yes
          
          cmp byte [esi+BD_FAT_Changed],0	; is fat changed?
          je .no_changed		; no
          call FAT_Write		; yes. write it into disk
          jb .error
          
.no_changed:
          mov [esi+BD_FAT_Lba],eax    	; save fat sector
          call ReadSector0
          jb .error

.fat_loaded:
          test word [esi+BD_FAT_SPF], 0xffff
          je .test32

.set16:
          xchg [edi+ebp],dx		; save new value and get old value
          jmp .write

.test32:
          mov eax,FATmask		;

.set32:
          and edx,eax
          xor eax,-1                	; mask for high bits
          and eax,[edi+ebp]         	; get high 4 bits
          or eax,edx
          mov edx,[edi+ebp]         	; get old value
          mov [edi+ebp],eax         	; save new value

.write:
          mov byte [esi+BD_FAT_Changed], 1 ; fat has changed

.nonzero:
          and edx,FATmask		; ???

.error:   pop ebx 
          pop eax
          ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Pesquisa um cluster livre na fat - KolibriOS Copyright
; IN:
;         eax= # cluster for start the searching
;         bl = partition
;         esi= data block memory base
; OUT:
;         if CARRY=0 EAX = # first cluster found free
;         if CARRY=1 disk full

align 4
FAT_GetFree:
          push ecx
          mov ecx,[esi+BD_HD_LastCluster]	; counter for full disk
          sub ecx,2				; começa do cluster dois
          
.test:
          cmp eax,[esi+BD_HD_LastCluster]	; if above last cluster start at cluster 2
          jbe .in_range
          mov eax,2
          
.in_range:
          push eax
          call FAT_Get		; get cluster state
          jb .not_found_1
          
          test eax,eax		; is it free?
          pop eax
          je .found 		; yes
          inc eax 			; next cluster
          dec ecx  			; is all checked?
          jns .test			; no
          jmp .not_found
          
.not_found_1:
          add   esp,4
.not_found:
          pop   ecx			; yes. disk is full
          stc
          ret

.found:
          pop   ecx
          clc
          ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  5 =============================================
; Zera uma cadeia de clusters - KolibriOS Copyright
; IN:
;          eax = Cluster inicial
;          bl = partition
;          esi= data block memory base
; OUT:
;          if CARRY=0 EAX = # first cluster found free
;          if CARRY=1 disk full

align 4
FAT_SetFree:
          ret


;-----------------------------------------------------------------------------------------


;======================== S U B F U N C T I O N  1 =======================================
; Grava um setor da fat - KolibriOS Copyright
; IN:
;	eax = cluster
;	esi= unit data block
;	bl= partition
; OUT:
;	EAX = next cluster

align 4
FAT_Write:
int 1
nop
nop          
          push eax 
          push ecx
          push edi 
          
  ; Verifica se a fat foi mudada
          and byte [esi+BD_FAT_Changed],0
          mov eax, [esi+BD_FAT_Lba]
          cmp eax,-1
          jz .fim
          lea edi, [esi+BD_Fat]    ;BD_FAT_Base]		; edi= fat base
          mov ecx, 1		;mov ecx, [esi+BD_FAT_QTDE] ; variavel com dados corrompidos
          
  ; Grava a fat
.next_fat:
          push eax
int1          
          call WriteSector0			; write command
int 1	
          jb .error
          pop eax

          add eax, [esi+BD_FAT_SPF]
          dec ecx
          jnz .next_fat
          
  ; Fim
.fim:     pop edi
          pop ecx 
          pop eax
          ret

.error:   add esp, 4
          jmp .fim
          ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Devolve o ponteiro do Data Block
; IN:
;	 bl= particao
; OUT:
;	esi= Data Block


FAT_GetDataBloco:
	push edi

  ; Se (Particao== Aberta)  -> SAI
	movzx edi, bl
	dec edi
	shl edi, 5
	mov esi, [TabelaPart+edi+Part_Padmin]
	or esi, esi			; verifica se a particao está aberta
	jnz .end
  
  ; Senao  -> Abre particao
	call FAT_Open  	
   	mov [TabelaPart+edi+Part_Padmin], esi
 
.end:   	pop edi
	ret

;-----------------------------------------------------------------------------------------