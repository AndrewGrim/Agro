#ifndef AGRO_FS_HPP
    #define AGRO_FS_HPP

    #include "string.hpp"
    #include "../result.hpp"

    namespace fs {
        enum class Error {
            Open,
            IsDir,
            InvalidUtf8
        };
    }

    #ifdef __WIN32__
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>

        namespace fs {
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

            struct File {
                FILE *_f = nullptr;

                File() {}

                /*
                    "r"     Opens for reading. If the file doesn't exist or can't be found, the fopen call fails.
                    "w"     Opens an empty file for writing. If the given file exists, its contents are destroyed.
                    "a"     Opens for writing at the end of the file (appending) without removing the end-of-file (EOF) marker before new data is written to the file. Creates the file if it doesn't exist.
                    "r+"    Opens for both reading and writing. The file must exist.
                    "w+"    Opens an empty file for both reading and writing. If the file exists, its contents are destroyed.
                    "a+"    Opens for reading and appending. The appending operation includes the removal of the EOF marker before new data is written to the file. The EOF marker isn't restored after writing is completed. Creates the file if it doesn't exist.
                */
                File(String path, String mode) {
                    _f = _wfopen((wchar_t*)path.toUtf16Le().data(), (wchar_t*)mode.toUtf16Le().data());
                }

                File(const File &rhs) = delete;

                File(File &&rhs) {
                    FILE *temp = rhs._f;
                    rhs._f = _f;
                    _f = temp;
                }

                ~File() {
                    if (_f) { fclose(_f); }
                }

                template <typename T> usize read(void *buffer, usize count) {
                    return fread(buffer, sizeof(T), count, _f);
                }

                template <typename T> usize write(const void *buffer, usize count) {
                    return fwrite(buffer, sizeof(T), count, _f);
                }

                // Where origin: SEEK_SET, SEEK_CUR, SEEK_END
                i32 seek(i64 offset, i32 origin) {
                    return fseek(_f, offset, origin);
                }

                File& operator=(const File &rhs) = delete;

                File& operator=(File &&rhs) {
                    FILE *temp = rhs._f;
                    rhs._f = _f;
                    _f = temp;
                    return *this;
                }

                static Result<fs::Error, File> open(String path, String mode) {
                    File f = File(path, mode);
                    if (!f._f) { return Result<fs::Error, File>(fs::Error::Open); }
                    return Result<fs::Error, File>(std::move(f));
                }
            };

            Result<fs::Error, u64> getSize(String path) {
                struct _stat data;
                if (_wstat((wchar_t*)path.toUtf16Le().data(), &data)) { return Result<fs::Error, u64>(fs::Error::Open); }
                if (data.st_mode & _S_IFDIR) { return Result<fs::Error, u64>(fs::Error::IsDir); }
                return Result<fs::Error, u64>(data.st_size);
            }

            Result<fs::Error, String> load(String path) {
                Result<fs::Error, File> file = fs::File::open(path, "rb");
                if (!file) { return Result<fs::Error, String>(fs::Error::Open); }
                Result<fs::Error, u64> size = fs::getSize(path);
                if (!size) { return Result<fs::Error, String>(fs::Error::Open); }
                String s = String(size.value);
                file.value.read<u8>(s.data(), size.value);
                s.data()[s.size()] = '\0';
                if (!utf8::validate(s)) { return Result<fs::Error, String>(fs::Error::InvalidUtf8); }
                return Result<fs::Error, String>(std::move(s));
            }

            Result<fs::Error, String> loadAsRawBytes(String path) {
                Result<fs::Error, File> file = fs::File::open(path, "rb");
                if (!file) { return Result<fs::Error, String>(fs::Error::Open); }
                Result<fs::Error, u64> size = fs::getSize(path);
                if (!size) { return Result<fs::Error, String>(fs::Error::Open); }
                String s = String(size.value);
                file.value.read<u8>(s.data(), size.value);
                s.data()[s.size()] = '\0';
                return Result<fs::Error, String>(std::move(s));
            }
        }
    #else
        #include <dirent.h>
        #include <sys/stat.h>

        namespace fs {
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

            struct File {
                FILE *_f = nullptr;

                File() {}

                /*
                    "r"     Opens for reading. If the file doesn't exist or can't be found, the fopen call fails.
                    "w"     Opens an empty file for writing. If the given file exists, its contents are destroyed.
                    "a"     Opens for writing at the end of the file (appending) without removing the end-of-file (EOF) marker before new data is written to the file. Creates the file if it doesn't exist.
                    "r+"    Opens for both reading and writing. The file must exist.
                    "w+"    Opens an empty file for both reading and writing. If the file exists, its contents are destroyed.
                    "a+"    Opens for reading and appending. The appending operation includes the removal of the EOF marker before new data is written to the file. The EOF marker isn't restored after writing is completed. Creates the file if it doesn't exist.
                */
                File(String path, String mode) {
                    _f = fopen(path.data(), mode.data());
                }

                File(const File &rhs) = delete;

                File(File &&rhs) {
                    FILE *temp = rhs._f;
                    rhs._f = _f;
                    _f = temp;
                }

                ~File() {
                    if (_f) { fclose(_f); }
                }

                template <typename T> usize read(void *buffer, usize count) {
                    return fread(buffer, sizeof(T), count, _f);
                }

                template <typename T> usize write(const void *buffer, usize count) {
                    return fwrite(buffer, sizeof(T), count, _f);
                }

                // Where origin: SEEK_SET, SEEK_CUR, SEEK_END
                i32 seek(i64 offset, i32 origin) {
                    return fseek(_f, offset, origin);
                }

                File& operator=(const File &rhs) = delete;

                File& operator=(File &&rhs) {
                    FILE *temp = rhs._f;
                    rhs._f = _f;
                    _f = temp;
                    return *this;
                }

                static Result<fs::Error, File> open(String path, String mode) {
                    File f = File(path, mode);
                    if (!f._f) { return Result<fs::Error, File>(fs::Error::Open); }
                    return Result<fs::Error, File>(std::move(f));
                }
            };

            Result<fs::Error, u64> getSize(String path) {
                struct stat data;
                if (stat(path.data(), &data)) { return Result<fs::Error, u64>(fs::Error::Open); }
                if (S_ISDIR(data.st_mode)) { return Result<fs::Error, u64>(fs::Error::IsDir); }
                return Result<fs::Error, u64>(data.st_size);
            }

            Result<fs::Error, String> load(String path) {
                Result<fs::Error, File> file = fs::File::open(path, "rb");
                if (!file) { return Result<fs::Error, String>(fs::Error::Open); }
                Result<fs::Error, u64> size = fs::getSize(path);
                if (!size) { return Result<fs::Error, String>(fs::Error::Open); }
                String s = String(size.value);
                file.value.read<u8>(s.data(), size.value);
                s.data()[s.size()] = '\0';
                if (!utf8::validate(s)) { return Result<fs::Error, String>(fs::Error::InvalidUtf8); }
                return Result<fs::Error, String>(std::move(s));
            }

            Result<fs::Error, String> loadAsRawBytes(String path) {
                Result<fs::Error, File> file = fs::File::open(path, "rb");
                if (!file) { return Result<fs::Error, String>(fs::Error::Open); }
                Result<fs::Error, u64> size = fs::getSize(path);
                if (!size) { return Result<fs::Error, String>(fs::Error::Open); }
                String s = String(size.value);
                file.value.read<u8>(s.data(), size.value);
                s.data()[s.size()] = '\0';
                return Result<fs::Error, String>(std::move(s));
            }
        }
    #endif
#endif
