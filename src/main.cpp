#include <iostream>
#include <vector>
#include <memory>

#include "util.hpp"
#include "app.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"
#include "controls/slider.hpp"
#include "controls/scrollbar.hpp"
#include "controls/scrolledbox.hpp"

int main() { 
    Application *app = new Application();
        // Box *top_sizer = new Box(Align::Horizontal);
        //     top_sizer->append(new Button("TTTTTTTTTT"), Fill::Both);
        //     top_sizer->append(new Button("T"), Fill::Both);
        //     top_sizer->append(new Button("T"), Fill::Both);
        //     top_sizer->append(new Button("T"), Fill::Both);
        //     top_sizer->append(new Button("TTTTTTTTTT"), Fill::Both);
        // app->append(top_sizer, Fill::Horizontal);
        // Box *top_sizer = new Box(Align::Horizontal);
        //     top_sizer->append(new Button("T"), Fill::None);
        //     top_sizer->append(new Button("T"), Fill::None);
        //     top_sizer->append(new Button("TTTTTTTTTTT"), Fill::Both);
        //     top_sizer->append(new Button("T"), Fill::None);
        //     top_sizer->append(new Button("T"), Fill::None);
        // app->append(top_sizer, Fill::Horizontal);
        Box *top_sizer = new Box(Align::Vertical);
            // top_sizer->append(new Slider(Align::Vertical, "S"), Fill::Vertical);
            // top_sizer->append(new ScrollBar(Align::Vertical, "SB"), Fill::Vertical);
            for (int i = 0; i < 25; i++) {
                top_sizer->append(new Button(std::to_string(i)), Fill::Both);
            }
            top_sizer->append(new Button("Nick is a homosexual."), Fill::Both);
            // top_sizer->append(new Button("2"), Fill::Both);
            // top_sizer->append(new Button("3"), Fill::Both);
            // top_sizer->append(new Button("4"), Fill::Both);
            // top_sizer->append(new Button("5"), Fill::Both);
        app->append(top_sizer, Fill::Both);
        // Box *bottom_sizer = new Box(Align::Vertical);
        //     bottom_sizer->append(new Button("6"), Fill::Both);
        //     bottom_sizer->append(new Button("7"), Fill::Both);
        //     bottom_sizer->append(new Button("8"), Fill::Both);
        //     bottom_sizer->append(new Button("9"), Fill::Both);
        //     bottom_sizer->append(new Button("10"), Fill::Both);
        // app->append(bottom_sizer, Fill::Both);
    app->run();

    return 0; 
}
