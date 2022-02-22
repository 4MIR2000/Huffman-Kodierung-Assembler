.intel_syntax noprefix
.global coding

.text 
//rdi: dataMemPtr, rsi: resultPtr, rdx: resultMaxSize 

//diese Funktion übersetzt das Eingabewort in die Kodierung
//Rückgabewert beim Erfolgsfall: Länge der gesamten Kodierung in Bytes
//Rückgabewert in alle anderen Fällen: -1 (z.B result-Array zu klein)
/*

Stack (immer 8 Byte Aligned): 
 _______________________
| rbp gesichert   		|        
|_______________________|		   
| rbx gesichert         |
|_______________________|		   
| Wörterbuch Größe      |
|_______________________|
| result Größe	        |
|_______________________| 
| Startadresse Kodierung| <- rbp zeigt hier drauf
|_______________________|
| result insert_index   |
|_______________________|
| Puffer				|
|_______________________|
| rcx gesichert			|
|_______________________|
| rdi gesichert 		|
|_______________________|


*/

coding: 
	push rbp 
	push rbx 
	xor rbx, rbx 
	push rbx 					//später: Wörterbuch Größe 
	push rdx					//später: resultMaxSize: wird nach jedem Einfügen subtrahiert   	
	mov rbp,rsp
	push rbx 					//später: Startadresse der Kodierung 
	push rbx 					//später: aktuelles Result insert_index 	
	push rbx            		//später: Puffer: enthält komprimierten code, der noch nicht rausgeschrieben wurde 
	xor rax, rax 
	xor rcx,rcx 				
    xor rdx, rdx 				 
	xor r8,r8 					//r8b: Der Puffer insert_index. Also die nächste freie stelle im Puffer
	xor r9, r9 

	.LgetMemoryStartAdr:  		//am Anfang vom Result ist das Wörterbuch gespeichert, welches mit einem 1-Byte endet. Danach beginnt die Kodierung
    cmp	BYTE ptr [rsi+rax],1	//rax ist counter im Moment
    je .LstartAdrFound 
	add rax, 2 					
	jmp .LgetMemoryStartAdr 

	.LstartAdrFound: 
	inc rax 					//Trenn-Byte überspringen 
	mov [rbp+8], rax 			//dict size 
	sub [rbp],rax 				//verbleibende max. size von result updaten 
	cmp qword ptr [rbp], 0		//maximale größe schon überschritten? 
	je .Lfail 	
	add rax, rsi 				//in rax steht nun start adr. von der Kodierung 
	mov [rbp-8], rax  

	.LdataLoop:						
	xor rdx,rdx					//Wörterbuch_array_index
	mov al,[rdi+rcx]    		//al: nächstes Zeichen
	cmp al,0					//alle Zeichen schon abgearbeitet? 
	je .Lending
	
	.LdictLoop:
	cmp rdx,[rbp+8] 			//größe vom Wörterbuch schon überschritten? 
	jge .Lfail					//es kommt ein Zeichen vor, welches nicht im Wörterbuch vorkommt->Fehler. 
	mov bx,[rsi+rdx]			//nächsten Wörterbuch Eintrag holen. bl:buchstabe  bh:code
	mov ah,bl          			//ah: Buchstabe im Wörterbuch Eintrag
	and ah,128          		//letzten bit davon holen 
	cmp ah,128
	jne .LcompareDictEntryAndChar
	or al,128					//letzten bit von der Buchstabe in data auch auf 1 setzen um einen Korrekten Vergleich zu machen

	.LcompareDictEntryAndChar:
	cmp al,bl   
	je .LdictEntryFound 
	and al, 127					//letzten bit von der Buchstabe in data wieder auf 0 setzen 
	add rdx,2					//zur nächsten Eintrag in dict gehen 
	jmp .LdictLoop 
	
	.LdictEntryFound: 
	mov dl, -1
	push rcx					//rcx auf dem Stack sichern, um das später verwenden zu können
	xor rcx,rcx 
	cmp ah,128
	mov al, 128					//letzten bit auf 1 setzen 
	je .LfirstBitIsOne  

	.LfirstBitIsZero: 
	test al, bh 
	jz .LcheckIfEnoughSpace
	shr al,1					//shiften al solange weiter, bis wir auf eine Null stößen (da beginnt die eigentliche Kodierung) 
	shr dl,1					//dl wird nachher verwendet, um die bits vor der Kodierung auf 0 zu setzen
	inc cl 						//cl: wid später verwendet, um die Größe der Kodierung zu berechnen. 
	jmp .LfirstBitIsZero 

	.LfirstBitIsOne: 
	test  al, bh 
	jnz .LcheckIfEnoughSpace 	//shiften al solange weiter, bis wir auf eine eins stößen (da beginnt die eigentliche Kodierung) 
	shr al,1
	shr dl,1					//dl wird nachher verwendet, um die bits vor der Kodierung auf 0 zu setzen
	inc cl 						//cl: wid später verwendet, um die Größe der Kodierung zu berechnen. 
	jmp .LfirstBitIsOne 

	.LcheckIfEnoughSpace: 
	mov r9b, cl 
	sub r9b, 8 
	neg r9b  					//in r9b steht jetzt die eigentliche größe des codes 
	add r9b, r8b      			// in r9b steht jetzt die Größe vom Puffer nach dem hinzufügen des neuen codes in Bits  
	test r9b, 7
	jnz  .LroundUp				//falls die Größe nicht durch 8 teilbar ist, müssen wir aufrunden  
	shr r9b, 3					//in r9b steht jetzt die Größe vom Puffer in Bytes
	jmp .Lcheck 

	.LroundUp: 
	shr r9b, 3 
	inc r9b						//in r9b steht jetzt die Größe vom Puffer in bytes (aufgerundet) 

	.Lcheck:  
	cmp r9, [rbp]
    jg .Lfail 					//Noch genug platz im result_array?  

	.Lcompress: 
	and bh,dl   				//die Bits vor der eigentlichen Kodierung auf 0 setzen 
	push rdi 
	mov rdi,rbx					//bh, also das Codewort in rdi kopieren, um es nachher mit dem Puffer addieren zu können  
	shr rdi, 8 
	add cl, 56 					//der start_index von der Kodierung ist geändert, da der code in rdi(um 56 größer) steht!
	cmp cl,r8b 					//den start_index von der Kodierung und den insert_index vom Puffer vergleichen 
	jge .LshiftCodeLeft 		//die nächste freie Stelle im Puffer ist links. jge und nicht jg, um später markcodeEnding einfach Ausführen zu können
		
	.LshiftCodeRight:
	mov r9b, cl  
	sub r9b, r8b
	neg r9b 					//in r9b steht die Anzahl der Stellen um die wir den code shiften müssen 
	mov ch, cl 
	mov cl, r9b 
	shr rdi, cl 
	add [rbp-24], rdi			//die Kodierung in den Puffer einfügen 
	jmp .LwriteQWToMemory		//Puffer ist jetzt voll und sollte in result reingeschrieben werden 

	.LshiftCodeLeft: 
	mov r9b,cl	 	    		//in r9b steht der start_index von der Kodierung in rdi
	sub r9b,r8b		   			//in r9b steht jetzt die Anzahl der Stellen um die wir die Kodierung shiften müssen,
								//um es dann mit dem Buffer richtig zu addieren 
	mov ch, cl 
	mov cl, r9b 
	shl rdi, cl  
    add [rbp-24], rdi			//die Kodierung im Puffer einfügen 
    mov cl, ch 
    sub cl,64					//die länge von der Kodierung herausfinden 
    neg cl
    add r8b, cl					//Puffer insert_index updaten     	 
	jmp .LnextChar 	
	
	.LwriteQWToMemory: 
	mov cl, ch 					//in cl steht wieder der start_index von der Kodierung in rdi 
	sub ch, 64					//in ch steht jetzt die Größe der letzten Kodierung
	neg ch 
	mov rdi,rbx 				//wir müssen noch den Teil von dem code, der nicht mehr in den Puffer reingepasst hat, noch einfügen 
	shr rdi,8					//kopiere bh also das Codewort im Wörterbuch wieder ins rdi 
	shl rdi, cl 				//die Kodierung an den Anfang bringen 

								//in r8b steht noch der insert_index vom Puffer vor dem Einfügen der letzten Kodierung
	sub r8b, 64					
	neg r8b 					//in r8b steht jetzt die Anzahl der Bits die noch in den Puffer eingefügt werden konnten 
	mov cl, r8b 			
	shl rdi,cl   				//die bereits eingefügten Bits überspringen  

	mov cl, ch
	mov r9b, cl 				//in r9b steht jetzt die Größe der letzten Kodierung
	sub r8b, r9b  				
	neg r8b 					//in r8b steht nun die Anzahl der Bits, die in dem neuen Puffer eingefügt wurden = der neue insert_index vom Puffer

	mov r9, [rbp-24] 			//vollen Puffer in r9 sichern, bevor er mit dem neuen überschrieben wird
	bswap r9

	mov [rbp-24], rdi 			//in den neuen Puffer einfügen 
	mov rbx, [rbp-16]			//rbx: Result insert_index holen 
	mov rax, [rbp-8] 			//rax: Startadresse der Kodierung 

	mov [rax+rbx], r9			//vollen Puffer in result einfügen  
	add rbx, 8 					//Result insert_index updaten 
	mov [rbp-16],rbx
	sub qword ptr [rbp], 8 		//maximale verbleibende Größe updaten 

	.LnextChar:			
	pop rdi
	pop rcx 
	inc rcx 					//zum nächsten Zeichen 
	xor rbx, rbx 
	xor rax, rax 
	xor r9, r9 
	jmp .LdataLoop 
	
	.Lending: 
	mov rax, [rbp-16] 			//in rax steht unser rückgabewert: Größe der Gesamten-Kodierung(Alle Kodierungen)	         
	mov rdi, [rbp-24]			//in rdi steht der Puffer
	mov r9b, r8b 
	shr r8b, 3  				//wir brauchen die Größe in Bytes->durch 8 teilen 
	add rax, r8 				//zu der Größe der Kodierungen in Result die Anzahl der gespeicherten Bytes im Puffer addieren
	test r9b,7					//schauen, ob die größe durch 8 teilbar ist 
	jz .LwriteRemainingBytes 
						
								//falls size nicht durch 8 teilbar ist, müssen wir aufrunden  	 
	inc r8b             		//in r8b steht jetzt die Größe vom Puffer in bytes (aufgerundet) 
    inc rax	 					//Größe der Gesamt-Kodierung auch anpassen

	.LmarkcodeEnding: 			//-> wir müssen das Ende der Gesamt-Kodierung angeben, damit eine korrekte Ausführung der Dekodierung möglich ist. 								 
	mov cl, r9b 				//in r9b steht noch der insert_index des Puffers 
	sub cl, 64
	neg cl 						//in cl steht die Anzahl der Stellen um die wir 1 nach links schiften müssen, um auf die Stelle des letzten Bits des letzten Bytes zu landen
	mov rbx,1
	shl rbx, cl 
	test rbx, rdi 				//überprüfe, ob das letzte bit 0 oder 1 ist 
	jnz .LlastBitOne
	
	.LlastBitZero:				//die Bits des letzten Bytes nach der Kodierung auf 1 setzen 	
	mov cl, r9b 				//in cl steht die Anzahl der Rechts shifts, die für -1 Ausgeführt werden sollen, um an den Stellen nach der Kodierung im Puffer nur Einsen zu haben 
	mov rbx, -1
	shr rbx, cl
	or rdi, rbx 
	jmp .LwriteRemainingBytes

	.LlastBitOne: 				//das zweite Bit des Splitters muss nun eine 1 sein 
	mov r9, [rbp-8]				//in r9 steht die Startadresse der Kodierung 
	dec r9 						//in r9 steht jetzt die Adresse des Splitters
	mov BYTE ptr [r9], 3		//splitter updaten 

	.LwriteRemainingBytes:
	bswap rdi 					//weil little endian	
	mov rbx,[rbp-8] 			//in rbx steht jetzt die Startadresse der Gesamt-Kodierung(Alle Kodierungen)
	mov rcx ,[rbp-16]			//in rcx steht jetzt der Result-insert_index
	
	.LwriteLoop: 
	cmp r8b, 0 
	jle .Lclose 		    	//nichts mehr übrig-->fertig

	cmp r8b, 8					
	je .LcopyEightBytes			//noch 8 Bytes im Puffer?

	cmp r8b, 4					
	jge .LcopyFourBytes			//noch 4 oder 5 oder 6 oder 7 Bytes?

	cmp r8b, 2					
	jge .LcopyTwoBytes 			//noch 2 oder 3 Bytes?

	.LcopyOneByte: 				//noch 1 Byte übrig.
	mov [rbx+rcx], dil 
	jmp .Lclose				

	.LcopyTwoBytes:
	mov [rbx+rcx], di  
	sub r8b, 2
	shr rdi, 16
	add rcx, 2					//result insert_index updaten
	jmp .LwriteLoop 

	.LcopyFourBytes: 				 
	mov [rbx+rcx], edi 
	sub r8b, 4 
	shr rdi, 32
	add rcx, 4 					//result insert_index updaten
	jmp .LwriteLoop			 	//restliche Bytes kopieren 
	
	.LcopyEightBytes: 
	mov [rbx+rcx], rdi 		

	.Lclose:  
	mov rsp,rbp  
	add rsp, 16					 
	pop rbx 
	pop rbp 
	ret 

	.Lfail: 
	mov rax, -1 
	jmp .Lclose 

