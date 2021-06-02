#ifndef SPACER_HPP
    #define SPACER_HPP

    #include "widget.hpp"

    class Spacer : public Widget {
        public:
            Spacer(Size min_size = Size());
            ~Spacer();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            Size minSize();
            void setMinSize(Size min_size);
    };
#endif
