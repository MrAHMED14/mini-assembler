#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"
#define MAX_LENGTH 256
/**
    @struct rMap[]
    @struct iMap[]
    @struct jMap[]
    @struct insCode
    @struct Label
    @struct registerMap[]

    //Functions
    @binaryToDecimal
    @isLineEmpty
    @instructionType
    @RegSearch
    @SearchR
    @isStringNumeric
    @decimalToBinary
    @insCodeF
    @OutputP
    @containsNumber
    @checkString
    @removeLeadingSpaces
    @removeTrailingSpaces
    @isLabel
    @SearchLabel

**/

/**
---------------- Status -----------------
    - 75% Completed

-------------- ERRORS LOG ---------------
    - Comment at first line
    - (beq) label
    -
-----------------------------------------

**/

int main(){
    FILE *file, *dataFile, *textFile, *outFile;
    bool dataSection = false, textSection = false;
    char line[MAX_LENGTH + 2], Type;
    int hasErr = 0, line_count = 0, PC = 0x00400000, i = 0;

    system("color f");

    file = fopen("Assembly File/main.txt", "r");
    if (file == NULL) {
        printf("File could not be opened.");
        hasErr = 1;
        return 1;
    }

    dataFile = fopen("bin/lib/.data.txt", "w");
    textFile = fopen("bin/lib/.text.txt", "w");

    if (dataFile == NULL || textFile == NULL) {
        printf("Error creating output files.");
        hasErr = 1;
        return 1;
    }

    printf("Assembling . . .\n");
    system("cls");

    //First process
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strlen(line) == MAX_LENGTH + 1 && line[MAX_LENGTH] != '\n'){
            printf("Error: Line %d exceeds maximum length.\n", line_count + 1);
            hasErr = 1;
            fclose(file);
            return 1;
        }

        removeLeadingSpaces(line);
        if (line[0] == '#' || line[0] == '\n' || isLineEmpty(line))
            continue;

        if (!dataSection && strstr(line, ".data") != NULL) {
            dataSection = true;
            continue;
        }

        if (!textSection && strstr(line, ".text") != NULL) {
            textSection = true;
            continue;
        }

        if (dataSection && !textSection)
            fprintf(dataFile, "%s", line);
        else if (textSection)
            fprintf(textFile, "%s", line);
        else {
            printf("Error: .data and/or .text not found in the correct order.\n");
            hasErr = 1;
            break;
        }
    }

    fclose(dataFile);
    fclose(textFile);
    /** --------------------------- Data Section --------------------------- **/
    //0% Done!
    if(dataSection){
        dataFile = fopen("bin/lib/.data.txt", "r");

        unsigned int dataAddrPC = 0x10010000;
        while (fgets(line, sizeof(line), dataFile) != NULL){
            char *ins = strtok(line, "#\n");

            removeTrailingSpaces(ins);

            char *tmp = strdup(ins);
            if (strchr(tmp, ':')){
                char *label = strtok(tmp, ":");
                char *rest = strtok(NULL,"#\n");

                removeTrailingSpaces(label);

                if(isLabel(label)){
                    removeLeadingSpaces(rest);
                    char *type = strtok(rest," \t");
                    char *content = strtok(NULL,"\n");

                    removeTrailingSpaces(content);
                    removeLeadingSpaces(content);

                    if(!strcmp(type, ".asciiz") && isValidFormat(content)){
                        char *tmp = strtok(content,"\"");
                        DataSegemntF(label, dataAddrPC, &hasErr);
                        int size = strlen(tmp);

                        dataAddrPC += size + 1;

                    }else if(!strcmp(type, ".word") && isStringNumeric(content)){

                        while(dataAddrPC % 4 != 0)
                            dataAddrPC++;

                        DataSegemntF(label, dataAddrPC, &hasErr);

                        dataAddrPC += 4;

                    }else if(!strcmp(type, ".space")){
                        printf("Error: type space,byte, half,double,float,ascii not yet supported.\n");
                        hasErr = 1;
                    }
                    else{
                        printf("Error: type undined and/.or wrong usege at line %d.",line_count + 1);
                        hasErr = 1;
                    }
                }else{
                    printf("Error: label err at line %d\n",line_count + 1);
                    hasErr = 1;
                }
            }else{
                printf("Error: Unknown instruction at line %d.\n",line_count + 1);
                hasErr = 1;
            }

            line_count += 1;
        }
    }

    /** --------------------------- Text Section --------------------------- **/
    //80% Done!
    line_count = 0; i = 0;
    if(textSection){
        textFile = fopen("bin/lib/.text.txt", "r");
        PC = 0x00400000;
        //process label
        while (fgets(line, sizeof(line), textFile) != NULL){
            char *ins = strtok(line, "#\n");

            if (strstr(ins, "la") == ins) {
                PC += 4;
            }

            removeTrailingSpaces(ins);

            char *tmp = strdup(ins);

            if(checkString(ins)){
                char *label = strtok(tmp, ":");

                removeLeadingSpaces(label);
                removeTrailingSpaces(label);

                if(isLabel(label)){
                    if(i < 50){
                        labelMap[i].name = label;
                        labelMap[i].address = PC;
                        i++;
                        continue;
                    }else{
                        printf("Error: Max Labels reaches\n");
                        hasErr = 1;
                        break;
                    }
                }
            }
            PC += 4;
        }

        fclose(textFile);

        textFile = fopen("bin/lib/.text.txt", "r");
        PC = 0x00400000;

        //second process
        while (fgets(line, sizeof(line), textFile) != NULL) {
            char *instruction = strtok(line, "#\n");

            if (strstr(instruction, "la") == instruction) {
                PC += 4;
            }

            removeTrailingSpaces(instruction);

            if(checkString(instruction))
                continue;

            Type = instruction_type(instruction);

            if(Type == 'r')
                FromatR(instruction, line_count + 1, &hasErr);

            else if (Type == 'i')
                FromatI(instruction, line_count + 1, &hasErr, PC + 4);

            else if(Type == 'j')
                FromatJ(instruction, line_count + 1, &hasErr);

            else{
                printf("Error: Unrecognized instruction at line %d.\n", line_count + 1);
                hasErr = 1;
            }

            line_count++;
            PC += 4;
        }

        fclose(textFile);
    }else{
        printf("Warning: '.text' section not found.\n");
    }
    /** -------------------------------------------------------------------- **/
    //Output
    if(!hasErr){
        system("color b");
        printf("Assemble operation completed successfully.\n");
        OutputP(outFile);
        printf("\n'Output.txt' file has been created.\n");

    }else{
        system("color c");
        printf("\nAssemble operation completed with errors.\n");
    }

    // Close the file
    fclose(file);
    return 0;
}
