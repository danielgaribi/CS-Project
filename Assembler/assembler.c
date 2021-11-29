
#include "assembler.h"

static Lable *LableDB[ DB_MAX_NUM_LABLES ] = { 0 };
static int lableIndex = 0;
static Command *CommandDB[ MAX_NUM_OF_COMMANDS ] = { 0 };
static int cmdIndex = 0;
static Memory *MemoryDB[ MAX_NUM_OF_COMMANDS ];
static int memIndex = 0;
static int PC = 0; 

void setCommand( Command *CMD, char **CMDArgs ) {
    for ( int cmdPart = 1; cmdPart <= NUM_OF_CMD_FIELDS; cmdPart++ ) {
        char *cleanCMDArg = strtok( CMDArgs[ cmdPart ], " \t");

        switch ( cmdPart ) {
        case OPCODE_INDEX:
            for ( int i = 0; i < NUM_OF_OPCODES; i++) {
                if ( strcmp( cleanCMDArg, opcodes[ i ]) == 0 ) {
                    CMD->Opcode = i;
                    break;
                }
            }
            break;
        case RD_INDEX:
            for ( int i = 0; i < NUM_OF_REGISTERS; i++ ) {
                if ( strcmp( cleanCMDArg, registers[ i ]) == 0 ) {
                    CMD->RD = i;
                    break;
                }
            }
            break;
        case RS_INDEX:
            for ( int i = 0; i < NUM_OF_REGISTERS; i++ ) {
                if ( strcmp( cleanCMDArg, registers[ i ]) == 0 ) {
                    CMD->RS = i;
                    break;
                }
            }
            break;
        case RT_INDEX:
            for ( int i = 0; i < NUM_OF_REGISTERS; i++ ) {
                if ( strcmp( cleanCMDArg, registers[ i ]) == 0 ) {
                    CMD->RT = i;
                    break;
                }
            }
            break;
        case RM_INDEX:
            for ( int i = 0; i < NUM_OF_REGISTERS; i++ ) {
                if ( strcmp( cleanCMDArg, registers[ i ]) == 0 ) {
                    CMD->RM = i;
                    break;
                }
            }
            break;
        case IMM1_INDEX: // keep in string and handle in second round 
            CMD->Imm1 = cleanCMDArg;
            break;
        case IMM2_INDEX:
            CMD->Imm2 = cleanCMDArg;
            break;
        default:
            printf("ERROR: too many arguments!\n");
            exit( 1 ); // How to exit in case of error?
    }

    }
}

void printCMD( Command *CMD ) { /* TODO: debug */
    printf("PC: %d\t", CMD->PC );
    printf("Opcode: %s\t", opcodes[ CMD->Opcode ] );
    printf("RD: %s\t", registers[CMD->RD] );
    printf("RS: %s\t", registers[CMD->RS] );
    printf("RT: %s\t", registers[CMD->RT] );
    printf("RM: %s\t", registers[CMD->RM] );
    printf("Imm1: %s\t", CMD->Imm1 );
    printf("Imm2: %s\t", CMD->Imm2 );
    printf("Lable: %s\t", CMD->Lable );
    printf("Note: %s\n", CMD->Note );
}

void printCommandDB( void ) {
    for ( int i = 0; i < MAX_NUM_OF_COMMANDS; i++ ) {
        if ( CommandDB[ i ] == NULL ) {
            return;
        }
        printCMD( CommandDB[ i ] );
    }
}

void printLable( Lable *lable ) {
    printf("Lable_name: %s\t", lable->Lable_name );
    printf("PC: %d\n", lable->PC );
}

void printLableDB( void ) {
    printf("Lable_name:\t PC:\n");
    for ( int i = 0; i < DB_MAX_NUM_LABLES; i++ ) {
        if ( LableDB[ i ] == NULL ) {
            return;
        }
        printLable( LableDB[ i ] );
    }
}

void printMemory( Memory *memory ) {
    printf("location: %s\t", memory->location );
    printf("value: %s\n", memory->value );
}

void printMemoryDB( void ) {
    for ( int i = 0; i < DB_MAX_NUM_LABLES; i++ ) {
        if ( MemoryDB[ i ] == NULL ) {
            return;
        }
        printMemory( MemoryDB[ i ] );
    }
}



