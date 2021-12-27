#include "Simulator.h"
#include "files.h"




void registersToFile(){
	for (int i = 2; i < NUM_OF_REGISTERS; i++){
		fprintf(context.regout_fd, "%08X\n", registers_values[i]);
	}
}

void memoryToFile(){
	for (int i = 0; i < MEM_SIZE; i++){
		fprintf(context.dmemout_fd, "%08X\n", memory[i]);
	}
}

void cyclesToFile(){
	fprintf(context.cycles_fd, "%d\n", clockCycles);
}

void hwRegTraceToFile(Command cmd){
    char* mode;
    if(cmd.Opcode == op_out){
        mode = "WRITE";
    } else{
        mode = "READ";
    }
    int index = registers_values[cmd.RS] + registers_values[cmd.RT];

    int val;
    if(strcmp(cmd.Opcode, op_in) == 0){
        val = io_registers_values[cmd.RD];
    } else{
        val = io_registers_values[index];
    }    
		
	fprintf(context.hwregtrace_fd, "%d %s %s %08X\n", 
		clockCycles + 1, mode, io_registers_names[index], val);
	
}

void traceToFile(Command cmd){
	fprintf(context.trace_fd, "%03X %s", cmd.PC, cmd.INST);

	for (int i = 0; i < NUM_OF_REGISTERS; i++){
		fprintf(context.trace_fd, " %08X", registers_values[i]);
	}
	fputc('\n', context.trace_fd);

	if (cmd.Opcode == op_out || cmd.Opcode == op_in))
		hwRegTraceToFile(cmd);
}