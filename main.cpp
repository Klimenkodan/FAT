#include <iostream>
#include <unistd.h>
#include <vector>
#include <fcntl.h>
#include <algorithm>
#include <bitset>
#include <map>
#include <cstdint>
#include "structures_for_fat16.h"
#include "parsing_formating_functions.h"
#define FAT16_SEC_SIZE 512
#define FAT16_DIR_ENTRY_SIZE 32
#define FILE_NAME_LENGTH 8
#define EXT_LENGTH 3


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



    std::cout << "                            Information about the files in root directory";
    int num_of_files = 0;
    for (int i = 0; i < boot_sector_t.root_entry_count; i++) {
        fread(&file_fat16, 1, 32, FAT16);
        if (file_fat16.attributes == 0) {
            continue;
        }
        num_of_files++;

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
                  << "Number of the first sector:                             "<< file_fat16.first_cluster_lo * boot_sector_t.sectors_per_cluster << std::endl << std::endl;
    }

    std::cout << "                            Information about file system" << std::endl << std::endl
              << "Bytes per sector:                            " << boot_sector_t.bytes_per_sector << std::endl
              << "Sectors per cluster:                         " << static_cast<int>(boot_sector_t.sectors_per_cluster)<< std::endl
              << "Number of reserved sectors:                  " << boot_sector_t.reserved_sector_count << std::endl
              << "Number of FAT copies:                        "<<  static_cast<int>(boot_sector_t.table_count) << std::endl
              << "Number of entries in root directory:         "<<  num_of_files << std::endl
              << "Size of root directory in bytes:             "<<  boot_sector_t.root_entry_count *  FAT16_DIR_ENTRY_SIZE << std::endl
              << "Number of reserved sectors:                  "<<  boot_sector_t.reserved_sector_count << std::endl
              << "Correctness of signature:                    "<<  ((boot_sector_t.boot_sector_signature == 43605) ? "correct" : "incorrect") << std::endl;

    fclose(FAT16);
    return 0;
}