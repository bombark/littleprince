;-----------------------------------------------------------------------------------------

; Testes com os objetos graficos

;-----------------------------------------------------------------------------------------


Teste_Window dd 0
Teste_String db "Felipe",0xd,0xa,"Bombardelli",0
	     rb 0x50


Teste_Main:
	call Teste_Init
	
.loop:	pushad
	call Mouse_Click
	popad
	mov ebp, 0x3
	int 0x42
	
	jmp .loop
	
Teste_Init:
	mov eax, 0x00200020
	mov ebx, 0x02000180
	mov ebp, 1
	int 0x42
	mov [Teste_Window], edi
	
	mov eax, 0x00200020
	mov ebx, 0x00100010
	mov ecx, 0x00020001
	mov edx, edi
	;mov ebp, 5
	;int 0x42
	;mov ebp, 7
	;int 0x42
		
	
	mov eax, 0x00300090
	mov ebx, 0x00140060
	mov ecx, 0x00080001
	;mov ebp, 5
	;int 0x42
	;mov ebp, 7
	;int 0x42
	
	
  ; Barra de progresso	
	mov eax, 0x01000020
	mov ebx, 0x00140120
	mov ecx, 0x000a0001
	;mov ebp, 5
	;int 0x42
	;mov ebp, 7
	;int 0x42
	
	
  ; Texto
	;mov eax, 0x00200020
	;mov ebx, 0x01000100
	;mov ecx, 0x00060000
	;mov ebp, 5
	;int 0x42
	
	
	;mov esi, Teste_String
	;call ObjEdit_SetText
	
	
	;mov ebp, 7
	;int 0x42
	
	;mov [Obj_Active], edi
	
  ; Barra de progresso
	;mov eax, 0x00000000
	;mov ebx, 0x00100068
	;mov ecx, 0x000a0001
	;mov ebp, 5
	;int 0x42
	;mov ebp, 7
	;int 0x42
	
  ; Imagem
  	mov eax, 0x00100010
	mov ebx, 0x01500150
	mov ecx, 0x00040005
	mov ebp, 5
	int 0x42
	mov dword [edi+0x30], 0x80000600
	mov dword [edi+0x3c], 0x00000000
	mov byte  [edi+0x40], 0x1c
	;mov ebp, 7
	;int 0x42
	
	call ObjImage_SetAttr
	call Object_AllDraw
		
	ret
	
	