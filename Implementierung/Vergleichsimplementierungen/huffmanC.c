#include <stdio.h>
#include <stdlib.h>

//ist im Prinzip ein minHeap 
struct Tree {
    struct TreeNode** nodes; 

    unsigned int currentSize; 

    unsigned capacity; 
};

typedef struct TreeNode{
    char data; 

    unsigned int frequency; 

    struct TreeNode *left, *right; 
};

struct TreeNode* createNewNode(char data, unsigned int freq){
    struct TreeNode* newNode = (struct TreeNode*)malloc(sizeof(struct TreeNode)); 

    newNode->left = NULL; 
    newNode->right = NULL; 
    newNode->data = data; 
    newNode->frequency = freq; 

    return newNode; 
}

struct TreeNode* deleteMin(struct Tree* tree){
    struct TreeNode* min = tree->nodes[0]; 
    tree->nodes[0] = tree->nodes[tree->currentSize-1]; 

    --tree->currentSize; 
    siftDown(tree,0);
    return min; 
}

void siftDown(struct Tree* tree, int index){
    while(1){
        int smallestIdx = index; 
        int leftIdx = 2*index+1; 
        int rightIdx = 2*index+2; 

        if(leftIdx < tree->currentSize)
            if(tree->nodes[leftIdx]->frequency < tree->nodes[smallestIdx]->frequency)
                smallestIdx = leftIdx; 
        
        if(rightIdx < tree->currentSize)
            if(tree->nodes[rightIdx]->frequency < tree->nodes[smallestIdx]->frequency)
                 smallestIdx = rightIdx;    
        
        if(smallestIdx!=index){
            swap(&tree->nodes[smallestIdx],&tree->nodes[index]);
            //siftDown(tree,smallestIdx);
            index = smallestIdx;
        }else{
            return; 
        }
    }
}

void siftup(struct Tree* tree, int index){
    while(index>0&&tree->nodes[index]->frequency < tree->nodes[(index-1)/2]->frequency){
        //Knoten an der Stelle i kleiner als Vaterknoten 
        //->Vaterknoten und neuen Knoten tauschen 
        swap(&tree->nodes[index],&tree->nodes[(index-1)/2]);
        index = (index-1)/2;
    }
}

void swap(struct TreeNode** node1, struct TreeNode** node2){
    struct TreeNode* temp = *node1;

    *node1 = *node2; 
    *node2 = temp;  
}

void insertNode(struct Tree* tree, struct TreeNode* newNode){
    ++tree->currentSize;
    //new Knoten sollte hier eingefügt werden
    int i = tree->currentSize -1; 
    //neuen Knoten ganz unten rechts im Baum einfügen 
    tree->nodes[i] = newNode; 
    
    siftup(tree,i);
}

int isLeaf(struct TreeNode* root){
    return root->left==NULL && root->right==NULL; 
}

//größe von data sollte gleich capacity sein!
struct Tree* buildMinHeap(char data[], unsigned frequency[], int capacity){
    struct Tree* tree = (struct tree*)malloc(sizeof(struct Tree)); 
    tree->currentSize = 0; 
    tree->capacity = capacity; 
    tree->nodes = (struct Tree**)malloc(capacity*sizeof(struct MinHeapNode*)); 

    for(int i=0; i<capacity; i++){
        //erst alle Buchstaben unsortiert einfügen 
        tree->nodes[i] = createNewNode(data[i],frequency[i]); 
    }
    tree->currentSize = capacity; 

    //nun min Heap Invariante herstellen 
    int i = tree->currentSize-1; 
    for(int p=(i-1)/2; p>=0; p--){
        siftDown(tree,p);
    } 

    return tree; 
}

struct TreeNode* buildHuffmanTree(char data[], unsigned frequency[], int size){
    struct Tree* minHeap = buildMinHeap(data,frequency,size); 

    struct TreeNode* left;
    struct TreeNode* right;
    struct TreeNode* root;

    unsigned int sumOfFreq; 

    //mergen bis es nur noch einen Knoten gibt. 
    while(minHeap->currentSize !=1){
        //zwei Knoten mit den kleinsten Häufigkeiten holen  
        left = deleteMin(minHeap);
        right = deleteMin(minHeap);
        
        sumOfFreq = left->frequency+right->frequency; 
        //diese zwei Knoten sollen gemerged werden 
        //1 wird als dummy für internal nodes verwendet
        root = createNewNode(1,sumOfFreq); 

        root->left = left; 
        root->right = right; 

        insertNode(minHeap,root);
    }
    //wurzel zurückgeben
    return deleteMin(minHeap);
}

