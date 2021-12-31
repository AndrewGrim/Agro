#ifndef CHECK_BUTTON_HPP
    #define CHECK_BUTTON_HPP

    #include <string>

    #include "widget.hpp"
    #include "image.hpp"

    class CheckButton : public Widget {
        public:
            EventListener<bool> onValueChanged = EventListener<bool>();

            CheckButton(std::string text, bool is_checked = false);
            ~CheckButton();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            int isFocusable() override;

            std::string m_text = "";
            Image *m_checked_image = nullptr;
            Image *m_unchecked_image = nullptr;
            bool m_is_checked = false;
            int m_tab_width = 4;
            int m_padding = 5;
    };
#endif
