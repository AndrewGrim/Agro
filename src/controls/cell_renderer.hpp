#ifndef CELL_RENDERER_HPP
    #define CELL_RENDERER_HPP

    #include "../renderer/drawing_context.hpp"
    #include "../controls/image.hpp"
    #include "../core/string.hpp"

    struct CellRenderer : public Drawable {
        CellRenderer();
        virtual ~CellRenderer();
        virtual void draw(DrawingContext &dc, Rect rect, i32 state) override = 0;
        virtual Size sizeHint(DrawingContext &dc) override = 0;
    };

    struct Cells {
        std::shared_ptr<std::vector<CellRenderer*>> data = std::make_shared<std::vector<CellRenderer*>>();
        Size *max_cell_size = new Size();

        Cells();

        ~Cells();
        void append(CellRenderer *cell);
        CellRenderer* operator[](usize index);
        void clear();
    };

    struct EmptyCell : public CellRenderer {
        EmptyCell();
        ~EmptyCell();
        void draw(DrawingContext &dc, Rect rect, i32 state);
        Size sizeHint(DrawingContext &dc);
    };

    struct TextCellRenderer : public CellRenderer {
        // TODO setters for text, alignment to set m_size_changed
        // ^ i could see this being a problem because we dont recalculate
        // the virtual_size for treeview? but maybe im missing something
        String text;
        HorizontalAlignment h_align = HorizontalAlignment::Left;
        VerticalAlignment v_align = VerticalAlignment::Center;

        TextCellRenderer(String text);
        ~TextCellRenderer();
        void draw(DrawingContext &dc, Rect rect, i32 state) override;
        Size sizeHint(DrawingContext &dc) override;
    };

    struct ImageCellRenderer : public CellRenderer {
        Image *image = nullptr;

        ImageCellRenderer(Image *image);
        ~ImageCellRenderer();
        void draw(DrawingContext &dc, Rect rect, i32 state) override;
        Size sizeHint(DrawingContext &dc) override;
    };

    struct MultipleImagesCellRenderer : public CellRenderer {
        std::vector<Image> images;

        MultipleImagesCellRenderer(std::vector<Image> &&images);
        ~MultipleImagesCellRenderer();
        void draw(DrawingContext &dc, Rect rect, i32 state) override;
        Size sizeHint(DrawingContext &dc) override;
    };

    struct ImageTextCellRenderer : public CellRenderer {
        // TODO account for alignment, especially important for the first col
        Image *image = nullptr;
        String text;
        // TODO maybe we want to move alignment into style? at least for text
        HorizontalAlignment h_align = HorizontalAlignment::Left;

        ImageTextCellRenderer(
            Image *image,
            String text,
            HorizontalAlignment h_align = HorizontalAlignment::Left
        );
        ~ImageTextCellRenderer();
        void draw(DrawingContext &dc, Rect rect, i32 state) override;
        Size sizeHint(DrawingContext &dc) override;
    };
#endif
