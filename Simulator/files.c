#include "Simulator.h"
#include "files.h"

void write_regout_file(){
	char word[CMD_LENGTH_HEX+1];
	for (int i = 2; i < NUM_OF_REGISTERS; i++){
		sprintf_s(word, CMD_LENGTH_HEX + 1, "%08X", registers_values[i]);
		fputs(word, context.regout_fd);
		fputc("\r\n", context.regout_fd);
	}
}

void write_dmemout_file(){
	char word[CMD_LENGTH_HEX+1];
	for (int i = 0; i < MEM_SIZE; i++){
		sprintf_s(word, CMD_LENGTH_HEX + 1, "%08X", memory[i]);
		fputs(word, context.dmemout_fd);
		fputc("\r\n", context.dmemout_fd);
	}
}

void write_cycles_file(){
	char word[CMD_LENGTH_HEX+1];
	sprintf_s(word, CMD_LENGTH_HEX + 1, "%d", io_registers_values[clks]);
	fputs(word, context.cycles_fd);
	fputc("\r\n", context.cycles_fd);

}

void add_to_hwregtrace_file(Command *cmd){
	char line[4*CMD_LENGTH_HEX+4];
    char* mode;
    if(cmd->Opcode == op_out){
        mode = "WRITE";
    } else{
        mode = "READ";
    }
    int index = registers_values[cmd->RS] + registers_values[cmd->RT];

    int val;
    if(cmd->Opcode == op_in){
        val = io_registers_values[cmd->RD];
    } else{
        val = io_registers_values[index];
    }    
		

	sprintf_s(line, 4*CMD_LENGTH_HEX + 4, "%d %s %s %08X", 
		io_registers_values[clks] + 1, mode, io_registers_names[index], val);
	fputs(line, context.hwregtrace_fd);
	fputc("\r\n", context.hwregtrace_fd);

}

void add_to_trace_file(Command *cmd){
	char word[2*CMD_LENGTH_HEX+2];
	sprintf_s(word, 2*CMD_LENGTH_HEX+2, "%03X %s", cmd->PC, cmd->INST);
	fputs(word, context.trace_fd);

	for (int i = 0; i < NUM_OF_REGISTERS; i++){
		sprintf_s(word, 2*CMD_LENGTH_HEX+2, " %08X",  registers_values[i]);
        fputs(word, context.trace_fd);
	}
	fputc("\r\n", context.trace_fd);

	if (cmd->Opcode == op_out || cmd->Opcode == op_in)
		add_to_hwregtrace_file(cmd);
}