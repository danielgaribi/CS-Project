#include "Simulator.h"
#include "interrupt.h"


int isInterrupt;
int* irq2Arr;
int irq2Counter;
int irq2ArrLen;


void updateInterrupts() {  // need to be called at the end of every "main loop" iteration before interruptHandler()
    updateIrq0()
    updateIrq2();

}

void updateIrq0(){
    if(registers_values[timerenable]){
        if(registers_values[timercurrent] >= registers_values[timermax]){
            registers_values[irq0status] = 1;
            registers_values[timercurrent] = 0;
        }
    } else{
        registers_values[timercurrent]++;
    }
}
void updateIrq2(){
    if(irq2Counter < irq2ArrLen){
        if(registers_values[clks] == irq2Arr[irq2Counter]){
            registers_values[irq2status] = 1;
            irq2Counter++;
        }
    }
}

int interruptHandler() // need to be called at the end of every "main loop" iteration
{
    int irq = (registers_values[irq0enable] && registers_values[irq0status]) ||
              (registers_values[irq1enable] && registers_values[irq1status]) ||
              (registers_values[irq2enable] && registers_values[irq2status]);
    if (irq)
    {
        if (isInterrupt)
        {
            return;
        }
        registers_values[irqreturn] = pc;
        pc = registers_values[irqhandler];
        isInterrupt = 1;
    }
    return;
}

void initInterrupts()// need to be called once before the "main loop"
{
    isInterrupt = 0;
    initIrq2Arr();
}

void initIrq2()
{
    FILE *file;
    char line[MAX_LINE_LENGTH];
    irq2ArrLen = 0;
    irq2Counter = 0;
    file = fopen(irq2in_file, "r");
    while (fgets(line, 500, file) != NULL)
    {
        irq2ArrLen++;
    }

    if (irq2ArrLen == 0)
        return;

    rewind(file);
    irq2Arr = calloc(irq2ArrLen, sizeof(int));

    int index = 0;
    while (fgets(line, 500, file) != NULL)
    {
        Irq2Array[index] = atoi(line);
        index++;
    }
}