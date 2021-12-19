#include "Simulator.h"
#include "interrupt.h"
#include "disk.h"


uint32_t registers_values[NUM_OF_REGISTERS] = { 0 };
uint32_t io_registers_values[NUM_OF_IO_REGISTERS] = { 0 };
uint32_t memory[MEM_SIZE] = { 0 };
uint8_t  monitor[MONITOR_SIZE][MONITOR_SIZE] = { 0 };
Command* commands[MAX_NUM_OF_COMMANDS] = { NULL };
FD_Context context = { 0 };
int pc;

void simulator() {
    pc = 0;
    initInterrupts();
    initDisk();

    while (TRUE)
    {
        registers_values[0] = 0; /* set $zero to 0 */
        if (call_action(commands[pc]) == FALSE) {
            break;
        }
        diskHandler();
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
        /* Add more cases ?? */
    }
}
void add(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    registers_values[cmd->RD] = rs_value + rt_value + rm_value;
}

void sub(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    registers_values[cmd->RD] = rs_value - rt_value - rm_value;
}

void mac(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    registers_values[cmd->RD] = rs_value * rt_value + rm_value;
}

void and(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    registers_values[cmd->RD] = rs_value & rt_value & rm_value;
}

void or(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    registers_values[cmd->RD] = rs_value | rt_value | rm_value;
}

void xor(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    registers_values[cmd->RD] = rs_value ^ rt_value ^ rm_value;
}

void sll(Command *cmd) {
    uint32_t rs_value, rt_value;
    READ_REGISTERS_VALUE_NO_RM(cmd, rs_value, rt_value);
    registers_values[cmd->RD] = rs_value << rt_value;
}

void sra(Command *cmd) {
    uint32_t rs_value, rt_value;
    READ_REGISTERS_VALUE_NO_RM(cmd, rs_value, rt_value);
    /**
     * R[rd] = R[rs] >>(logical) R[rt], then the msb is 0.
     * do R[rs] and 1 << 31 to get R[rs] msb with 31 0 to the right.
     * Or between them both will reter arithmetic shift
     */
    registers_values[cmd->RD] = (rs_value >> rt_value) | (rs_value & (1 << 31)); 
}

void srl(Command *cmd) {
    uint32_t rs_value, rt_value;
    READ_REGISTERS_VALUE_NO_RM(cmd, rs_value, rt_value);
    registers_values[cmd->RD] = rs_value >> rt_value;
}

void beq(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    if (rs_value == rt_value) {
        pc = (rm_value & MASK_12_LOWER_BITS) - 1; /** pc will increment in main loop by one */
    }
}

void bne(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    if (rs_value != rt_value) {
        pc = (rm_value & MASK_12_LOWER_BITS) - 1; /** pc will increment in main loop by one */
    }
}

void blt(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    if (rs_value < rt_value) {
        pc = (rm_value & MASK_12_LOWER_BITS) - 1; /** pc will increment in main loop by one */
    }
}

void bgt(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    if (rs_value > rt_value) {
        pc = (rm_value & MASK_12_LOWER_BITS) - 1; /** pc will increment in main loop by one */
    }
}

void ble(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    if (rs_value <= rt_value) {
        pc = (rm_value & MASK_12_LOWER_BITS) - 1; /** pc will increment in main loop by one */
    }
}

void bge(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    if (rs_value >= rt_value) {
        pc = (rm_value & MASK_12_LOWER_BITS) - 1; /** pc will increment in main loop by one */
    }
}

void jal(Command *cmd) {
    uint32_t rm_value = registers_values[cmd->RM];
    registers_values[cmd->RD] = pc + 1;
    pc = (rm_value & MASK_12_LOWER_BITS) - 1; /** pc will increment in main loop by one */
}

void lw(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    registers_values[cmd->RD] = memory[rs_value + rt_value] + rm_value;
}

void sw(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    memory[rs_value + rt_value] = rm_value + registers_values[cmd->RD];
}

void reti(Command *cmd) {
    pc = io_registers_values[irqreturn] - 1; /** pc will increment in main loop by one */
}

void in(Command *cmd) {
    uint32_t rs_value, rt_value;
    READ_REGISTERS_VALUE_NO_RM(cmd, rs_value, rt_value);
    registers_values[cmd->RD] = io_registers_values[rs_value + rt_value];
}

void out(Command *cmd) {
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    
    /* If command changes leds status, append to ledsFile */
    if ( ( rs_value + rt_value == leds ) && ( io_registers_values [ leds ] != rm_value ) ) {
        addToledsTraceFile();
    }
    /* If command set monitorcmd to 1, change monitor status */
    else if ( (rs_value + rt_value == monitorcmd ) && ( rm_value == 1 ) ) {
        changeMonitor();
    }

    io_registers_values[rs_value + rt_value] = rm_value;
}

void addToledsTraceFile() {
    uint32_t time = registers_values[ timercurrent ]; 
    uint32_t leds_status = io_registers_values[ leds ];
    char leds_status_str[ LEDS_BUFFER_SIZE ]; /* TODO: check buffer size is big enough */ 
    sprintf(leds_status_str, "%d ", time);
    sprintf(leds_status_str, "%08X", leds_status);
    fputs(leds_status_str, context.led_fd);
    fputs("\r\n", context.led_fd);
}

void changeMonitor() {
    int row, col;
    uint32_t addr = io_registers_values[monitoraddr];
    
    col = addr & MASK_8_LOWER_BITS;
    row = (addr >> 8) & MASK_8_LOWER_BITS;

    /* Validate */
    if ((row < 0) || (row > MONITOR_SIZE) || (col < 0) || (col > MONITOR_SIZE) || ( addr >> 16 != 0 )) {
        printf("monitor addr is invalid!\n");
        exit(EXIT_FAILURE); /* TODO: check how to exit on failure */
    }
    else if (io_registers_values[monitordata] > 255) {
        printf("monitor data is invalid\n");
        exit(EXIT_FAILURE); /* TODO: check how to exit on failure */
    }
    monitor[row][col] = io_registers_values[monitordata];
}

void print_pixel_monitor_file(int row, int col) {
    char pixel_status_str[PIXEL_BUFFER_SIZE];
    sprintf(pixel_status_str, "%02X", monitor[row][col]);
    fputs(pixel_status_str, context.led_fd);
    fputs("\r\n", context.monitor_fd);
}

void print_pixel_monitor_yuv_file(int row, int col) { /* TODO: check */
    fwrite(&monitor[row][col], sizeof(uint8_t), 1, context.monitor_yuv_fd );
    fputs("\r\n", context.monitor_yuv_fd);
}

void write_monitor_file() {
    for (int row = 0; row < MONITOR_SIZE; row++) {
        for (int col = 0; col < MONITOR_SIZE; col++) {
            print_pixel_monitor_file(row, col);
            print_pixel_monitor_yuv_file(row, col);
        }
    }
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
    context.monitor_yuv_fd   = fopen( argv[ 14 ], "ab+" );  assert( context.monitor_yuv_fd != NULL);
}

void close_FD_context() {
    fclose(context.imemin_fd);
    fclose(context.dmemin_fd);
    fclose(context.diskin_fd);
    fclose(context.irq2in_fd);
    fclose(context.dmemout_fd);
    fclose(context.regout_fd);
    fclose(context.trace_fd);
    fclose(context.hwregtrace_fd);
    fclose(context.cycles_fd);
    fclose(context.led_fd);
    fclose(context.display7reg_fd);
    fclose(context.diskout_fd);
    fclose(context.monitor_fd);
    fclose(context.monitor_yuv_fd);
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
    close_FD_context();

    return 0;
}
