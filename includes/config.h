#ifndef CONFIG_H
#define CONFIG_H

#define HESOS_TOTAL_INTERRUPTS 512
#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

//100MB Kernel heap size
#define HESOS_HEAP_SIZE_BYTES 104857600
#define HESOS_HEAP_BLOCK_SIZE 4096

//0x00100000 -> Kerneli yüklemiştik 0x01000000 -> Heapi yüklüyoruz.
#define HESOS_HEAP_ADDRESS 0x01000000
//Tableyi başka yere yüklüyoruz.
#define HESOS_HEAP_TABLE_ADDRESS 0x00007E00

#define HESOS_DISK_SECTOR_SIZE 512

#define HESOS_MAX_PATH 108

#endif