int irq0enable;
int irq1enable;
int irq2enable;
int irq0status;
int irq1status;
int irq2status;
int irqhandler;
int irqreturn;
int isInterrupt;
int pc;
int* irq2Arr;
int irq2Counter;
int irq2ArrLen;
int clockCycle;
int timerenable;
int timercurrent;
int timermax;
int MAX_LINE_LENGTH;

void updateInterrupts() {  // need to be called at the end of every "main loop" iteration before interruptHandler()
    updateIrq0()
    updateIrq2();

}

void updateIrq0(){
    if(timerenable){
        if(timercurrent >= timermax){
            irq0status = 1;
            timercurrent = 0;
        }
    } else{
        timercurrent++;
    }
}
void updateIrq2(){
    if(irq2Counter < irq2ArrLen){
        if(clockCycle == irq2Arr[irq2Counter]){
            irq2status = 1;
            irq2Counter++;
        }
    }
}

int interruptHandler() // need to be called at the end of every "main loop" iteration
{
    int irq = (irq0enable && irq0status) || (irq1enable && irq1status) || (irq2enable && irq2status);
    if (irq)
    {
        if (isInterrupt)
        {
            return;
        }
        isInterrupt = pc;
        pc = irqhandler;
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
    char line[MAX_LINE_LENGTH];
    irq2ArrLen = 0;
    irq2Counter = 0;

    while (fgets(line, 500, Irq2InFile) != NULL)
    {
        irq2ArrLen++;
    }

    if (irq2ArrLen == 0)
        return;

    rewind(Irq2InFile);
    irq2Arr = calloc(irq2ArrLen, sizeof(int));

    int index = 0;
    while (fgets(line, 500, Irq2InFile) != NULL)
    {
        Irq2Array[index] = atoi(line);
        index++;
    }
}