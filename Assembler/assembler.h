
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#define _POSIX_C_SOURCE 200809L

/* Defines and Macros*/
#define bool                int
#define TRUE                1
#define FALSE               0
#define DB_NUM_COLS         2
#define DB_MAX_NUM_LABLES   128
#define BUFFER_MAX_SIZE     1024
#define MAX_NUM_OF_COMMANDS 500
#define OPCODE_INDEX        1
#define RD_INDEX            2
#define RS_INDEX            3
#define RT_INDEX            4
#define RM_INDEX            5
#define IMM1_INDEX          6
#define IMM2_INDEX          7
#define NUM_OF_CMD_FIELDS   7
#define NUM_OF_OPCODES      21
#define NUM_OF_REGISTERS    15

#define MASK_IMM1           0xfff
#define MASK_IMM2           0xfff   >>  12 
#define MASK_RM             0xf     >>  24
#define MASK_RT             0xf     >>  28
#define MASK_RS             0xf     >>  32
#define MASK_RD             0xf     >>  36
#define MASK_OPCODE         0xff    >>  40


/* 
 * TODO: 
 * parse command into parts and enter command struct
 */

/* Structs and Enums*/
typedef char* Register;
typedef struct {
    int PC;
    char *Opcode;
    char *RD;
    char *RS;
    char *RT;
    char *RM;
    char *Imm1;
    char *Imm2;
    char *Lable;
    char *Note;
} Command;

typedef struct {
    char *Lable_name;
    int PC;
} Lable;

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
    "$sp",    /* 13 */
    "$gp",    /* 14 */
    "$ra"     /* 15 */
};

/* Debug functions */
void printCMD( Command *CMD );
void printReg( Register reg );
void hex2dec();
void dec2hex();

/* Assembler Input/Output */
void setCommand( Command *CMD, char **CMDArg );
void setLable2PCDB( FILE file );
void parseLine( Command *CMD, char *line, int *pc );
void print_imemin( void );
void print_dmemin( void );


/* Assembler Commands - move to simulator */
int setBinaryCommand( Command cmd );
