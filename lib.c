#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include "lib.h"
#include <time.h>
#define MAX_LENGTH 256 // Maximum length of each line
#define MAX_RESULTS 100

label labelMap[50];
label dataLabelMap[50];

rType rMap[] = {
		{ "add", "100000" },
		{ "sub", "100010" },
		{ "and", "100100" },
		{ "or",  "100101" },
		{ "sll", "000000" },
		{ "slt", "101010" },
		{ "srl", "000010" },
		{ "jr",  "001000" },
		{ NULL, 0 }
};

iType iMap[] = {
		{ "lw",   "100011" },
		{ "sw",   "101011" },
		{ "andi", "001100" },
		{ "ori",  "001101" },
		{ "lui",  "001111" },
		{ "beq",  "000100" },
		{ "bne",  "000101" },
		{ "slti", "001010" },
		{ "addi", "001000" },
		{ NULL, 0 }
};

jType jMap[] = {
		{ "j", "000010" },
		{ "jal", "000011" },
		{ NULL, 0 }
};

reg registerMap[] = {
        { "$zero", "00000" },
        { "$at", "00001" },
		{ "$v0", "00010" },
		{ "$v1", "00011" },
		{ "$a0", "00100" },
		{ "$a1", "00101" },
		{ "$a2", "00110" },
		{ "$a3", "00111" },
		{ "$t0", "01000" },
		{ "$t1", "01001" },
		{ "$t2", "01010" },
		{ "$t3", "01011" },
		{ "$t4", "01100" },
		{ "$t5", "01101" },
		{ "$t6", "01110" },
		{ "$t7", "01111" },
		{ "$s0", "10000" },
		{ "$s1", "10001" },
		{ "$s2", "10010" },
		{ "$s3", "10011" },
		{ "$s4", "10100" },
		{ "$s5", "10101" },
		{ "$s6", "10110" },
		{ "$s7", "10111" },
		{ "$t8", "11000" },
		{ "$t9", "11001" },
		{ "$ra", "11111" },
		{ NULL, 0 }
};





insCode Result[MAX_RESULTS];

int i = 0; // Global index variable to keep track of the array index
void insCodeF(char *instruction, int code, int *hasErr) {
    if (i < MAX_RESULTS) { // Check if the array limit is not exceeded
        Result[i].ins = malloc(strlen(instruction) + 1); // Allocate memory for the string
        if (Result[i].ins != NULL) {
            strcpy(Result[i].ins, instruction); // Copy instruction to the structure
            Result[i].machineCode = code;
            i += 1;
        } else {
            printf("Memory allocation failed!\n");
            return;
        }
    } else {
        printf("Error: The number of instruction limit was exceeded!\n");
        *hasErr = 1;
        return;
    }
}


void OutputP(FILE *outFile) {
    time_t rawtime;
    struct tm *timeinfo;
    char time_string[80];

    // Open file in write mode ("w")
    outFile = fopen("Assembly File/Output/output.txt", "w");

    if (outFile == NULL) {
        printf("File could not be opened.");
        return;
    }

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    fprintf(outFile, "---------------------------------------------\n");
    strftime(time_string, sizeof(time_string), "          %Y/%m/%d  -  %H:%M:%S", timeinfo);
    fprintf(outFile, "%s\n", time_string);
    fprintf(outFile, "---------------------------------------------\n");

    for (int j = 0; j < i; ++j) {
        fprintf(outFile,"%-d - %-30s 0x%08X\n", j + 1, Result[j].ins, Result[j].machineCode);
    }

    fprintf(outFile, "---------------------------------------------\n");
    // Close the file
    fclose(outFile);
}



/**     Functions       **/
unsigned int binaryToDecimal(const char *binaryStr) {
    return (unsigned int)strtoul(binaryStr, NULL, 2);
}

int isLineEmpty(const char *line) {
    // Check if the line is empty (contains only spaces)
    while (*line != '\0') {
        if (!isspace((unsigned char)*line))
            return 0; // Line is not empty
        line++;
    }
    return 1; // Line is empty
}

char instruction_type(char *instruction) {
    char *tmp = strdup(instruction);
    char *Mnemonic = strtok(tmp, " \t");

	if (!strcmp(Mnemonic, "add")
     || !strcmp(Mnemonic, "sub")
     || !strcmp(Mnemonic, "and")
     || !strcmp(Mnemonic, "or")
     || !strcmp(Mnemonic, "sll")
     || !strcmp(Mnemonic, "slt")
     || !strcmp(Mnemonic, "srl")
     || !strcmp(Mnemonic, "jr"))
		return 'r';

	else if (!strcmp(Mnemonic, "lw")
          || !strcmp(Mnemonic, "sw")
          || !strcmp(Mnemonic, "andi")
          || !strcmp(Mnemonic, "ori")
          || !strcmp(Mnemonic, "beq")
          || !strcmp(Mnemonic, "bne")
          || !strcmp(Mnemonic, "slti")
          || !strcmp(Mnemonic, "addi")
          || !strcmp(Mnemonic, "la")
          || !strcmp(Mnemonic, "lui"))
        return 'i';


	else if (!strcmp(Mnemonic, "j")
          || !strcmp(Mnemonic, "jal"))
		return 'j';

	return 0;
}

