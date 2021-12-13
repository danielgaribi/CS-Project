
#include "assembler.h"

static Command CommandDB[ MAX_NUM_OF_COMMANDS ] = { 0 };
static int GlobalPC = 0;
static Lable LableDB[ DB_MAX_NUM_LABLES ] = { 0 };
static int lableIndex = 0;
static Memory MemoryDB[ MAX_NUM_OF_COMMANDS ];
static int memIndex = 0;

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
            strcpy( CMD->Imm1, cleanCMDArg );
            break;
        case IMM2_INDEX:
            strcpy( CMD->Imm2, cleanCMDArg ); 
            break;
        default:
            printf("ERROR: too many arguments!\n");
            exit( 1 ); // How to exit in case of error?
    }
    }
}

void printCMD( Command CMD ) { 
    printf("PC: %d\t", CMD.PC );
    printf("Opcode: %s\t", opcodes[ CMD.Opcode ] );
    printf("RD: %s\t", registers[CMD.RD] );
    printf("RS: %s\t", registers[CMD.RS] );
    printf("RT: %s\t", registers[CMD.RT] );
    printf("RM: %s\t", registers[CMD.RM] );
    printf("Imm1: %s\t", CMD.Imm1 );
    printf("Imm2: %s\t", CMD.Imm2 );
    printf("Lable: %s\t", CMD.Lable );
    printf("Note: %s", CMD.Note );
}

void printCommandDB( void ) {
    for ( int i = 0; i < GlobalPC; i++ ) {
        printCMD( CommandDB[ i ] );
    }
}

void printLable( Lable lable ) { 
    printf("Lable_name: %s\t", lable.Lable_name );
    printf("PC: %d\n", lable.PC );
}

void printLableDB( void ) { 
    for ( int i = 0; i < lableIndex; i++ ) {
        if ( LableDB[ i ].Lable_name == NULL ) {
            return;
        }
        printLable( LableDB[ i ] );
    }
}

void printMemory( Memory memory ) {
    printf("location: %d\t", memory.location );
    printf("value: %s\n", memory.value );
}

void printMemoryDB( void ) { 
    for ( int i = 0; i < memIndex; i++ ) {
        if ( MemoryDB[ i ].value == NULL ) {
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

void setLable( Command *CMD, char *lableName ) {
    strcpy( LableDB[ lableIndex ].Lable_name, lableName );
    LableDB[ lableIndex ].PC = GlobalPC;
    lableIndex++;
    strcpy( CMD->Lable, lableName );
}

void setMemory( char *line ) {
    char *word, *token;
    word = strtok( line, " \t" );
    if ( strcmp( word, ".word" ) != 0 ) {
        printf("command with dot and not .word found!\n"); /* TODO: debug */
        return;
    }
    MemoryDB[ memIndex ].location = (int)strtol(strtok( NULL, " "), NULL, 0);
    token = strtok( NULL, " ");
    strcpy( MemoryDB[ memIndex ].value, token ); /* TODO: debug */
    memIndex++;
}

void parseLine( char *line ) { 
    char *lableName, *fullCommand, *note, *token, *token2;
    char *commandStrings[ NUM_OF_CMD_FIELDS ] = { 0 };
    int cmdPartIndex;
    Command *CMD = &CommandDB[ GlobalPC ];
    if ( isLineEmptyOrNoteOnly( line ) == FALSE ) {
        return;
    }

    /* Split line into lable+command and note */
    token = strtok( line, "#");
    note = strtok( NULL, "#" );
    if ( note != NULL ) { /* line with comment */
        strcpy( CMD->Note, note );
    }

    /* Split lable+command to 2 seperated parts */
    bool isLableFound = FALSE;
    bool isCMDFound = FALSE;
    bool isDotWordFound = FALSE;
    classifiedCMD( token, &isLableFound, &isCMDFound, &isDotWordFound ); 

    if ( isLableFound ) {
        lableName = strtok( token, ":" );
        setLable( CMD, lableName );
        if ( !isCMDFound ) {
            return;
        } else {
            fullCommand = strtok( NULL, ":" );
        }
    } else if ( isDotWordFound ) {
        setMemory( line );
        return;
    } else {
        fullCommand = token;
    }

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
    CMD->PC = GlobalPC; 
    CommandDB[ GlobalPC++ ] = *CMD;
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
        if (LableDB[ i ].Lable_name == NULL) {
            break;
        }
        if (strcmp(LableDB[ i ].Lable_name, imm) == 0) { /** if imm is label i */
            return LableDB[ i ].PC;
        }
    }
    
    return (int)strtol(imm, NULL, 0);
}

void print_imemin( char *imemin_file ) {
    int i;
    uint64_t cmd;
    char cmd_string[CMD_LENGTH_HEX+1];
    FILE *file = fopen(imemin_file, "w+");
    assert(file != NULL);

    for (i = 0; i < GlobalPC; i++) {
        cmd = parse_cmd_to_uint64_t(CommandDB[i]);
        sprintf(cmd_string, "%012lX", cmd);
        fputs(cmd_string, file);
        fputs("\r\n", file);
    }

    fclose(file);
}

void get_memory_array(int **memory, int *size) {
    int loc, i;
    *size = 0;

    for (i = 0; i < MAX_NUM_OF_COMMANDS; i++) {
        if (MemoryDB[i].value == NULL) {
            break;
        }
        if(MemoryDB[i].location > *size) {
            *size = MemoryDB[i].location;
        }
    }
    *size += 1;

    *memory = (int*) calloc(*size, sizeof(int));
    assert(*memory != NULL);

    for (i = 0; i < MAX_NUM_OF_COMMANDS; i++) {
        if (MemoryDB[i].value == NULL) {
            break;
        }
        loc = MemoryDB[i].location;

        (*memory)[loc] = (int)strtol(MemoryDB[i].value, NULL, 0);
    }
}

void print_dmemin( char *dmemin_file ) {
    int size, i;
    int *memory = NULL;
    FILE *file;
    char mem_value_string[MEM_LENGTH_HEX+1];

    get_memory_array(&memory, &size);
    file = fopen(dmemin_file, "w+");
    assert(file != NULL);

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
        parseLine( line );
    }    
    fclose( file );
    
    // printf("print Command DB status:\n"); /* TODO: debug */
    // printCommandDB();
    
    // printf("\nprint Lable DB status:\n"); /* TODO: debug */
    // printLableDB();
    
    // printf("\nprint Memory DB status:\n"); /* TODO: debug */
    // printMemoryDB();

    print_imemin(imemin_file);
    print_dmemin(dmemin_file);
    return 0;
}
