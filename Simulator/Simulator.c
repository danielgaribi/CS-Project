#include "Simulator.h"

int registers_values[NUM_OF_REGISTERS] = { 0 };
uint32_t io_registers_values[NUM_OF_IO_REGISTERS] = { 0 };
int memory[MEM_SIZE] = { 0 };
uint8_t  monitor[MONITOR_SIZE][MONITOR_SIZE] = { 0 };
Command* commands[MAX_NUM_OF_COMMANDS] = { NULL };
FD_Context context = { 0 };
int diskMemory[SECTOR_NUMBER][SECTOR_SIZE];

/* interrupts */
int isInterrupt;
int* irq2Arr;
int irq2Counter;
int irq2ArrLen;

char* io_registers_names[] = {
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


/*########################################################################################################################################################*/
/* Main Simulator */

void simulator() {
    pc = 0;
    io_registers_values[clks] = 0;

    for (; TRUE; io_registers_values[clks]++, innerClks++, pc++) {
        registers_values[0] = 0; /* set $zero to 0 */
        interruptHandler();

        if (commands[pc] == NULL) { /* TODO: remove???? */
            assert(FALSE);
            break;
        }

        if (call_action(commands[pc]) == FALSE) {
            io_registers_values[clks]++;
            innerClks++;
            break;
        }

        diskHandler();
        updateInterrupts();
    }
}

bool call_action(Command *cmd) {
    registers_values[1] = cmd->Imm1;
    registers_values[2] = cmd->Imm2;
    add_to_trace_file(cmd);
    switch (cmd->Opcode)
    {
        case op_add:
            add(cmd);
            break;
        case op_sub:
            sub(cmd);
            break;
        case op_mac:
            mac(cmd);
            break;
        case op_and:
            and (cmd);
            break;
        case op_or:
            or (cmd);
            break;
        case op_xor:
            xor (cmd);
            break;
        case op_sll:
            sll(cmd);
            break;
        case op_sra:
            sra(cmd);
            break;
        case op_srl:
            srl(cmd);
            break;
        case op_beq:
            beq(cmd);
            break;
        case op_bne:
            bne(cmd);
            break;
        case op_blt:
            blt(cmd);
            break;
        case op_bgt:
            bgt(cmd);
            break;
        case op_ble:
            ble(cmd);
            break;
        case op_bge:
            bge(cmd);
            break;
        case op_jal:
            jal(cmd);
            break;
        case op_lw:
            lw(cmd);
            break;
        case op_sw:
            sw(cmd);
            break;
        case op_reti:
            reti(cmd);
            break;
        case op_in:
            in(cmd);
            add_to_hwregtrace_file(cmd);
            break;
        case op_out:
            out(cmd);
            add_to_hwregtrace_file(cmd);
            break;
        case op_halt:
            return FALSE;
        default:
            assert(FALSE);
            return FALSE;
    }
    return TRUE;
}

/* opcode actions: */

void add(Command *cmd) {
    int rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    registers_values[cmd->RD] = rs_value + rt_value + rm_value;
}

void sub(Command *cmd) {
    int rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    registers_values[cmd->RD] = rs_value - rt_value - rm_value;
}

void mac(Command *cmd) {
    int rs_value, rt_value, rm_value;
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
    int rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    if (rs_value == rt_value) {
        pc = (rm_value & MASK_12_LOWER_BITS) - 1; /** pc will increment in main loop by one */
    }
}

void bne(Command *cmd) {
    int rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    if (rs_value != rt_value) {
        pc = (rm_value & MASK_12_LOWER_BITS) - 1; /** pc will increment in main loop by one */
    }
}

void blt(Command *cmd) {
    int rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    if (rs_value < rt_value) {
        pc = (rm_value & MASK_12_LOWER_BITS) - 1; /** pc will increment in main loop by one */
    }
}

void bgt(Command *cmd) {
    int rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    if (rs_value > rt_value) {
        pc = (rm_value & MASK_12_LOWER_BITS) - 1; /** pc will increment in main loop by one */
    }
}

void ble(Command *cmd) {
    int rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    if (rs_value <= rt_value) {
        pc = (rm_value & MASK_12_LOWER_BITS) - 1; /** pc will increment in main loop by one */
    }
}

void bge(Command *cmd) {
    int rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);
    if (rs_value >= rt_value) {
        pc = (rm_value & MASK_12_LOWER_BITS) - 1; /** pc will increment in main loop by one */
    }
}

void jal(Command *cmd) {
    int rm_value = registers_values[cmd->RM];
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
    pc = io_registers_values[irqreturn]; /** pc will increment in main loop by one */
    return_from_interrupt();
}

