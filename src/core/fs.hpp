#ifndef AGRO_FS_HPP
    #define AGRO_FS_HPP

    #include "string.hpp"

    #ifdef __WIN32__
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>

        struct Dir {
            struct Iterator {
                struct Entry {
                    enum class  Type {
                        None,
                        File,
                        Directory,
                        Other,
                    };

                    String name;
                    Type type = Type::None;

                    Entry() {}
                    Entry(String name, Type type) : name{name}, type{type} {}
                    ~Entry() {}
                };

                LPWIN32_FIND_DATAW data;
                HANDLE handle = INVALID_HANDLE_VALUE;
                Entry entry;
                bool is_done = false;
                bool one_more = false;

                Iterator(HANDLE handle, LPWIN32_FIND_DATAW data) : handle{handle}, data{data} {}
                ~Iterator() {}

                Iterator next() {
                    do {
                        if (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                            // Skip over the "." current dir and the ".." parent dir.
                            if (memcmp(data->cFileName, ".\0.\0", 4) == 0 || memcmp(data->cFileName, ".\0\0\0", 4) == 0) { continue; }
                            entry = Entry(String(data->cFileName).toUtf8(), Entry::Type::Directory);
                            if (one_more) { one_more = false; return *this; }
                            is_done = FindNextFileW(handle, data);
                            if (!is_done) { one_more = true; }
                            return *this;
                        // TODO this might not be ideal we can maybe omit some stuff
                        // https://docs.microsoft.com/en-us/windows/win32/fileio/file-attribute-constants
                        } else {
                            entry = Entry(String(data->cFileName).toUtf8(), Entry::Type::File);
                            if (one_more) { one_more = false; return *this; }
                            is_done = FindNextFileW(handle, data);
                            if (!is_done) { one_more = true; }
                            return *this;
                        }
                    } while ((is_done = FindNextFileW(handle, data)));
                    return *this;
                }

                operator bool() {
                    return one_more || is_done;
                }
            };

            WIN32_FIND_DATAW data;
            HANDLE handle = INVALID_HANDLE_VALUE;
            String path;

            Dir(String _path) {
                path = _path;
                if (path.size() + 4 > MAX_PATH) {
                    assert(false && "Path exceeds MAX_PATH!");
                }
            }

            ~Dir() {
                FindClose(handle);
            }

            Iterator iter() {
                handle = FindFirstFileW(
                    // Note: On windows trying to query contents of a directory without \* at the end will not produce any results.
                    (LPCWSTR)(path + "\\*").toUtf16Le().data(),
                    (LPWIN32_FIND_DATAW)&data
                );
                if (handle == INVALID_HANDLE_VALUE) {
                    assert(false && "INVALID_HANDLE_VALUE FindFirstFileW!");
                }
                return Iterator(handle, (LPWIN32_FIND_DATAW)&data);
            }
        };
    #else
        #include <dirent.h>

        struct Dir {
            struct Iterator {
                struct Entry {
                    enum class  Type {
                        None,
                        File,
                        Directory,
                        Other,
                    };

                    String name;
                    Type type = Type::None;

                    Entry() {}
                    Entry(String name, Type type) : name{name}, type{type} {}
                    ~Entry() {}
                };

                dirent *data = nullptr;
                DIR *handle = nullptr;
                Entry entry;

                Iterator(DIR *handle) : handle{handle} {}
                ~Iterator() {}

                Iterator next() {
                    while ((data = readdir(handle))) {
                        if (data->d_type == DT_REG) {
                            entry = Entry(data->d_name, Entry::Type::File);
                            return *this;
                        } else if (data->d_type == DT_DIR) {
                            // Skip over the "." current dir and the ".." parent dir.
                            if (memcmp(data->d_name, "..", 2) == 0 || memcmp(data->d_name, ".\0", 2) == 0) { continue; }
                            entry = Entry(data->d_name, Entry::Type::Directory);
                            return *this;
                        }
                    }
                    return *this;
                }

                operator bool() {
                    return data;
                }
            };

            String path;
            DIR *handle = nullptr;

            Dir(String _path) {
                path = _path;
                handle = opendir(path.data());
                if (!handle) {
                    assert(false && "Couldn't open directory!");
                }
            }

            ~Dir() {
                closedir(handle);
            }

            Iterator iter() { return Iterator(handle); }
        };
    #endif
#endif
