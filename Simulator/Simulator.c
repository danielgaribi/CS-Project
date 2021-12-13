#include "Simulator.h"

uint32_t registers_values[NUM_OF_REGISTERS] = { 0 };
uint32_t momory[MEM_SIZE] = { 0 };
Command* commands[MAX_NUM_OF_COMMANDS] = { NULL };
int pc;

void simulator(char *trace_file) {
    pc = 0;
    registers_values[0] = 0;

    while (TRUE)
    {
        if (call_action(commands[pc], trace_file) == FALSE) {
            break;
        }
        pc++;
    }
    
}

bool call_action(Command *cmd, char *trace_file) {
    registers_values[1] = cmd->Imm1;
    registers_values[2] = cmd->Imm2;
    add_to_trace_file(cmd, trace_file);
    switch (opcodes[cmd->Opcode])
    {
        case "add":
            add(cmd);
            break;
        case "sub":
            sub(cmd);
            break;
        case "mac":
            mac(cmd);
            break;
        case "and":
            and(cmd);
            break;
        case "or":
            or(cmd);
            break;
        case "xor":
            xor(cmd);
            break;
        case "sll":
            sll(cmd);
            break;
        case "sra":
            sra(cmd);
            break;
        case "srl":
            srl(cmd);
            break;
        case "beq":
            beq(cmd);
            break;
        case "bne":
            bne(cmd);
            break;
        case "blt":
            blt(cmd);
            break;
        case "bgt":
            bgt(cmd);
            break;
        case "ble":
            ble(cmd);
            break;
        case "bge":
            bge(cmd);
            break;
        case "jal":
            jal(cmd);
            break;
        case "lw":
            lw(cmd);
            break;
        case "sw":
            sw(cmd);
            break;
        case "reti":
            reti(cmd);
            break;
        case "in":
            in(cmd);
            break;
        case "out":
            out(cmd);
            break;
        case "halt":
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
    FILE *file = fopen(filename, "r");
    assert(file != NULL);

    local_pc = 0;
    while (fgets(line, BUFFER_MAX_SIZE, file) != NULL) {
        parse_cmd_line(line);
        local_pc++;
    }

    fclose( file );
}

void parse_cmd_line(char *line, int local_pc) {
    uint64_t bin_cmd = 0;
    Command *cmd;
    bin_cmd = (uint64_t) strtol(imm, NULL, 16);
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
    read_diskin_file(diskin_file);
    read_irq2in_file(irq2in_file);

    simulator(trace_file);

    return 0;
}