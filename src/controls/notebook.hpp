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
            virtual bool handleScrollEvent(ScrollEvent event) override;
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
            NoteBook* appendTab(Widget *root, std::string text, Image *icon = nullptr, bool close_button = false);
            // NoteBook* insertTab(Widget *root, std::string text, Image *icon = nullptr, bool close_button = false);
            NoteBook* destroyTab(int index);
            // NoteBook* setTabWidget(Widget *root);
            int currentTab();
            NoteBook* setCurrentTab(int index);

        protected:
            int m_tab_index = 0;
            NoteBookTabBar *m_tabs = new NoteBookTabBar();
    };

    class NoteBookTabButton : public Button {
        public:
            NoteBookTabButton(NoteBook *notebook, std::string text, Image *image = nullptr, bool close_button = false);
            ~NoteBookTabButton();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual bool isLayout() override;
            virtual void* propagateMouseEvent(State *state, MouseEvent event) override;
            bool isActive();
            void setActive(bool is_active);
            bool hasCloseButton();
            void setCloseButton(bool close_button);

        protected:
            bool m_is_active = false;
            bool m_close_button = false;
            Image *m_close_image = nullptr;
    };
#endif