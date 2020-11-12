#include <iostream>
#include <unistd.h>
#include <vector>
#include <fcntl.h>
#include <algorithm>
#include <bitset>
#include <map>

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


inline std::string remove_spaces(const unsigned char* str, size_t size) {
    std::string s;
    for (size_t i =0; i < size; i++) {
        if (!isspace(str[i])) {
            s += str[i];
        }
        else {
            break;
        }
    }
    return s;
}

std::string file_attributes(uint8_t attributes) {
    std::string binary = std::bitset<8>(attributes).to_string();
    std::string str_attributes;
//    0x01 -- Read only
//    0x02 -- Hidden
//    0x04 -- System
//    0x08 -- Volum label
//    0x10 -- Subdirectory
//    0x20 -- Archive
//    0x40 -- Device
//    0x80 -- Reserved
    std::vector<std::string> bit_attributes= {"Read only", "Hidden", "System", "Volume label", "Subdirectory", "Archive", "Device", "Reserved"};
    for (int i = 0; i < 8; i++) {
        if (binary[i] == '1') {
            str_attributes += bit_attributes[i] + "  ";
        }
    }
    return str_attributes;
}

inline std::string date_form(const std::string& date_time) {
    return (date_time.length() == 1) ? "0" + date_time : date_time;
}

std::string date_parsing(uint16_t date) {
    std::bitset<16> binary_date = std::bitset<16>(date);
    std::string day = std::to_string((binary_date & std::bitset<16>(31)).to_ulong());
    std::string month = std::to_string(((binary_date & std::bitset<16>(480)) >> 5).to_ulong());
    std::string year = std::to_string(1980 + ((binary_date & std::bitset<16>(65024)) >> 9).to_ulong());

    std::string str_date = date_form(day) + '.' + date_form(month) + '.' + date_form(year);

    return str_date;
}

std::string time_parsing(uint16_t time) {
    std::bitset<16> binary_date = std::bitset<16>(time);
    std::string seconds = std::to_string((binary_date & std::bitset<16>(31)).to_ulong() * 2);
    std::string minutes = std::to_string(((binary_date & std::bitset<16>(2016)) >> 5).to_ulong());
    std::string hours = std::to_string(((binary_date & std::bitset<16>(63488)) >> 11).to_ulong());

    std::string str_time = date_form(hours) + ':' + date_form(minutes) + ':' + date_form(seconds);
    return str_time;

}
int main(int argc, char ** argv) {
    int fd;
    fat_boot_sector_t boot_sector_t{};
    fat_dir_t file_fat16{};

    if (argc < 2) {
        std::cerr << "No file system image is given" << std::endl;
        exit(-1);
    }

    FILE * FAT16 = fopen(argv[1], "r");
    if (FAT16 == nullptr) {
        std::cerr << "There is no such file" << std::endl;
        exit(-1);
    }

    if (fread(&boot_sector_t, 1, 512, FAT16) != 512) {
        std::cerr << "Error while reading boot sector" << std::endl;
        exit(-1);
    }

    fseek(FAT16, (boot_sector_t.reserved_sector_count-1 + boot_sector_t.table_count * boot_sector_t.table_size_16) * boot_sector_t.bytes_per_sector, SEEK_CUR);


    std::cout << "                            Information about file system" << std::endl << std::endl
              << "Bytes per sector:                            " << boot_sector_t.bytes_per_sector << std::endl
              << "Sectors per cluster:                         " << static_cast<int>(boot_sector_t.sectors_per_cluster)<< std::endl
              << "Number of reserved sectors:                  " << boot_sector_t.reserved_sector_count << std::endl
              << "Number of FAT copies:                        "<<  static_cast<int>(boot_sector_t.table_count) << std::endl
              << "Number of entries in root directory:         "<<  boot_sector_t.root_entry_count << std::endl
              << "Size of root directory in bytes:             "<<  boot_sector_t.root_entry_count *  FAT16_DIR_ENTRY_SIZE << std::endl
              << "Number of reserved sectors:                  "<<  boot_sector_t.reserved_sector_count << std::endl
              << "Correctness of signature:                    "<<  !boot_sector_t.boot_sector_signature << std::endl;

    std::cout << "                            Information about the files in root directory";

    for (int i = 0; i < boot_sector_t.root_entry_count; i++) {
        fread(&file_fat16, 1, 32, FAT16);
        if (file_fat16.attributes == 0) {
            continue;
        }
        std::string file_name = remove_spaces(file_fat16.file_name, FILE_NAME_LENGTH);
        std::string ext = remove_spaces(file_fat16.ext, EXT_LENGTH);
        std::string attributes = file_attributes(file_fat16.attributes);
        std::cout << std::endl << std::endl << std::endl;
        std::cout << "File name:                                              "<<  file_name.append((ext.length() > 0) ? ( "." + ext ) : "") << std::endl
                  << "Size of the file or -1 if file is a directory:          "<< ((attributes.find("Subdirectory") != std::string::npos) ? -1 : static_cast<ssize_t>(file_fat16.file_size)) <<std::endl
                  << "Date and time of the last modification:                 "<<  date_parsing(file_fat16.modify_date) << "   " << time_parsing(file_fat16.modify_time) << std::endl
                  << "Date and time of the creation:                          "<<  date_parsing(file_fat16.create_date) << "   " << time_parsing(file_fat16.create_time) << std::endl
                  << "File attributes:                                        "<<  attributes << std::endl
                  << "Number of the first cluster of the file:                "<<  file_fat16.first_cluster_lo << std::endl
                  << "Number of the first sector:                             "<< file_fat16.first_cluster_lo * boot_sector_t.sectors_per_cluster << std::endl;
    }


    fclose(FAT16);
    return 0;
}