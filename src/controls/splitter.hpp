#pragma once

#include "widget.hpp"

class Splitter : public Widget {
    public:
        Splitter(Align alignment, Size min_size = Size(100, 100));
        ~Splitter();
        virtual const char* name() override;
        virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
        virtual Size sizeHint(DrawingContext &dc) override;
        virtual Widget* propagateMouseEvent(Window *window, State *state, MouseEvent event) override;
        virtual bool isLayout() override;
        virtual i32 isFocusable() override;
        virtual Widget* handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) override;
        void forEachDrawable(std::function<void(Drawable *drawable)> action) override;

        Widget* append(Widget *widget, Fill fill_policy, u32 proportion) override;
        void top(Widget *widget);
        void bottom(Widget *widget);
        void left(Widget *widget);
        void right(Widget *widget);
        void setSplit(f64 new_split);
        f64 split();

        Align m_align_policy;
        Size m_viewport;
        Widget *m_first = nullptr;
        Widget *m_second = nullptr;
        i32 m_sash_size = 10;
        f64 m_split = 0.5;
        bool m_dragging = false;
};
