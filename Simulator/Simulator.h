#ifndef SIMULATOR_H 
#define SIMULATOR_H 

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int pc;
uint32_t innerClks; // TODO: fix sprintf from %d
int diskTimer;

/* Defines and Macros*/
#define bool                    int
#define TRUE                    1
#define FALSE                   0
#define MAX_NUM_OF_COMMANDS     4096
#define MEM_SIZE                4096
#define NUM_OF_OPCODES          22
#define NUM_OF_REGISTERS        16
#define NUM_OF_IO_REGISTERS     23
#define CMD_LENGTH_HEX          12
#define MEM_LENGTH_HEX          8
#define BUFFER_SIZE             32 
#define PIXEL_BUFFER_SIZE       5
#define MONITOR_SIZE            256
#define MAX_LINE_LENGTH         500 
/* Disk Defines:*/
#define SECTOR_NUMBER           128
#define SECTOR_SIZE             128
#define MAX_DISK_LINE_LENGTH    8
#define DISK_OPERATION_TIME     1024
#define MAX_LINE_LENGTH         500 

#define SHIFT_IMM2              0
#define SHIFT_IMM1              12
#define SHIFT_RM                24
#define SHIFT_RT                28
#define SHIFT_RS                32
#define SHIFT_RD                36
#define SHIFT_OPCODE            40

#define MASK_IMM2               (uint64_t)  0xfff   <<  SHIFT_IMM2 
#define MASK_IMM1               (uint64_t)  0xfff   <<  SHIFT_IMM1
#define MASK_RM                 (uint64_t)  0xf     <<  SHIFT_RM
#define MASK_RT                 (uint64_t)  0xf     <<  SHIFT_RT
#define MASK_RS                 (uint64_t)  0xf     <<  SHIFT_RS
#define MASK_RD                 (uint64_t)  0xf     <<  SHIFT_RD
#define MASK_OPCODE             (uint64_t)  0xff    <<  SHIFT_OPCODE
#define MASK_12_LOWER_BITS      0xfff
#define MASK_8_LOWER_BITS       0xff

#define SET_CMD_VAL(_bin_cmd, _mask, _shift)                            ((uint64_t) ((_bin_cmd) & (_mask))) >> (_shift)

#define SET_IMM1(_bin_cmd, _cmd)                                        (_cmd)->Imm1     =  CONVERT_12_BIT_TO_32_BIT_UNSIGNED(SET_CMD_VAL((_bin_cmd), MASK_IMM1,   SHIFT_IMM1))
#define SET_IMM2(_bin_cmd, _cmd)                                        (_cmd)->Imm2     =  CONVERT_12_BIT_TO_32_BIT_UNSIGNED(SET_CMD_VAL((_bin_cmd), MASK_IMM2,   SHIFT_IMM2))
#define SET_RM(_bin_cmd, _cmd)                                          (_cmd)->RM       =  SET_CMD_VAL((_bin_cmd), MASK_RM,     SHIFT_RM)
#define SET_RT(_bin_cmd, _cmd)                                          (_cmd)->RT       =  SET_CMD_VAL((_bin_cmd), MASK_RT,     SHIFT_RT)
#define SET_RS(_bin_cmd, _cmd)                                          (_cmd)->RS       =  SET_CMD_VAL((_bin_cmd), MASK_RS,     SHIFT_RS)
#define SET_RD(_bin_cmd, _cmd)                                          (_cmd)->RD       =  SET_CMD_VAL((_bin_cmd), MASK_RD,     SHIFT_RD)
#define SET_OPCODE(_bin_cmd, _cmd)                                      (_cmd)->Opcode   =  SET_CMD_VAL((_bin_cmd), MASK_OPCODE, SHIFT_OPCODE)


#define GET_REGISTER_VALUE(_index, _dest)                               (_dest) = (_index) < NUM_OF_REGISTERS ? registers_values[(_index)] : 0;
#define SET_REGISTER_VALUE(_index, _value)                              if ((_index) < NUM_OF_REGISTERS) { registers_values[(_index)] = (_value); }
#define GET_IO_REGISTER_VALUE(_index, _dest)                            (_dest) = (_index) < NUM_OF_IO_REGISTERS ? io_registers_values[(_index)] : 0;
#define SET_IO_REGISTER_VALUE(_index, _value)                           if ((_index) < NUM_OF_IO_REGISTERS) { io_registers_values[(_index)] = (_value); }
#define SET_MEMORY_VALUE(_index, _value)                                if ((_index) < MEM_SIZE) { memory[(_index)] = (_value); }
#define GET_MEMORY_VALUE(_index, _dest)                                 (_dest) = (_index) < MEM_SIZE ? memory[(_index)] : 0;
#define SET_DISK_VALUE(_sector, _index, _value)                         if ((_sector) < SECTOR_NUMBER && (_index) < SECTOR_SIZE) { diskMemory[(_sector)][(_index)] = (_value); }
#define GET_DISK_VALUE(_sector, _index, _dest)                          (_dest) = ((_sector) < SECTOR_NUMBER && (_index) < SECTOR_SIZE) ? diskMemory[(_sector)][(_index)] : 0;
#define CONVERT_12_BIT_TO_32_BIT_UNSIGNED(x)                            (x >> 11) == 0 ? x : -1 ^ 0xFFF | x

