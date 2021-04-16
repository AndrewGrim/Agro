#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include <sqlite3/sqlite3.h>

#include "contact.hpp"
#include "flag.hpp"

static int contact_callback(void *user_data, int column_count, char **column_values, char **column_names) {
    if (std::vector<Contact> *contacts = static_cast<std::vector<Contact>*>(user_data)) {
        contacts->push_back(Contact(column_count, column_values));
        return 0;
    }
    printf("Failed to cast back user_data!\n");
    exit(1);
}

static int flag_callback(void *user_data, int column_count, char **column_values, char **column_names) {
    if (std::unordered_map<std::string, Flag> *flags = static_cast<std::unordered_map<std::string, Flag>*>(user_data)) {
        Flag f = Flag(column_count, column_values);
        flags->insert(std::make_pair(f.countrycode, f));
        return 0;
    }
    printf("Failed to cast back user_data!\n");
    exit(1);
}

static int texture_callback(void *user_data, int column_count, char **column_values, char **column_names) {
    if (std::unordered_map<std::string, std::shared_ptr<Texture>> *textures = static_cast<std::unordered_map<std::string, std::shared_ptr<Texture>>*>(user_data)) {
        textures->insert(std::make_pair(column_values[0], std::make_shared<Texture>(true, (unsigned char*)column_values[2], std::stoi(column_values[4]))));
        return 0;
    }
    printf("Failed to cast back user_data!\n");
    exit(1);
}

class Database {
    public:
        Database(std::string path) {
            m_conn = sqlite3_open(path.c_str(), &m_db);
            if (m_conn) {
                printf("Can't open database: %s\n", sqlite3_errmsg(m_db));
                exit(1);
            }
        }

        ~Database() {
            sqlite3_close(m_db);
        }

        std::vector<Contact> loadAllContacts() {
            std::vector<Contact> contacts;
            m_conn = sqlite3_exec(m_db, "SELECT * FROM contacts ORDER BY first_name;", contact_callback, &contacts, &m_error);
            if (m_conn != SQLITE_OK) {
                printf("SQL error: %s\n", m_error);
                sqlite3_free(m_error);
                exit(1);
            }

            return contacts;
        }

        std::unordered_map<std::string, Flag> loadAllFlags() {
            std::unordered_map<std::string, Flag> flags;
            m_conn = sqlite3_exec(m_db, "SELECT *, length(flag_png_) FROM countries;", flag_callback, &flags, &m_error);
            if (m_conn != SQLITE_OK) {
                printf("SQL error: %s\n", m_error);
                sqlite3_free(m_error);
                exit(1);
            }

            return flags;
        }

        std::unordered_map<std::string, std::shared_ptr<Texture>> loadAllTextures() {
            std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
            m_conn = sqlite3_exec(m_db, "SELECT *, length(flag_png_) FROM countries;", texture_callback, &textures, &m_error);
            if (m_conn != SQLITE_OK) {
                printf("SQL error: %s\n", m_error);
                sqlite3_free(m_error);
                exit(1);
            }

            return textures;
        }

    private:
        sqlite3 *m_db = nullptr;
        int m_conn = -1;
        char *m_error = nullptr;
};