void in(Command *cmd) {
    uint32_t rs_value, rt_value;
    READ_REGISTERS_VALUE_NO_RM(cmd, rs_value, rt_value);
    
    if (rs_value + rt_value == monitorcmd) {
        registers_values[cmd->RD] = 0; /* Reading from monitorcmd register return 0 */
    }
    else {
        registers_values[cmd->RD] = io_registers_values[rs_value + rt_value];
    }
    
}

void out(Command *cmd) {
    int is_changed = FALSE;
    uint32_t rs_value, rt_value, rm_value;
    READ_REGISTERS_VALUE(cmd, rs_value, rt_value, rm_value);

    /* If command set monitorcmd to 1, change monitor status */
    if ( (rs_value + rt_value == monitorcmd ) && ( rm_value == 1 ) ) {
        changeMonitor();
    }

    is_changed = io_registers_values[rs_value + rt_value] != rm_value;
    io_registers_values[rs_value + rt_value] = rm_value;

    if ((rs_value + rt_value == display7seg) && is_changed ) {
        add_to_display_7_seg_trace_file();
    }
    /* If command changes leds status, append to ledsFile */
    else if ((rs_value + rt_value == leds) && is_changed) {
        add_to_leds_trace_file();
    }
}

/*########################################################################################################################################################*/
/* Monitor */

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
    sprintf_s(pixel_status_str, PIXEL_BUFFER_SIZE, "%02X", monitor[row][col]);
    fputs(pixel_status_str, context.monitor_fd);
    fputs("\r", context.monitor_fd);
}

void print_pixel_monitor_yuv_file(int row, int col) { /* TODO: check */
    uint8_t pixel = monitor[row][col];
    fwrite(&pixel, sizeof(uint8_t), 1, context.monitor_yuv_fd);
}

void write_monitor_file() {
    for (int row = 0; row < MONITOR_SIZE; row++) {
        for (int col = 0; col < MONITOR_SIZE; col++) {
            print_pixel_monitor_file(row, col);
            print_pixel_monitor_yuv_file(row, col);
        }
    }
}

void add_to_leds_trace_file() {
    uint32_t leds_status = io_registers_values[leds];
    char leds_status_str[BUFFER_SIZE];
    sprintf_s(leds_status_str, BUFFER_SIZE, "%d %08x", innerClks, leds_status);
    fputs(leds_status_str, context.led_fd);
    fputs("\r", context.led_fd);
}

void add_to_display_7_seg_trace_file() {
    uint32_t display7Seg_status = io_registers_values[display7seg];
    char disp7seg_status_str[BUFFER_SIZE];
    sprintf_s(disp7seg_status_str, BUFFER_SIZE, "%d %08x", innerClks, display7Seg_status);
    fputs(disp7seg_status_str, context.display7reg_fd);
    fputs("\r", context.display7reg_fd);
}

/*########################################################################################################################################################*/
/* Interrupt: */

void updateInterrupts() {
    updateIrq0();
    updateIrq2();
}

void updateIrq0() {
    if (io_registers_values[timerenable]) {
        io_registers_values[timercurrent]++;
        if (io_registers_values[timercurrent] >= io_registers_values[timermax]) {
            io_registers_values[irq0status] = 1;
            io_registers_values[timercurrent] = 0;
        }
    }
}

void updateIrq2() {
    if (irq2Counter < irq2ArrLen) {
        if (io_registers_values[clks] == irq2Arr[irq2Counter]) {
            io_registers_values[irq2status] = 1;
            irq2Counter++;
        }
    }
}

int interruptHandler()
{
    int irq = (io_registers_values[irq0enable] && io_registers_values[irq0status]) ||
        (io_registers_values[irq1enable] && io_registers_values[irq1status]) ||
        (io_registers_values[irq2enable] && io_registers_values[irq2status]);
    if (irq) {
        if (isInterrupt) {
            return FALSE;
        }
        io_registers_values[irqreturn] = pc;
        pc = io_registers_values[irqhandler];
        isInterrupt = 1;
        return TRUE;
    }
    return FALSE;
}

void return_from_interrupt() {
    isInterrupt = 0;
    if (!interruptHandler()) {
        pc--;
    }
}

void initInterrupts()
{
    isInterrupt = 0;
    initIrq2();
}

