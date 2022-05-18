#pragma once

#include "../core/string.hpp"

#include "box.hpp"

class GroupBox : public Box {
    public:
        GroupBox(Align align_policy, String label);
        ~GroupBox();
        virtual const char* name() override;
        virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
        virtual Size sizeHint(DrawingContext &dc) override;

        String label;
};
