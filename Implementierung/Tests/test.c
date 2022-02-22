#include <stdio.h>
#include <stdlib.h>

// Die Funktion void test() führt Tests zu der Funktion huffman_encode durch. Zuerst wird das Beispiel aus der Aufgabenstellung getestet. Dann werden einige Tests ausgeführt, welche gezielt Fehler provozieren, um die Fehlerbehandlung der Funktion huffman_encode zu testen. Weiterhin werden Randfälle überprüft und zuletzt die eigentliche Überprüfung der Huffmankodierung durchgeführt. Die Tests enthalten die korrekten (erwarteten) Ausgaben der Funktion huffman_encode und vergleichen diese mit den errechneten. So wird direkt ausgegeben ob ein Test bestanden ist oder nicht. Ausgabe erfolt über die Konsole. Die Funktion test() kann im Rahmenprogramm mit dem Argument -t aufgerufen werden. 


extern int huffman_encode(char *data, char *result, unsigned int result_size);

void test_huffman_encode(char *data, int data_length, char *result, unsigned int result_size, char *dictionary, char *coding, char *result_expected) {

	printf("Eingabedaten: ");
	
	for(int i = 0; i < data_length; i++) {
		printf("%c", data[i]);
	}
	
	printf("\nEingabegröße: %d", data_length);
	
	printf("\n\nErwartetes Ergebnis:\n");
	printf("Dictionary: ");
	int i = 0;
	while(dictionary[i] != 0) {
		printf("%c", dictionary[i]);
		i++;
	}
	printf("\nCoding: ");
	i = 0;
	while(coding[i] != 0) {
		printf("%c", coding[i]);
		i++;
	}
	
	int retValue = huffman_encode(data, result, result_size);
	printf("\n\nRückgabewert: %d\n", retValue);
	if(retValue == 0) {
		printf("\nKeine Zeichen zu kodieren.\n");
	} else if(retValue == -1) {
		printf("\nFehlerhafter Programmablauf.\n");
	} else {
		int successful = 1;
		for(int i = 0; i < retValue; i++) {
			if(result[i] != result_expected[i]) {
				successful = 0;
				break;
			}
		}
		if(successful == 1) {
			printf("Errechnetes Ergebnis entspricht dem erwartetem Ergebnis.\nTest bestanden!\n");
		} else {
			printf("Errechnetes Ergebnis entspricht nicht dem erwartetem Ergebnis.\nTest nicht bestanden!\n");
		}
	}
}


void test_huffman_encode_fehler(char *data, int data_length, char *result, unsigned int result_size, int print_data) {
	
	if(print_data == 1) {
		printf("Eingabedaten: ");
		for(int i = 0; i < data_length; i++) {
			printf("%c", data[i]);
		}
	} else {
		printf("Eingabedaten aufgrund von Größe nicht ausgegeben.");
	}
	
	printf("\nEingabegröße: %d", data_length);
	
	int retValue = huffman_encode(data, result, result_size);
	printf("\n\nRückgabewert: %d\n", retValue);
	if(retValue == 0) {
		printf("\nKeine Zeichen zu kodieren.\nTest bestanden!\n");
	} else if(retValue == -1) {
		printf("\nFehlerhafter Programmablauf.\nTest bestanden!\n");
	} else {
		printf("Programm nicht wie erwartet abgebochen.\nTest nicht bestanden!\n");
	}
}


