#ifndef RADIO_BUTTON_HPP
    #define RADIO_BUTTON_HPP

    #include <string>

    #include "check_button.hpp"

    class RadioButton;

    struct RadioGroup {
        EventListener<RadioButton*> onValueChanged = EventListener<RadioButton*>();
        std::vector<RadioButton*> buttons;
        RadioGroup() {}
    };

    class RadioButton : public CheckButton {
        public:
            RadioButton(std::shared_ptr<RadioGroup> group, std::string text);
            ~RadioButton();
            virtual const char* name() override;
            void draw(DrawingContext &dc, Rect rect, i32 state) override;

            std::shared_ptr<RadioGroup> m_group;
            Image *m_background_image = nullptr;
    };
#endif
