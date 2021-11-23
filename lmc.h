
#ifndef LMC_H
#define LMC_H

#define MEMORYSIZE 100
#define LINELENGTH 8
#define LMCMAXINT 999

struct LMC {
    /* int inbox;  */    /* inbox and outbox don't actually */
    /* int outbox; */    /* need to exist as values         */
    int accumulator;
    int counter;
    int memory[MEMORYSIZE];
};

#endif
