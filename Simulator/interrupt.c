#include "Simulator.h"
#include "interrupt.h"

int isInterrupt;
int* irq2Arr;
int irq2Counter;
int irq2ArrLen;


void updateInterrupts() {  // need to be called at the end of every "main loop" iteration before interruptHandler()
    updateIrq0();
    updateIrq2();
}

void updateIrq0(){
    if(io_registers_values[timerenable]) {
        if(io_registers_values[timercurrent] >= io_registers_values[timermax]){
            io_registers_values[irq0status] = 1;
            io_registers_values[timercurrent] = 0;
        }
        else {
            io_registers_values[timercurrent]++;
        }
    }
}
void updateIrq2(){
    if(irq2Counter < irq2ArrLen) {
        if(io_registers_values[clks] == irq2Arr[irq2Counter]){
            io_registers_values[irq2status] = 1;
            irq2Counter++;
        }
    }
}

int interruptHandler() // need to be called at the end of every "main loop" iteration
{
    int irq = (io_registers_values[irq0enable] && io_registers_values[irq0status]) ||
              (io_registers_values[irq1enable] && io_registers_values[irq1status]) ||
              (io_registers_values[irq2enable] && io_registers_values[irq2status]);
    if (irq) {
        if (isInterrupt) {
            return FALSE;
        }
        io_registers_values[irqreturn] = pc;
        pc = io_registers_values[irqhandler];
        isInterrupt = 1;
        return TRUE;
    }
    return FALSE;
}

void return_from_interrupt() {
    isInterrupt = 0;
    if (!interruptHandler()) {
        pc--;
    }
}

void initInterrupts()// need to be called once before the "main loop"
{
    isInterrupt = 0;
    initIrq2();
}

void initIrq2()
{
    char line[MAX_LINE_LENGTH];
    irq2ArrLen = 0;
    irq2Counter = 0;
    while (fgets(line, 500, context.irq2in_fd) != NULL)
    {
        irq2ArrLen++;
    }

    if (irq2ArrLen == 0){
        return;
    }
    rewind(context.irq2in_fd);
    irq2Arr = calloc(irq2ArrLen, sizeof(int));

    int index = 0;
    while (fgets(line, 500, context.irq2in_fd) != NULL)
    {
        irq2Arr[index] = atoi(line);
        index++;
    }
}