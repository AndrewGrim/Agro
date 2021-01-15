#ifndef NOTEBOOK_HPP
    #define NOTEBOOK_HPP

    #include "widget.hpp"
    #include "image.hpp"

    struct NoteBookTab {
        Widget *root;
        std::string text;
        Image *icon;
    };

    class NoteBook : public Widget {
        public:
            NoteBook();
            ~NoteBook();
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual const char* name() override;
            virtual Size sizeHint(DrawingContext *dc) override;
            NoteBook* appendTab(Widget *root, std::string text, Image *icon = nullptr);
            NoteBook* insertTab(Widget *root, std::string text, Image *icon = nullptr);
            NoteBook* removeTab(int index);
            NoteBook* setRootWidget(Widget *root);
            NoteBook* setTabText(std::string text);
            NoteBook* setTabIcon(Image *icon);
            int currentTab();
            NoteBook* setCurrentTab(int index);

        protected:
            int m_tab_index = 0;
            std::vector<NoteBookTab> m_tabs;
    };
#endif