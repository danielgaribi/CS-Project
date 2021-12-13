#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_LINE_LENGTH                


void updateInterrupts();
void updateIrq0();
void updateIrq2();
int interruptHandler();
void initInterrupts();
void initIrq2();