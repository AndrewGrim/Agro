#include <algorithm>

#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/text_edit.hpp"
#include "../src/controls/scrolled_box.hpp"
#include "../src/controls/label.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (String(argv[1]) == "quit") {
                    window->quit();
                }
            }
        };
        app->mainWindow()->setTitle("TextEdit");
        std::vector<String> monster_names = {
            "Great Jagras", "Kulu-Ya-Ku", "Pukei-Pukei", "Barroth",
            "Jyuratodus", "Tobi-Kadachi", "Anjanath", "Rathian",
            "Tzitzi-Ya-Ku", "Paolumu", "Great Girros", "Radobaan",
            "Legiana", "Odogaron", "Rathalos", "Diablos", "Kirin",
            "Zorah Magdaros", "Dodogama", "Pink Rathian", "Bazelgeuse",
            "Deviljho", "Lavasioth", "Uragaan", "Azure Rathalos", "Black Diablos",
            "Nergigante", "Teostra", "Lunastra", "Kushala Daora", "Vaal Hazak",
            "Kulve Taroth", "Xeno'jiiva", "Behemoth", "Leshen", "Ancient Leshen",
        };
        // sort(monster_names.begin(), monster_names.end());
        ScrolledBox *results_view;
        TextEdit *edit = new TextEdit();
            edit->setPlaceholderText("Search by name");
            edit->onTextChanged.addEventListener([&]{
                while (results_view->children.size()) {
                    results_view->children[0]->destroy();
                }
                String search_phrase = edit->text().toLower();
                for (String m : monster_names) {
                    String monster = String(m).toLower();

                    if (monster.find(search_phrase)) {
                        results_view->append(new Label(m));
                    }
                }
            });
        app->mainWindow()->append(edit, Fill::Horizontal);
        results_view = new ScrolledBox(Align::Vertical);
            for (auto m : monster_names) {
                results_view->append(new Label(String(m.data())));
            }
        app->mainWindow()->append(results_view, Fill::Both);
    app->run();

    return 0;
}
