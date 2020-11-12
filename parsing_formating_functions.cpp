
#include "parsing_formating_functions.h"



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