#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_LINE_LENGTH 1024 /** TODO ?????? value was missing - yuval plese add value*/


void updateInterrupts();
void updateIrq0();
void updateIrq2();
int interruptHandler();
void initInterrupts();
void initIrq2();