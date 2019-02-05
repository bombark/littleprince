;-----------------------------------------------------------------------------------------

; Gerenciador de Tarefa (Programa)

;-----------------------------------------------------------------------------------------


;======================== V A R I A V E I S ==============================================

; OBS: (nao mudar essas ordem sem mudar os valores das variaveis colocados na pilha)
; Int
; pushad
; call

; Variaveis colocado na pilha por um int
reg_EFLAGS	equ 0x2c
reg_CS		equ 0x28
reg_EIP		equ 0x24

; Variaveis colocados na pilha por um pushad
reg_EAX		equ 0x20
reg_ECX		equ 0x1c
reg_EDX		equ 0x18
reg_EBX		equ 0x14
reg_ESP		equ 0x10
reg_EBP		equ 0xc
reg_ESI		equ 0x8
reg_EDI		equ 0x4

; Variaveis colocado na pilha por um call
reg_RET		equ 0x0

  ; Status das Tarefas ===========================
    Task_Actived	equ 0x0
    Task_Paused	equ 0x1
    Task_Timed	equ 0x2
    Task_Travado	equ 0x3	; Quando estiver travado, todos os comandos irao para o sistema
    Task_Condenada	equ 0x4
  ;-----------------------------------------------  

; Nome da tarefa principal do SO
OS_Name	db "Sistema",0
OS_Path	db "C:\",0

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 ==============================================
; Initialize main OS task
; IN:
;	Sistema de Memoria Iniciado
; OUT:
;	nothing

align 4
init_TaskSystem:	; Inicia a tarefa do sistema

; inicializa OS_TSS (????) - necessario quando houver troca de nivel 3 para 1
; pode ser usado como local para guarda temporaria de contexto
; pode se usar troca de tarefas dentro do mesmo segmento, via call ou jmp
	mov dword [TSS_OS+TSS_ESP0], 0x8009effe
	mov word [TSS_OS+TSS_SS0], segData
	mov dword [TSS_OS+TSS_ESP1], 0
	mov dword [TSS_OS+TSS_SS1], 0
	mov dword [TSS_OS+TSS_ESP2], 0
	mov dword [TSS_OS+TSS_SS2], 0
	mov dword [TSS_OS+TSS_CR3], OS_PageTable
	mov dword [TSS_OS+TSS_EIP], OS_loop		; pensar em algo melhor....
	mov word [TSS_OS+TSS_EFLAGS], 001000000010b
	mov word [TSS_OS+TSS_CS], segCode
	mov word [TSS_OS+TSS_DS], segData
	mov word [TSS_OS+TSS_ES], segData
	mov word [TSS_OS+TSS_FS], segData
	mov word [TSS_OS+TSS_GS], segData
	mov word [TSS_OS+TSS_SS], segData
	mov dword [TSS_OS+TSS_ESP], 0x8009fffe
	mov dword [TSS_OS+TSS_BYTE], 0

; Cria uma tarefa e coloca na lista de tarefa
	mov word [Timer_cont], 0x2
	mov dword [QtdeBytes_List], Size_DescTask
	
  ; Aloca um espaco	
	mov eax, Size_DescTask+7
	call aloca_HB
  	mov edi, esi	
	
  ; Coloca na lista
  	mov [Task_List_Ini], edi
  	mov [Task_List_End], edi
  	mov [Task_Current], edi	
	mov dword [edi+Task_Prox], 0
	mov dword [edi+Task_Ant], 0
	
  ; Coloca o nome
  	push esi	
	push edi
	mov ecx, 7
	mov esi, OS_Name
	add edi, Size_DescTask
	rep movsb
	pop edi
	pop esi
	
  ; Coloca as variaveis	
	mov byte [edi+Task_Status], Task_Actived
	mov byte [edi+Task_PageTable], OS_PageNumberTable
	mov word [edi+Task_Cont], 0x2		; ciclos para a tarefa
	mov [edi+Task_Pilha_n0], esp
	mov byte [edi+Task_Attr], Task_Attr_Sistema or Task_Attr_Destroy	
		; Tarefa do sistema, nao pausavel, nao destrutivel

  ; Inicia o Heap padrao no sistema
	mov eax, [ini_Heap]
	mov [edi+Task_Heap_Base], eax
	mov eax, [size_Heap]
	mov [edi+Task_Heap_Size], eax

