#pragma once

#include "../renderer/drawing_context.hpp"
#include "../controls/image.hpp"

class CellRenderer : public Drawable {
    public:
        CellRenderer();
        virtual ~CellRenderer();
        virtual void draw(DrawingContext &dc, Rect rect, i32 state) override = 0;
        virtual Size sizeHint(DrawingContext &dc) override = 0;
};

class EmptyCell : public CellRenderer {
    public:
        EmptyCell();
        ~EmptyCell();
        void draw(DrawingContext &dc, Rect rect, i32 state);
        Size sizeHint(DrawingContext &dc);
};

class TextCellRenderer : public CellRenderer {
    public:
        // TODO setters for text, alignment, padding, font to set m_size_changed
        // ^ i could see this being a problem because we dont recalculate
        // the virtual_size for treeview? but maybe im missing something
        std::string text;
        i32 padding;
        std::shared_ptr<Font> font = nullptr;
        HorizontalAlignment h_align = HorizontalAlignment::Left;
        VerticalAlignment v_align = VerticalAlignment::Center;

        TextCellRenderer(
            std::string text,
            i32 padding = 5
        );
        ~TextCellRenderer();
        void draw(DrawingContext &dc, Rect rect, i32 state) override;
        Size sizeHint(DrawingContext &dc) override;

    protected:
        Size m_size;
};

class ImageCellRenderer : public CellRenderer {
    public:
        Image *image = nullptr;

        ImageCellRenderer(Image *image);
        ~ImageCellRenderer();
        void draw(DrawingContext &dc, Rect rect, i32 state) override;
        Size sizeHint(DrawingContext &dc) override;
};

class MultipleImagesCellRenderer : public CellRenderer {
    public:
        std::vector<Image> images;

        MultipleImagesCellRenderer(std::vector<Image> &&images);
        ~MultipleImagesCellRenderer();
        void draw(DrawingContext &dc, Rect rect, i32 state) override;
        Size sizeHint(DrawingContext &dc) override;

    protected:
        Size m_size;
};

class ImageTextCellRenderer : public CellRenderer {
    public:
        // TODO account for alignment, especially important for the first col
        Image *image = nullptr;
        std::shared_ptr<Font> font = nullptr;
        std::string text;
        HorizontalAlignment h_align = HorizontalAlignment::Left;
        i32 padding;

        ImageTextCellRenderer(
            Image *image,
            std::string text,
            HorizontalAlignment h_align = HorizontalAlignment::Left,
            i32 padding = 5
        );
        ~ImageTextCellRenderer();
        void draw(DrawingContext &dc, Rect rect, i32 state) override;
        Size sizeHint(DrawingContext &dc) override;

    protected:
        Size m_size;
};
