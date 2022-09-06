#include "../src/application.hpp"
#include "../src/core/fs.hpp"

// TODO would be nice to be able to apply a custom filter
void recurseDir(String path, u32 depth = 0) {
    Dir dir(path);
    auto iter = dir.iter();
    while ((iter = iter.next())) {
        for (u32 i = 0; i < depth; i++) {
            printf(" ");
        }
        printf("%d ", (int)iter.entry.type);
        printf("%s\n", iter.entry.name.data());
        if (iter.entry.type == Dir::Iterator::Entry::Type::Directory) {
            String child_path = String::format("%s/%s", dir.path.data(), iter.entry.name.data());
            recurseDir(child_path, depth + 4);
        }
    }
}

int main(int argc, char **argv) {
    recurseDir(".");
    return 0;
}