/* Return False if line is empty or contain only note, Otherwise return True*/
bool classifiedCMD( char *line, bool *isLableFound, bool *isNoteFound, bool *isDotWordFound ) {
    for ( int i = 0; i < strlen( line ); i++ ) {
        char tmp = line[ i ];
        if ( tmp == ':' ) {
            *isLableFound = TRUE;
        }
        if ( tmp == ',' ) {
            *isNoteFound = TRUE;
        }
        if ( tmp == '.' ) {
            *isDotWordFound = TRUE;
        }
    }
    return FALSE;
}

bool isLineValid( char *line ) {
    return TRUE; /* TODO change */
}

void setLable( Command *CMD, char *lableName ) {
    CMD->Lable = lableName;
    Lable newLable = { 0 };
    newLable.Lable_name = lableName;
    newLable.PC = PC;
    LableDB[ lableIndex++ ] = &newLable;
}

void setMemory( char *line ) {
    Memory newMemory = { 0 };
    char *word = strtok( line, " \t" );
    if ( strcmp( word, ".word" ) != 0 ) {
        printf("command with dot and not .word found!\n"); /* TODO: debug */
        return;
    }
    newMemory.location = strtok( NULL, " ");
    newMemory.value = strtok( NULL, " ");
    MemoryDB[ memIndex++ ] = &newMemory;
}

void parseLine( Command *CMD, char *line ) { 
    char *lableName, *fullCommand, *note, *token, *token2;
    char *commandStrings[ NUM_OF_CMD_FIELDS ] = { 0 };
    int cmdPartIndex;
    if ( isLineValid( line ) == FALSE ) {
        return;
    }

    /* Split line into lable+command and note */
    token = strtok( line, "#");
    printf("token: %s\n", token );
    note = strtok( NULL, "#" );
    if ( note != NULL ) { /* line with comment */
        CMD->Note = note;
    }
    printf("after #: token = '%s'\n", token); /* TODO: debug */
    printf("after #: note = '%s'\n", note); /* TODO: debug */

    /* Split lable+command to 2 seperated parts */
    bool isLableFound = FALSE;
    bool isCMDFound = FALSE;
    bool isDotWordFound = FALSE;
    classifiedCMD( token, &isLableFound, &isCMDFound, &isDotWordFound );
    printf("isLableFound: %d\tisCMDFound: %d\n", isLableFound, isCMDFound); /* TODO: debug */
    if ( isLableFound && !isCMDFound ) {
        lableName = strtok( token, ":" );
        setLable( CMD, lableName );
        if ( !isCMDFound ) {
            printCMD( CMD );
            return;
        } else {
            fullCommand = strtok( NULL, ":" );
            printf("after ':': lableName = '%s'\n", lableName); /* TODO: debug */
            printf("after ':': fullCommand = '%s'\n", fullCommand); /* TODO: debug */
        }
    } else if ( isDotWordFound ) {
        setMemory( line );
        return;
    } else {
        fullCommand = token;
    }

    printf("after if: fullCommand = '%s'\n", fullCommand); /* TODO: debug */

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

    CMD->PC = PC++; 
    CommandDB[ cmdIndex++ ] = CMD;
    printf("in array\n");
    printCMD(CommandDB[cmdIndex-1]);
    printf("out array\n");
    printCMD( CMD );
}

int main( int argc, char *argv[] ) {
    /* Read program.asm (program file) - find labels's pc and update LableDB DB */
    assert( argc == 2 );

    FILE *file = fopen( argv[1], "r");
    assert( file != NULL );

    char line[ BUFFER_MAX_SIZE ];
    while ( fgets( line, BUFFER_MAX_SIZE, file ) != NULL ) {
        printf("line: %s\n", line ); /* TODO: debug */
        Command CMD = { 0 };
        parseLine( &CMD, line );
    }    
    fclose( file );

    printf("print DBs status\n");

    /* printCommandDB(); */
    printf("chack 1\n");
    /* printLableDB(); */
    printf("chack 2\n");
    /* printMemoryDB(); */
    printf("chack 3\n");
    /* Replace Lables with the correct pc - Garibi */

    return 0;
}
