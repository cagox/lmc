#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lmc.h"

/*
 * The entire purpose of initialize is to make sure that the LMC
 * program starts off with a clean slate. No return value is needed.
 * If an error takes place at this stage, it will be an exception outside
 * of the normal scope of this program.
 */
void initialize(struct LMC * lmc) {
    lmc->accumulator = 0;
    lmc->counter = 0;

    for(int i = 0; i < MEMORYSIZE; i++) {
        lmc->memory[i] = 0;
    }
}


/*
 * loadProgram takes a reference to the LMC struct and a path to the
 * program file to load. By not hardcoding the file path, we leave
 * ourselves open to potentially extend the project at a later time
 * by adding more interaction at the command prompt. i.e. The ability
 * for a user to specify a program to load. This initial version isn't
 * going to do that, but there is no point in not doing simple things
 * now that won't take up any extra resources and will simplify things
 * later. The return value is an integer. This won't mean anything for
 * the first iteration of the program, but it will allow us to more
 * easily add error checking later.
 */
int loadProgram(struct LMC *lmc, char* filename) {
    FILE *fileptr;

    /* fileptr = fopen(filename, "r"); */
    fopen_s(&fileptr, filename, "r");

    if(fileptr == NULL)
    {
        printf("File %s failed to load.", filename);
        exit(-1);
    }
    char buffer[LINELENGTH];
    int count = 0;


    while(fgets(buffer, LINELENGTH, fileptr) != NULL) {


        if(buffer[0] == '#' || buffer[0] == '/') {
            continue;
        }


        /*
         * MEMORYSIZE represents the hard limit on the LMC's memory capacity.
         * This means we can't have more instructions/data than memory size.
         * As arrays in C (and most languages) are 0 indexed, this means that
         * reaching count == MEMORYSIZE represents an error.
         */
        if(count >= MEMORYSIZE){
            printf("Memory Out of Bounds error at line %d.", count);
        }

        int value = -1;
        char *token1 = strtok(buffer, " \n");
        char *token2 = strtok(NULL, " \n");

        /* printf("Line[%d]: Token1: %s, Token2: %s\n", count, token1, token2); */


        if(strcmp(token1, "HLT") == 0) {
            lmc->memory[count] = 0;
            count++;
            continue;
        }
        if(strcmp(token1, "INP") == 0) {
            lmc->memory[count] = 901;
            count++;
            continue;
        }
        if(strcmp(token1, "OUT") == 0) {
            lmc->memory[count] = 902;
            count++;
            continue;
        }
        if(strcmp(token1, "ADD") == 0) {
            value = 100;
        }
        if(strcmp(token1, "SUB") == 0) {
            value = 200;
        }
        if(strcmp(token1, "STA") == 0) {
            value = 300;
        }
        if(strcmp(token1, "LDA") == 0) {
            value = 500;
        }
        if(strcmp(token1, "BRA") == 0) {
            value = 600;
        }
        if(strcmp(token1, "BRZ") == 0) {
            value = 700;
        }
        if(strcmp(token1, "BRP") == 0) {
            value = 800;
        }
        if(strcmp(token1, "DAT") == 0) {
            value = 0;
        }
        if(value < 0) { /*We got this far and haven't found an identifiable token*/
            printf("Invalid instruction at line %d.", count);
            exit(-1);
        }

        if(token2 == NULL) { /*If we are here, there SHOULD be an address or value*/
            printf("Value expected but not found at line %d.", count);
        }

        /*
         * For the moment, we are going to assume that token2 is an
         * integer. If it is not, that may cause undesirable behavior.
         * In a near future iteration we will add a little more error
         * checking to help keep things a little more stable and produce
         * more reasonable error messages.
         */
        value += atoi(token2);

        /*A little more minimal load time error checking*/
        if(value > 999) {
            printf("Overflow error at line %d.", count);
            fclose(fileptr);
            exit(-1);
        }
        if(value < 0) {
            printf("Underflow error at line %d.", count);
            fclose(fileptr);
            exit(-1);
        }

        lmc->memory[count] = value;
        count++;
    }
    fclose(fileptr);
    return(0);
}



int run(struct LMC *lmc) {
    lmc->counter = 0;
    while(lmc->counter < MEMORYSIZE)
    {
        int value = lmc->memory[lmc->counter];

        if(value == 0) {
            return(0); /*halt*/
        }

        if(value == 901) {
            int val;
           printf("Please provide input: ");
           scanf("%d", &val);
            if(val > LMCMAXINT || val < 0) {
                while(val > LMCMAXINT || val < 0) {
                    printf("Please provide input between 0 and 999: ");
                    scanf("%d", &val);
                }
            }

            lmc->accumulator = val;
            lmc->counter++;
            continue;
        }
        if(value == 902) {
            /* TODO: error checking for overflow and underflow */
            printf("Output: %d.\n", lmc->accumulator);
            lmc->counter++;
            continue;
        }

        int instruction = value/100;
        int address = value % 100;

        switch(instruction) {
            case 1: /*ADD*/
                lmc->accumulator += lmc->memory[address];
                /* TODO: Error checking for overflow*/
                lmc->counter++;
                break;
            case 2: /*SUB*/
                lmc->accumulator -= lmc->memory[address];
                lmc->counter++;
                break;
            case 3: /*STA*/
                lmc->memory[address] = lmc->accumulator;
                lmc->counter++;
                break;
            case 5: /*LDA*/
                lmc->accumulator = lmc->memory[address];
                lmc->counter++;
                break;
            case 6: /*BRA*/
                lmc->counter = address;
                break;
            case 7: /*BRZ*/
                if(lmc->accumulator == 0) {
                    lmc->counter = address;
                }
                else {
                    lmc->counter++;
                }
                break;
            case 8: /*BRP*/
                if(lmc->accumulator >= 0) {
                    lmc->counter = address;
                }
                else {
                    lmc->counter++;
                }
                break;
            default: /*ERRROR*/
                printf("Unidentified Instruction at line %d.", lmc->counter);
                exit(-1);
        } /* end switch */
    }
    return(0);
}


int main(int argc, char *argv[])
{
    struct LMC lmc;
    initialize(&lmc);
    loadProgram(&lmc, "program.lmc");
    int returnval = run(&lmc);

    if(returnval == 0) {
        printf("Execution Successful.\n");
    }
    else {
        printf("Exited with error code: %d.", returnval);
    }


    return(0);
}

