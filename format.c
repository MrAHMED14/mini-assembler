#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"
#include "format.h"


//Format R 95% Done!
void FromatR(char *instruction, int line, int *hasErr){
    char *insO = strdup(instruction);

    char *tmp1 = strdup(instruction);
    char *tmp2 = strdup(instruction);

    char *Mnemonic = strtok(tmp1, " \t");
    char *Rd, *Rt, *Rs, *Shamt = "00000", *func = SearchR(Mnemonic), *op = "000000", ResultR[33];

    if(!strcmp(Mnemonic,"jr")){
        char *NRs = strtok(NULL, ",");

        removeLeadingSpaces(NRs);
        removeTrailingSpaces(NRs);

        char *extra = strtok(NULL, "");
        if (extra != NULL) {
            printf("Error: Additional content discovered at line %d.\n", line);
            *hasErr = 1;
            return;
        }

        if(NRs != NULL){
            if((Rs = RegSearch(NRs))){
                Rd = "00000";
                Rt = "00000";
                snprintf(ResultR, sizeof(ResultR), "%s%s%s%s%s%s", op, Rs, Rt, Rd, Shamt, func);
                int M = binaryToDecimal(ResultR);
                insCodeF(insO, M, hasErr);
            }else{
                printf("Error: No matching register found at line %d.\n", line);
                *hasErr = 1;
                return;
            }
        }
    }
    else {
        char *Mnemonic = strtok(tmp2, " \t");
        char *NRd = strtok(NULL, ",");
        char *NRs = strtok(NULL, ",");
        char *NRt = strtok(NULL, "");

        removeLeadingSpaces(NRd);
        removeTrailingSpaces(NRd);

        removeLeadingSpaces(NRs);
        removeTrailingSpaces(NRs);

        removeLeadingSpaces(NRt);
        removeTrailingSpaces(NRt);

        char *extra = strtok(NULL, "");
        if (extra != NULL) {
            printf("Error: Additional content discovered at line %d.\n", line);
            *hasErr = 1;
            return;
        }

        if (NRd == NULL || NRs == NULL || NRt == NULL || Mnemonic == NULL) {
            printf("Error: Unable to find Rd, Rs, or Rt at line %d.\n", line);
            *hasErr = 1;
            return;
        }

        if(!strcmp(Mnemonic,"srl") || !strcmp(Mnemonic,"sll")){
            if(isStringNumeric(NRt)){
                int num = atoi(NRt);
                if(num >= 0 && num < 32){
                        Shamt = decimalToBinary(num,5);
                        if((Rd = RegSearch(NRd)) && (Rt = RegSearch(NRs))){
                            Rs = "00000";
                            snprintf(ResultR, sizeof(ResultR), "%s%s%s%s%s%s", op, Rs, Rt, Rd, Shamt, func);
                            int M = binaryToDecimal(ResultR);
                            insCodeF(insO, M, hasErr);
                        }
                        else{
                            printf("Error: No matching register found at line %d.\n", line);
                            *hasErr = 1;
                            return;
                        }
                }
                else{
                    printf("Error: Overflow detected at line %d.\n", line);
                    *hasErr = 1;
                    return;
                }
            }
            else{
                printf("Error: Instruction error detected at line %d.\n", line);
                *hasErr = 1;
                return;
            }
        }

        else if((Rd = RegSearch(NRd)) && (Rs = RegSearch(NRs)) && (Rt = RegSearch(NRt))){
            snprintf(ResultR, sizeof(ResultR), "%s%s%s%s%s%s", op, Rs, Rt, Rd, Shamt, func);
            int M = binaryToDecimal(ResultR);
            insCodeF(insO, M, hasErr);
        }

        else{
            printf("Error: No matching register found at line %d.\n", line);
            *hasErr = 1;
            return;
        }
    }
}


