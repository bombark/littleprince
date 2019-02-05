;-----------------------------------------------------------------------------------------

; VESA 2.0

;-----------------------------------------------------------------------------------------
; OBS:

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 ==============================================
; Inicia o VESA 2.0
; Funcionamento:
; IN:
; OUT:

Vesa20_init:
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 2 ==============================================
; Finaliza o VESA 2.0
; Funcionamento:
; IN:
; OUT:

Vesa20_deinit:
	ret

;-----------------------------------------------------------------------------------------



; REFERENCIA de cima para baixo; esquerda para direita
;======================== F U N C T I O N 5 ==============================================
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	esi: Descritor da imagem
; OUT:
;	cl= cor
; Altera:
;	eax, ebx, edi

getPixel8:
	xor edi, edi
	imul ebx, [esi+IBytesPerLine]
	add edi, ebx
	add edi, eax
	add edi, [esi+IPointer]
	mov cl, [edi]
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 3 ==============================================
; Pega pixel 24
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	esi: Descritor da imagem
; OUT:
;	ecx: color

Vesa20_getPixel24:
	imul ebx, [esi+IBytesPerLine]
	lea edi, [eax+eax*2]
	add edi, ebx
	add edi, [esi+IPointer]
	mov ecx, [edi]
	and ecx, 0xffffff
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
; Pega pixel 32
; Kolibrios copyright
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	esi: Descritor da imagem
; OUT:
;	ecx: color

Vesa20_getPixel32:
	imul    ebx, [esi+IBytesPerLine]	; ebx = y * y multiplier
     	lea     edi, [ebx+eax*4]		; edi = x*4+(y*y multiplier)
     	add     edi, [esi+IPointer]   	; ebx = where pixel is in memory
     	mov     ecx, [edi]
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	 cl: color
;	edi: Descritor da imagem
; OUT:
;	nothing
; Altera:
;	eax, ebx, esi

putPixel8:
	xor esi, esi
	imul ebx, [edi+IBytesPerLine]
	add esi, ebx
	add esi, eax
	add esi, [edi+IPointer]
	mov byte [esi], cl
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Coloca pixel 24
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	ecx: color
;	edi: Descritor da imagem
; OUT:
;	nothing
; Altera:
;	eax, ebx, esi

Vesa20_putPixel24:
	and ecx, 0xffffff
	imul ebx, [edi+IBytesPerLine]
	lea esi, [eax+eax*2]
	add esi, ebx
	add esi, [edi+IPointer]
	mov [esi], cx
	shr ecx, 8
	mov [esi+2], ch
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 6 ==============================================
; Coloca pixel 32
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx= Y coordenate
;	ecx: color
;	edi: descritor da imagem
; OUT:
;	nothing

Vesa20_putPixel32:
	imul ebx, [edi+IBytesPerLine]    ; ebx = y * y multiplier
     	lea esi, [ebx+eax*4] 	; edi = x*4+(y*y multiplier)
     	add esi, [edi+IPointer]    	; ebx = where pixel is in memory
     	mov [esi], ecx
     	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Coloca pixel 24 em negativo
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	edi: Descritor da imagem
; OUT:
;	nothing
; Altera:
;	eax, ebx, esi, ecx

Vesa20_putPixel24_Neg:
	and ecx, 0xffffff
	imul ebx, [edi+IBytesPerLine]
	lea esi, [eax+eax*2]
	add esi, ebx
	add esi, [edi+IPointer]
	mov ecx, [esi]
	not ecx
	mov [esi], cx
	shr ecx, 8
	mov [esi+2], ch
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 6 ==============================================
; Coloca pixel 32 em negativo
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx= Y coordenate
;	edi: descritor da imagem
; OUT:
;	nothing
; Altera:
;	ebx, eax, edi, ecx