char *RegSearch(char *registerName) {
	for (int i = 0; registerMap[i].name != NULL; i++) {
		if (!strcmp(registerName, registerMap[i].name))
			return registerMap[i].address;
	}
	return NULL;
}

char *SearchR(char *name){
    for (int i = 0; rMap[i].name != NULL; i++){
		if (!strcmp(name, rMap[i].name))
			return rMap[i].function;
	}
}

char *SearchI(char *name){
    for (int i = 0; iMap[i].name != NULL; i++){
		if (!strcmp(name, iMap[i].name))
			return iMap[i].address;
	}
}

char *SearchJ(char *name){
    for (int i = 0; jMap[i].name != NULL; i++){
		if (!strcmp(name, jMap[i].name))
			return jMap[i].address;
	}
}

int isStringNumeric(const char *str) {
    char *endptr;
    strtol(str, &endptr, 10);
    return !(errno == ERANGE) && (*endptr == '\0');
}

char* decimalToBinary(int number, int bits) {
    char* binaryString = (char*)malloc((bits + 1) * sizeof(char));
    if(binaryString == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int index = 0;

    // Convert number to binary
    while (bits > 0) {
        binaryString[index++] = (char)((number & 1) + '0');
        number >>= 1; // Right shift the number by 1 bit
        bits--;
    }

    // Fill remaining bits with '0'
    while (index < bits) {
        binaryString[index++] = '0';
    }

    binaryString[index] = '\0';

    // Reverse the string
    int start = 0;
    int end = strlen(binaryString) - 1;
    while (start < end) {
        char temp = binaryString[start];
        binaryString[start] = binaryString[end];
        binaryString[end] = temp;
        start++;
        end--;
    }

    return binaryString;
}

int containsNumber(const char* str) {
    int length = strlen(str);
    for (int i = 0; i < length; ++i) {
        if (isdigit(str[i])) {
            return 1; // Return 1 if the string contains a number
        }
    }
    return 0; // Return 0 if the string does not contain a number
}

int checkString(const char *str) {
    const char *colon = strchr(str, ':');
    if (colon != NULL) {
        int index = colon - str + 1;
        while (isspace(str[index]) && str[index] != '\0') {
            index++;
        }
        if (str[index] == '\0') {
            return 1;
        }
    }
    return 0;
}

void removeLeadingSpaces(char *str) {
    if(str == NULL)
        return;

    int i, j = 0;
    int len = strlen(str);

    while (isspace(str[j])) {
        j++;
    }

    for (i = 0; i < len - j; i++) {
        str[i] = str[i + j];
    }
    str[i] = '\0';
}

bool isLabel(const char *str) {
    // Check if the string is empty or starts with a digit
    if (strlen(str) == 0 || isdigit(str[0]))
        return false;

    // Check each character in the string
    for (int i = 0; str[i] != '\0'; i++) {
        if (!(isalnum(str[i]) || str[i] == '_')) {
            return false; // Invalid character found
        }
    }

    return true; // Valid variable name
}

void removeTrailingSpaces(char* str) {
    if(str == NULL)
        return;

    int len = strlen(str);
    int i = len - 1;


    // Remove trailing spaces
    while (i >= 0 && isspace(str[i])) {
        str[i] = '\0';
        i--;
    }
}

int SearchLabel(const char* label) {
    for (int i = 0; i < 50; i++) {
        if (labelMap[i].name != NULL && !strcmp(label, labelMap[i].name)) {
            return labelMap[i].address;
        }
    }
    return -1; // Indicate error: Label not found
}

int SearchDataLabel(const char* label) {
    for (int i = 0; i < 50; i++) {
        if (dataLabelMap[i].name != NULL && !strcmp(label, dataLabelMap[i].name)) {
            return dataLabelMap[i].address;
        }
    }
    return -1; // Indicate error: Label not found
}


int indexDL = 0;
void DataSegemntF(char *label, int address, int *hasErr){
    if (indexDL < 50) { // Check if the array limit is not exceeded
        dataLabelMap[indexDL].name = malloc(strlen(label) + 1); // Allocate memory for the string
        if (dataLabelMap[indexDL].name != NULL) {
            strcpy(dataLabelMap[indexDL].name, label); // Copy instruction to the structure
            dataLabelMap[indexDL].address = address;
            indexDL++;
        } else {
            printf("Memory allocation failed!\n");
            return;
        }
    } else {
        printf("Error: The number of variables limit was exceeded!\n");
        *hasErr = 1;
        return;
    }
}

bool isValidFormat(char *str) {
    int len = strlen(str);

    if (str[0] != '\"' || str[len - 1] != '\"') {
        return false;
    }
    return true;
}

