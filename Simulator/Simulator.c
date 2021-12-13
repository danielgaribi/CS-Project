#include "Simulator.h"

uint32_t registers_values[NUM_OF_REGISTERS] = { 0 };
uint32_t io_registers_values[NUM_OF_IO_REGISTERS] = { 0 };
uint32_t memory[MEM_SIZE] = { 0 };
Command* commands[MAX_NUM_OF_COMMANDS] = { NULL };
int pc;

void simulator(char *trace_file) {
    pc = 0;

    while (TRUE)
    {
        registers_values[0] = 0;
        if (call_action(commands[pc], trace_file) == FALSE) {
            break;
        }
        pc++;
    }
    
}

bool call_action(Command *cmd, char *trace_file) {
    registers_values[1] = cmd->Imm1;
    registers_values[2] = cmd->Imm2;
    // add_to_trace_file(cmd, trace_file);
    switch (cmd->Opcode)
    {
        case op_add:
            add(cmd);
            break;
        // case op_sub:
        //     sub(cmd);
        //     break;
        // case op_mac:
        //     mac(cmd);
        //     break;
        // case op_and:
        //     and(cmd);
        //     break;
        // case op_or:
        //     or(cmd);
        //     break;
        // case op_xor:
        //     xor(cmd);
        //     break;
        // case op_sll:
        //     sll(cmd);
        //     break;
        // case op_sra:
        //     sra(cmd);
        //     break;
        // case op_srl:
        //     srl(cmd);
        //     break;
        // case op_beq:
        //     beq(cmd);
        //     break;
        // case op_bne:
        //     bne(cmd);
        //     break;
        // case op_blt:
        //     blt(cmd);
        //     break;
        // case op_bgt:
        //     bgt(cmd);
        //     break;
        // case op_ble:
        //     ble(cmd);
        //     break;
        // case op_bge:
        //     bge(cmd);
        //     break;
        // case op_jal:
        //     jal(cmd);
        //     break;
        // case op_lw:
        //     lw(cmd);
        //     break;
        // case op_sw:
        //     sw(cmd);
        //     break;
        // case op_reti:
        //     reti(cmd);
        //     break;
        // case op_in:
        //     in(cmd);
        //     break;
        // case op_out:
        //     out(cmd);
        //     break;
        case op_halt:
            return FALSE;
        
        default:
            assert(FALSE);
            return FALSE;
    }
    return TRUE;
}

void add (Command *cmd) {
    int rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    registers_values[cmd->RD] = rs_value + rt_value + rm_value;
}

void read_imemin_file(char *filename) {
    FILE *file;
    int local_pc;
    char line[CMD_LENGTH_HEX + 1];
    file = fopen(filename, "r");
    assert(file != NULL);

    local_pc = 0;
    while (fgets(line, CMD_LENGTH_HEX + 1, file) != NULL) {
        if ( isLineEmptyOrNoteOnly( line ) == FALSE ) {
            continue;
        }
        parse_cmd_line(line, local_pc);
        local_pc++;
    }

    fclose( file );
}

void parse_cmd_line(char *line, int local_pc) {
    uint64_t bin_cmd = 0;
    Command *cmd;
    bin_cmd = (uint64_t) strtol(line, NULL, 16);
    cmd = (Command*) malloc(sizeof(Command));

    cmd->PC = local_pc;
    strcpy(cmd->INST, line);
    SET_IMM1(bin_cmd, cmd);
    SET_IMM2(bin_cmd, cmd);
    SET_RM(bin_cmd, cmd);
    SET_RT(bin_cmd, cmd);
    SET_RS(bin_cmd, cmd);
    SET_RD(bin_cmd, cmd);
    SET_OPCODE(bin_cmd, cmd);

    commands[local_pc] = cmd;
}

void read_dmemin_file(char *filename) {
    FILE *file;
    int loc = 0;
    char line[MEM_LENGTH_HEX + 1];
    file = fopen(filename, "r");
    assert(file != NULL);

    while (fgets(line, MEM_LENGTH_HEX + 1, file) != NULL) {
        if ( isLineEmptyOrNoteOnly( line ) == FALSE ) {
            continue;
        }
        memory[loc] = (uint32_t) strtol(line, NULL, 16);
        loc++;
    }

    fclose( file );
}

bool isLineEmptyOrNoteOnly( char *line ) {
    while ( line[ 0 ] != '\0' ) {
        if ( line[ 0 ] == '\n' || line[ 0 ] == '#' ) {
            return FALSE;
        } else if ( line[ 0 ] == ' ' || line[ 0 ] == '\t' ) {
            line++;
        } else {
            break;
        }
    }
    return TRUE;
}

int main( int argc, char *argv[] ) {
    char *imemin_file,
         *dmemin_file,
         *diskin_file,
         *irq2in_file,
         *dmemout_file,
         *regout_file,
         *trace_file,
         *hwregtrace_file,
         *cycles_file,
         *led_file,
         *display7reg_file,
         *diskout_file,
         *monitor_file,
         *monitor_yuv_file;

    assert( argc == 15 );

    imemin_file         = argv[1];
    dmemin_file         = argv[2];
    diskin_file         = argv[3];
    irq2in_file         = argv[4];
    dmemout_file        = argv[5];
    regout_file         = argv[6];
    trace_file          = argv[7];
    hwregtrace_file     = argv[8];
    cycles_file         = argv[9];
    led_file            = argv[10];
    display7reg_file    = argv[11];
    diskout_file        = argv[12];
    monitor_file        = argv[13];
    monitor_yuv_file    = argv[14];

    read_imemin_file(imemin_file);
    read_dmemin_file(dmemin_file);
    // read_diskin_file(diskin_file);
    // read_irq2in_file(irq2in_file);

    simulator(trace_file);

    return 0;
}