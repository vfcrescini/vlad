#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BLOCKSIZE 		1

#define OK			 0
#define ERROR_INVALIDFILENAME	-1
#define ERROR_CANTOPENFILE	-2
#define ERROR_INVALIDARGUMENTS	-3

unsigned long encryptBlock(unsigned long, unsigned long);
unsigned long decryptBlock(unsigned long, unsigned long);
int encryptFile(char *, char *, unsigned long);
int decryptFile(char *, char *, unsigned long);
int transformFile(char *, char *, unsigned long, unsigned long(unsigned long, unsigned long));

int main(int argc, char *argv[]) {
	
	if (argc != 4) {
		printf("%s -de infile outfile\n", argv[0]);
		return ERROR_INVALIDARGUMENTS;
	}
	
	if (!strcasecmp("-e", argv[1]))
		return encryptFile(argv[2], argv[3], 1000);
	
	if (!strcasecmp("-d", argv[1]))
		return decryptFile(argv[2], argv[3], 1000);
	
	return ERROR_INVALIDARGUMENTS;
}

unsigned long encryptBlock(unsigned long block, unsigned long key) {
	return block + key;
}

unsigned long decryptBlock(unsigned long block, unsigned long key) {
	return block - key;
}

int encryptFile(char *inFileName, char *outFileName, unsigned long key) {
	return transformFile(inFileName, outFileName, key, encryptBlock);
}

int decryptFile(char *inFileName, char *outFileName, unsigned long key) {
	return transformFile(inFileName, outFileName, key, decryptBlock);
}

int transformFile(char *inFileName, char *outFileName, unsigned long key, unsigned long transformBlock(unsigned long, unsigned long)) {
	FILE *inFile;
	FILE *outFile;
	unsigned long int pBlock;
	unsigned long int eBlock;
	
	if (!inFileName || !outFileName)
		return ERROR_INVALIDFILENAME;

	if (	!(inFile = fopen(inFileName, "r")) ||
		!(outFile = fopen(outFileName, "w")))
		return ERROR_CANTOPENFILE;

	while (fread(&pBlock, BLOCKSIZE, 1, inFile)) {
		eBlock = transformBlock(pBlock, key);
		fwrite(&eBlock, BLOCKSIZE, 1, outFile);
	}

	return fclose(inFile) || fclose(outFile);	
}