void test() {

	printf("\nEs werden eine Reihe von Tests ausgeführt, welche die Methode huffman_encode prüfen. Zu jeder Eingabe wurden die korrekten Ergebnisse aus anderer Quelle berechnet (per Hand oder mit anderen Tools, die den Huffmanalgorithmus ausführen) und werden dem Test jeweils mit übergeben, sodass die Ergebnisse der Funktion mit den Lösungen automatisch verglichen werden können. Die Ausgabe auf der Konsole von Eingaben und Wörterbuch erfolgt nicht in der tatsächlich abgespeicherten Binärform, sondern zur besseren Nachvollziehbarkeit in Form von ASCII-Zeichen. Durch vergleichen der Werte von Eingabegröße und Rückgabewert kann direkt festgestellt werden, ob sich die Kodierung für das jeweilige Beispiel lohnt, da die Eingabegröße der Anzahl an benötigten Bytes für die Speicherung im ASCII-Format und der Rückgabewert der Anzahl an benötigten Bytes für die Speicherung mit der Huffmankodierung entspricht. Beim Rückgabewert wird Dictionary, Trennbyte und Kodierung gezählt, das heißt für größere Eingaben mit gleichen Zeichen würde die Kompression besser werden.\n\n\n");

	printf("Test 1 (Beispiel aus der Aufgabenstellung):\n\n");

	char testdaten1[] = "ABRAKADABRAB";
	int testdaten1_groesse = sizeof(testdaten1);
	char result1[20];
	char dictionary1[] = "A 0 B 10 R 110 K 1110 D 1111";
	char coding1[] = "0101100111001111010110010";
	char result_expected1[] = {65, 254, 194, 2, 210, 6, 203, 14, 196, 15, 1, 89, 207, 89, 127};
	
	test_huffman_encode(testdaten1, testdaten1_groesse, result1, 20, dictionary1, coding1, result_expected1);
	
	printf("\n\n\nEs folgen einige Tests, welche gezielt Fehler provozieren:\n");
	
	
	printf("\n\n\nTest 2:\n\nEs wird ein zu kleines result-Array übergeben, weshalb das Programm abbrechen sollte.\n\n");
	
	char result2[10];

	test_huffman_encode_fehler(testdaten1, 12, result2, 10, 1);
	
	
	
	
	printf("\n\n\nTest 3:\n\nEs kommt ein Zeichen vor, welches nicht von 7-Bit-ASCII kodiert wird, weshalb das Programm abbrechen sollte.\n\n");
	
	char testdaten2[] = "ABRAKADäBRAB";
	test_huffman_encode_fehler(testdaten2, 12, result1, 24, 1);
	
	
	
	
	printf("\n\n\nTest 4:\n\nEs kommt ein Zeichen mehr als 65535 mal vor, weshalb das Programm abbrechen sollte.\n\n");
	
	char* testdaten3 = malloc(65600);
	if(testdaten3 == NULL) {
		printf("Fehler bei der Speicherallokation!");
		abort();
	}
	for(int i = 0; i < 65600; i++) {
		testdaten3[i] = 65;
	}
	
	test_huffman_encode_fehler(testdaten3, 65600, result1, 24, 0);
	
	
	
	
	printf("\n\n\nTest 5:\n\nDas Eingabewort hat die Länge 0, weshalb das Programm abbrechen sollte.\n\n");
	
	char testdaten4[] = "";
	
	test_huffman_encode_fehler(testdaten4, 0, result1, 24, 1);
	
	

	
	printf("\n\n\nEs folgen Tests zur weiteren Überprüfung von Randfällen:\n");
	
	printf("\n\n\nTest 6:\n\n");

	char testdaten7[] = "A";
	char result7[4];
	char dictionary7[] = "A 0";
	char coding7[] = "0";
	char result_expected7[] = {65, 254, 1, 127};
	
	test_huffman_encode(testdaten7, 1, result7, 4, dictionary7, coding7, result_expected7);
	
	printf("\n\n\nTest 7:\n\n");

	char testdaten8[] = "AAAAAAAA";
	char result8[8];
	char dictionary8[] = "A 0";
	char coding8[] = "00000000";
	char result_expected8[] = {65, 254, 1, 0};
	
	test_huffman_encode(testdaten8, 8, result8, 4, dictionary8, coding8, result_expected8);
	
	
	
	printf("\n\n\nTest 8:\n\n");

	char testdaten9[] = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
	char result9[9];
	char dictionary9[] = "B 0";
	char coding9[] = "000000000000000000000000000000000000000000000";
	char result_expected9[] = {66, 254, 1, 0, 0, 0, 0, 0, 7};
	
	test_huffman_encode(testdaten9, 45, result9, 9, dictionary9, coding9, result_expected9);
	
	
	
	printf("\n\n\nEs folgen Tests zur weiteren Überprüfung der Huffmancodierung:\n");
	
	
	
	printf("\n\n\nTest 9:\n\n");

	char testdaten10[] = "Rechnerarchitektur";
	char result10[72];
	char dictionary10[] = "c 000 t 001 h 010 a 0110 R 0111 n 1000 k 1001 u 1010 i 1011 r 110 e 111";
	char coding10[] = "011111100001010001111100110110000010101100111110010011010110";
	char result_expected10[] = {99,248,116,249,104,250,97,246,82,247,238,8,235,9,245,10,233,11,242,6,229,7,1,126,20,124,216,43,62,77,111};
	
	test_huffman_encode(testdaten10, 18, result10, 72, dictionary10, coding10, result_expected10);
	
	
	
	
	
	printf("\n\n\nTest 10:\n\n");
	
	char testdaten6[] = "Die Datenkompression ist ein Teilgebiet der Kodierungstheorie und beschaeftigt sich damit, wie man den Speicherbedarf von Datenmengen auf ein Minimum reduzieren kann.";
	int data_length6 = sizeof(testdaten6);
	
	char* result6 = malloc(data_length6*2);
	if(result6 == NULL) {
		printf("Fehler bei der Speicherallokation!");
		abort();
	}
	
	char result_expected6[] = {97,240,114,241,116,242,103,230,107,206,112,207,105,250,44,176,119,177,108,178,84,179,75,180,77,181,46,182,122,183,111,238,117,239,160,4,246,80,211,81,
196,41,243,21,237,22,227,46,230,47,229,6,228,28,226,58,232,59,238,15,1,165,105,72,22,243,157,99,199,86,169,223,21,74,101,241,158,76,141,
186,88,167,48,198,142,226,194,255,53,75,188,225,90,63,249,58,213,187,176,215,146,49,74,171,187,156,11,34,97,24,173,44,31,57,190,81,
63,43,187,195,214,224,13,242,131,190,82,5,190,219,205,190,3,239,153,124,106,189,89,246,131,184,246,235,14,248,112,127,182};

	char dictionary6[] = "a 0000 r 0001 t 0010 g 00110 k 001110 p 001111 i 010 , 0110000 w 0110001 l 0110010 T 0110011 K 0110100 M 0110101 . 0110110 z 0110111 o 01110 u 01111   100 v 1010000 S 1010001 D 101001 s 10101 m 10110 c 101110 f 101111 e 110 d 11100 b 111010 h 111011 n 1111\n\n";
	
	char coding6[] = "10100101 01101001 01001000 00010110 11110011 10011101 01100011 11000111 01010110 10101001 11011111 00010101 01001010 01100101 11110001 10011110 01001100 10001101 10111010 01011000 10100111 00110000 11000110 10001110 11100010 11000010 11111111 00110101 01001011 10111100 11100001 01011010 00111111 11111001 00111010 11010101 10111011 10110000 11010111 10010010 00110001 01001010 10101011 10111011 10011100 00001011 00100010 01100001 00011000 10101101 00101100 00011111 00111001 10111110 01010001 00111111 00101011 10111011 11000011 11010110 11100000 00001101 11110010 10000011 10111110 01010010 00000101 10111110 11011011 11001101 10111110 00000011 11101111 10011001 01111100 01101010 10111101 01011001 11110110 10000011 10111000 11110110 11101011 00001110 11111000 01110000 01111111 10110110\n\n";
	
	test_huffman_encode(testdaten6, data_length6, result6, data_length6*2, dictionary6, coding6, result_expected6);


}
