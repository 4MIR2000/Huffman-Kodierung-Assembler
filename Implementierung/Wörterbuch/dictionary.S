.intel_syntax noprefix
.global dictionary

.text

//	Es wird anhand der Häufigkeitsanalyse die Kodierung erstellt.
//	Dabei wird ein Heap von Knoten erstellt, welche die jeweiligen
//	Buchstaben und deren Häufigkeiten enthält. Diese werden
//	Schritt nach Schritt "zusammengeführt" bis ein finaler Baum
//	zur Kodierung entstanden ist.
//	Dieser Baum wird traversiert und anhand von dem, das Wörterbuch
//	erstellt. Nach dem Wörterbuch wird das byte auf 1 gesetzt als
//	"Splitter" des Wörterbuchs und der Kodierung.
//	Die Rückgabe ist die Größe des Wörterbuchs.

// 	rdi: Häufigkeitsanalyse
// 	rsi: Länge der Häufigkeitsanalyse

dictionary:

	cmp rsi, 4
	je .LnurEinElement

	push rbp
	mov rbp, rsp
	sub rsp, 8 	
	mov [rbp - 8], rdi		// Pointer auf result array wird oft verändert
	push rsi
	
	call .LerstelleHeap
	add rax, 0x08			// damit man leichter auf die Elemente zugreifen kann

	push rax
	mov rsi, rax
	mov rdi, [rbp - 8]
	call .LfugeAllesHinzu 	// erstelle alle Elemente und füge sie dem Heap hinzu
	pop rdi
	mov [rdi - 4], eax 		// aktualisiere die Anzahl an Baumen im Array
	
	push rdi
	call .LhuffmanBaumErstellen
	pop rdi
	push rax

	xor rsi, rsi
	mov esi, [rdi - 8]
	mov rax, 0xB			// System Call munmap
	syscall					// Gebe Speicher für Heap frei

	pop rdi
	push rdi

	call .LmaxLange
	cmp rax , 9
	jg .Lfehler

	pop rdi
	push rdi
	mov rsi, [rbp - 8]
	call .LtraversierenStart

	pop rdi
	call .LbaumFreigeben

	// Größe des Wörterbuchs als Rückgabe
	pop rax
	sar eax, 1

	add rsp, 8
	pop rbp
ret

// nur den ersten Eintrag aus der Häufigkeitsanalyse kopieren

.LnurEinElement:
	mov BYTE PTR [rdi + 1], 254
	mov BYTE PTR [rdi + 2], 1
	mov rax, rsi
	sar rax, 1
ret

.Lfehler:
	add rsp, 24
	pop rbp
	mov rax, -1
ret

// rsi Größe der Häufigkeitsanalyse

.LerstelleHeap:
	add rsi, 4 			// 8 Bytes mehr für aktuelle Größe des Heaps und
	imul rsi, 0x02		// und maximale Kapazität

	// Speicherallokation für Pointer Array
	xor rdi, rdi
	mov rdx, 3
	mov r10, 0x22
	mov r9, 0x00
	mov rax, 0x09		// mmap
	syscall
	mov [rax], esi 				// wie viele Bytes besetzt wurden
	mov DWORD PTR [rax + 4], 0 	// wie viele Elemente drinnen sind
ret

// 	rdi: char Charakter
// 	rsi: byte Häufigkeit

.LerstelleKnoten:
	push rdi
	push rsi

	// Speicher allokation
	xor rdi, rdi 	// NULL heißt beliebige Adresse wird genommen

	mov rsi, 24 	// 24 Bytes weil: 2 * 8 Bytes für Pointer,
					// 2 * 1 Byte für Charakter und Häufigkeit, 24 wegen Alignment

	mov rdx, 3 		// 0x1 xor 0x2 damit der Bereich modifizierbar
					// und lesbar ist

	mov r10, 0x22 	// MAP_PRIVATE xor MAP MAP_ANONYMOUS
	mov r9, 0x00 	// kein Offset
	mov rax, 0x09	// mmap
	syscall

	pop rsi
	pop rdi

	// Knoten Initialisierung
	mov [rax], di 		// Charakter 
	mov [rax + 2], si	// Häufigkeit
	xor rdi, rdi
	mov [rax + 8], rdi 	// Pointer auf links initialisieren mit null
	mov [rax + 16], rdi	// Pointer auf  rechts initialisieren mit null
