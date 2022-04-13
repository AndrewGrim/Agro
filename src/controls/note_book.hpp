#ifndef NOTEBOOK_HPP
    #define NOTEBOOK_HPP

    #include "widget.hpp"
    #include "scroll_bar.hpp"
    #include "image.hpp"
    #include "icon_button.hpp"

    class NoteBookTabBar : public Widget {
        public:
            SimpleScrollBar *m_horizontal_scrollbar = nullptr;

            NoteBookTabBar(Widget *notebook_parent);
            ~NoteBookTabBar();
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual const char* name() override;
            virtual Size sizeHint(DrawingContext &dc) override;
            virtual bool isLayout() override;
            virtual i32 isFocusable() override;
            Widget* handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) override;
            virtual Widget* propagateMouseEvent(Window *window, State *state, MouseEvent event) override;
            virtual bool handleScrollEvent(ScrollEvent event) override;

            Widget *m_notebook_parent = nullptr;
    };

    class NoteBook : public Widget {
        public:
            NoteBook();
            ~NoteBook();
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual const char* name() override;
            virtual Size sizeHint(DrawingContext &dc) override;
            virtual bool isLayout() override;
            virtual i32 isFocusable() override;
            Widget* handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) override;
            virtual Widget* propagateMouseEvent(Window *window, State *state, MouseEvent event) override;
            NoteBook* appendTab(Widget *root, std::string text, Image *icon = nullptr, bool close_button = false);
            // NoteBook* insertTab(Widget *root, std::string text, Image *icon = nullptr, bool close_button = false);
            NoteBook* destroyTab(u64 index);
            // NoteBook* setTabWidget(Widget *root);
            u64 currentTab();
            NoteBook* setCurrentTab(u64 index);

        protected:
            u64 m_tab_index = 0;
            NoteBookTabBar *m_tabs = new NoteBookTabBar(this);
    };

    class NoteBookTabButton : public Button {
        public:
            NoteBookTabButton(NoteBook *notebook, std::string text, Image *image = nullptr, bool close_button = false);
            ~NoteBookTabButton();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            virtual bool isLayout() override;
            virtual i32 isFocusable() override;
            Widget* handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) override;
            virtual Widget* propagateMouseEvent(Window *window, State *state, MouseEvent event) override;
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
