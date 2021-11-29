
#include "assembler.h"

static Lable Lable2PC[ DB_MAX_NUM_LABLES ] = { 0 };
static int lableIndex = 0;
static Command *CommandDB[ MAX_NUM_OF_COMMANDS ] = { 0 };
static int cmdIndex = 0;

void setCommand( Command *CMD, char **CMDArgs ) {
    for (int cmdPart = 1; cmdPart <= NUM_OF_CMD_FIELDS; cmdPart++) {
        char *cleanCMDArg = strtok( CMDArgs[ cmdPart ], " \t");

        switch ( cmdPart ) {
        case OPCODE_INDEX:
            for ( int i = 0; i < NUM_OF_OPCODES; i++) {
                if ( strcmp( cleanCMDArg, opcodes[ i ]) == 0 ) {
                    CMD->Opcode = cleanCMDArg;
                    printf("in switch - OPCODE: %s\n", CMD->Opcode); /* TODO: debug */
                    break;
                }
            }
            break;
        case RD_INDEX:
            for ( int i = 0; i < NUM_OF_REGISTERS; i++ ) {
                if ( strcmp( cleanCMDArg, registers[ i ]) == 0 ) {
                    CMD->RD = cleanCMDArg;
                    printf("in switch - RD: %s\n", CMD->RD); /* TODO: debug */
                    break;
                }
            }
            break;
        case RS_INDEX:
            for ( int i = 0; i < NUM_OF_REGISTERS; i++ ) {
                if ( strcmp( cleanCMDArg, registers[ i ]) == 0 ) {
                    CMD->RS = cleanCMDArg;
                    printf("in switch - RS: %s\n", CMD->RS); /* TODO: debug */
                    break;
                }
            }
            break;
        case RT_INDEX:
            for ( int i = 0; i < NUM_OF_REGISTERS; i++ ) {
                if ( strcmp( cleanCMDArg, registers[ i ]) == 0 ) {
                    CMD->RT = cleanCMDArg;
                    printf("in switch - RT: %s\n", CMD->RT); /* TODO: debug */
                    break;
                }
            }
            break;
        case RM_INDEX:
            for ( int i = 0; i < NUM_OF_REGISTERS; i++ ) {
                if ( strcmp( cleanCMDArg, registers[ i ]) == 0 ) {
                    CMD->RM = cleanCMDArg;
                    printf("in switch - RM: %s\n", CMD->RM); /* TODO: debug */
                    break;
                }
            }
            break;
        case IMM1_INDEX: // keep in string and handle in second round 
            CMD->Imm1 = cleanCMDArg;
            printf("in switch - Imm1: %s\n", CMD->Imm1); /* TODO: debug */
            break;
        case IMM2_INDEX:
            CMD->Imm2 = cleanCMDArg;
            printf("in switch - Imm2: %s\n", CMD->Imm2); /* TODO: debug */
            break;
        default:
            printf("ERROR: too many arguments!\n");
            exit( 1 ); // How to exit in case of error?
    }

    }
}

void printCMD( Command *CMD ) { /* TODO: debug */
    printf("#######################\n");
    printf("PC: %d\n", CMD->PC );
    printf("Opcode: %s\n", CMD->Opcode );
    printf("RD: %s\n", CMD->RD );
    printf("RS: %s\n", CMD->RS );
    printf("RT: %s\n", CMD->RT );
    printf("RM: %s\n", CMD->RM );
    printf("Imm1: %s\n", CMD->Imm1 );
    printf("Imm2: %s\n", CMD->Imm2 );
    printf("Lable: %s\n", CMD->Lable );
    printf("Note: %s\n", CMD->Note );
    printf("#######################\n");
}

void parseLine( Command *CMD, char *line, int *PC ) { /* TODO: try again */
    char *lableName, *fullCommand, *note, *token, *token2;
    char *commandStrings[ NUM_OF_CMD_FIELDS ] = { 0 };
    int cmdPartIndex;
    
    /* Split line into lable+command and note */
    token = strtok( line, "#");
    if ( token == NULL ) { 
        /* empty line or line with comment only - ignore and contine to next line */
        return;
    }
    note = strtok( NULL, "#" );
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
    
    if ( fullCommand != NULL ) { 
        Lable newLable = { 0 };
        newLable.Lable_name = lableName;
        newLable.PC = *PC;
        Lable2PC[ lableIndex ++ ];
    } else {
        fullCommand = lableName;
    }
    printf("fullCommand after if: %s\n", fullCommand); /* TODO: debug */

    /* Split command into opcode, registers, imms... */
    token = strtok( fullCommand, "," );

    cmdPartIndex = 3;
    token2 = strtok( NULL, "," );
    while ( token2 != NULL ) { /* Set the rest of command */
        commandStrings[ cmdPartIndex++ ] = token2;
        token2 = strtok( NULL, "," );
    }
    token2 = strtok( token, " ");
    commandStrings[ OPCODE_INDEX ] = token2; /* Set OPCODE register */
    token2 = strtok( NULL, ","); 
    commandStrings[ RD_INDEX ] = token2; /* Set RD register */
    setCommand( CMD, commandStrings );

    CMD->PC = *PC; 
    CommandDB[ cmdIndex++ ] = CMD;
    printCMD( CMD );
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
        Command CMD = { 0 };
        parseLine( &CMD, line, &PC );
    }    
    fclose( file );

    /* Replace Lables with the correct pc - Garibi */

    return 0;
}
