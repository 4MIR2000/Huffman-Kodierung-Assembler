#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void bench() {
	int return_value =
	    system("gcc -w -O3 -o benchmark ./Benchmark/benchmark.c "
		   "./Rahmenprogramm/huffman_encode.S "
		   "./Häufigkeitsanalyse/frequency_analysis.S "
		   "./Häufigkeitsanalyse/quicksort_result.S "
		   "./Wörterbuch/dictionary.S ./Kodierung/coding.S");
	sleep(2);
	printf("\n\n Assembler mit SIMD:\n\n");
	return_value = system("./benchmark ./Benchmark/Mit_SIMD.txt");
	sleep(2);
	return_value = system(
	    "gcc -w -O3 -o benchmark ./Benchmark/benchmark.c "
	    "./Rahmenprogramm/huffman_encode.S "
	    "./Vergleichsimplementierungen/frequency_analysis_ohne_SIMD.S "
	    "./Häufigkeitsanalyse/quicksort_result.S "
	    "./Wörterbuch/dictionary.S ./Kodierung/coding.S");
	sleep(2);
	printf("\n\n Assembler ohne SIMD:\n\n");

	return_value = system("./benchmark ./Benchmark/Ohne_SIMD.txt");
	sleep(2);
	return_value =
	    system("gcc -w -O3 -o benchmark ./Benchmark/benchmarkC.c");
	sleep(2);
	printf("\n\n Vergleichsimplemetierung in C:\n\n");

	return_value = system("./benchmark ./Benchmark/CImpl.txt");
	sleep(2);
	return_value = system("rm benchmark");

	if (return_value < 0) {
		printf("Es kam beim Benchmark zu einem Fehler.\n");
	}
}
