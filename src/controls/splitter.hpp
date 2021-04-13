#pragma once

#include "widget.hpp"

class Splitter : public Widget {
    public:
        Splitter(Align alignment, Size min_size = Size(100, 100));
        ~Splitter();
        virtual const char* name() override;
        virtual void draw(DrawingContext *dc, Rect rect) override;
        virtual Size sizeHint(DrawingContext *dc);
        virtual void* propagateMouseEvent(Window *window, State *state, MouseEvent event);
        virtual  bool isLayout() override;
        void append();
        void top(Widget *widget);
        void bottom(Widget *widget);
        void left(Widget *widget);
        void right(Widget *widget);
        void setSplit(float new_split);
        float split();

    protected:
        Align m_align_policy;
        Size m_viewport;
        Widget *m_first = nullptr;
        Widget *m_second = nullptr;
        float m_sash_size = 10;
        float m_split = 0.5;
        bool m_dragging = false;
};