ret

// 	fügt die letzten Einträge im char/freq Array
// 	als Erstes in den Heap

// 	rdi Pointer auf char/freq Array
// 	rsi Pointer auf Heap

.LfugeAllesHinzu:
	mov r8, 2

	mov eax, [rsi - 8] 	// um wie viel übersprungen werden soll
	xor rdx, rdx
	sub eax, 16
	div r8 		  		// reduziere dies auf Länge von char/freq
	mov r8d, eax
	xor rax, rax
.LfugeAllesHinzuLoop:
	push rax
	push rdi
	push rsi
	
	mov rax, rdi

	mov di, [rax + r8] 		// Parameter Übergabe für erstellung des Knotens
	mov si, [rax + r8 + 2]	
	call .LerstelleKnoten 	// Pointer auf erstellten Knoten in rax

	pop rsi
	pop rdi
	mov [rsi], rax 	// bewege Knoten in den Heap
	add rsi, 8 		// bewege Index
	sub r8, 4		// nächster Eintrag in dem char/freq array
	pop rax
	inc rax
	cmp r8, -4		
	jne .LfugeAllesHinzuLoop
ret

//	Erstellt den Huffman Baum indem es aus den kleinsten 2 Teilbäumen im
//	Heap, einen neuen Baum erstellt. Dies wird solange gemacht bis am Ende
//	nur noch ein Baum übrig ist. Dies ist der finale Baum.

// 	rdi Pointer auf Heap mit allen Elementen

.LhuffmanBaumErstellen:
	xor r8, r8
	mov r8d, [rdi - 4]	// Größe des arrays
.LhuffmanBaumLoop:
	xor rax, rax

	mov r9, [rdi] 		// erstes Element
	mov r10, [rdi + 8]	// zweites Element

	mov ax, [r9 + 2]	// erste Häufigkeit
	add ax, [r10 + 2]	// addiere zweite Häufigkeit

	push rdi
	push r9
	push r10
	mov di, 1			// Parameter
	mov si, ax			// Parameter
	call .LerstelleKnoten
	pop r10
	pop r9

	mov [rax + 8], r9 		// setze Pointer auf den linken
	mov [rax + 16], r10		// setze Pointer auf den rechten

	pop rdi			// die Pointer auf die zwei Kinder "löschen"
	add rdi, 8
	mov [rdi], rax
	dec r8d			// größe des Heaps verkleinern

	cmp r8d, 1 		// wenn nur noch ein Baum übrig dann fertig
	je .LendeHuffmanBaumErstellen

	push rdi
	push rsi
	push r8
	mov esi, r8d
	call .LrichtigEinsortieren
	pop r8
	pop rsi
	pop rdi
	jmp .LhuffmanBaumLoop

.LendeHuffmanBaumErstellen:
	mov rax, [rdi]
ret

//	Bring den neuen zusammengeführten Teilbaum auf die richtige Stelle

// 	rdi Pointer auf Heap
// 	rsi (esi) mögliche Tauschungen

.LrichtigEinsortieren:
	mov r8, [rdi] 		// hole erstes Element
	mov r9, [rdi + 8]	// hole zweites Element

	mov r8w, [r8 + 2]	// hole Häufigkeit vom ersten Element
	mov r9w, [r9 + 2]	// hole Häufigkeit vom zweiten Element

	cmp r9w, r8w
	jg .LrichtigEinsortierenEnde

	// Tauschen
	mov r8, [rdi]
	xor r8, [rdi + 8]
	xor [rdi + 8], r8
	xor r8, [rdi + 8]
	mov [rdi], r8

	// alles inkrementieren / dekrementieren
	add rdi, 0x08
	dec esi
	cmp esi, 1
	jg .LrichtigEinsortieren
