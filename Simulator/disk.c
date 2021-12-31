#include "disk.h"
#include "Simulator.h"

int diskMemory[SECTOR_NUMBER][SECTOR_SIZE];

void initDisk() {
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

void readFromDisk() {
    int sectorIndex;
    int sector = io_registers_values[disksector];
    int memoryPointer = io_registers_values[diskbuffer];
    for(sectorIndex = 0; sectorIndex < SECTOR_SIZE; sectorIndex++){
        memory[memoryPointer + sectorIndex] = diskMemory[sector][sectorIndex];
    }
}

void writeToDisk() {
    int sectorIndex;
    int sector = io_registers_values[disksector];
    int memoryPointer = io_registers_values[diskbuffer];
    for(sectorIndex = 0; sectorIndex < SECTOR_SIZE; sectorIndex++){
        diskMemory[sector][sectorIndex] = memory[memoryPointer + sectorIndex];
    }
}

int diskHandler() {

    if(io_registers_values[diskcmd] != 0) {
        io_registers_values[diskstatus] = 1;
    
        if(diskTimer < DISK_OPERATION_TIME){
            diskTimer++;
            return;
        } else{
            io_registers_values[diskcmd] = 0;
            io_registers_values[diskstatus] = 0;
            io_registers_values[irq1status] = 1;
        }
        if(io_registers_values[diskcmd] == 1){
            readFromDisk();
        }
        if(io_registers_values[diskcmd] == 2){
            writeToDisk();
        }
        diskTimer = 0;
    }


}

void write_diskout_file() {
    int zero_counter = 0;
    int j, sector, sectorIndex;
    char word[CMD_LENGTH_HEX+1];
    for(sector = 0; sector < SECTOR_NUMBER; sector++){
        for(sectorIndex = 0; sectorIndex < SECTOR_SIZE; sectorIndex++) {
            if (diskMemory[sector][sectorIndex] == 0) {
                zero_counter++;
                continue;
            }
            for (j = 0; j < zero_counter; j++) {
                sprintf_s(word, CMD_LENGTH_HEX + 1, "%08X", diskMemory[sector][sectorIndex]);
                fputs(word, context.diskout_fd);
                fputs("\r\n", context.diskout_fd);
            }
            sprintf_s(word, CMD_LENGTH_HEX + 1, "%08X", diskMemory[sector][sectorIndex]);
		    fputs(word, context.diskout_fd);
		    fputs("\r\n", context.diskout_fd);
            zero_counter = 0;
        }
    }
}
