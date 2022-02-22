# `1 Einleitung`

## Datenkompression
- Vorstellung der Huffmankodierung
- Huffmankodierung ist ein verlustfreies Datenkompressionsverfahren
- Datenkompression ist ein Teilgebiet der Kodierungstheorie und beschäftigt sich damit, wie man den Speicherbedarf von Datenmengen auf ein Minimum reduzieren kann
- verlustfreier Datenkompression heißt die verdichteten Daten enthalten den ursprünglichen Inhalt noch vollständig und es kann durch Dekodierung der originale Code wiederhergestellt werden
- Beispiel: Text- oder Programmdateien
- Gegensatz: verlustbehaftete Kompression: der Inhalt bleibt zwar in den Grundzügen erkennbar, es können aber Informationen verloren gehen, um den Speicherbedarf noch weiter zu optimieren
- Offtrade: Informationsgehalt vs. Speicherbedarf
- Beispiel: Bild- und Audiodateien
- Huffmankodierung generiert präfix-freier Code -> kein Codewort Anfang eines anderen Codeworts ist -> keine Trennzeichen bei der Speicherung des neu kodierten Worts nötig
- Grundprinzip: häufig auftretende Symbole werden durch kurze Codewörter und selten vorkommende Symbole durch lange Codewörter beschreiben
- Algorithmus: 
	1) Häufigkeitsanalyse der Zeichen des Eingabeworts -> erstellt eine sortierte Häufigkeitstabelle
	2) Generierung eines Codebaums zur Erstellung von Codewörtern mit optimalen Längen
	3) Übersetzung des Eingabeworts in die neue Kodierung
- Besonderheit: Die mittlere Codewortlänge kann nicht weiter reduziert werden, d.h. der Code ist optimal
	
	
	
# `2 Lösungsansatz`

- genauen Funktionsweise und Umsetzung des Huffmanalgorithmus; unterteilt sich in die drei zuvor beschriebenen Schritte
- Funktionssignatur: int huffman_encode(char *data, char *result, unsigned int result_size)
	- data-Array: zu kodierenden Daten gespeichert; Ende durch ein Nullbyte markiert
	- result-Array: dictionary + neu kodierten Daten geschrieben
	- result_size: maximale Größe des result-Arrays
	- Rückgabe: -1 falls Fehler, sonst Anzahl der benötigten Bytes für dictionary, Trennbyte, Kodierung
- nur Zeichen aus der klassischen 7-Bit-ASCII-Variante kodierbar	
	
	
## 2.1 Häufigkeitsanalyse

### 2.1.1 Erstellen der unsortierten Häufigkeitstabelle

- beinhaltet alle Zeichen des Eingabeworts mit zugehöriger Häufigkeit
- result-Array wird zum Abspeichern verwendet, da nur Zwischenergebnis
- abwechselnd mit jeweils zwei Bytes für ein Zeichen und dann zwei weiteren Bytes für dessen Häufigkeit abgespeichert
- im kleinsten Fall (Eingabewort verwendet nur ein Zeichen) können 2¹⁶- 1 = 65535 Zeichen verarbeitet werden
- im größten Fall (Eingabewort verwendet alle 126 Zeichen mit maximaler Häufigkeit) können 126 x 2¹⁶ - 1 = 8.257.410 analysiert werden
- kommt mindetstens ein Zeichen zu häufig vor, wird abgebrochen und -1 zurückgegeben
	
- Vorstellung des Algorithmus am Beispiel AADZDAADDZAAAADAZZD:
	
