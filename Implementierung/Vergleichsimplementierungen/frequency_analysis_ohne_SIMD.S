.intel_syntax noprefix
.global frequency_analysis

.text
  
  // int frequency_analysis(char* data, short* result, long result_size)
  
frequency_analysis:

  // Dies ist eine Version von frequency_analysis, welche keine SIMD-Operationen verwendent;
  // wird für die Performanzanalyse verwendet und soll zeigen, ob sich SIMD-Operationen hier lohnen.
  // Die Funktion frequency_analysis führt die Häufigkeitsanalyse auf einem übergebenen data-Array durch und schreibt das Ergebnis (die unsortierte Häufigkeitstabelle) in das übergebene result-Array,
  // wobei im result-Array 2-byteweise abwechselnd der ASCII-Code und dann die jeweilige Häufigkeit abgespeichert werden.
  // Bsp.: ABRABBBBRR wird als 'A', 2, 'B', 5, 'R', 3, wobei jew. zwei Bytes für das Zeichen und zwei Bytes für die Häufigkeit verwendet werden, gespeichert,
  // d.h. an Bytes 0,1 im result-Array steht 'A', an Bytes 2,3 steht 2, an Bytes 4,5 steht 'B', ...
  // Das result-Array dient nur als Zwischenspeicher, damit kein zusätzlicher Speicherplatz alloziert werden muss; die Häufigkeitstabelle wird von der Funktion huffman_encode nicht ausgegeben.
  // Das result-Array wird von dieser Funktion nicht sortiert, sondern muss an die Funktion quicksort_result übergeben werden. Die Zeichen werden hier in der Reihenfolge des ersten Auftretens abgespeichert.
  // Falls ein Fehler auftritt gibt die Funktion -1 zurück (Fehlerkategorien s.u.), und sonst die Anzahl des im result-Array verwendeten Speicherbedarfs in Shorts.


  // Übergebene Parameter:
  //	rdi: Zeiger auf data-Array
  //	rsi: Zeiger auf result-Array
  //	rdx: result_size (in Bytes)
  
  // Register:
  // 	r8: Zählerregister für data-Array (äußere Schleife)
  //	r9: Zählerregister für result-Array
  //	r10b: aktuelles ASCII-Zeichen
  //	r11b: Häufigkeit des aktuellen ASCII-Zeichens
  //	r12: tmp-Register
  //	r13: tmp-Register
  //	r14: Schleifenzähler für die beiden inneren Schleifen: zählt von r8 bis data_size-1 bzw. von (r8-(r8%16)) bis data_size-1 bei SIMD-Operationen, um das 16-Byte-Alignment einzuhalten
  //	r15: Länge des data-Arrays
  //	rcx: Länge des data-Arrays - 1
  
  
	// Register initialisieren, falls nötig sichern

	xor r9, r9
	xor r10, r10
	xor r11, r11
	push r12
	push r13
	push r14 
	xor r14, r14
	push r15
	
	
	// Länge des data-Arrays ermitteln
  	
	or r8, -1
  	
	.LlängeDataArrayErmitteln:
		inc r8
		cmp byte ptr [rdi+r8], 0
		jne .LlängeDataArrayErmitteln
		mov r15, r8 // r15 speichert die Länge des data-Arrays
		mov rcx, r8 // rcx speichert die Länge das data-Arrays -1
		dec rcx
  			
	
	xor r8, r8
	dec rdx // result_size auf Indexzählweise anpassen
	
	
	// data-Array auf dem Stack sichern
	// Wird auf dem Stack gesichert, da die übliche Stackgröße für das data-Array ausreichend ist (die max. Größe der zu verarbeitenden Daten ist ohnehin durch andere Faktoren begrenzt),
	// Speicherallokation teuer ist und der Stack in dieser Funktion sonst nur zum Sichern von einigen Registern benötigt wird.
	
	mov r13, r15 		// r8 und r15 zum Zählen
	
	.L8BytesVonDataSichern:
		cmp r15, 8322937
		jge .Lfehler
		cmp r15, 8
		jl .LrestlicheBytesVonDataSichern
		mov r12, [rdi+r8]
		push r12
		add r8, 8
		sub r15, 8
		jmp .L8BytesVonDataSichern
		
	.LrestlicheBytesVonDataSichern:
		cmp r15, 0
		jle .Lgesichert
		shl rax, 8
		mov al, [rdi+r8]
		inc r8
		dec r15
		jmp .LrestlicheBytesVonDataSichern
		
	.Lgesichert:
		xor r8, r8
		mov r15, r13
		xor r13, r13
	



	or r8, -1




	.LschleifeAußen: // iteriert über das data-Array; Schleifenzähler: r8: zählt von 0 bis data_size-1 (=rcx)
  
	  	// Schleifenbedingung abprüfen
  	
	  	inc r8
	  	cmp r8, rcx
	  	jg .LgesamtesDataArrayBearbeitet


		// Zeichen aus data holen; prüfen ob Zeichen in erlaubten Bereich (127-ASCII -> Zeichen zw. 0 und 127) liegt und ggf. in result schreiben

		mov r10b, [rdi+r8]
		cmp r10b, -1
		je .LschleifeAußen
		cmp r10b, 2
		jl .Lfehler
		cmp r10b, 127
		jg .Lfehler
		cmp r9, rdx
		jg .Lfehler
		mov [rsi+r9*2], r10w
		inc r9
		mov r14, r8

		
	
			
		
		.Lschleife:
	
			cmp r14, r15
			je .Lbearbeitet		
		
			cmp r10b, [rdi+r14]
			jne .Lungleich
			mov BYTE ptr [rdi+r14], -1
			inc r11
		
			.Lungleich:
				inc r14
				jmp .Lschleife
	

		// ein Zeichen wurde bearbeitet, d.h. die Häufigkeit im data-Array wurde ermittelt
		// und jedes Vorkommen des Zeichens auf -1 gesetzt, um es als bearbeitet zu markieren
		// die Häufigkeit kann ins result-Array geschrieben werden; die Zähler werden für das nächste zu vergleichende Zeichen angepasst
		// und es wird zum Beginn der Schleife, die über das data-Array iteriert, gesprungen

		.Lbearbeitet:
	
			cmp r9, rdx
			jg .Lfehler
			mov [rsi+r9*2], r11w
			and r11w, 0x0000
			cmp r11, 0
			jne .Lfehler	 // Zeichen ist zu häufig vorgekommen
			inc r9
			xor r11, r11
			xor r14, r14
			jmp .LschleifeAußen
			
			
			



		
		// Wiederherstellen das data-Arrays und der gesicherten Register
		// Falls ein Fehler aufgetreten ist, gibt die Funktion -1 zurück, sonst die Anzahl der verwendeten Shorts in result-Array.
		// Gründe für Fehler können sein:
		//	Das result-Array ist zu klein, um die Kodierung durchzuführen-
		//	Ein Zeichen kommt zu häufig vor (mehr als 2¹⁶-1 = 65535 mal) und kann die nicht mehr gespeichert werden.
		//	Es kommt ein Zeichen vor, welches nicht on 7-Bit-ASCII (ASCII Zeichen 0 - 127) kodiert wird.
		
		
		
		.Lfehler:
			mov r9, -1		
		
		
		.LgesamtesDataArrayBearbeitet:
		
		
			.LrestlicheBytesVonDataWiederherstellen:
				test r15, 7
				jz .L8BytesVonDataWiederherstellen
				dec r15
				mov [rdi+r15], al
				shr rax, 8
				jmp .LrestlicheBytesVonDataWiederherstellen
	
	
			.L8BytesVonDataWiederherstellen:
				cmp r15, 8
				jl .Lwiederhergestellt
				pop r12
				sub r15, 8
				mov [rdi+r15], r12
				jmp .L8BytesVonDataWiederherstellen
		
			.Lwiederhergestellt:			

				pop r15
				pop r14
				pop r13
				pop r12
				mov rax, r9
				ret
		