#define READ_REGISTERS_VALUE(_cmd, _rs_value, _rt_value, _rm_value)     GET_REGISTER_VALUE((_cmd)->RS, _rs_value);\
                                                                        GET_REGISTER_VALUE((_cmd)->RT, _rt_value);\
                                                                        GET_REGISTER_VALUE((_cmd)->RM, _rm_value);

#define READ_REGISTERS_VALUE_NO_RM(_cmd, _rs_value, _rt_value)          GET_REGISTER_VALUE((_cmd)->RS, _rs_value);\
                                                                        GET_REGISTER_VALUE((_cmd)->RT, _rt_value);

/* Structs and Enums*/
enum opcode {
    op_add  = 0,
    op_sub  = 1,
    op_mac  = 2,
    op_and  = 3,
    op_or   = 4,
    op_xor  = 5,
    op_sll  = 6,
    op_sra  = 7,
    op_srl  = 8,
    op_beq  = 9,
    op_bne  = 10,
    op_blt  = 11,
    op_bgt  = 12,
    op_ble  = 13,
    op_bge  = 14,
    op_jal  = 15,
    op_lw   = 16,
    op_sw   = 17,
    op_reti = 18,
    op_in   = 19,
    op_out  = 20,
    op_halt = 21,
};

enum io_register {
    irq0enable      = 0,
    irq1enable      = 1,
    irq2enable      = 2,
    irq0status      = 3,
    irq1status      = 4,
    irq2status      = 5,
    irqhandler      = 6,
    irqreturn       = 7,
    clks            = 8,
    leds            = 9,
    display7seg     = 10,
    timerenable     = 11,
    timercurrent    = 12,
    timermax        = 13,
    diskcmd         = 14,
    disksector      = 15,
    diskbuffer      = 16,
    diskstatus      = 17,
    reserved1       = 18,
    reserved2       = 19,
    monitoraddr     = 20,
    monitordata     = 21,
    monitorcmd      = 22,
};

typedef struct {
    uint32_t PC;
    char INST[CMD_LENGTH_HEX + 1];
    uint32_t Opcode;
    uint32_t RD;
    uint32_t RS;
    uint32_t RT;
    uint32_t RM;
    int Imm1;
    int Imm2;
} Command;

typedef struct {
    FILE *imemin_fd;
    FILE *dmemin_fd;
    FILE *diskin_fd;
    FILE *irq2in_fd;
    FILE *dmemout_fd;
    FILE *regout_fd;
    FILE *trace_fd;
    FILE *hwregtrace_fd;
    FILE *cycles_fd;
    FILE *led_fd;
    FILE *display7reg_fd;
    FILE *diskout_fd;
    FILE *monitor_fd;
    FILE *monitor_yuv_fd;
} FD_Context;

void set_FD_context( char *argv[] );
void read_imemin_file();
void read_dmemin_file();

void write_dmemout_file();
void write_regout_file();
void write_cycles_file();
void write_diskout_file();
void write_monitor_file();

void add_to_trace_file(Command *cmd); /** need to add args */
void add_to_hwregtrace_file(); /** need to add args */
void add_to_display_7_seg_trace_file();
void add_to_leds_trace_file();

void simulator();
bool call_action(Command *cmd);
void parse_cmd_line(char *line, int local_pc);
bool isLineEmptyOrNoteOnly( char *line );
void changeMonitor();

void initDisk(void);
void readFromDisk(void);
void writeToDisk(void);
void diskHandler(void);

void updateInterrupts();
void updateIrq0();
void updateIrq2();
int interruptHandler();
void return_from_interrupt();
void initInterrupts();
void initIrq2();

void add (Command *cmd);
void sub (Command *cmd);
void mac (Command *cmd);
void and (Command *cmd);
void or  (Command *cmd);
void xor (Command *cmd);
void sll (Command *cmd);
void sra (Command *cmd);
void srl (Command *cmd);
void beq (Command *cmd);
void bne (Command *cmd);
void blt (Command *cmd);
void bgt (Command *cmd);
void ble (Command *cmd);
void bge (Command *cmd);
void jal (Command *cmd);
void lw  (Command *cmd);
void sw  (Command *cmd);
void reti(Command *cmd);
void in  (Command *cmd);
void out (Command *cmd);

#endif