Vesa20_putPixel32_Neg:
	imul ebx, [edi+IBytesPerLine]    ; ebx = y * y multiplier
     	lea esi, [ebx+eax*4] 	; edi = x*4+(y*y multiplier)
     	add esi, [edi+IPointer]    	; ebx = where pixel is in memory
     	mov ecx, [esi]
     	not ecx
     	mov [esi], ecx
     	ret

;-----------------------------------------------------------------------------------------




; REFERENCIA de baixo para cima; esquerda para direita
;======================== F U N C T I O N 3 ==============================================
; Pega pixel 24
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	esi: Descritor da imagem
; OUT:
;	ecx: color

Vesa20_getPixel242:
	movzx edx, word [esi+IHeight]
	sub edx, ebx
	mov ebx, edx
	dec ebx
	imul ebx, [esi+IBytesPerLine]
	lea edi, [eax+eax*2]
	add edi, ebx
	add edi, [esi+IPointer]
	mov ecx, [edi]
	and ecx, 0xffffff
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
; Pega pixel 32
; Kolibrios copyright
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	esi: Descritor da imagem
; OUT:
;	ecx: color

Vesa20_getPixel322:
	movzx edx, word [esi+IHeight]
	sub edx, ebx
	mov ebx, edx
	imul ebx, [esi+IBytesPerLine]	; ebx = y * y multiplier
     	lea edi, [ebx+eax*4]		; edi = x*4+(y*y multiplier)
     	add edi, [esi+IPointer]   	; ebx = where pixel is in memory
     	mov ecx, [edi]
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Coloca pixel 24
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	ecx: color
;	edi: Descritor da imagem
; OUT:
;	nothing
; Altera:
;	eax, ebx, esi

Vesa20_putPixel242:
	movzx edx, word [edi+IHeight]
	sub edx, ebx
	mov ebx, edx
	dec ebx
	and ecx, 0xffffff
	imul ebx, [edi+IBytesPerLine]
	lea esi, [eax+eax*2]
	add esi, ebx
	add esi, [edi+IPointer]
	mov [esi], cx
	shr ecx, 8
	mov [esi+2], ch
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 6 ==============================================
; Coloca pixel 32
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	ecx: color
;	edi: descritor da imagem
; OUT:
;	nothing

Vesa20_putPixel322:
	movzx edx, word [edi+IHeight]
	sub edx, ebx
	mov ebx, edx
	and ecx, 0xffffff
	imul ebx, [edi+IBytesPerLine]    ; ebx = y * y multiplier
     	lea esi, [ebx+eax*4] 	; edi = x*4+(y*y multiplier)
     	add esi, [edi+IPointer]    	; ebx = where pixel is in memory
     	mov [esi], ecx
     	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Coloca pixel 24
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	edi: Descritor da imagem
; OUT:
;	nothing
; Altera:
;	eax, ebx, esi

Vesa20_putPixel242_Neg:
	movzx edx, word [edi+IHeight]
	sub edx, ebx
	mov ebx, edx
	dec ebx
	and ecx, 0xffffff
	imul ebx, [edi+IBytesPerLine]
	lea esi, [eax+eax*2]
	add esi, ebx
	add esi, [edi+IPointer]
	mov ecx, [esi]
	not ecx
	mov [esi], cx
	shr ecx, 8
	mov [esi+2], ch
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 6 ==============================================
; Coloca pixel 32
; Funcionamento:
; IN:
;	eax: X coordenate
;	ebx: Y coordenate
;	edi: descritor da imagem
; OUT:
;	nothing

Vesa20_putPixel322_Neg:
	movzx edx, word [edi+IHeight]
	sub edx, ebx
	mov ebx, edx
	imul ebx, [edi+IBytesPerLine]    ; ebx = y * y multiplier
     	lea esi, [ebx+eax*4] 	; edi = x*4+(y*y multiplier)
     	add esi, [edi+IPointer]    	; ebx = where pixel is in memory
     	mov ecx, [edi]
     	not ecx
     	mov [esi], ecx
     	ret

;-----------------------------------------------------------------------------------------