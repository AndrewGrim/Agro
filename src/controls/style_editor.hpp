#include "../application.hpp"

class StyleEditor : public ScrolledBox {
    public:
        StyleEditor();
        ~StyleEditor();
        static Window* asWindow(const char *title, Size size);
};
