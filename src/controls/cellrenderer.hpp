#pragma once

#include "../renderer/drawing_context.hpp"
#include "../controls/image.hpp"

class CellRenderer {
    public:
        enum State {
            STATE_DEFAULT = 0x0000,
            STATE_HOVERED = 0x0010,
            STATE_SELECTED = 0x0100,
        };

        CellRenderer() {}
        virtual ~CellRenderer() {}
        virtual void draw(DrawingContext *dc, Rect rect, int state) = 0;
        virtual Size sizeHint(DrawingContext *dc) = 0;
};

class EmptyCell : public CellRenderer {
    public:
        EmptyCell();
        ~EmptyCell();
        void draw(DrawingContext *dc, Rect rect, int state);
        Size sizeHint(DrawingContext *dc);
};

class TextCellRenderer : public CellRenderer {
    public:
        // TODO setters for text, alignment, padding, font to set m_size_changed
        std::string text;
        Color foreground;
        Color background;
        int padding;
        Font *font = nullptr;
        HorizontalAlignment h_align = HorizontalAlignment::Left;
        VerticalAlignment v_align = VerticalAlignment::Center;

        TextCellRenderer(
            std::string text, 
            Color foreground = COLOR_BLACK, 
            Color background = COLOR_NONE, 
            int padding = 5
        );
        ~TextCellRenderer();
        void draw(DrawingContext *dc, Rect rect, int state) override;
        Size sizeHint(DrawingContext *dc) override;

    protected:
        Size m_size;
        bool m_size_changed = true;
};

class ImageCellRenderer : public CellRenderer {
    public:
        Image *image = nullptr;

        // TOOD why? lol
        // because its just passing the parameters to image constructors
        // we could just pass image and nothing else
        ImageCellRenderer(std::string file_path);
        ImageCellRenderer(bool from_memory, const unsigned char *image_data, int length);
        ImageCellRenderer(std::shared_ptr<Texture> texture);
        ImageCellRenderer(Image *img);
        ~ImageCellRenderer();
        void draw(DrawingContext *dc, Rect rect, int state) override;
        Size sizeHint(DrawingContext *dc) override;
};

class MultipleImagesCellRenderer : public CellRenderer {
    public:
        std::vector<Image> images;

        MultipleImagesCellRenderer(std::vector<Image> &&images);
        ~MultipleImagesCellRenderer();
        void draw(DrawingContext *dc, Rect rect, int state) override;
        Size sizeHint(DrawingContext *dc) override;

    protected:
        Size m_size;
        bool m_size_changed = true;
};

class ImageTextCellRenderer : public CellRenderer {
    public:
        // TODO account for alignment, especially important for the first col
        Image *image = nullptr;
        Font *font = nullptr;
        std::string text; 
        Color foreground;
        Color background;
        HorizontalAlignment h_align = HorizontalAlignment::Left;
        int padding;

        ImageTextCellRenderer(
            Image *image,
            std::string text, 
            Color foreground = COLOR_BLACK, 
            Color background = COLOR_NONE, 
            HorizontalAlignment h_align = HorizontalAlignment::Left,
            int padding = 5
        );
        ~ImageTextCellRenderer();
        void draw(DrawingContext *dc, Rect rect, int state) override;
        Size sizeHint(DrawingContext *dc) override;

    protected:
        Size m_size;
        bool m_size_changed = true;
};