void initIrq2()
{
    char line[MAX_LINE_LENGTH];
    irq2ArrLen = 0;
    irq2Counter = 0;
    while (fgets(line, 500, context.irq2in_fd) != NULL)
    {
        irq2ArrLen++;
    }

    if (irq2ArrLen == 0) {
        return;
    }
    rewind(context.irq2in_fd);
    irq2Arr = calloc(irq2ArrLen, sizeof(int));

    int index = 0;
    while (fgets(line, 500, context.irq2in_fd) != NULL)
    {
        irq2Arr[index] = atoi(line);
        index++;
    }
    fclose(context.irq2in_fd);
}

/*########################################################################################################################################################*/
/* Disk: */

void initDisk() {
    diskTimer = 0;
    char line[MAX_DISK_LINE_LENGTH + 2];
    int lineIndex = 0;
    int sector, sectorIndex;
    memset(diskMemory, 0, sizeof(diskMemory));

    while (fgets(line, MAX_LINE_LENGTH + 2, context.diskin_fd) != NULL)
    {
        if (isLineEmptyOrNoteOnly(line) == FALSE) {
            continue;
        }
        sector = lineIndex / SECTOR_SIZE;
        sectorIndex = lineIndex % SECTOR_SIZE;
        if (sector < SECTOR_NUMBER && sectorIndex < SECTOR_SIZE) {
            diskMemory[sector][sectorIndex] = (int) strtoul(line, NULL, 16);
            lineIndex++;
        }
        else {
            break;
        }
    }
    fclose(context.diskin_fd);
}

void readFromDisk() {
    int sectorIndex;
    int sector = io_registers_values[disksector];
    int memoryPointer = io_registers_values[diskbuffer];
    for (sectorIndex = 0; sectorIndex < SECTOR_SIZE; sectorIndex++) {
        memory[memoryPointer + sectorIndex] = diskMemory[sector][sectorIndex];
    }
}

void writeToDisk() {
    int sectorIndex;
    int sector = io_registers_values[disksector];
    int memoryPointer = io_registers_values[diskbuffer];
    for (sectorIndex = 0; sectorIndex < SECTOR_SIZE; sectorIndex++) {
        diskMemory[sector][sectorIndex] = memory[memoryPointer + sectorIndex];
    }
}

void diskHandler() {

    if (io_registers_values[diskcmd] != 0) {
        io_registers_values[diskstatus] = 1;

        if (diskTimer < DISK_OPERATION_TIME) {
            diskTimer++;
            return;
        }
        else {
            io_registers_values[diskcmd] = 0;
            io_registers_values[diskstatus] = 0;
            io_registers_values[irq1status] = 1;
        }
        if (io_registers_values[diskcmd] == 1) {
            readFromDisk();
        }
        if (io_registers_values[diskcmd] == 2) {
            writeToDisk();
        }
        diskTimer = 0;
    }
}

void write_diskout_file() {
    int zero_counter = 0;
    int j, sector, sectorIndex;
    char word[CMD_LENGTH_HEX + 1];
    for (sector = 0; sector < SECTOR_NUMBER; sector++) {
        for (sectorIndex = 0; sectorIndex < SECTOR_SIZE; sectorIndex++) {
            if (diskMemory[sector][sectorIndex] == 0) {
                zero_counter++;
                continue;
            }
            for (j = 0; j < zero_counter; j++) {
                sprintf_s(word, CMD_LENGTH_HEX + 1, "%08X", diskMemory[sector][sectorIndex]);
                fputs(word, context.diskout_fd);
                fputs("\r", context.diskout_fd);
            }
            sprintf_s(word, CMD_LENGTH_HEX + 1, "%08X", diskMemory[sector][sectorIndex]);
            fputs(word, context.diskout_fd);
            fputs("\r", context.diskout_fd);
            zero_counter = 0;
        }
    }
}

/*########################################################################################################################################################*/
/* Input/Outout Files */

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
    bin_cmd = (uint64_t) strtoll(line, NULL, 16);
    cmd = (Command*) malloc(sizeof(Command));
    assert( cmd != NULL);

    cmd->PC = local_pc;
    strcpy_s(cmd->INST, CMD_LENGTH_HEX + 1, line);
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
        memory[loc] = (int) strtoul(line, NULL, 16);
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

void write_regout_file() {
    int i;
    char word[CMD_LENGTH_HEX + 1];
    for (i = 3; i < NUM_OF_REGISTERS; i++) {
        sprintf_s(word, CMD_LENGTH_HEX + 1, "%08X", registers_values[i]);
        fputs(word, context.regout_fd);
        fputs("\r", context.regout_fd);
    }
}