//Format I 85% Done!
void FromatI(char *instruction, int line, int *hasErr, int PC){
    char *insO = strdup(instruction);

    char *tmp = strdup(instruction);
    char *tmp2 = strdup(instruction);

    char *Mnemonic = strtok(tmp, " \t");

    char *Rt, *Rs, *immed16, *op = SearchI(Mnemonic), ResultR[33];

    if(!strcmp(Mnemonic,"lw") || !strcmp(Mnemonic,"sw")){
        char* Mnemonic = strtok(tmp2, " \t");
        char* NRt = strtok(NULL, ",");
        char* restOfInstruction = strtok(NULL, "");

        removeLeadingSpaces(restOfInstruction);
        removeTrailingSpaces(restOfInstruction);

        removeLeadingSpaces(NRt);
        removeTrailingSpaces(NRt);

        if(NRt == NULL || Mnemonic == NULL || restOfInstruction == NULL){
            printf("Error: instruction error at line %d\n",line);
            *hasErr = 1;
            return;
        }

        if (strstr(restOfInstruction, "(") != NULL && strstr(restOfInstruction, ")") != NULL){
                char *tmp3 = strdup(restOfInstruction);
                char* rest = strtok(restOfInstruction, "$");

                removeLeadingSpaces(rest);
                removeTrailingSpaces(rest);

                //Case lw Rt, (Rs)
                if(!strcmp(rest,"(")){
                    char *set = strtok(tmp3, "(");
                    char *NRs = strtok(set, ")");
                    char *extra = strtok(NULL, "");

                    if (extra != NULL) {
                        printf("Error: Additional content discovered at line %d.\n", line);
                        *hasErr = 1;
                        return;
                    }

                    removeLeadingSpaces(NRs);
                    removeTrailingSpaces(NRs);

                    if(NRs == NULL){
                        printf("Error: instruction error at line %d\n",line);
                        *hasErr = 1;
                        return;
                    }

                    if((Rs = RegSearch(NRs)) && (Rt = RegSearch(NRt))){
                        immed16 = decimalToBinary(0,16);
                        snprintf(ResultR, sizeof(ResultR), "%s%s%s%s", op, Rs, Rt, immed16);
                        int M = binaryToDecimal(ResultR);

                        insCodeF(insO, M, hasErr);
                    }
                    else{
                        printf("Error: Unable to find Rd or Rt at line %d.\n", line);
                        *hasErr = 1;
                        return;
                    }
                }

                //Case lw Rt, offset(Rs)
                else if(containsNumber(rest)){
                    char *set = strtok(tmp3, "(");

                    char *NRs = strtok(NULL, ")");

                    char* offset = strtok(rest, "(");

                    removeLeadingSpaces(NRs);
                    removeTrailingSpaces(NRs);

                    removeLeadingSpaces(offset);
                    removeTrailingSpaces(offset);

                    if(NRs == NULL || offset == NULL){
                        printf("Error: instruction error at line %d\n",line);
                        *hasErr = 1;
                        return;
                    }

                    if(isStringNumeric(offset)){
                        int num = atoi(offset);

                        if((Rs = RegSearch(NRs)) && (Rt = RegSearch(NRt))){
                            if(num >= -32768 && num < 32768){
                                immed16 = decimalToBinary(num,16);
                                snprintf(ResultR, sizeof(ResultR), "%s%s%s%s", op, Rs, Rt, immed16);
                                int M = binaryToDecimal(ResultR);

                                insCodeF(insO, M, hasErr);
                            }
                            else{
                                printf("Error: Offset out of range - pseudo instruction not supported at line %d.\n", line);
                                *hasErr = 1;
                                return;
                            }
                        }
                        else{
                            printf("Error: No matching register found at line %d.\n", line);
                            *hasErr = 1;
                            return;
                        }
                    }
                    else{
                        printf("Error: Offset error detected at line %d.\n", line);
                        *hasErr = 1;
                        return;
                    }

                }

                //Case lw Rt, lable(Rs)
                else{
                    printf("Error: Pseudo instruction not supported at line %d.\n", line);
                    *hasErr = 1;
                    return;
                }
            }

            //Case lw Rt, lable
            else {
                printf("Error: Pseudo instruction not supported at line %d.\n", line);
                *hasErr = 1;
                return;
            }
    }

    else if(!strcmp(Mnemonic,"beq") || !strcmp(Mnemonic,"bne")){
        char *Mnemonic = strtok(tmp2, " \t");
        char *NRt = strtok(NULL, ",");
        char *NRs = strtok(NULL, ",");
        char *offset = strtok(NULL, "");

        removeLeadingSpaces(NRt);
        removeTrailingSpaces(NRt);

        removeLeadingSpaces(NRs);
        removeTrailingSpaces(NRs);

        removeLeadingSpaces(offset);
        removeTrailingSpaces(offset);

        if(NRt == NULL || NRs == NULL || offset == NULL || Mnemonic == NULL){
            printf("Error: instruction error at line %d\n",line);
            *hasErr = 1;
            return;
        }

        int labelAddress = SearchLabel(offset);

        if(isLabel(offset) && (labelAddress != -1)){
            int address = (labelAddress - PC)/4;
            immed16 = decimalToBinary(address, 16);
            if((Rs = RegSearch(NRs)) && (Rt = RegSearch(NRt))){
                snprintf(ResultR, sizeof(ResultR), "%s%s%s%s", op, Rt, Rs, immed16);
                int M = binaryToDecimal(ResultR);

               insCodeF(insO, M, hasErr);
            }else{
                printf("Error: Unable to find Rs or Rt at line %d.\n", line);
                *hasErr = 1;
                return;
            }
        }else{
            printf("Error: Label not found at line %d.\n", line);
            *hasErr = 1;
            return;
        }
    }

    else if(!strcmp(Mnemonic,"la")){
        char *Mnemonic = strtok(tmp2, " \t");
        char *NRt = strtok(NULL, ",");
        char *offset = strtok(NULL, "");

        removeLeadingSpaces(offset);
        removeTrailingSpaces(offset);

        removeLeadingSpaces(NRt);
        removeTrailingSpaces(NRt);

        if(NRt == NULL || offset == NULL || Mnemonic == NULL){
            printf("Error: instruction error at line %d\n",line);
            *hasErr = 1;
            return;
        }

        int labelAddress = SearchDataLabel(offset);

        if(isLabel(offset) && (labelAddress != -1)){
            int upper = (labelAddress >> 16) & 0xFFFF;
            int lower = labelAddress & 0xFFFF;

            if(Rt = RegSearch(NRt)){
                char *insLui = "lui";
                char *insOri = "ori";
                char ins[256];

                snprintf(ins, sizeof(ins), "%s $at, %d", insLui, upper);
                FromatI(ins, line, hasErr, PC);

                snprintf(ins, sizeof(ins), "%s %s, $at, %d", insOri, NRt, lower);
                FromatI(ins, line, hasErr, PC);

            }else{
                printf("Error: No matching register found at line %d.\n", line);
                *hasErr = 1;
                return;
            }
        }else{
            printf("Error: Label not found at line %d.\n", line);
            *hasErr = 1;
            return;
        }
    }//endif

    else if(!strcmp(Mnemonic,"lui")){
        char *Mnemonic = strtok(tmp2, " \t");
        char *NRt = strtok(NULL, ",");
        char *immed = strtok(NULL, "");

        removeLeadingSpaces(immed);
        removeTrailingSpaces(immed);

        removeLeadingSpaces(NRt);
        removeTrailingSpaces(NRt);

        if(NRt == NULL || immed == NULL || Mnemonic == NULL){
            printf("Error: instruction error at line %d\n",line);
            *hasErr = 1;
            return;
        }

        if(isStringNumeric(immed)){
            int num = atoi(immed);
            if(num < 65536){
                immed16 = decimalToBinary(num,16);
                if(Rt = RegSearch(NRt)){
                    Rs = "00000";
                    snprintf(ResultR, sizeof(ResultR), "%s%s%s%s", op, Rs, Rt,immed16);
                    int M = binaryToDecimal(ResultR);
                    insCodeF(insO, M, hasErr);
                }else{
                    printf("Error: No matching register found at line %d.\n", line);
                    *hasErr = 1;
                    return;
                }
            }else{
                printf("Error: Overflow detected at line %d.\n", line);
                *hasErr = 1;
                return;
            }

        }else{
            printf("Error: Instruction error detected at line %d.\n", line);
            *hasErr = 1;
            return;
        }
    }

    else{
        char *Mnemonic = strtok(tmp2, " \t");
        char *NRt = strtok(NULL, ",");
        char *NRs = strtok(NULL, ",");
        char *immed = strtok(NULL, ",");

        removeLeadingSpaces(NRs);
        removeTrailingSpaces(NRs);

        removeLeadingSpaces(NRt);
        removeTrailingSpaces(NRt);

        if (NRs == NULL || NRt == NULL || immed == NULL || Mnemonic == NULL) {
            printf("Error: instruction error at line %d\n",line);
            *hasErr = 1;
            return;
        }
        char *extra = strtok(NULL, "");
        if (extra != NULL) {
            printf("Error: Additional content discovered at line %d.\n", line);
            *hasErr = 1;
            return;
        }

        if(isStringNumeric(immed)){
            int num = atoi(immed);
            if(num >= -32768 && num < 32768){
                    immed16 = decimalToBinary(num,16);
                    if((Rs = RegSearch(NRs)) && (Rt = RegSearch(NRt))){
                        snprintf(ResultR, sizeof(ResultR), "%s%s%s%s", op, Rs, Rt,immed16);
                        int M = binaryToDecimal(ResultR);

                        insCodeF(insO, M, hasErr);
                    }
                    else{
                        printf("Error: No matching register found at line %d.\n", line);
                        *hasErr = 1;
                        return;
                    }
            }
            else{
                printf("Error: Overflow detected at line %d.\n", line);
                *hasErr = 1;
                return;
            }
        }
        else{
            printf("Error: Instruction error detected at line %d.\n", line);
            *hasErr = 1;
            return;
        }
    }
}


//Format I 80% Done!
void FromatJ(char *instruction, int line, int *hasErr){
    char *insO = strdup(instruction);

    char *Mnemonic = strtok(instruction, " \t");
    char *label = strtok(NULL, "");

    removeLeadingSpaces(label);
    removeTrailingSpaces(label);

    if(Mnemonic == NULL || label == NULL){
         printf("Error: Instruction error detected at line %d.\n", line);
         *hasErr = 1;
         return;
    }

    char *immed26, *op = SearchJ(Mnemonic), ResultR[33];

    int hex = SearchLabel(label);

    if(hex != -1){
        int address = hex / 4;
        immed26 = decimalToBinary(address,26);

        snprintf(ResultR, sizeof(ResultR), "%s%s", op, immed26);
        int M = binaryToDecimal(ResultR);

        insCodeF(insO, M, hasErr);
    }else{
        printf("Error: Label not found at line %d.\n", line);
        *hasErr = 1;
        return;
    }
}


