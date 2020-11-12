#include <unistd.h>
#include <cstdint>

#ifndef FAT_STRUCTURES_FOR_FAT16_H
#define FAT_STRUCTURES_FOR_FAT16_H

#define FAT16_SEC_SIZE 512
#define FAT16_DIR_ENTRY_SIZE 32
#define FILE_NAME_LENGTH 8
#define EXT_LENGTH 3

#pragma pack(push, 1)
struct alignas(16) fat_dir_t {
    unsigned char file_name[FILE_NAME_LENGTH];
    unsigned char ext[EXT_LENGTH];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t create_time_tenth;
    uint16_t create_time;

    uint16_t create_date;
    uint16_t last_access_date;
    uint16_t first_cluster_hi;
    uint16_t modify_time;
    uint16_t modify_date;
    uint16_t first_cluster_lo;
    uint32_t file_size;
};

struct fat_boot_sector_t {
    unsigned char jmp[3];
    char oem[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sector_count;
    unsigned char table_count;
    unsigned short root_entry_count;
    unsigned short total_sectors_16;
    unsigned char media_type;
    unsigned short table_size_16;
    unsigned short sectors_per_track;
    unsigned short head_side_count;
    unsigned long hidden_sector_count;
    unsigned long total_sectors_32;

    unsigned char drive_number;
    unsigned char current_head;
    unsigned char boot_signature;
    unsigned long volume_id;
    char volume_label[11];
    char fs_type[8];
    char boot_code[448];
    unsigned short boot_sector_signature;
};

#pragma pack(pop)
#endif //FAT_STRUCTURES_FOR_FAT16_H