void write_dmemout_file() {
    int zero_counter = 0;
    int i, j;
    char word[CMD_LENGTH_HEX + 1];
    for (i = 0; i < MEM_SIZE; i++) {
        if (memory[i] == 0) {
            zero_counter++;
            continue;
        }
        for (j = 0; j < zero_counter; j++) {
            sprintf_s(word, CMD_LENGTH_HEX + 1, "%08X", 0);
            fputs(word, context.dmemout_fd);
            fputs("\r", context.dmemout_fd);
        }
        sprintf_s(word, CMD_LENGTH_HEX + 1, "%08X", memory[i]);
        fputs(word, context.dmemout_fd);
        fputs("\r", context.dmemout_fd);
        zero_counter = 0;
    }
}

void write_cycles_file() {
    char word[CMD_LENGTH_HEX + 1];
    sprintf_s(word, CMD_LENGTH_HEX + 1, "%d", innerClks);
    fputs(word, context.cycles_fd);
    fputs("\r", context.cycles_fd);
}

void add_to_hwregtrace_file(Command* cmd) {
    int index, val;
    char line[4 * CMD_LENGTH_HEX + 4];
    char* mode;
    if (cmd->Opcode == op_out) {
        mode = "WRITE";
    }
    else {
        mode = "READ";
    }
    index = registers_values[cmd->RS] + registers_values[cmd->RT];
    val = io_registers_values[index];

    /* TODO: before change
    if(cmd->Opcode == op_in){
        val = io_registers_values[cmd->RD];
    } else{
        val = io_registers_values[index];
    }
    */

    sprintf_s(line, 4 * CMD_LENGTH_HEX + 4, "%d %s %s %08x",
        innerClks, mode, io_registers_names[index], val);
    fputs(line, context.hwregtrace_fd);
    fputs("\r", context.hwregtrace_fd);
}

void add_to_trace_file(Command* cmd) {
    int i;
    char word[2 * CMD_LENGTH_HEX + 2];
    sprintf_s(word, 2 * CMD_LENGTH_HEX + 2, "%03X %s", cmd->PC, cmd->INST);
    fputs(word, context.trace_fd);

    for (i = 0; i < NUM_OF_REGISTERS; i++) {
        sprintf_s(word, 2 * CMD_LENGTH_HEX + 2, " %08x", registers_values[i]);
        fputs(word, context.trace_fd);
    }
    fputs("\r", context.trace_fd);
}

void set_FD_context(char* argv[]) {
    assert(fopen_s(&(context.imemin_fd), argv[1], "r") == 0);      assert(context.imemin_fd != NULL);
    assert(fopen_s(&(context.dmemin_fd), argv[2], "r") == 0);      assert(context.dmemin_fd != NULL);
    assert(fopen_s(&(context.diskin_fd), argv[3], "r") == 0);      assert(context.diskin_fd != NULL);
    assert(fopen_s(&(context.irq2in_fd), argv[4], "r") == 0);      assert(context.irq2in_fd != NULL);
    assert(fopen_s(&(context.dmemout_fd), argv[5], "w+") == 0);     assert(context.dmemout_fd != NULL);
    assert(fopen_s(&(context.regout_fd), argv[6], "w+") == 0);     assert(context.regout_fd != NULL);
    assert(fopen_s(&(context.trace_fd), argv[7], "w+") == 0);     assert(context.trace_fd != NULL);
    assert(fopen_s(&(context.hwregtrace_fd), argv[8], "w+") == 0);     assert(context.hwregtrace_fd != NULL);
    assert(fopen_s(&(context.cycles_fd), argv[9], "w+") == 0);     assert(context.cycles_fd != NULL);
    assert(fopen_s(&(context.led_fd), argv[10], "w+") == 0);    assert(context.led_fd != NULL);
    assert(fopen_s(&(context.display7reg_fd), argv[11], "w+") == 0);    assert(context.display7reg_fd != NULL);
    assert(fopen_s(&(context.diskout_fd), argv[12], "w+") == 0);    assert(context.diskout_fd != NULL);
    assert(fopen_s(&(context.monitor_fd), argv[13], "w+") == 0);    assert(context.monitor_fd != NULL);
    assert(fopen_s(&(context.monitor_yuv_fd), argv[14], "wb+") == 0);   assert(context.monitor_yuv_fd != NULL);
}

void close_FD_context() {
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

/*########################################################################################################################################################*/
/* Main */

int main( int argc, char *argv[] ) {
    assert( argc == 15 );
    set_FD_context( argv );

    /* get fd for files from sturct FD_context */
    read_imemin_file();
    read_dmemin_file();
    initInterrupts();
    initDisk();

    simulator();

    write_cycles_file();
    write_regout_file();
    write_dmemout_file();
    write_diskout_file();
    write_monitor_file();

    close_FD_context();

    return 0;
}