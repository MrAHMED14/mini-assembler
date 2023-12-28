#ifndef LIB_H_INCLUDED
#define LIB_H_INCLUDED


#include <stdbool.h>

typedef struct Label label;
struct Label{
	const char *name;
	int address;
};

typedef struct rF rType;
struct rF{
	const char *name;
	char *function;
};



// Struct for I-Type instructions
typedef struct iF iType;
struct iF{
	const char *name;
	char *address;
};



// Struct for J-Type instructions
typedef struct jF jType;
struct jF{
	const char *name;
	char *address;
};


// Struct that stores registers and their respective binary reference
typedef struct registers reg;
struct registers{
	const char *name;
	char *address;
};



typedef struct Machine insCode;
struct Machine{
    char *ins;
    int machineCode;
};

extern label labelMap[50];
extern label dataLabelMap[50];

unsigned int binaryToDecimal(const char *binaryStr);
int isLineEmpty(const char *line);
char instruction_type(char *instruction);
char *RegSearch(char *registerName);
char *SearchR(char *name);
char *SearchI(char *name);
char *SearchJ(char *name);
int isStringNumeric(const char *str);
char *decimalToBinary(int number, int bits);
int containsNumber(const char* str) ;
int checkString(const char *str);
void removeLeadingSpaces(char *str);
bool isLabel(const char *str) ;
void removeTrailingSpaces(char* str);
int SearchLabel(const char* label);



#endif // LIB_H_INCLUDED
