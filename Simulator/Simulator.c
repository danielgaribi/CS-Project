#include "Simulator.h"
#include "interrupt.h"


uint32_t registers_values[NUM_OF_REGISTERS] = { 0 };
uint32_t io_registers_values[NUM_OF_IO_REGISTERS] = { 0 };
uint32_t memory[MEM_SIZE] = { 0 };
Command* commands[MAX_NUM_OF_COMMANDS] = { NULL };
FD_Context context = { 0 };
int pc;

void simulator() {
    pc = 0;
    initInterrupts();

    while (TRUE)
    {
        registers_values[0] = 0;
        if (call_action(commands[pc]) == FALSE) {
            break;
        }
         //update clks
        updateInterrupts();
        interruptHandler();

        pc++;
       
    }
    
}

bool call_action(Command *cmd) {
    registers_values[1] = cmd->Imm1;
    registers_values[2] = cmd->Imm2;
    // add_to_trace_file(cmd, context.trace_fd);
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

void read_imemin_file() {
    int local_pc;
    char line[CMD_LENGTH_HEX + 1];

    local_pc = 0;
    while (fgets(line, CMD_LENGTH_HEX + 1, context.imemin_fd) != NULL) {
        if ( isLineEmptyOrNoteOnly( line ) == FALSE ) {
            continue;
        }
        parse_cmd_line(line, local_pc);
        local_pc++;
    }

    fclose( context.imemin_fd );
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

void read_dmemin_file() {
    int loc = 0;
    char line[MEM_LENGTH_HEX + 1];

    while (fgets(line, MEM_LENGTH_HEX + 1, context.dmemin_fd) != NULL) {
        if ( isLineEmptyOrNoteOnly( line ) == FALSE ) {
            continue;
        }
        memory[loc] = (uint32_t) strtol(line, NULL, 16);
        loc++;
    }

    fclose( context.dmemin_fd );
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

void set_FD_context( char *argv[] ) {
    context.imemin_fd        = fopen( argv[ 1 ], "a+" );   assert( context.imemin_fd != NULL);
    context.dmemin_fd        = fopen( argv[ 2 ], "a+" );   assert( context.dmemin_fd != NULL);
    context.diskin_fd        = fopen( argv[ 3 ], "a+" );   assert( context.diskin_fd != NULL);
    context.irq2in_fd        = fopen( argv[ 4 ], "a+" );   assert( context.irq2in_fd != NULL);
    context.dmemout_fd       = fopen( argv[ 5 ], "a+" );   assert( context.dmemout_fd != NULL);
    context.regout_fd        = fopen( argv[ 6 ], "a+" );   assert( context.regout_fd != NULL);
    context.trace_fd         = fopen( argv[ 7 ], "a+" );   assert( context.trace_fd != NULL);
    context.hwregtrace_fd    = fopen( argv[ 8 ], "a+" );   assert( context.hwregtrace_fd != NULL);
    context.cycles_fd        = fopen( argv[ 9 ], "a+" );   assert( context.cycles_fd != NULL);
    context.led_fd           = fopen( argv[ 10 ], "a+" );  assert( context.led_fd != NULL);
    context.display7reg_fd   = fopen( argv[ 11 ], "a+" );  assert( context.display7reg_fd != NULL);
    context.diskout_fd       = fopen( argv[ 12 ], "a+" );  assert( context.diskout_fd != NULL);
    context.monitor_fd       = fopen( argv[ 13 ], "a+" );  assert( context.monitor_fd != NULL);
    context.monitor_yuv_fd   = fopen( argv[ 14 ], "a+" );  assert( context.monitor_yuv_fd != NULL);
}

int main( int argc, char *argv[] ) {
    assert( argc == 15 );
    set_FD_context( argv );


    /* get fd for files from sturct FD_context */
    read_imemin_file();
    read_dmemin_file();
    // read_diskin_file(diskin_file);
    // read_irq2in_file(irq2in_file);

    simulator();

    return 0;
}
