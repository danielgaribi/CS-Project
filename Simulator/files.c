#include "Simulator.h"
#include "files.h"

void write_regout_file() {
	int i;
	char word[CMD_LENGTH_HEX+1];
	for (i = 3; i < NUM_OF_REGISTERS; i++){
		sprintf_s(word, CMD_LENGTH_HEX + 1, "%08X", registers_values[i]);
		fputs(word, context.regout_fd);
		fputs("\r\n", context.regout_fd);
	}
}

void write_dmemout_file() {
	int zero_counter = 0;
	int i, j;
	char word[CMD_LENGTH_HEX+1];
	for (i = 0; i < MEM_SIZE; i++) {
		if (memory[i] == 0) {
			zero_counter++;
			continue;
		}
		for (j = 0; j < zero_counter; j++) {
			sprintf_s(word, CMD_LENGTH_HEX + 1, "%08X", 0);
			fputs(word, context.dmemout_fd);
			fputs("\r\n", context.dmemout_fd);
		}
		sprintf_s(word, CMD_LENGTH_HEX + 1, "%08X", memory[i]);
		fputs(word, context.dmemout_fd);
		fputs("\r\n", context.dmemout_fd);
		zero_counter = 0;
	}
}

void write_cycles_file() {
	char word[CMD_LENGTH_HEX+1];
	sprintf_s(word, CMD_LENGTH_HEX + 1, "%d", io_registers_values[clks]);
	fputs(word, context.cycles_fd);
	fputs("\r\n", context.cycles_fd);
}

void add_to_hwregtrace_file(Command *cmd) {
	int index, val;
	char line[4*CMD_LENGTH_HEX+4];
    char* mode;
    if(cmd->Opcode == op_out){
        mode = "WRITE";
    } else{
        mode = "READ";
    }
    index = registers_values[cmd->RS] + registers_values[cmd->RT];

    if(cmd->Opcode == op_in){
        val = io_registers_values[cmd->RD];
    } else{
        val = io_registers_values[index];
    }    
		

	sprintf_s(line, 4*CMD_LENGTH_HEX + 4, "%d %s %s %08x", 
		io_registers_values[clks], mode, io_registers_names[index], val);
	fputs(line, context.hwregtrace_fd);
	fputs("\r\n", context.hwregtrace_fd);

}

void add_to_trace_file(Command *cmd) {
	int i;
	char word[2*CMD_LENGTH_HEX+2];
	sprintf_s(word, 2*CMD_LENGTH_HEX+2, "%03X %s", cmd->PC, cmd->INST);
	fputs(word, context.trace_fd);

	for (i = 0; i < NUM_OF_REGISTERS; i++){
		sprintf_s(word, 2*CMD_LENGTH_HEX+2, " %08x",  registers_values[i]);
        fputs(word, context.trace_fd);
	}
	fputs("\r\n", context.trace_fd);
}