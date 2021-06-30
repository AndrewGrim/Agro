#ifndef PROGRESS_BAR_HPP
    #define PROGRESS_BAR_HPP

    #include "widget.hpp"

    class ProgressBar : public Widget {
        public:
            ProgressBar(int custom_width = 0);
            ~ProgressBar();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;

            double m_min = 0.0;
            double m_max = 1.0;
            double m_value = m_min;
            int m_custom_width = 0;
    };
#endif
