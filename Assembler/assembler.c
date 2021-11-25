
#include "assembler.h"

static Lable Lable2PC[ DB_MAX_NUM_LABLES ] = { 0 };
static int lableIndex = 0;
static Command *CommandDB[ MAX_NUM_OF_COMMANDS ] = { 0 };
static int cmdIndex = 0;

void setCommand( Command *CMD, int cmdPart, char *CMDArg ) {
    cmdPart = 0;
    CMDArg = 0;
    /* TODO: Parse from string to Registers, opcodes and so on .. */
    /* 
    CMD->PC = 0;
    Register newRegister = { 0 };
    
    switch ( cmdPart ) {
        case OPCODE_INDEX:
            opcode newOP;
            strcpy( CMD->opcode, CMDArg );
            break;
        case RD_INDEX:
            CMD->RD;
            break;
        case RS_INDEX:
            strcpy( CMD->RS, CMDArg );
            break;
        case RT_INDEX:
            strcpy( CMD->RT, CMDArg );
            break;
        case RM_INDEX:
            strcpy( CMD->RM, CMDArg );
            break;
        case IMM1_INDEX:
            strcpy( CMD->Imm1, CMDArg );
            break;
        case IMM2_INDEX:
            strcpy( CMD->Imm2, CMDArg );
            break;
        default:
            printf("ERROR: too many arguments!\n");
            exit( 1 ); // How to exit in case of error?
    }
    */
}

void printCMD( Command *CMD ) { /* TODO: debug */
    printf("#######################\n");
    printf("PC: %d\n", CMD->PC );
    printf("Opcode: %d\n", CMD->Opcode );
    printf("RD: %d\n", CMD->RD );
    printf("RS: %d\n", CMD->RS );
    printf("RT: %d\n", CMD->RT );
    printf("RM: %d\n", CMD->RM );
    printf("Imm1: %d\n", CMD->Imm1 );
    printf("Imm2: %d\n", CMD->Imm2 );
    printf("Lable: %s\n", CMD->Lable );
    printf("Note: %s\n", CMD->Note );
    printf("#######################\n");
}

void parseLine( Command *CMD, char *line, int *PC ) { /* TODO: try again */
    char *lableName, *fullCommand, *note, *token, *token2, *partialCommand;
    int cmdPartIndex;
    
    /* Split line into lable+command and note */
    token = strtok( line, "#");
    if ( token == NULL ) { 
        /* empty line or line with comment only - ignore and contine to next line */
        return;
    }
    printf("token: %s\n", token); /* TODO: debug */
    note = strtok( NULL, "#" );
    printf("note: %s\n", note); /* TODO: debug */
    if ( note != NULL ) { /* line with comment */
        /* TODO: debug why crush 
        CMD->Note = note;
        printf("CMD.note: %s\nnote: %s\n", CMD->Note, note); // TODO: debug 
        */
    }

    /* Split lable+command to 2 seperated parts */
    lableName = strtok( token, ":" );
    printf("lableName: %s\n", lableName); /* TODO: debug */
    fullCommand = strtok( NULL, ":" );
    printf("fullCommand before if: %s\n", fullCommand); /* TODO: debug */
    if ( fullCommand != NULL ) { /* Lable found */
        Lable newLable = { 0 };
        newLable.Lable_name = lableName;
        newLable.PC = *PC;
        Lable2PC[ lableIndex ++ ];
    } else { /* Lable not found, set command into fullCommand */
        fullCommand = lableName;
    }
    printf("fullCommand after if: %s\n", fullCommand); /* TODO: debug */

    /* Split command into opcode, registers, imms... */
    cmdPartIndex = 2;
    token = strtok( fullCommand, "," );
    printf("opcode and first reg: %s\n", token); /* TODO: debug */
    token2 = strtok( NULL, "," );
    printf("token2: %s\tcmdPartIndex: %d\n", token2, cmdPartIndex ); /* TODO: debug */
    while ( token2 != NULL ) { /* Set the rest of command */
        setCommand( CMD, cmdPartIndex++, token2 );
        printf("token2: %s\tcmdPartIndex: %d\n", token2, cmdPartIndex ); /* TODO: debug */
        token2 = strtok( NULL, "," );
    }
    token2 = strtok( token, " ");
    printf("opcode: %s\n", token2 ); /* TODO: debug */
    setCommand( CMD, OPCODE_INDEX, token2 ); /* Set opcode */
    token2 = strtok( NULL, ","); /* Set first register */
    printf("first reg: %s\n", token2 ); /* TODO: debug */
    setCommand( CMD, RD_INDEX, token2 ); /* Set opcode */

    /*CMD->PC = *PC;*/ /* TODO: debug why crush */
    CommandDB[ cmdIndex++ ] = CMD;
    /* printCMD( CMD ); */
}

int main( int argc, char *argv[] ) {
    /* Read program.asm (program file) - find labels's pc and update Lable2PC DB */
    assert( argc == 2 );

    FILE *file = fopen( argv[1], "r");
    assert( file != NULL );

    char line[ BUFFER_MAX_SIZE ];
    int PC = 0; /* starting from 0 or 1? */
    while ( fgets( line, BUFFER_MAX_SIZE, file ) != NULL ) {
        printf("line: %s\n", line );
        Command *CMD = { 0 };
        parseLine( CMD, line, &PC );
    }    
    fclose( file );

    /* Replace Lables with the correct pc */

    return 0;
}
