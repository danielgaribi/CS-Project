
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define _POSIX_C_SOURCE 200809L

/* Defines and Macros*/
#define bool                int
#define TRUE                1
#define FALSE               0
#define DB_NUM_COLS         2
#define DB_MAX_NUM_LABLES   128
#define BUFFER_MAX_SIZE     1024
#define MAX_NUM_OF_COMMANDS 4096
#define OPCODE_INDEX        1
#define RD_INDEX            2
#define RS_INDEX            3
#define RT_INDEX            4
#define RM_INDEX            5
#define IMM1_INDEX          6
#define IMM2_INDEX          7
#define NUM_OF_CMD_FIELDS   7
#define NUM_OF_OPCODES      22
#define NUM_OF_REGISTERS    16
#define CMD_LENGTH_HEX      12
#define MEM_LENGTH_HEX      8

#define SHIFT_IMM2           0
#define SHIFT_IMM1           12
#define SHIFT_RM             24
#define SHIFT_RT             28
#define SHIFT_RS             32
#define SHIFT_RD             36
#define SHIFT_OPCODE         40

#define MASK_IMM2           (uint64_t)  0xfff
#define MASK_IMM1           (uint64_t)  0xfff
#define MASK_RM             (uint64_t)  0xf
#define MASK_RT             (uint64_t)  0xf
#define MASK_RS             (uint64_t)  0xf
#define MASK_RD             (uint64_t)  0xf
#define MASK_OPCODE         (uint64_t)  0xff

#define SET_CMD_COMP(_cmd, _comp, _mask, _shift)    (_cmd) = ((_cmd) & (~((_mask) << (_shift)))) | ((uint64_t) ((_comp) & (_mask))<< (_shift))

#define SET_IMM1(_cmd, _imm1)                       SET_CMD_COMP((_cmd), (_imm1),   MASK_IMM1,  SHIFT_IMM1)
#define SET_IMM2(_cmd, _imm2)                       SET_CMD_COMP((_cmd), (_imm2),   MASK_IMM2,  SHIFT_IMM2)
#define SET_RM(_cmd, _rm)                           SET_CMD_COMP((_cmd), (_rm),     MASK_RM,  SHIFT_RM)
#define SET_RT(_cmd, _rt)                           SET_CMD_COMP((_cmd), (_rt),     MASK_RT,  SHIFT_RT)
#define SET_RS(_cmd, _rs)                           SET_CMD_COMP((_cmd), (_rs),     MASK_RS,  SHIFT_RS)
#define SET_RD(_cmd, _rd)                           SET_CMD_COMP((_cmd), (_rd),     MASK_RD,  SHIFT_RD)
#define SET_OPCODE(_cmd, _opcode)                   SET_CMD_COMP((_cmd), (_opcode), MASK_OPCODE,  SHIFT_OPCODE)

/* Structs and Enums*/
typedef char* Register;
typedef struct {
    int PC;
    int Opcode;
    int RD;
    int RS;
    int RT;
    int RM;
    char Imm1[ BUFFER_MAX_SIZE ];
    char Imm2[ BUFFER_MAX_SIZE ];
    char Lable[ BUFFER_MAX_SIZE ];
    char Note[ BUFFER_MAX_SIZE ];
} Command;

typedef struct {
    char Lable_name[ BUFFER_MAX_SIZE ];
    int PC;
    int is_created;
} Lable;

typedef struct {
    int location;
    char value[ BUFFER_MAX_SIZE ];
} Memory;

char *opcodes[] = { 
    "add",  /* 0 */
    "sub",  /* 1 */
    "mac",  /* 2 */
    "and",  /* 3 */
    "or",   /* 4 */
    "xor",  /* 5 */
    "sll",  /* 6 */
    "sra",  /* 7 */
    "srl",  /* 8 */
    "beq",  /* 9 */
    "bne",  /* 10 */
    "blt",  /* 11 */
    "bgt",  /* 12 */
    "ble",  /* 13 */
    "bge",  /* 14 */
    "jal",  /* 15 */
    "lw",   /* 16 */ 
    "sw",   /* 17 */ 
    "reti", /* 18 */
    "in",   /* 19 */
    "out",  /* 20 */ 
    "halt"  /* 21 */
};

char *registers[] = { 
    "$zero",  /* 0 */
    "$imm1",  /* 1 */
    "$imm2",  /* 2 */
    "$v0",    /* 3 */
    "$a0",    /* 4 */
    "$a1",    /* 5 */
    "$a2",    /* 6 */
    "$t0",    /* 7 */
    "$t1",    /* 8 */
    "$t2",    /* 9 */
    "$s0",    /* 10 */
    "$s1",    /* 11 */
    "$s2",    /* 12 */
    "$gp",    /* 13 */
    "$sp",    /* 14 */
    "$ra"     /* 15 */
};

/* Assembler Input/Output */
void setCommand( int pc, char **CMDArg );
void parseLine( char *line );
char* cleanWord(char* word);
char* cleanStartWord(char* word);
char* cleanEndWord(char* word);
bool isLineEmptyOrNoteOnly( char *line );
bool classifiedCMD( char *line, bool *isLableFound, bool *isNoteFound, bool *isDotWordFound );
void setLable( int pc, char *lableName );
void setMemory( char *line );
void print_imemin( char *imemin_file );
void print_dmemin( char *dmemin_file );
uint64_t parse_cmd_to_uint64_t( Command cmd );
int convert_imm_to_int(char *imm);
void get_memory_array(int **memory, int *size);
