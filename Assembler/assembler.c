
#include "assembler.h"

static Command CommandDB[ MAX_NUM_OF_COMMANDS ] = { 0 };
static int GlobalPC = 0;
static Lable LableDB[ DB_MAX_NUM_LABLES ] = { 0 };
static int lableIndex = 0;
static Memory MemoryDB[ MAX_NUM_OF_COMMANDS ];
static int memIndex = 0;

void setCommand( int pc, char **CMDArgs ) {
    char next[BUFFER_MAX_SIZE] = { 0 };
    char* token;
    for ( int cmdPart = 1; cmdPart <= NUM_OF_CMD_FIELDS; cmdPart++ ) {
        char* cleanCMDArg = cleanWord(CMDArgs[cmdPart]);

        switch ( cmdPart ) {
        case OPCODE_INDEX:
            for ( int i = 0; i < NUM_OF_OPCODES; i++) {
                if ( strcmp( cleanCMDArg, opcodes[ i ]) == 0 ) {
                    CommandDB[pc].Opcode = i;
                    break;
                }
            }
            break;
        case RD_INDEX:
            for ( int i = 0; i < NUM_OF_REGISTERS; i++ ) {
                if ( strcmp( cleanCMDArg, registers[ i ]) == 0 ) {
                    CommandDB[pc].RD = i;
                    break;
                }
            }
            break;
        case RS_INDEX:
            for ( int i = 0; i < NUM_OF_REGISTERS; i++ ) {
                if ( strcmp( cleanCMDArg, registers[ i ]) == 0 ) {
                    CommandDB[pc].RS = i;
                    break;
                }
            }
            break;
        case RT_INDEX:
            for ( int i = 0; i < NUM_OF_REGISTERS; i++ ) {
                if ( strcmp( cleanCMDArg, registers[ i ]) == 0 ) {
                    CommandDB[pc].RT = i;
                    break;
                }
            }
            break;
        case RM_INDEX:
            for ( int i = 0; i < NUM_OF_REGISTERS; i++ ) {
                if ( strcmp( cleanCMDArg, registers[ i ]) == 0 ) {
                    CommandDB[pc].RM = i;
                    break;
                }
            }
            break;
        case IMM1_INDEX: // keep in string and handle in second round 
            token = strtok_s(cleanCMDArg, " \t \n", &next);
            strcpy_s(CommandDB[pc].Imm1, BUFFER_MAX_SIZE, token );
            break;
        case IMM2_INDEX:
            token = strtok_s(cleanCMDArg, " \t \n", &next);
            strcpy_s(CommandDB[pc].Imm2, BUFFER_MAX_SIZE, token );
            break;
        default:
            printf("ERROR: too many arguments!\n");
            exit( 1 );
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

// Return False if line is empty or contain only note, Otherwise return True
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

char* cleanWord( char *word ) {
    // Trim spaces and tabs from beginning 
    while (word[0] != '\0') {
        if ( (word[0] == ' ') || (word[0] == '\t') ) {
            word++;
        }
        else {
            break;
        }
    }
    return word;
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

void setLable( int pc, char *lableName ) {
    strcpy_s( LableDB[ lableIndex ].Lable_name, BUFFER_MAX_SIZE, lableName );
    LableDB[ lableIndex ].PC = GlobalPC;
    LableDB[ lableIndex ].is_created = TRUE;
    lableIndex++;
    strcpy_s( CommandDB[pc].Lable, BUFFER_MAX_SIZE, lableName );
}

void setMemory( char *line ) {
    char next[BUFFER_MAX_SIZE] = { 0 };
    char *word, *token;
    word = strtok_s( line, " \t", &next );
    if ( strcmp( word, ".word" ) != 0 ) {
        printf("command with dot and not .word found!\n");
        return;
    }
    MemoryDB[ memIndex ].location = (int)strtol(strtok_s( NULL, " ", &next ), NULL, 0);
    token = strtok_s( NULL, " ", &next );
    strcpy_s( MemoryDB[ memIndex ].value, BUFFER_MAX_SIZE, token );
    memIndex++;
}

bool isLableValid( char *lable ) { 
    char ch = lable[ 0 ];
    // Check if the first char is a letter
    if (!(('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z')) ) {
        return FALSE;
    }
    // Check if the all other chars are letters or digits
    for (int i = 0; i < strlen(lable); i++) {
        ch = lable[i];
        if (!(('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ('0' <= ch && ch <= '9'))) {
            return FALSE;
        }
    }
    return TRUE;
}

void parseLine( char *line ) { 
    char* next[BUFFER_MAX_SIZE] = { 0 };
    char *lableName, *fullCommand, *note, *token, *token2;
    char** commandStrings;
    int cmdPartIndex;
    if ( isLineEmptyOrNoteOnly( line ) == FALSE ) {
        return; // continue to next line
    }
    // Split line into lable+command and note
    token = strtok_s( line, "#", &next );
    note = strtok_s( NULL, "#", &next );
    if ( note != NULL ) { // line with comment
        strcpy_s(CommandDB[GlobalPC].Note, BUFFER_MAX_SIZE, note );
    }
    // Split lable+command to 2 seperated parts
    bool isLableFound = FALSE;
    bool isCMDFound = FALSE;
    bool isDotWordFound = FALSE;
    classifiedCMD( token, &isLableFound, &isCMDFound, &isDotWordFound ); 
    if ( isLableFound ) {
        lableName = strtok_s( token, ":", &next );
        if (!isLableValid) {
            printf("Lable must start with a letter and than has only letters and numbers\n");
            exit(1);
        }
        setLable(GlobalPC, lableName );
        if ( !isCMDFound ) {
            return;
        } else {
            fullCommand = strtok_s( NULL, ":", &next );
        }
    } else if ( isDotWordFound ) {
        setMemory( line );
        return;
    } else {
        fullCommand = token;
    }

    // Split command into opcode, registers, imms..
    token = strtok_s( fullCommand, ",", &next );
    cmdPartIndex = 3;
    token2 = strtok_s( NULL, ",", &next );

    commandStrings = (char**) calloc(sizeof(char*), NUM_OF_CMD_FIELDS + 1);
    assert(commandStrings != NULL);

    while ( token2 != NULL ) { // Set the rest of command
        commandStrings[ cmdPartIndex++ ] = token2;
        token2 = strtok_s( NULL, ",", &next );
    }
    token2 = strtok_s( token, " ", &next );
    commandStrings[ OPCODE_INDEX ] = token2; // Set OPCODE register
    token2 = strtok_s( NULL, ",", &next );
    commandStrings[ RD_INDEX ] = token2; // Set RD register
    setCommand(GlobalPC, commandStrings );
    CommandDB[GlobalPC].PC = GlobalPC;
    GlobalPC++;
    free(commandStrings);
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
    
    // Check if imm is label, if so return label's pc
    for (i = 0; i < DB_MAX_NUM_LABLES; i++) {
        if (LableDB[ i ].is_created == FALSE) {
            break;
        }
        if (strcmp(LableDB[ i ].Lable_name, imm) == 0) { // if imm is label i
            return LableDB[ i ].PC;
        }
    }
    
    return (int)strtol(imm, NULL, 0);
}

void print_imemin( char *imemin_file ) {
    int i;
    uint64_t cmd;
    char cmd_string[CMD_LENGTH_HEX+1];
    FILE* file;    
    assert(fopen_s(&file, imemin_file, "a+") == 0);
    assert(file != NULL);

    for (i = 0; i < GlobalPC; i++) {
        cmd = parse_cmd_to_uint64_t(CommandDB[i]);
        sprintf_s(cmd_string, CMD_LENGTH_HEX + 1, "%012I64X", cmd);
        fputs(cmd_string, file);
        fputs("\n", file);
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
    assert(fopen_s(&file, dmemin_file, "a+") == 0);
    assert(file != NULL);

    for (i = 0; i < size; i++) {
        sprintf_s(mem_value_string, MEM_LENGTH_HEX + 1, "%08x", memory[i]);
        fputs(mem_value_string, file);
        fputs("\n", file);
    }

    fclose(file);
    free(memory);
}

int main( int argc, char *argv[] ) {
    char *asm_file_name, *imemin_file, *dmemin_file;
    FILE* file;

    // Read program.asm (program file) - find labels's pc and update Lable2PC DB
    assert( argc == 4 );
    asm_file_name   = argv[1];
    imemin_file     = argv[2];
    dmemin_file     = argv[3];

    assert(fopen_s(&file, asm_file_name, "a+") == 0);
    assert( file != NULL );
    char line[ BUFFER_MAX_SIZE ];
    while ( fgets( line, BUFFER_MAX_SIZE, file ) != NULL ) {
        parseLine( line );
    }    
    fclose( file );

    print_imemin(imemin_file);
    print_dmemin(dmemin_file);
    return 0;
}
