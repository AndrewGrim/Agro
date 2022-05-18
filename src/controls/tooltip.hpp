#ifndef TOOLTIP_HPP
    #define TOOLTIP_HPP

    #include "label.hpp"

    class Tooltip : public Label {
        public:
            Tooltip(String text);
            ~Tooltip();
            virtual const char* name() override;
    };
#endif
