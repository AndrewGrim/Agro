#pragma once

#include <string>

#include "box.hpp"

class GroupBox : public Box {
    public:
        GroupBox(Align align_policy, std::string label);
        ~GroupBox();
        virtual const char* name() override;
        virtual void draw(DrawingContext &dc, Rect rect, int state) override;
        virtual Size sizeHint(DrawingContext &dc) override;

        std::string label;
};
