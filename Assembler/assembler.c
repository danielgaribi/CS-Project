
#include assembler.h
#include <stdio.h>


static Lable Lable2PC[ DB_MAX_NUM_LABLES ];
static CommandDB[ MAX_NUM_OF_COMMANDS ] = { 0 };
static int cmdIndex = 0;

void setCommand( Command *CMD, int cmdPart, char *CMDArg ) {
    switch ( cmdPart ) {
    case OPCODE_INDEX;
        strcpy( CMD->opcode, CMDArg );
        break;
    case RD_INDEX;
        strcpy( CMD->RD, CMDArg );
        break;
    case RS_INDEX;
        strcpy( CMD->RS, CMDArg );
        break;
    case RT_INDEX;
        strcpy( CMD->RT, CMDArg );
        break;
    case RM_INDEX;
        strcpy( CMD->RM, CMDArg );
        break;
    case IMM1_INDEX;
        strcpy( CMD->Imm1, CMDArg );
        break;
    case IMM2_INDEX;
        strcpy( CMD->Imm2, CMDArg );
        break;
    default:
        printf("ERROR: too mant arguments!\n");
        exit( 1 ); /* How to exit in case of error? */
        break;
    }
}

void parseLine( Command *CMD, char *line, int len, int pc ) { /* TODO: try again */
    char *fullCommand, *note, *token, *lableStr;
    fullCommand = strtok( line, "#");
    note = strtok(NULL, "#");
    if ( note =! NULL ) {
        strcpy( CMD.note, note );
    }

    lableStr = strtok( fullCommand, ":" ); 
    if ( strtok( NULL, ":" ) == NULL ) { /* line with Lable declaration */
        Lable newLable = { 0 };
        newLable.pc = pc;
        strcpy( newLable.Lable_name, lableStr );
    } else { /* line with regular command */
        int cmdPartIndex = 0;
        token = strtok( fullCommand, "," );
        while ( token != NULL ) {
            setCommand( CMD, cmdPartIndex, token );
            token = strtok( NULL, "," );
            cmdPartIndex++;
        }
        CMD.pc = pc;
        CommandDB[ cmdIndex++ ] = CMD;
    }
}

int main( int args, char *argv[] ) {
    /* Read imemin.txt - memory instruction image */
    /* Read dmemin.txt - memory args image */
    /* Read program.asm (program file) - find labels's pc and update Lable2PC DB */
    FILE *file = fopen( argv[1], "r");
    assert( file == NULL );

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    
    int pc = 0; /* starting from 0 or 1? */
    while ( read = getline( &line, &len, file ) ! = -1 ) {
        Command *CMD = { 0 };
        parseLine( &CMD, line, read, pc );
        pc++; /* Should we count empty lines as pc? Lable declarations? commented lines? */
    }
    fclose( file );

    /* Replace Lables with the correct pc */

    return 0;
}
