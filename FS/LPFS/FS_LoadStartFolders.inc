

; Init Disk unit operation


; Como definir a parti��o da unidade de partida ???????
; deve vir depois de montada a tabela de dispositivos (parti��es)
; provisoriamente ir� carregar o diretorio mestre da unidade HD(0:0)

;======================== F U N C T I O N  1 =============================================
; Inicializa uma particao
; IN:
; OUT:


InitStartDisk:
; verificar se a unidade de partida � LP...quando a partida for realizada diretamente do HD
; agora deve-se verificar se bl tem realmente o sistema LP
;

	mov bl, 3			; c: (3a. parti��o)
	call verify_LPFS		; se HD c: nao tem sistema LPFS sai sem
	jb .b			; carregar os folders

	call DefineBootPartition	; endere�o da parti�ao ativa
	call Load_DM		; carrega diret�rio mestre da unidade de partida
				; ebx= posi�ao DM na memoria

; provisoriamente � usado o disco C:...depois usar a unidade da partida

; busca pasta sistema
;int 1
	mov esi, PSys_nome		; see FS_SystemInstall.inc
	mov al, Kn1-Kn0
	call pesquisa_diretorio	; se existe carrega
	jb .noSys
	mov [SystemFolderBase], ebx	; salva base para acesso r�pido
.noSys:	;continua


; carrega o diret�rio administrador - fazer carregar apos digitar senha

	;call testa_senha
	;jb .f

;	# busca a pasta publica...a primeira em DM (alterar em HD.inc)
;	# testa a senha se for valida busca o diretorio do usuario correspondente
;	# buscar tambem a pasta do sistema  (salvar base em variavel fixa para acesso rapido)

	mov edi, [BootPartitionBase]	;
	mov ebx, [edi+Part_DMpos]	; ebx j� definido pela rotina anterior
	mov esi, [ebx+DM_ini_item]	; primeiro item do diretorio mestre
	call carrega_diretorio 	; carrega diret�rio admin, retorna ebx=ponteiro
	mov [edi+Part_Padmin],ebx	; salva base do diret�rio topo (administrador)
	mov [BootAdmFolderBase],ebx

	mov [pasta_aberta], ebx	; provisorio


.b:	ret

	;mov esi, [esi]		; proximo item (Pasta publica- fixa)
	;mov [edi+p_Ppublica], ebx	; salva base do diret�rio topo (administrador)


;-----------------------------------------------------------------------------------------

verify_LPFS:

	push ebx
	dec bl
	and ebx, 0xff
	shl ebx, 5
	mov bh, [TabelaPart + ebx + Part_tipo ]
	and bh, 0xf0
	cmp bh, 0x10
	jz .a
	stc
.a:	pop ebx
	ret


;-----------------------------------------------------------------------------------------





