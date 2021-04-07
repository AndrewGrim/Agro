#include "util.hpp"
#include "application.hpp"
#include "controls/slider.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->resize(400, 400);
        auto h = new Slider(Align::Horizontal);
            h->m_slider_button_size = 50;
        app->append(h, Fill::Horizontal);
        auto v = new Slider(Align::Vertical);
            v->m_slider_button_size = 50;
        app->append(v, Fill::Vertical);
    app->run();

    return 0;
}