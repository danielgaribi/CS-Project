#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_LINE_LENGTH 500 


void updateInterrupts();
void updateIrq0();
void updateIrq2();
int interruptHandler();
void return_from_interrupt();
void initInterrupts();
void initIrq2();

#endif
