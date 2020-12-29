#ifndef HEADLESS_HPP
    #define HEADLESS_HPP
    app->dc->default_font = new Font("fonts/DejaVu/DejaVuSans.ttf", 14, Font::Type::Sans);
    app->setMainWidget(app->mainWidget());
    app->show();
    if (app->onReady) {
        app->onReady(app);
    }
#endif