- äußere Schleife: iteriert über die zu verarbeitenden Daten -> im ersten Schritt wird das Zeichen 0 aus dem data-Array geladen
- innere Schleife: zählt für das aktuelle Zeichen 'A' wie häufig es vorkommt
	- min. 16 zu vergleichende Zeichen -> SIMD
	- Maske erstellen mit 16 mal (in jedem Byte) dem zu vergleichenden Zeichen chende Zeichen enthält
	- Vergleich generiert neue Makse mit -1 in dem Bytes mit Übereinstimmung und 0 sonst
		1) mithilfe der Maske werden die Zeichen, die gezählt wurden im data-Array auf -1 gesetzt -> zwar muss das data-Array gesichert werden, dafür aber deutlich weniger Speicherzugriffe, da kein Abprüfen, ob Zeichen bereits bearbeitet wurde, nötig
		2) mit der Maske Zählen wie häufig das Zeichen in den verglichenen Daten vorkommt: Bytes, die -1 enthalten werden auf 1 gesetzt, sonst 0; alle Bytes werden aufaddiert
	- verbleibende Vergleiche mittels SISD
- nächste Iteration der äußeren Schleife: Zeichens 1 wäre ein ‚A‘ -> wird sofort als gezählt erkannt -> mit Zeichen 2: 'D' fortsetzen

### 2.1.2 Erstellen der sortierten Häufigkeitstabelle
- die unsortierte Häufigkeitstabelle wird mithilfe des Quicksort-Algorithmus sortiert
- Ergebnis für das Beispiel: A 9, D 6, Z 4
	
### 2.1.3 Fehlerbehandlung

- ein Zeichen kommt zu häufig vor
- ein Zeichen wird nicht von der 7-Bit-ASCII-Variante kodiert
- result-Array zu klein	
	

## 2.2 Erstellung des Wörterbuchs

- Erfolgt in 3 Schritten:
	1. Es wird ein Haufen erstellt und alle Elemente hinzufügt
	2. Anhand des Haufens wird der Huffman-Baum erstellt
	3. Erstellung des Wörterbuchs

### 2.2.1 Erstellung des Haufens
- Bei diesem Schritt ist der System Call 9 - mmap sehr wichtig.
- Es wird Speicher für Pointer auf alle Knoten alloziert (mit mmap) also n\*8 Bytes insgesamt
- Dann wird für jeden Knoten 24 Bytes Speicher alloziert
- Ein Knoten besteht aus:
- 8 Bytes fürs Zeichen und Häufigkeit, jeweils 2 Bytes, die restlichen 4 Bytes bleiben nicht besetzt
- 8 Bytes für den Pointer auf den linken Knoten
- 8 Bytes für den Pointer auf der rechten Knoten
- Der Pointer auf jeden von den Knoten wird in den Haufen eingefügt
- Diese sind aufsteigend nach der Häufigkeit sortiert

### 2.2.2 Erstellung des Huffman-Baums
- Als Erstes werden die 2 Knoten, welche die niedrigste Häufigkeit haben "zusammengeführt" (Das sind immer die 2 Knoten ganz Links im Haufen)
- Das heißt die Häufigkeiten dieser 2 Teilbäume zusammenaddiert werden und ein neuer Knoten mit der Summe und einem Dummy Zeichen erstellt wird
- Das Dummy Zeichen ist eine 1 (eine 1 kann in der ASCII Kodierung nicht dargestellt werden)
- Die Zeiger auf den linken und rechten Knoten werden mit den 2 Teilbäumen besetzt, welche "zusammengeführt wurden"
- Der neue Knoten kommt dann an in den Heap, die 2 Kinder werden aus dem Heap gelöscht
- Der Teilbaum wird so eingefügt, dass alle Knoten nach der Häufigkeit aufsteigend sortiert sind

### 2.2.3 Finaler Baum
- Dies wird so lange gemacht, bis nur noch ein Baum im Heap übrig ist - das ist der Huffman-Baum

