
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

#define MASK_IMM1           0xfff
#define MASK_IMM2           0xfff   >>  12 
#define MASK_RM             0xf     >>  24
#define MASK_RT             0xf     >>  28
#define MASK_RS             0xf     >>  32
#define MASK_RD             0xf     >>  36
#define MASK_OPCODE         0xff    >>  40

/* Structs and Enums*/
enum reg_type { Zero, Imm, Result, Arg, Tmp, Save, GP, SP, RA };
enum opcode { 
    add  =   0, 
    sub  =   1, 
    mac  =   2, 
    and  =   3,
    or   =   4,
    xor  =   5, 
    sll  =   6, 
    sra  =   7, 
    srl  =   8, 
    beq  =   9, 
    bne  =   10, 
    blt  =   11, 
    bgt  =   12, 
    ble  =   13, 
    bge  =   14, 
    jal  =   15, 
    lw   =   16, 
    sw   =   17, 
    reti =   18, 
    in   =   19, 
    out  =   20, 
    halt =   21
};

typedef struct {
    int reg_number;
    enum reg_type reg_type;
    char *reg_name;
} Register;

typedef struct {
    int PC;
    int Opcode;
    int RD;
    int RS;
    int RT;
    int RM;
    int Imm1;
    int Imm2;
    char *Lable;
    char *Note;
} Command;

typedef struct {
    char *Lable_name;
    int PC;
} Lable;

/* Debug functions */
void printCMD( Command *CMD );
void printReg( Register reg );
void hex2dec();
void dec2hex();

/* Assembler Input/Output */
void setCommand( Command *CMD, int cmdPart, char *CMDArg );
void setLable2PCDB( FILE file );
void parseLine( Command *CMD, char *line, int *pc );
void print_imemin( void );
void print_dmemin( void );


/* Assembler Commands - move to simulator */
int setBinaryCommand( Command cmd );
/*
void add( Command cmd );
void sub( Command cmd );
void mac( Command cmd );
void and( Command cmd );
void or( Command cmd );
void xor( Command cmd );
void sll( Command cmd );
void sra( Command cmd );
void srl( Command cmd );
void beq( Command cmd );
void bne( Command cmd );
void blt( Command cmd );
void bgt( Command cmd );
void ble( Command cmd );
void bge( Command cmd );
void jal( Command cmd );
void lw( Command cmd );
void sw( Command cmd );
void reti( Command cmd );
void in( Command cmd );
void out( Command cmd );
void halt( Command cmd );
*/




