; Testes com o sistema de arquivo

;arq1 db "d:\papel.bmp",0
;arq2 db "d:\felipe\mmexx.bin",0
;arq2 db "c:\sistema\MMexx",0
arq2 db "d:\felipe\garota.jpg",0

;pasta db "imagens\",0

arq3 db "FELIPE     ","MMEXX   BIN",0


testFS:
int 1
	;mov esi, arq2
	;mov bl, 4
	;call Fat_Open

int 1
	;mov eax, [esi+FAT_RootCluster]
	;mov edx, arq3
	;call FAT_File_Search
;int 1

	;mov edi, 0x80000600
	;call File_AllRead

	ret