; Aloca os blocos
   ; Coloca o path do sistema
   	push esi
   	push edi
   	mov esi, OS_Path
   	call strlen
   	mov eax, ecx
   	call aloca_HB
   	mov [edi+Task_Path], esi
   	mov edi, esi
	mov esi, OS_Path
	call cpystr
	pop edi

   ; Cria os buffers circulares do sistema (Eventos e teclado)
	mov eax, Size_CicleBuffer
	call aloca_HB
	mov dword [edi+Task_EventoBuffer], esi
	mov byte [edi+Task_EventoPR], 0
	mov byte [edi+Task_EventoPW], 0
	mov byte [edi+Task_EventoStatus], 0
	mov byte [edi+Task_EventoSize], Size_CicleBuffer
	call aloca_HB	
	mov dword [edi+Task_EventoSBuffer], esi
	mov byte [edi+Task_EventoSPR], 0
	mov byte [edi+Task_EventoSPW], 0
	mov byte [edi+Task_EventoSStatus], 0
	mov byte [edi+Task_EventoSSize], Size_CicleBuffer

   ; Zera as variaveis para o sistema de arquivo
	mov dword [edi+Task_Ass], 'TRF1'
	xor eax, eax
	mov [edi+Task_Directory], eax
   	mov [edi+Task_PathLba], eax		; coloca o lba do diretorio base
   	
   	pop esi
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 1 ==============================================
; Cria um espaco para a tarefa
; IN:
;	eax= quantidade de memoria
;	 cl= quantidade de clock
;	esi= nome da tarefa (A string deve terminar com 0)
; OUT:
;	esi= tabela de diretorio


;Obs: Criar uma protecao para que nao seja alocada uma pagina nos descritores acima de 2gb
;Obs: O eax deve estar arrendondado em multiplos de 4mb

Task_Create:
	pushad

; Coloca na lista de programas
	push ebx
	push eax
	push ecx
	call strlen
	mov eax, ecx
	mov ebx, Task_List_Ini	
	call List_AddItem				
	pop ecx
	
; Inicializa as variaveis 
	xor eax, eax
	and cx, 0xff
	mov [edi+Task_Cont], cx
	mov [edi+Task_Reserved], ax
	mov [edi+Task_Heap_Base], eax
	mov [edi+Task_Heap_Size], eax
	mov [edi+Task_EventoSPR], al
	mov [edi+Task_EventoSPW], al
	mov [edi+Task_EventoSSize], ax
	mov [edi+Task_EventoSBuffer], eax
	mov [edi+Task_EventoPR], al
	mov [edi+Task_EventoPW], al
	mov [edi+Task_EventoSize], ax
	mov [edi+Task_EventoBuffer], eax
	mov [edi+Task_Path], eax
	mov [edi+Task_PathLba], eax
	mov [edi+Task_Directory], eax
	mov dword [edi+Task_Ass], 'TRF1'
	mov [edi+Task_Free3], eax
	mov byte [edi+Task_Status], Task_Actived
	mov byte [edi+Task_Attr], Task_Attr_Pausavel 	; Tarefa normal e pausavel
	pop eax

; Inicia a tabela de paginas do programas
  ; Procura uma tabela livre
	mov esi, PageTable+0x1000	; elimina a tabela do sistema
.a:	cmp dword [esi], 0
	je .found
	add esi, 0x1000
	cmp esi, 0x200000
	jae .error
	jmp .a

  ; Apaga os primeiros 512 descritores (memoria para o programa - 2gb)
.found:	mov ecx, esi
	and ecx, 0x000ff000
	shr ecx, 0x0c		; dividi por 4kb
	mov [edi+Task_PageTable], cl
	push eax
	push esi
	xor eax, eax
	mov ecx, 0x200		; 512 descritores para apagar
