#include <stdlib.h>
#include <stdio.h> 

extern int coding(char* data, char* result, unsigned int result_size);

void test(char data[], unsigned int dataSize, char result[], unsigned int resultSize, char expectedCode[], unsigned int expectedCodeSize, unsigned int codeStartPos);
void binary(unsigned char n);

int main(){
    const int dataSize = 12; 
    char data[] = {"ABRAKADABRAB"}; 

    const int expectedCodeSize = 4; 
    unsigned char expectedCode[] = {89,207,89,0};

     
    const int resultSize = dataSize*3+1; //im schlimmsten Fall: alle Buchstaben in data unterschiedlich und alle werden mit 1 Byte kodiert. +1 wegen splitter
    char result[resultSize];   
       
    result[0] = 'A';
	result[1] = 0b11111110;

	result[2] = 'B'|128; 
	result[3] = 0b00000010; 

	result[4] = 'R'|128; 
	result[5] = 0b00000110; 

	result[6] = 'K'|128; 
	result[7] = 0b00001110; 

	result[8] = 'D'|128; 
	result[9] = 0b00001111; 
	
	result[10] = 1;         //splitter 

    const int codeStartPos = 11; 
    test(data,dataSize,result,resultSize,expectedCode,expectedCodeSize,codeStartPos); 
}

void test(char data[], unsigned int dataSize, char result[], unsigned int resultSize, char expectedCode[], unsigned int expectedCodeSize, unsigned int codeStartPos){
    int codeSize = coding(data,result,22);

    if(codeSize!=4){
        printf("code size expected: %d, but was: %d",expectedCodeSize, codeSize);
        exit(EXIT_FAILURE);
    }

    //check if code is write
    for(int i=0; i<4; i++){
        if(result[codeStartPos+i]!=expectedCode[i]){
            printf("expected Byte at position %d: ",i);
            binary(expectedCode[i]); 
            printf(" ,but was: "); 
            binary(result[11+i]); 
            printf("\n"); 
            exit(EXIT_FAILURE);
        }
    }

    printf("code: "); 
    for(int i=codeStartPos; i<codeStartPos+codeSize;i++){
        binary(result[i]); 
    }
    printf("\n\n"); 

    printf("everthing correct.\n\n");
}

void binary(unsigned char n) {
	unsigned i;
	for (i = 128; i > 0; i = i / 2) {
		if (n & i)
			printf("1");
		else
			printf("0");
	}
}