### 2.2.4 Traversierung des Baumes
- Das Codewort für den Buchstaben wird im Register **rdx** gespeichert
- Dieser wird für den linken Teilbaum mit lauter 1 gefüllt und dann beim ersten Schritt, sowie bei jeder Traversierung zum linken Kind, um 1 nach Links geshiftet
- Der Grund hierfür ist, dass alle Codewörter im linken Teilbaum mit 0 anfangen
- Das 8. Bit beim Buchstaben indiziert dann, ob das Codewort mit 0 oder 1 anfängt
- Alles vor dem ersten Bit des Buchstaben wird mit dem entgegengesetzten Bit gefüllt
- Dieses 8. Bit wird im register **cl** gespeichert
- Falls beim rekursiven traversieren ein Knoten erreicht wird, welches nicht das Dummy Zeichen besitzt, wird das Codeword sowie der Buchstaben verodert mit CL in das result-Array reingeschrieben
- Beim rechten Kind der Wurzel wird **rdx** geleert und dann beim ersten Schritt, sowie bei jeder Traversierung zum rechten Kind, um 1 nach Links geshitet und 1 dazu addiert
- Wenn der ganze Baum traversiert wurde, ist das Wörterbuch fertig		
	
		
## 2.3 Übersetzung
- wird in 6 Hauptschritte zerlegt: 
	- diese werden anhand des Beispiels erklärt
	
### 2.3.1 Das nächste Zeichen im Eingabewort wird gelesen
- Das Z aus der Beispieleingabe wird gelesen 
	
### 2.3.2 Das Codewort dazu wird aus dem Wörterbuch rausgesucht
- Wörterbuch wird in zwei-Byte Schritte durchlaufen
- oberstes Bit des Eintrags gibt an, ob Kodierung mit 0 oder 1 anfängt  
- Quellsymbol wird modifiziert, weil vergleich mit 7 Bits aufwändiger ist. 
- -> oberstes Bit wird gleich gesetzt und es werden 8 Bits verglichen. 
- in diesem Fall ändert es nichts, da die Kodierung mit 0 anfängt 
- modifiziertes Quellsymbol und obere Byte des Eintrags sind nicht gleich
- ->zum nächsten Eintrag springen
- `(nächste Folie)`
- modifiziertes Quellsymbol = obere Byte des Eintrags -> richtigen Eintrag gefunden 
	
### 2.3.3/4 Codewort anpassen und in den Puffer einfügen 
- Die Schritte 3,4 in der Ausarbeitung werden hier zusammengefasst dargestellt:
- Codewort muss auf 64 Bit erweitert werden 
- die Bits vor der Kodierung müssen genullt werden 
- man kann dann das modifizierte Codewort dann mit dem Puffer addieren 
- davor: die eigentliche Kodierung an die richtige Stelle shiften. 
- Kodierung muss ab dem ersten freien Bit anfangen 
- -> Links-shift
- danach: Puffer mit Codewort addieren 
- ->neue Kodierung hinzugefügt 
- erstes freie Bit wird aktualisiert
- `(nächste Folie)`
	- die Schritte 1-4 werden für alle Quellsymbole wiederholt 
	
### 2.3.5 Puffer wird in den Speicher rausgeschreiben
- a) kann mitten in der Schleife passieren: 
	- falls er schon voll ist
		- Teil der letzten Kodierung, die nicht mehr reingepasst hat, kommt in das neue Puffer
- b)kann nach der Schleife passieren: 
	- falls er, nachdem die Kodierungen abgeschloßen sind, noch etwas enthält. 
		- nur die tatsächlich verwendeten Bytes werden kopiert			
	- Kodierungen kommen im result-Array nach dem Splitter 
	
### 2.3.6 Kodierungsende markieren und Rückgabewert bestimmen
- gehen nun davon aus, dass alle Quellsymbole kodiert wurden.
- im letzten Byte: 3 Bits gehören nicht mehr zu der Kodierung. 
- die Dekodierung muss Bescheid wissen: 
	- -> ende der Gesamten Kodierung markieren
		- a) zweite Bit des Splitters 1 -> kodierung endet mit 1 
		- b) zweite Bit des Splitters 0 -> Kodierung dendet mit 0 
			- letzte Bits auf 1 setzten
	- -> Ende eindeutig erkennbar
	- Rückgabewert: Länge der gesamten Kodierung in Bytes
	- Falls result-Array zu klein: Rückgabewert = -1
	


