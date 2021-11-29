
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
    printf("location: %d\t", memory->location );
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
    if ( line[0] == '\n' || line[0] == '#' ) {
        return FALSE;
    }
    return TRUE; 
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
    newMemory.location = (int)strtol(strtok( NULL, " "), NULL, 0);
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

uint64_t parse_cmd_to_uint64_t( Command cmd ) {
    uint64_t bin_cmd = 0;
    int imm1, imm2;

    imm1 = convert_imm_to_int(cmd.Imm1);
    imm2 = convert_imm_to_int(cmd.Imm2);

    SET_IMM1(bin_cmd, imm1);
    SET_IMM2(bin_cmd, imm2);
    SET_RM(bin_cmd, cmd.RM);
    SET_RT(bin_cmd, cmd.RT);
    SET_RS(bin_cmd, cmd.RS);
    SET_RD(bin_cmd, cmd.RD);
    SET_OPCODE(bin_cmd, cmd.Opcode);

    return bin_cmd;
}

int convert_imm_to_int(char *imm) {
    int i;
    
    /** Check if imm is label, if so return label's pc */ 
    for (i = 0; i < DB_MAX_NUM_LABLES; i++) {
        if (LableDB[ i ] == NULL) {
            break;
        }
        if (strcmp(LableDB[ i ]->Lable_name, imm) == 0) { /** if imm is label i */
            return LableDB[ i ]->PC;
        }
    }
    
    return (int)strtol(imm, NULL, 0);
}

void print_imemin( char *imemin_file ) {
    int i;
    uint64_t cmd;
    char cmd_string[CMD_LENGTH_HEX+1];
    FILE *file = fopen(imemin_file, "w+");

    for (i = 0; i < MAX_NUM_OF_COMMANDS; i++) {
        if (CommandDB[i] == NULL) {
            break;
        }
        cmd = parse_cmd_to_uint64_t(*(CommandDB[i]));
        sprintf(cmd_string, "%012lx", cmd);
        fputs(cmd_string, file);
        fputs("\r\n", file);
    }

    fclose(file);
}

void get_memory_array(int **memory, int *size) {
    int loc, i;
    *size = 0;

    for (i = 0; i < MAX_NUM_OF_COMMANDS; i++) {
        if (MemoryDB[i] == NULL) {
            break;
        }
        if(MemoryDB[i] -> location > *size) {
            *size = MemoryDB[i] -> location;
        }
    }
    *size += 1;

    *memory = (int*) calloc(*size, sizeof(int));
    assert(*memory != NULL);

    for (i = 0; i < MAX_NUM_OF_COMMANDS; i++) {
        if (MemoryDB[i] == NULL) {
            break;
        }
        loc = MemoryDB[i] -> location;

        (*memory)[loc] = (int)strtol(MemoryDB[i] -> value, NULL, 0);
    }
}

void print_dmemin( char *dmemin_file ) {
    int size, i;
    int *memory = NULL;
    FILE *file;
    char mem_value_string[MEM_LENGTH_HEX+1];

    get_memory_array(&memory, &size);
    file = fopen(dmemin_file, "w+");

    for (i = 0; i < size; i++) {
        sprintf(mem_value_string, "%08x", memory[i]);
        fputs(mem_value_string, file);
        fputs("\r\n", file);
    }

    fclose(file);
    free(memory);
}


int main( int argc, char *argv[] ) {
    char *asm_file_name, *imemin_file, *dmemin_file;
    /* Read program.asm (program file) - find labels's pc and update Lable2PC DB */
    assert( argc == 4 );
    asm_file_name = argv[1];
    imemin_file = argv[2];
    dmemin_file = argv[3];

    FILE *file = fopen( asm_file_name, "r");
    assert( file != NULL );

    char line[ BUFFER_MAX_SIZE ];
    while ( fgets( line, BUFFER_MAX_SIZE, file ) != NULL ) {
        printf("line: %s\n", line );
        Command CMD = { 0 };
        parseLine( &CMD, line );
    }    
    fclose( file );

    /* test data for im print */
    // CommandDB[0] = (Command*) malloc(sizeof(Command));
    // CommandDB[0]->PC = 0;
    // CommandDB[0]->Opcode = 13;
    // CommandDB[0]->RD = 4;
    // CommandDB[0]->RS = 3;
    // CommandDB[0]->RT = 2;
    // CommandDB[0]->RM = 1;
    // CommandDB[0]->Imm1 = "5";
    // CommandDB[0]->Imm2 = "10";

    // CommandDB[1] = (Command*) malloc(sizeof(Command));
    // CommandDB[1]->PC = 1;
    // CommandDB[1]->Opcode = 12;
    // CommandDB[1]->RD = 5;
    // CommandDB[1]->RS = 6;
    // CommandDB[1]->RT = 7;
    // CommandDB[1]->RM = 8;
    // CommandDB[1]->Imm1 = "0xff";
    // CommandDB[1]->Imm2 = "0x9";
    
    printf("print DBs status\n");

    printCommandDB();
    printf("chack 1\n");
    /* printLableDB(); */
    printf("chack 2\n");
    /* printMemoryDB(); */
    printf("chack 3\n");
    /* Replace Lables with the correct pc - Garibi */
    print_imemin(imemin_file);
    print_dmemin(dmemin_file);

    return 0;
}
