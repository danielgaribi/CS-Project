#include "disk.h"
#include "Simulator.h"


void initDisk(){
    FILE *file;
    diskTimer = 0;
    char line[MAX_DISK_LINE_LENGTH];
    file = fopen(diskin_file, "r");
    int lineIndex = 0;
    while (fgets(line, 500, file) != NULL)
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

void diskToFile(){}
