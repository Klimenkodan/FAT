#include <string>
#include <vector>
#include <bitset>
#ifndef FAT_PARSING_FORMATING_FUNCTIONS_H
#define FAT_PARSING_FORMATING_FUNCTIONS_H

std::string file_attributes(uint8_t attributes);
inline std::string date_form(const std::string& date_time);
std::string date_parsing(uint16_t date);
std::string time_parsing(uint16_t time);
#endif //FAT_PARSING_FORMATING_FUNCTIONS_H