.b:	mov [esi], eax
	add esi, 4
	loop .b
	pop esi
	pop eax

  ; Copia os ultimos 512 descritores da tabela de pagina do sistema
	push esi
	push edi
	lea edi, [esi+0x800]
	mov esi, OS_PageTable+0x800
	mov ecx, 0x200		; 512 descritores
	rep movsd
	pop edi
	pop esi

; Aloca uma pagina para o programa	
	mov ecx, eax
	shr ecx, 22		; divide por 4 mb
	mov ebx, esi
.c:	call aloc_Page
	jc .error
	loop .c
	pop ebx

; Ajusta os registradores
	sub eax, 0x40		; 0x40 = pushad + call + 'iretd' + 0x10 free
	mov dword [edi+Task_Pilha_n0], eax

; Aloca os buffers circulares (teclado e eventos)
	push esi
	mov eax, Size_CicleBuffer
	call aloca_HB	
	mov dword [edi+Task_EventoBuffer], esi
	mov byte [edi+Task_EventoPR], 0
	mov byte [edi+Task_EventoPW], 0
	mov byte [edi+Task_EventoStatus], 0
	call aloca_HB		
	mov dword [edi+Task_EventoSBuffer], esi
	mov byte [edi+Task_EventoSPR], 0
	mov byte [edi+Task_EventoSPW], 0
	mov byte [edi+Task_EventoSStatus], 0
	pop esi

; Passa os parametros
	mov [esp+reg_ESI-4], esi
	popad
	ret

.error:	stc
	popad
	ret


;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 2 ==============================================
; Destroi uma Tarefa
; IN:
;	esi= Programa a ser eliminado
; OUT:
;	nothing

Task_Destroy:		
	push eax
	push ebx
	push esi
	push edi
	cmp dword [esi+Task_Ass], 'TRF1'
	jne .end

	;test byte [esi+Task_Attr], Task_Attr_Destroy
	;jnz .end

  ; Destroy os objetos  - Nao testado
    ; Percorre a lista de objetos-pai, se for do programa -> libera 
	
  	mov edi, [ObjList_Ini]
  	mov eax, edi  
  
.loop1:   or edi, edi
  	jz .libarq	
  	mov eax, [edi+4]	; Obj_Prox
  	cmp [edi+0x14], esi	; Obj_Program
  	jne .prox1
  	call Object_Destroy	
  			
.prox1:	mov edi, eax	; Obj_Prox
  	jmp .loop1  	 
  	 
  ; Libera os arquivos - Nao testado
.libarq:	;mov edi, [File_Table]
	;xor eax, eax
.loop2:	;cmp ax, [File_Table_Qtde]
	;jae .timer
	;cmp [edi+0x8], esi		; File_Program
	;jne .prox2	
	;call File_Close
.prox2:	;add edi, 0x40		; File_DescSize
	;inc eax
	;jmp .loop2
  
  ; Retira do timer  
.timer:	mov ebx, Task_List_Ini
  	call List_DelItem
  
  ; Libera a tabela	
  	movzx edi, byte [esi+Task_PageTable]  	
  	shl edi, 0x0c	; multiplica por 4kb
  	add edi, OS_BASE+PageTable
  	xor esi, esi
  	
    ; For (esi=0; esi<512; esi++) 	
.loop3:  	mov eax, [edi+esi*4]
    
      ; Se pagina ocupada  -> Libera
    	or eax, eax
    	jz .inc
    	call Page_Free      	 	
.inc:	inc esi
	cmp esi, 0x200		; numeros de descritores/2    		
  	jb .loop3
  
    ; Zera as paginas marcadas pelo sistema  
      ; (Necessario apenas se a memoria do sistema diminuir)	
  	
  ; Fim	
.end:  	pop edi
	pop esi
  	pop ebx
  	pop eax
	ret

;-----------------------------------------------------------------------------------------







