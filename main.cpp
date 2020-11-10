#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>

#define FAT16_SEC_SIZE 512

#pragma pack(push, 1)
struct alignas(16) fat_dir_t {
    uint8_t Attr;
    uint8_t NTRes;
    uint8_t CrtTimeTenth;
    uint16_t CrtTime;

    uint16_t CrtDate;
    uint16_t LstAccDate;
    uint16_t FstClusHI;
    uint16_t WrtTime;
    uint16_t WrtDate;
    uint16_t FstClusLO;
    uint32_t FileSize;
};

struct fat_boot_sector_t {
    unsigned char bootjmp[3];
    unsigned char oem_name[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sector_count;
    unsigned char table_count;
    unsigned short root_entry_count;
    unsigned short total_sectors_16;
    unsigned char media_type;
    unsigned short table_size_16;
    unsigned short sectors_per_track;
    unsigned char head_side_count;
    unsigned int hidden_sector_count;
    unsigned int total_sectors_32;

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

int main(int argc, char ** argv) {
    int fd;
    fat_boot_sector_t boot_sector_t{};
    fat_dir_t root_dir{};

    if (argc < 2) {
        std::cerr << "No file system image is given" << std::endl;
        exit(-1);
    }

    FILE * FAT16 = fopen(argv[1], "r");
    if (FAT16 == nullptr) {
        std::cerr << "There is no such file" << std::endl;
        exit(-1);
    }

    fread(&boot_sector_t, 1, 512, FAT16);



    return 0;
}
