#include "disk.h"
#include "Simulator.h"

int diskMemory[SECTOR_NUMBER][SECTOR_SIZE];

void initDisk(){
    diskTimer = 0;
    char line[MAX_DISK_LINE_LENGTH];
    int lineIndex = 0;
    while (fgets(line, MAX_LINE_LENGTH, context.diskin_fd) != NULL)
    {
       int sector = lineIndex / SECTOR_NUMBER;
       int sectorIndex = lineIndex % SECTOR_SIZE;
       diskMemory[sector][sectorIndex] = (int) strtol(line, NULL, 16);
       lineIndex++;
    }
}

void readFromDisk(){
    int sector =  registers_values[disksector];
    int memoryPointer =  registers_values[diskbuffer];
    for(int sectorIndex = 0; sectorIndex < SECTOR_SIZE; sectorIndex++){
        memory[memoryPointer + sectorIndex] = diskMemory[sector][sectorIndex];
    }
}

void writeToDisk(){
    int sector =  registers_values[disksector];
    int memoryPointer =  registers_values[diskbuffer];
    for(int sectorIndex = 0; sectorIndex < SECTOR_SIZE; sectorIndex++){
        diskMemory[sector][sectorIndex] = memory[memoryPointer + sectorIndex];
    }
}

int diskHandler(){

    if(registers_values[diskcmd] != 0){
        registers_values[diskstatus] = 1;
    
        if(diskTimer < DISK_OPERATION_TIME){
            diskTimer++;
            return;
        } else{
            registers_values[diskcmd] = 0;
            registers_values[diskstatus] = 0;
            registers_values[irq1status] = 1;
        }
        if(registers_values[diskcmd] == 1){
            readFromDisk();
        }
        if(registers_values[diskcmd] == 2){
            writeToDisk();
        }
        diskTimer = 0;
    }


}

void diskToFile(){
    char word[CMD_LENGTH_HEX+1];
    for(int sector = 0; sector < SECTOR_NUMBER; sector++){
        for(int sectorIndex = 0; sectorIndex < SECTOR_SIZE; sectorIndex++){
            sprintf_s(word, CMD_LENGTH_HEX + 1, "%08X", diskMemory[sector][sectorIndex]);
		    fputs(word, context.diskout_fd);
		    fputc("\r\n", context.diskout_fd);
        }
    }
}
