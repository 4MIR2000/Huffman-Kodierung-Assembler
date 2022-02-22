.intel_syntax noprefix
.global quicksort_result

.text

  // void quicksort_result(char* data, int low, int high)

quicksort_result:

  // Die Funktion quicksort_result sortiert das result-Array aus der Häufigkeitsanalyse nach den Häufigkeiten geordnet absteigend,
  // unter Verwendung des Quicksort-Algorithmus (Musterlösung aus Übungsaufgabe wurde verwendet)
  // und behält dabei die ursprüngliche Speicherstruktur (abwechsend jew. 2 Bytes für das Zeichen und dann 2 Bytes für die Häufigkeit) bei.
  // Eine weitere Sortierung von Zeichen mit gleichen Häufigkeiten findet nicht statt, da dies für die Huffmankodierung irrelevant ist. 
  


	// rdi: data-Array
	// rsi: low
	// rdx: high
	// ecx: pivot
	// r12: i
	// r13: j
	// r8d: data[i]
	// r9d: data[j]
	// r14: tmp

	cmp rsi, rdx		// low >= high
	jge .Lend
		
	push r12
	push r13
	push r14
	push r15
	
	
	// Zähler initialisieren und Pivot-Element laden
	
	mov ecx, [rdi+rsi*4] // als Pivot-Element wird wird das erste im zu sortierenden Bereich gewählt
	
	mov r12, rsi
	dec r12
	mov r13, rdx
	inc r13
	
	
	// äußere Schleife, die bis zur Abbruchbedingung i>=j läuft
	
	.LSchleifeAußen:
	
		// Pivot-Vergleich vorbereiten:
		
		mov r15d, ecx
		shr r15, 16
		
		.LiInkrementieren:					// i = i+1 solange data[i] < pivot
			inc r12
			mov r8d, [rdi+r12*4]
			mov r14d, r8d
			shr r14, 16
			cmp r14w, r15w
			jg .LiInkrementieren
		
		.LjDekrementieren:					// j = j-1 solange data[i] > pivot
			dec r13
			mov r9d, [rdi+r13*4]
			mov r14d, r9d
			shr r14, 16
			cmp r14w, r15w
			jl .LjDekrementieren
			
				
		cmp r12, r13				// Abbruchbedingung i>=j
		jge .LsortierenAbgeschlossen
		
		
		mov [rdi+r12*4], r9d			// vertauschen von data[i] und data[j]
		mov [rdi+r13*4], r8d
		
		
		jmp .LSchleifeAußen
		
		
		
	.LsortierenAbgeschlossen:
	
	// rekursive Aufrufe des Quicksort-Algorithmus der beiden Teilbereiche des Arrays
	
		// erste Aufruf für linke Hälfte: quicksort(data, low, j) -> rdi = data, rsi = low, rdx = j
			mov r12, rdx		// r12 = low
			mov r14, rdi
			mov rdx, r13
			call quicksort_result
				
		
		
		
		// zweiter Aufruf für rechte Hälfte: quicksort(data, j+1, high) -> rdi = data, rsi = j+1, rdx = high
			mov rdi, r14
			mov rsi, r13
			inc rsi
			mov rdx, r12
			call quicksort_result
			
			
			
	// Registerinhalte wiederherstellen und an die aufrufende Funktion zurückspringen		
				
	pop r15
	pop r14
	pop r13
	pop r12		
			
  	
  	.Lend:
		ret
	