# `3 Testen`
- automatisierte Tests mit beispielhaften Eingaben
- Testfälle:	
	- Beispiel aus der Aufgabenstellung: „ABRAKADABRAB“
	- Überprüfung von Randfällen und der Fehlerbehandlung
		- Eingabewort der Länge 0, Eingabewort der Länge 1, ein Zeichen mehrmals wiederholt
		– mindestens ein Zeichen im Eingabewort ist nicht in der 7-Bit-ASCII-Variante enthalten
		– ein Zeichen kommt im Eingabewort mehr als 65535 mal vor
	- Eingaben in natürlicher Sprache zur Überprüfung des Wörterbuchs und der Kodierung
- für die Testfälle wurden die Lösungen zu den gewählten Eingabedaten von Hand oder mittels bereits existierenden Tools zur Huffmankodierung berechnet; die Lösungen werden automatisch mit den errechneten Ergebnissen verglichen
- Länge des Eingabeworts = Anzahl der benötigten Bytes für die Speicherung des Eingabeworts mittels ASCII-Kodierung; Rückgabewert = Anzahl der benötigten Bytes für die Speicherung mittels Huffmankodierung
- keine genauere Betrachtung der Kompressionsraten, da diese durch den Huffmanalgorithmus fixiert sind und trotz Wahlmöglichkeiten im Alogrithmus nicht verbessert werden können
	
	
	
# `4 Performanzanalyse`

- Implementierung mit SIMD Operationen läuft bei längeren Eingaben besser als SISD
- Vergleichsimplementierung läuft deutlich schlechter
- Dafür ist Teilweise der Compiler verantwortlich - kann SIMD Optimierungen nicht umsetzten
- Die meiste Zeit wird bei der Kodierung verbracht
- Es wird ohne ein Wörterbuch kodiert, heißt Baum wird bei jedem Buchstaben traversiert - was sehr viel Zeit kostet
- Compiler bemerkte das auch und optimiert die Funktion stark, es entsteht ein Assemblercode, welcher mehr als 5000 Zeilen hat	
	
	
	
# `5 Zusammenfassung und zum Ausblick`

## 5.1 Zusammenfassung

- Programm führt die Kodierung von Daten in Form der 7-Bit-ASCII-Darstellung mit dem Huffmanalgorithmus durch; komplett in Assembler implementiert
- zusätzlich im Rahmenprogramm: automatisierte Tests und Zeitmessungen für jeweils vordefinierte Eingaben
- Huffmankodierung lohnt sich nicht immer: kleine Eingaben mit vielen verschiedenen Zeichen lohnen sich nicht; große Eingaben mit wenig verschiedenen Zeichen erzielen gute Kompressionsraten
- wichtig aus Performanzanalyse: SIMD lohnt sich erst mit großen Eingaben; die Implementierung in Assembler kann tatsächlich einen beachtlichen Performanzvorteil gegenüber der Implementierung in einer Hochsprache wie C generieren

## 5.2 Ausblick
- Erweiterung der Kodierungsfunktion sinnvoll; in zahlreiche Richtungen denkbar
	- Funktion, die die Eingaben zuerst analysiert 
		-> Größe des result-Arrays könnte besser vorhergesagt werden -> kein nicht verwendeter Speicher wird alloziert
		-> Entscheiden ob die Huffmankodierung sinnvoll ist; sonst einfach in ASCII lassen
	- Weitere Kodierungsverfahren, die gegensätzlich für Huffmankodierung gut funktionieren,
		Bsp.: für "ABCDEFGHIJKLMMMMMMMMMMMM..." wäre Lauflängenkodierung besser
	- Sortierung der Zeichen mit gleichen Häufigkeiten -> Vergleichbarkeit zu anderen Implementierungen-> leichteres Testen	
