void registersToFile(void);
void cyclesToFile(void);
void memoryToFile(void);
void hwRegTraceToFile(Command cmd);
void traceToFile(Command cmd);

char *io_registers_names[] = { 
    "irq0enable",     
    "irq1enable",
    "irq2enable",
    "irq0status",
    "irq1status",
    "irq2status",
    "irqhandler",
    "irqreturn",
    "clks",
    "leds",
    "display7seg",
    "timerenable",
    "timercurrent",
    "timermax",
    "diskcmd",
    "disksector",
    "diskbuffer",
    "diskstatus",
    "reserved1",
    "reserved2",
    "monitoraddr",
    "monitordata",
    "monitorcmd"
};