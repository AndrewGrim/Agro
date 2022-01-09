#pragma once

#include "widget.hpp"

class Splitter : public Widget {
    public:
        Splitter(Align alignment, Size min_size = Size(100, 100));
        ~Splitter();
        virtual const char* name() override;
        virtual void draw(DrawingContext &dc, Rect rect, int state) override;
        virtual Size sizeHint(DrawingContext &dc) override;
        virtual Widget* propagateMouseEvent(Window *window, State *state, MouseEvent event) override;
        virtual bool isLayout() override;
        virtual int isFocusable() override;
        virtual Widget* handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) override;

        Widget* append(Widget *widget, Fill fill_policy, unsigned int proportion) override;
        void top(Widget *widget);
        void bottom(Widget *widget);
        void left(Widget *widget);
        void right(Widget *widget);
        void setSplit(double new_split);
        double split();

        Align m_align_policy;
        Size m_viewport;
        Widget *m_first = nullptr;
        Widget *m_second = nullptr;
        int m_sash_size = 10;
        double m_split = 0.5;
        bool m_dragging = false;
};