//in result wird der Pfad, der genommen wird abgespeichert
void printDictionary(struct TreeNode* root, int result[], int idx){
    if(root->left){
        //linker Teilbaum->0
        result[idx] = 0; 
        printDictionary(root->left,result,idx+1); 
    }

    if(root->right){
        //rechter Teilbaum->1
        result[idx] = 1; 
        printDictionary(root->right,result,idx+1); 
    }

    if(isLeaf(root)){
        printf("%c: ",root->data);
        for(int i=0; i<idx; i++){
            printf("%d",result[i]);
        }

        printf("\n");
    }
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

void huffmanCoding(char data[],unsigned freq[], int size){
    struct TreeNode* root = buildHuffmanTree(data,freq,size); 

    /*char code = 0; 
    int codeSize = getcode(root,'D',&code,0);*/
}

//codeSize ist am Anfang 0 und gibt die eigentliche Größe der Kodierung in Bits zurück
int getcode(struct TreeNode* root, char letter, char* code, int currentCodeSize){
    int codeLength = -1;
    if(root->left){
        //linker Teilbaum->0 
        char temp; 
        if(currentCodeSize>0){
            //arithmetic shift
            signed char temp2 = 128;
            temp = temp2>>(currentCodeSize-1);
        }

        //bit auf 0 setzen
        *code&=temp;
        codeLength = getcode(root->left,letter,code,currentCodeSize+1); 
        if(codeLength!=-1)
            //Buchstabe schon gefunden 
            return codeLength; 
    }

    if(root->right){
        //rechter Teilbaum->1
        unsigned  char newBit = 128>>currentCodeSize;
        *code = *code+newBit; 
        codeLength = getcode(root->right,letter,code,currentCodeSize+1); 
    }

    if(isLeaf(root)){
        if(root->data==letter){
            //Kodierung fertig
            return currentCodeSize;
        }
    }

    return codeLength;
}

//holt die Zeichen aus data, kodiert  sie und schreibt die Kodierung in Result
void writeCodes(struct TreeNode* root, char* data, char* result, unsigned int result_size){
    //zeigt auf das nächste freie Byte 
    int bytePtr = 0; 
    //zeigt auf das nächste freie Bit in dem nächsten freien Byte
    int bitPtr = 0; 

    char letter = data[0];
    
    for(int i=1; letter!='\0'; i++){
        //wichtig: muss unsigned sein, um shl, shr und kein sar, sal zu machen
        unsigned char newCode = 0; 
        int codeSize = getcode(root,letter,&newCode,0); 

        result[bytePtr]+=(newCode>>bitPtr);

        int remainingSizeInByte = 8-bitPtr;
        if(remainingSizeInByte<codeSize){
            //neues Byte
            bytePtr++;
            
            if(bytePtr>=result_size){
                printf("Result-Array war zu klein!\n");
                exit(EXIT_FAILURE);
            }

            //bereits eingefügte bits Auschneiden und rest in den neuen Byte 
            result[bytePtr]+=(newCode<<remainingSizeInByte);
            bitPtr = codeSize-remainingSizeInByte; 
        }else{
            bitPtr+=codeSize;
        }

        letter = data[i]; 
    }
}

int huffman_encode(char* data, char* result, unsigned int result_size){
    //Häufigkeitsanalyse: 
    unsigned tempFreqs[128] = {0}; 
    
    unsigned numberOfLetters = 0;

    char c = data[0];
    for(int i=1; c!='\0'; i++){
        if(tempFreqs[c]==0){
            ++numberOfLetters;
        }
        //häufigkeit erhöhen
        ++tempFreqs[c];

        c = data[i]; 
    }

    char letters[numberOfLetters]; 
    unsigned freqs[numberOfLetters];

    unsigned counter = 0; 
    for(int i=0; i<128; i++){
        if(tempFreqs[i]!=0){
            //der index in tempFreqs entspricht dem char mit dieser gespeicherten häufigkeit
            letters[counter] = i; 
            freqs[counter] = tempFreqs[i]; 
            counter++; 
        }
    }

    struct TreeNode* root = buildHuffmanTree(letters,freqs,numberOfLetters); 
    int res[8] = {0};

    int result2[10] = {0};
    printDictionary(root,result2,0);
    writeCodes(root,data,result,result_size);
}

int main(){
    char data[] = "ABRAKADABRAB";
    
    char result[4] = {0};
    huffman_encode(&data,&result,10);

    for(int i=0; i<4; i++){
        binary(result[i]);
        printf(" ");
    }

    printf("\n"); 
    
}
