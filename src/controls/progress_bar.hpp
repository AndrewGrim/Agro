#ifndef PROGRESS_BAR_HPP
    #define PROGRESS_BAR_HPP

    #include "widget.hpp"

    class ProgressBar : public Widget {
        public:
            ProgressBar(i32 custom_width = 0);
            ~ProgressBar();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual Size sizeHint(DrawingContext &dc) override;

            f64 m_min = 0.0;
            f64 m_max = 1.0;
            f64 m_value = m_min;
            i32 m_custom_width = 0;
    };
#endif