.LrichtigEinsortierenEnde:
ret

//	Als Erstes wird der linke Teilbaum traversiert, dann der Rechte.
//	Im linken Baum beginnt jede Kodierung mit der 0, während im rechten
// 	Teilbaum die Kodierung immer mit der 1 anfängt.
//	Das 8. bit vom Buchstaben entscheidet ob die jeweilige Kodierung
//	Mit einer 1 oder 0 anfängt, das entscheidende Bit ist in CL

// 	rdi Wurzel
// 	rsi char/freq array

.LtraversierenStart:
	xor dl, dl

	not dl 				// Links von der Wurzel starten alle Werte mit der 0
	push rdi			// Wurzel speichern
	xor cl, cl
	shl dl, 1
	mov rdi, [rdi + 8]	// linken Knoten .Ltraversieren
	call .Ltraversieren

	pop rdi				// die Wurzel wieder holen
	xor dl, dl			// Rechts von der Wurzel startet alles mit der 1
	mov cl, 128
	inc dl
	mov rdi, [rdi + 16]	// rechten Knoten .Ltraversieren
	call .Ltraversieren
	mov WORD PTR [rsi], 1	// setze den oberen Byte als Splitter von der Kodierung
ret

//	Traversiert den jeweiligen Teilbaum

// 	rdi Knoten, rsi char/freq array
// 	rdx (dl) Kodierung vom vorherigem Knoten
// 	rcx (cl) Anfangsbit

.Ltraversieren:
	cmp WORD PTR [rdi], 1
	jne .LendeTraversieren

	push rdx
	push rdi
	mov rdi, [rdi + 8]
	shl dl, 1	// linke Kante fügt immer nur eine 0 hinzu
	call .Ltraversieren
	pop rdi
	pop rdx

	mov rdi, [rdi + 16]
	shl dl, 1	// rechte Kante fügt immer
	inc dl		// eine 1 hinzu
	call .Ltraversieren
ret
	.LendeTraversieren:
	xor r8w, r8w
	mov r8b, dl 	// Kodierung
	shl r8w, 8		
	mov r8b, [rdi]	// Zeichen
	or r8b, cl		// oberstes Bit
	mov [rsi], r8w	// In das Wörterbuch kopieren
	add rsi, 2
ret

//	Gibt den Speicher, welcher für den Baum reserviert wurde, frei

//	rdi Wurzel

.LbaumFreigeben:
	cmp WORD PTR [rdi], 1
	jne .LendeFreigeben

	push rdi
	mov rdi, [rdi + 8]
	call .LbaumFreigeben	// Rekursiver Aufruf
	pop rdi
	push rdi

	mov rdi, [rdi + 8]		// Freigabe
	mov rsi, 24
	mov rax, 11
	syscall

	pop rdi
	push rdi
	mov rdi, [rdi + 16]
	call .LbaumFreigeben	// Rekursiver Aufruf
	pop rdi 
	
	mov rdi, [rdi + 16]		// Freigabe
	mov rsi, 24
	mov rax, 11
	syscall

.LendeFreigeben:
ret

//	rdi Wurzel

.LmaxLange:
	xor rax, rax
.LmaxLInner:
	cmp WORD PTR [rdi], 1
	jne .LendeMaxLange

	push rax
	push rdi
	mov rdi, [rdi + 8]
	call .LmaxLInner
	inc rax
	mov r8, rax
	pop rdi

	pop rax
	push r8
	mov rdi, [rdi + 16]
	call .LmaxLInner
	inc rax
	pop r8

	cmp rax, r8
	jl .Lr8Grosser
ret

.Lr8Grosser:
	mov rax, r8
ret

.LendeMaxLange:
	inc rax
ret
