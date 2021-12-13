
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* Defines and Macros*/
#define bool                int
#define TRUE                1
#define FALSE               0
#define MAX_NUM_OF_COMMANDS 4096
#define MEM_SIZE            4096
#define NUM_OF_OPCODES      22
#define NUM_OF_REGISTERS    16
#define NUM_OF_IO_REGISTERS 22
#define CMD_LENGTH_HEX      12
#define MEM_LENGTH_HEX      8

#define SHIFT_IMM2           0
#define SHIFT_IMM1           12
#define SHIFT_RM             24
#define SHIFT_RT             28
#define SHIFT_RS             32
#define SHIFT_RD             36
#define SHIFT_OPCODE         40

#define MASK_IMM2           (uint64_t)  0xfff   <<  SHIFT_IMM2 
#define MASK_IMM1           (uint64_t)  0xfff   <<  SHIFT_IMM1
#define MASK_RM             (uint64_t)  0xf     <<  SHIFT_RM
#define MASK_RT             (uint64_t)  0xf     <<  SHIFT_RT
#define MASK_RS             (uint64_t)  0xf     <<  SHIFT_RS
#define MASK_RD             (uint64_t)  0xf     <<  SHIFT_RD
#define MASK_OPCODE         (uint64_t)  0xff    <<  SHIFT_OPCODE

#define SET_CMD_VAL(_bin_cmd, _dest, _mask, _shift)    (_dest) = ((uint64_t) ((_bin_cmd) & (_mask))) >> (_shift)

#define SET_IMM1(_bin_cmd, _cmd)                       SET_CMD_VAL((_bin_cmd), (_cmd)->Imm1,   MASK_IMM1,   SHIFT_IMM1)
#define SET_IMM2(_bin_cmd, _cmd)                       SET_CMD_VAL((_bin_cmd), (_cmd)->Imm2,   MASK_IMM2,   SHIFT_IMM2)
#define SET_RM(_bin_cmd, _cmd)                         SET_CMD_VAL((_bin_cmd), (_cmd)->RM,     MASK_RM,     SHIFT_RM)
#define SET_RT(_bin_cmd, _cmd)                         SET_CMD_VAL((_bin_cmd), (_cmd)->RT,     MASK_RT,     SHIFT_RT)
#define SET_RS(_bin_cmd, _cmd)                         SET_CMD_VAL((_bin_cmd), (_cmd)->RS,     MASK_RS,     SHIFT_RS)
#define SET_RD(_bin_cmd, _cmd)                         SET_CMD_VAL((_bin_cmd), (_cmd)->RD,     MASK_RD,     SHIFT_RD)
#define SET_OPCODE(_bin_cmd, _cmd)                     SET_CMD_VAL((_bin_cmd), (_cmd)->Opcode, MASK_OPCODE, SHIFT_OPCODE)

#define READ_REGISTERS_VALUE(_cmd, _rs_value, _rt_value, _rm_value)     _rs_value = registers_values[(_cmd)->RS];\
                                                                        _rt_value = registers_values[(_cmd)->RT];\
                                                                        _rm_value = registers_values[(_cmd)->RM];

/*  
 * TODO: 
 * parse command into parts and enter command struct
 */

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

enum io_regoster {
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
    uint32_t Imm1;
    uint32_t Imm2;
} Command;

void read_imemin_file(char *filename);
void read_dmemin_file(char *filename);
void read_diskin_file(char *filename);
void read_irq2in_file(char *filename);

void write_dmemout_file(char *filename);
void write_regout_file(char *filename);
void write_cycles_file(char *filename);
void write_leds_file(char *filename);
void write_diskout_file(char *filename);
void write_monitor_file(char *filename);
void write_monitor_yuv_file(char *filename);

void add_to_trace_file(Command *cmd, char *trace_file); /** need to add args */
void add_to_hwregtrace_file(char *filename); /** need to add args */
void add_to_display7seg_file(char *filename); /** need to add args */

void simulator();
bool call_action(Command *cmd, char *trace_file);
void parse_cmd_line(char *line, int local_pc);
bool isLineEmptyOrNoteOnly( char *line );

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
