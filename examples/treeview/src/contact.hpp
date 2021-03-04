#pragma once

#include <stdio.h>
#include <ostream>
#include <string>

struct Contact {
    std::string first_name;
    std::string last_name;
    std::string email;
    std::string countrycode;
    std::string id;
    std::string category;

    Contact(int column_count, char **column_values) {
        if (column_count != 6) {
            printf("Error when creating a Contact, wrong number of columns '%d' should be 6!\n", column_count);
            exit(1);
        }
        first_name = column_values[0] ? column_values[0] : "";
        last_name = column_values[1] ? column_values[1] : "";
        email = column_values[2] ? column_values[2] : "";
        countrycode = column_values[3] ? column_values[3] : "";
        id = column_values[4] ? column_values[4] : "";
        category = column_values[5] ? column_values[5] : "";
    }

    ~Contact() {

    }

    friend std::ostream& operator<<(std::ostream &os, Contact &contact) {
        return os << "first_name: " << contact.first_name << ", "
                  << "last_name: " << contact.last_name << ", "
                  << "email: " << contact.email << ", "
                  << "countrycode: " << contact.countrycode << ", "
                  << "id: " << contact.id << ", "
                  << "category: " << contact.category;
    }
};