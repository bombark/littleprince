;-----------------------------------------------------------------------------------------

; Object_System - Botao1

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

  ; Estrutura da janela ==========================
     Win_Name	equ Obj_Size  	; dd - Ponteiro do nome da janela (fazer)
     Win_Status	equ Obj_Size+4	; db - Status da janela
     Win_Free	equ Obj_Size+5	; db - free

  ;-----------------------------------------------

    ; Atributos do Win_Status =====
      Win_Status_Ativada	equ 0x01	; Se a janela pode ser ativada (O desktop nao pode)
      Win_Status_Min	equ 0x02	; Se a janela está minizada
      Win_Status_Max	equ 0x04	; Se a janela está maximizada
      Win_Status_Borda	equ 0x10 	; Se ela tem borda
    ;------------------------------

  Win_Size equ Obj_Size+6

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  1 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjWin_Create:
	pushad


; Coloca as variaveis
.set:	xor eax, eax
	mov [edi+Win_Name], eax
	mov byte [edi+Win_Status], 0x11

; Cria o botao de Minimizar

; Cria o botao de Fechar
	mov eax, 0x00040000	
	mov ax, [edi+Obj_SizeX]
	sub ax, 0xe
	mov ebx, 0x000a000a
	mov ecx, 0x0002ffff
	mov edx, edi
	call Object_Create
	call Object_Draw
	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  2 =============================================
; Desenha a janela
; IN:
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjWin_Destroy:
	push eax
	push ebx

  ; Retira a janela da lista
	mov eax, [edi+Obj_Ant]
	mov ebx, [edi+Obj_Prox]

    ; Se eax == 0 -> Coloca o ebx como primeiro da lista
	or eax, eax
	jnz @f
	mov [ObjList_Ini], ebx
	jmp .prox

    ; Senao -> eax->prox = ebx
@@:	mov [eax+Obj_Prox], ebx

    ; Se ebx == 0 -> Coloca o eax como ultimo da lista
.prox:   	or ebx, ebx
    	jnz @f
    	mov [ObjList_End], eax
    	mov [ObjList_Atual], eax
	jmp .end

    ; Senao -> ebx->ant = eax
@@:	mov [ebx+Obj_Ant], eax

.end:	pop ebx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  3 =============================================
; Desenha a janela
; IN:
;	eax: [Y|X] coordenate adicional
;	edi: ponteiro do objeto janela
; OUT:
;	nothing

ObjWin_Draw:
	pushad

; Verifica se a janela tem borda
	mov edx, edi
	test byte [edx+Win_Status], Win_Status_Borda
	jz .sem

; Desenha a borda
	add eax, [edx+Obj_PosX]
	push eax
	mov ebx, [edx+Obj_SizeX]
	sub ebx, 0x00010001
	mov ecx, 0x708090
	mov edx, edi
	mov edi, Screen
	call draw_Rect

; Desenha o fundo da janela
	add eax, 0x00100001
	sub ebx, 0x00110002
	mov ecx, 0xdddddd
	call draw_RectFill
	pop eax

; Desenha o cabecario
	add eax, 0x00010001
	mov esi, edx
	mov dh, 0x10
	mov ecx, 0x00080000		; tamanho do cabecario/2 -> 0x10/2
	and ebx, 0xffff
	or ebx, ecx
	mov ecx, 0x708090
	call draw_RectDegradeUp	; Tem problema nessa rotina

	mov dh, 0x10
	add eax, 0x00070000
	mov ecx, 0xf0ffff
	call draw_RectDegradeDown

; Coloca o nome do programa
	;mov eax, [esi+posX_Win]
	;add eax, 0x00020010
	;mov ecx, 0x02000000
	;mov esi, [esi+program_Win]
	;add esi, Task_Nome
	;call draw_String

.sem:	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  4 =============================================
; Quando o mouse está sobre o botao
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjWin_Onhover:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o mouse estava sobre o objeto
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjWin_Saiu:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao do mouse está pressionado
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjWin_Pressed:
  ; Se janela nao está ativada  -> ativa
	cmp edi, [ObjList_Atual]
	je .arrasta
	call ObjWin_Active
	jmp .end

  ; Senao  -> arrasta a janela
.arrasta:	push eax	
  	mov [Mouse_Obj], edi
	xor eax, eax
  	call Object_GetPos	
  	mov dword [Mouse_ObjX], eax
  	or [Mouse_Status], Mouse_Moving
  	pop eax

.end:	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  6 =============================================
; Quando o botao do mouse parou de ser pressionado
; IN:
;	edi: ponteiro do objeto botao
; OUT:
;	nothing

ObjWin_Soltou:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  7 =============================================
; Ativa a janela
; IN:
;	edi: Ponteiro do objeto janela
; OUT:
;	nothing

ObjWin_Active:
	push eax
	push ebx

	test byte [edi+Win_Status], Win_Status_Ativada
	jz .end

; Coloca no ultimo lugar da lista - para sempre ser a ultima a ser desenhada na matriz
  ; retira o item de onde ele está
	mov eax, [edi+Obj_Prox]
	mov ebx, [edi+Obj_Ant]
	mov [eax+Obj_Ant], ebx
	or ebx, ebx
	jz .first
	mov [ebx+Obj_Prox], eax
	jmp .tolast

.first:	mov [ObjList_Ini], eax

  ; coloca no fim da lista
.tolast:	mov ebx, [ObjList_End]
	mov [ebx+Obj_Prox], edi
	mov [edi+Obj_Ant], ebx
	mov dword [edi+Obj_Prox], 0
	mov [ObjList_End], edi
	mov [ObjList_Atual], edi

; Desenha a janela
	call Object_MontaMatriz	; Monta a matriz
	xor eax, eax
	push edx
	mov dl, 1
	call Object_SendDraw	; Desenha a janela
	pop edx
.end:	pop ebx
	pop eax
	ret

.error:	stc
	pop ebx
	pop eax
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  8 =============================================
; Minimiza a janela
; IN:
;	edi: Ponteiro do objeto janela
; OUT:
;	nothing

ObjWin_Minimise:
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N  9 =============================================
; Maximiza a janela
; IN:
;	edi: Ponteiro do objeto janela
; OUT:
;	nothing

ObjWin_Maximize:
	ret


;-----------------------------------------------------------------------------------------