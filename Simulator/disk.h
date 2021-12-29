#ifndef DISK_H
#define DISK_H

#define SECTOR_NUMBER           128
#define SECTOR_SIZE             128
#define MAX_DISK_LINE_LENGTH    8
#define DISK_OPERATION_TIME     1024
#define MAX_LINE_LENGTH         500 
int diskTimer;

void initDisk(void);
void readFromDisk(void);
void writeToDisk(void);
int diskHandler(void);

#endif
