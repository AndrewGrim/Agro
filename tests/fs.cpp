#include "../src/application.hpp"
#include "../src/core/fs.hpp"

int main(int argc, char **argv) {
    Dir dir(".");
    auto iter = dir.iter();
    while ((iter = iter.next())) {
        printf("entry name: %s, ", iter.entry.name.data());
        printf("entry type: %d\n", (int)iter.entry.type);
    }

    return 0;
}
