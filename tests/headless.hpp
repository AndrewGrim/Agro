#ifndef HEADLESS_HPP
    #define HEADLESS_HPP
    app->dc->textRenderer->load("fonts/FreeSans.ttf", 14);
    app->dc->textRenderer->shader.use();
    app->dc->textRenderer->shader.setInt("text", 0);
    app->main_widget->m_app = (void*)app;
    for (Widget *child : app->main_widget->children) {
        child->m_app = (void*)app;
        child->attach_app((void*)app);
    }
    app->show();
    if (app->ready_callback) {
        app->ready_callback(app);
    }
#endif