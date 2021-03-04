#pragma once

#include <stdio.h>
#include <ostream>
#include <string>
#include <vector>

#include <gui/renderer/stb_image.hpp>

struct Flag {
    std::string countrycode;
    std::string filename;
    std::vector<unsigned char> data;

    Flag(int column_count, char **column_values) {
        if (column_count != 5) {
            printf("Error when creating a Flag, wrong number of columns '%d' should be 5!\n", column_count);
            exit(1);
        }
        countrycode = column_values[0] ? column_values[0] : "";
        filename = column_values[1] ? column_values[1] : "";
        for (int i = 0; i < std::stoi(column_values[4]); i++) {
            data.push_back(column_values[2][i]);
        }
    }

    ~Flag() {

    }

    friend std::ostream& operator<<(std::ostream &os, Flag &flag) {
        return os << "countrycode: " << flag.countrycode << ", "
                  << "filename: " << flag.filename << ", "
                  << "data: " << flag.data.size();
    }
};