#include <algorithm>

#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/line_edit.hpp"
#include "../src/controls/scrolled_box.hpp"
#include "../src/controls/label.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("LineEdit");
        std::vector<std::string> monster_names = {
            "Great Jagras", "Kulu-Ya-Ku", "Pukei-Pukei", "Barroth",
            "Jyuratodus", "Tobi-Kadachi", "Anjanath", "Rathian",
            "Tzitzi-Ya-Ku", "Paolumu", "Great Girros", "Radobaan",
            "Legiana", "Odogaron", "Rathalos", "Diablos", "Kirin",
            "Zorah Magdaros", "Dodogama", "Pink Rathian", "Bazelgeuse",
            "Deviljho", "Lavasioth", "Uragaan", "Azure Rathalos", "Black Diablos",
            "Nergigante", "Teostra", "Lunastra", "Kushala Daora", "Vaal Hazak",
            "Kulve Taroth", "Xeno'jiiva", "Behemoth", "Leshen", "Ancient Leshen",
        };
        sort(monster_names.begin(), monster_names.end());
        ScrolledBox *results_view;
        LineEdit *edit = new LineEdit();
            edit->setPlaceholderText("Search by name");
            edit->onTextChanged = [&]{
                while (results_view->children.size()) {
                    results_view->children[0]->destroy();
                }
                for (auto m : monster_names) {
                    auto monster = m;
                    std::transform(monster.begin(), monster.end(), monster.begin(), [](unsigned char c){
                        return std::tolower(c);
                    });
                    auto search_phrase = edit->text();
                    std::transform(search_phrase.begin(), search_phrase.end(), search_phrase.begin(), [](unsigned char c){
                        return std::tolower(c);
                    });
                    if (monster.find(search_phrase) != std::string::npos || !search_phrase.size()) {
                        results_view->append(new Label(m));
                    }
                }
            };
        app->append(edit, Fill::Horizontal);
        results_view = new ScrolledBox(Align::Vertical);
            for (auto m : monster_names) {
                results_view->append(new Label(m));
            }
        app->append(results_view, Fill::Both);
    app->run();

    return 0;
}