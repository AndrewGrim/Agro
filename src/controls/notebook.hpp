#ifndef NOTEBOOK_HPP
    #define NOTEBOOK_HPP

    #include "widget.hpp"
    #include "simplescrollbar.hpp"
    #include "image.hpp"

    class NoteBookTabBar : public Widget {
        public:
            // TODO scrollable interface
            SimpleScrollBar *m_vertical_scrollbar = nullptr;
            SimpleScrollBar *m_horizontal_scrollbar = new SimpleScrollBar(Align::Horizontal);

            NoteBookTabBar();
            ~NoteBookTabBar();
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual const char* name() override;
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual bool isLayout() override;
            virtual void* propagateMouseEvent(State *state, MouseEvent event) override;
    };

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
            virtual bool isLayout() override;
            virtual void* propagateMouseEvent(State *state, MouseEvent event) override;
            virtual Widget* attachApp(void *app) override;
            NoteBook* appendTab(Widget *root, std::string text, Image *icon = nullptr);
            NoteBook* insertTab(Widget *root, std::string text, Image *icon = nullptr);
            NoteBook* removeTab(int index);
            NoteBook* setTabWidget(Widget *root);
            NoteBook* setTabText(std::string text);
            NoteBook* setTabIcon(Image *icon);
            int currentTab();
            NoteBook* setCurrentTab(int index);

        protected:
            // TODO need to increment
            int m_tab_index = 0;
            NoteBookTabBar *m_tabs = new NoteBookTabBar();
    };
#endif