;======================== C O M E N T A R I O  ===========================================
; O programa vai ter 3 buffers para a comunicacao entre programas.
;   1° Buffer: vai ser utilizado para eventos pré-programaveis do mouse (mover,redimensionar a janela)
;   2° Buffer: vai ser utilizado para eventos comum do mouse (Botao pressionado,...) 
;   3° Buffer: uso livre 
; Foi criado 3 buffers, ao inves de 1, pois existe um problema na hora de pega os 
; os dados do buffer. Mande um email para maiores explicacoes, to sem paciencia agora.

; Para o 1° e 2° buffer seguiram o seguinte padrao
;     2b      1b     1b
; | Codigo | Acao | Extra | 

; 3° buffer:
;   2b      ?b
; | ID  | Dados |

;   obs: o ID é o codigo do programa que enviou os dados
;        Este buffer nao é criado imediatamente, caso for usa-lo, é necessario
;        chamar um servico do sistema para cria-lo 

; Palavra  do evento
	WordEv_Code	equ 0x0	; dw - Codigo do evento
	WordEv_Acao	equ 0x2	; db - Acao sobre o evento
	WordEv_Extra	equ 0x3	; db - Informacao extra (numero da janela, tecla digitada)

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
; Envia dados para o 1° ou 2° buffer circular do programa
; IN:
;	cx: codigo do evento
;	dl: acao
;	dh: byte extra
;	esi: ponteiro do programa
; OUT:
;	cy= 0  -> OK
;	cy= 1  -> Error, buffer cheio

Task_SendEvento:
	push ebx

	cmp dword [esi+Task_Ass], 'TRF1'
	jne .error

  ; Pega o ponteiro do buffer e atualiza o PW e PR
	movzx ebx, byte [esi+Task_EventoPW]
	add bl, 4
	cmp bl, Size_CicleBuffer
	jb @f
	xor bl, bl
@@:	cmp bl, [esi+Task_EventoPR]
	je .error				; Buffer full
	mov [esi+Task_EventoPW], bl
	add ebx, [esi+Task_EventoBuffer]

  ; Coloca o evento no buffer
.put:	mov [ebx+WordEv_Acao], dh		; Tipo da acao
	mov [ebx+WordEv_Code], cx		; Codigo do evento
	mov [ebx+WordEv_Extra], dl		; Byte extra

  ; Ativa o programa	
  	mov byte [esi+Task_Status], Task_Actived 

  ; Fim
	pop ebx
	ret

  ; Error - Buffer cheio
.error:	stc
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Recebe dados do 1° ou 2° buffer circular do programa
; IN:
;	esi: Ponteiro do programa
; OUT:
;	ecx: codigo

Task_ReceiveEvento:
	push ebx

	cmp dword [esi+Task_Ass], 'TRF1'
	jne .empty

  ; Ajusta ponteiros de leitura
	movzx ebx, byte [esi+Task_EventoPR]
	cmp bl, [esi+Task_EventoPW]
	je .empty			; Buffer full
	add bl, 4
	cmp bl, Size_CicleBuffer
	jb .a
	xor bl, bl
.a:	mov [esi+Task_EventoPR], bl
	add ebx, [esi+Task_EventoBuffer]
	
  ; Lê a palavra do evento ( Codigo | Acao )
	mov cx, [ebx+WordEv_Code]
	mov bl, [ebx+WordEv_Acao]
	test bl, 0x80
	jz .end	
	shl ecx, 0x10	; Caso seje um sub-objeto o codigo fica na parte alta
	
  ; Fim	
.end:	pop ebx
	ret

  ; Error - buffer cheio
.empty:	stc
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 4 ==============================================
; Envia dados para o 3° buffer circular do programa
; IN:
;	esi: Ponteiro do programa
;      dl/dx/edx: dados
;          cl: Qual registrador (0=al,1=ax,2=eax)
; OUT:
;	notihng

Task_SendDados:
	push ebx
	push ecx
	
	cmp dword [esi+Task_Ass], 'TRF1'
	jne .error
	
	or cl, cl
	jz .byte
	cmp cl, 1
	je .word
	cmp cl, 2
	je .dword
	jmp .end 

  ; Coloca um byte
