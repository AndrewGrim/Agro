#include <algorithm>

#include "../src/util.hpp"
#include "../src/app.hpp"
#include "../src/controls/lineedit.hpp"
#include "../src/controls/scrolledbox.hpp"
#include "../src/controls/label.hpp"

int main() { 
    Application *app = new Application("LineEdit Search Test", Size(400, 400));
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
                    auto compare = m;
                    std::transform(compare.begin(), compare.end(), compare.begin(), [](unsigned char c){
                        return std::tolower(c);
                    });
                    if (compare.find(edit->text()) != std::string::npos || !edit->text().size()) {
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

    #ifdef TEST
        #include "headless.hpp"
    #else
        app->run();
    #endif

    return 0; 
}
