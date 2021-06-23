#ifndef NOTEBOOK_HPP
    #define NOTEBOOK_HPP

    #include "widget.hpp"
    #include "scroll_bar.hpp"
    #include "image.hpp"
    #include "icon_button.hpp"

    class NoteBookTabBar : public Widget {
        public:
            SimpleScrollBar *m_horizontal_scrollbar = nullptr;

            NoteBookTabBar();
            ~NoteBookTabBar();
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual const char* name() override;
            virtual Size sizeHint(DrawingContext &dc) override;
            virtual bool isLayout() override;
            virtual void* propagateMouseEvent(Window *window, State *state, MouseEvent event) override;
            virtual bool handleScrollEvent(ScrollEvent event) override;
    };

    class NoteBook : public Widget {
        public:
            NoteBook();
            ~NoteBook();
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual const char* name() override;
            virtual Size sizeHint(DrawingContext &dc) override;
            virtual bool isLayout() override;
            virtual void* propagateMouseEvent(Window *window, State *state, MouseEvent event) override;
            NoteBook* appendTab(Widget *root, std::string text, Image *icon = nullptr, bool close_button = false);
            // NoteBook* insertTab(Widget *root, std::string text, Image *icon = nullptr, bool close_button = false);
            NoteBook* destroyTab(size_t index);
            // NoteBook* setTabWidget(Widget *root);
            size_t currentTab();
            NoteBook* setCurrentTab(size_t index);

        protected:
            size_t m_tab_index = 0;
            NoteBookTabBar *m_tabs = new NoteBookTabBar();
    };

    class NoteBookTabButton : public Button {
        public:
            NoteBookTabButton(NoteBook *notebook, std::string text, Image *image = nullptr, bool close_button = false);
            ~NoteBookTabButton();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            virtual bool isLayout() override;
            virtual void* propagateMouseEvent(Window *window, State *state, MouseEvent event) override;
            bool isActive();
            void setActive(bool is_active);
            bool hasCloseButton();
            void setCloseButton(bool close_button);

        protected:
            bool m_is_active = false;
            bool m_close_button = false;
            IconButton *m_close_image = nullptr;
    };
#endif