.byte:	movzx ebx, byte [esi+Task_EventoSPW]
	mov cl, bl
	add cl, 1
	cmp cl, Size_CicleBuffer
	jb @f
	mov cl, 1
	xor bl, bl
@@:	cmp cl, [esi+Task_EventoSPR]
	je .error				; Buffer full
	add ebx, [esi+Task_EventoSBuffer]
	mov [ebx], dl
	mov [esi+Task_EventoSPW], cl
	jmp .end

  ; Coloca um word
.word:	movzx ebx, byte [esi+Task_EventoSPW]
	mov cl, bl
	add cl, 2
	cmp cl, Size_CicleBuffer
	jb @f
	mov cl, 2
	xor bl, bl
@@:	cmp cl, [esi+Task_EventoSPR]
	je .error				; Buffer full
	add ebx, [esi+Task_EventoSBuffer]
	mov [ebx], dx
	mov [esi+Task_EventoSPW], cl
	jmp .end

  ; Coloca um dword
.dword:	movzx ebx, byte [esi+Task_EventoSPW]		
@@:	mov cl, bl
	add cl, 4
	cmp cl, Size_CicleBuffer
	jb @f
	mov cl, 4
	xor bl, bl
@@:	cmp cl, [esi+Task_EventoSPR]
	je .error				; Buffer full
	add ebx, [esi+Task_EventoSBuffer]
	mov [ebx], edx
	mov [esi+Task_EventoSPW], cl
	jmp .end

  ; Ativa o programa
.end:	mov byte [esi+Task_Status], Task_Actived 
	pop ecx
	pop ebx
	ret

  ; Error - buffer cheio  (espera um tempo e faz outra tentativa - Nao feito ainda)
.error:	stc
	pop ecx
	pop ebx
	ret

;-----------------------------------------------------------------------------------------


;======================== F U N C T I O N 5 ==============================================
; Recebe dados do 3° buffer circular do programa
; IN:
;	esi: Ponteiro do programa
;          cl: Qual registrador (0=al,1=ax,2=eax)
; OUT:
;      dl/dx/edx: dados

Task_ReceiveDados:
	push ebx
	push ecx
	
	cmp dword [esi+Task_Ass], 'TRF1'
	jne .empty
	cmp byte [esi+Task_EventoSStatus], 0	; verifica se o buffer está ocupado
	jne .empty
	
	or cl, cl
	jz .byte
	cmp cl, 1
	je .word
	cmp cl, 2
	je .dword
	jmp .end 

  ; Pega um byte
.byte:	movzx ebx, byte [esi+Task_EventoSPR]
	mov cl, bl
	cmp cl, [esi+Task_EventoSPW]
	je .empty
	add cl, 1
	cmp cl, Size_CicleBuffer
	jb @f
	mov cl, 1
	xor bl, bl
@@:	add ebx, [esi+Task_EventoSBuffer]
	mov dl, [ebx]
	mov [esi+Task_EventoSPR], cl
	jmp .end

  ; Pega um word 
.word:	movzx ebx, byte [esi+Task_EventoSPR]
	mov cl, bl
	cmp cl, [esi+Task_EventoSPW]
	je .empty
	add cl, 2
	cmp cl, Size_CicleBuffer
	jb @f
	mov cl, 1
	xor bl, bl
@@:	add ebx, [esi+Task_EventoSBuffer]
	mov dx, [ebx]
	mov [esi+Task_EventoSPR], cl
	jmp .end 
	
  ; Pega um dword	
.dword:	movzx ebx, byte [esi+Task_EventoSPR]
	mov ch, bl
	mov cl, bl
	cmp cl, [esi+Task_EventoSPW]
	je .empty
	add cl, 4
	cmp cl, Size_CicleBuffer
	jb @f
	mov cl, 4
	xor bl, bl
@@:	add ebx, [esi+Task_EventoSBuffer]
	mov edx, [ebx]	
	mov [esi+Task_EventoSPR], cl
 
  ; Fim
.end:	pop ecx
	pop ebx
	ret

  ; Error - Buffer Vazio
.empty:	stc
	pop ecx
	pop ebx
	ret

;-----------------------------------------------------------------------------------------