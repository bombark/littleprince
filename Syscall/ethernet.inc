;-----------------------------------------------------------------------------------------

; Servicos de Rede

;-----------------------------------------------------------------------------------------


;======================== T A B E L A ====================================================
; SRD = Servico de Rede

SRD_Table:
	dd SRD_reserved		; 0- Get information
;	dd SDR_Socket_Open		; 1- Abre um socket
;	dd SDR_Socket_Close		; 2- Fecha um socket
;	dd SDR_Send		; 3- Envia dados
;	dd SDR_Recebe		; 4- Recebe dados


;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================


;-----------------------------------------------------------------------------------------


;======================== E N T R A D A ==================================================
; Entrada  do servico de memoria
; IN:
;	registradores= parametros
;	ebp= numero da funcao
; OUT:
;	depende

SRD_Enter:
	cmp ebp, 0x28
	ja .error
	shl ebp, 2
	call dword [cs:SRD_Table+ebp]
	iretd

.error:	stc
	iretd

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  0 =============================================
; Devolve informacoes sobre o servico
; IN:
; OUT:

SRD_reserved:
	ret

;-----------------------------------------------------------------------------------------