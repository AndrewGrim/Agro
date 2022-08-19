#include "../src/util.hpp"
#include "../src/stop_watch.hpp"
#include "../src/application.hpp"
#include "../src/controls/code_edit.hpp"

String loadFileToString(const char *path) {
    FILE *file = fopen(path, "rb");
    // u64 size = 0;
    // while (fgetc(file) != EOF) {
    //     size++;
    // }
    // fseek(file, 0, 0);
    u64 size = 8462111;
    // u64 size = 64496;
    String s = String(size);
    fread(s.data(), sizeof(u8), size, file);
    s.data()[s.size()] = '\0';
    return s;
}

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (String(argv[1]) == "quit") {
                    window->quit();
                }
            }
        };
        app->mainWindow()->resize(600, 600);
        app->mainWindow()->center();
        std::shared_ptr<Font> mono = std::shared_ptr<Font>(new Font(Application::get(), "fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 14, Font::Type::Mono));
        CodeEdit *edit = new CodeEdit(loadFileToString("../c_lexer/sqlite3.c"), Size(200, 200));
        // CodeEdit *edit = new CodeEdit(loadFileToString("src/controls/tree_view.hpp"), Size(200, 200));
        edit->setFont(mono);
        app->mainWindow()->append(edit, Fill::Both);
    app->run();

    return 0;
}
