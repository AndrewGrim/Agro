#include "../src/application.hpp"
#include "resources.hpp"

class CustomWidget : public Widget {
    public:
        Font *small = new Font(Application::get()->ft, Karla_Regular_ttf, Karla_Regular_ttf_length, 12, Font::Type::Sans);
        Font *big = new Font(Application::get()->ft, Karla_Bold_ttf, Karla_Bold_ttf_length, 22, Font::Type::Sans);
        Font *italic = new Font(Application::get()->ft, Karla_Italic_ttf, Karla_Italic_ttf_length, 16, Font::Type::Sans);
        std::shared_ptr<Texture> lena = std::make_shared<Texture>(lena_png, lena_png_length);
        Image *normal = new Image(lena);
        Image *flipped_h = (new Image(lena))->flipHorizontally();
        Image *flipped_v = (new Image(lena))->flipVertically();

        CustomWidget() {
            normal->setMinSize(Size(24, 24));
        }

        ~CustomWidget() {
            delete small;
            delete big;
            delete italic;
            delete normal;
            delete flipped_h;
            delete flipped_v;
        }


        const char* name() {
            return "CustomWidget";
        }

        void draw(DrawingContext &dc, Rect rect, int state) {
            this->rect = rect;

            HorizontalAlignment h_align[3] = {
                HorizontalAlignment::Left, HorizontalAlignment::Right, HorizontalAlignment::Center
            };
            VerticalAlignment v_align[3] = {
                VerticalAlignment::Top, VerticalAlignment::Bottom, VerticalAlignment::Center
            };
            { // Text
                Font *fonts[3] = { small, big, italic };
                {
                    Color colors[3] = { Color("#ffaaaa"), Color("#aaffaa"), Color("#aaaaff") };
                    for (int i = 0; i < 3; i++) {
                        dc.fillRect(Rect(rect.x, rect.y, rect.w, 50), colors[i]);
                        dc.fillTextAligned(
                            fonts[i], "This text\n is on a\n single line.",
                            h_align[i], v_align[i],
                            Rect(rect.x, rect.y, rect.w, 50), 5, COLOR_BLACK
                        );
                        rect.y += 50;
                    }
                }
                {
                    Color colors[3] = { Color("#ffffaa"), Color("#aaffff"), Color("#ffaaff") };
                    for (int i = 0; i < 3; i++) {
                        dc.fillRect(Rect(rect.x, rect.y, rect.w, 100), colors[i]);
                        dc.fillTextMultilineAligned(
                            fonts[i], "This text\nspans multiple\nlines.",
                            h_align[i], v_align[i],
                            Rect(rect.x, rect.y, rect.w, 100), 5, COLOR_BLACK, 5
                        );
                        rect.y += 100;
                    }
                }
                // There are another two methods: fillText() and fillTextMultiline().
                // These are essentially the same as the aligned variants with
                // HorizontalAlignment::Left and VerticalAlignment::Center used, and so
                // the aligned versions should be preferred as they already deal
                // with the text measurement, padding and alignment.
                //
                // When the text drawn by the Widget is relevant to its size
                // it is important to use the corresponding text measurement method.
                // When drawing using fillText() or fillTextAligned() you should use measureText().
                // Likewise when drawing using fillTextMultiline() and fillTextMultilineAligned()
                // you should use measureTextMultiline().
                // Lastly there is a measureText() overload for individual characters.
            }

            { // Gradients
                // At the moment only two color gradients are supported.
                dc.fillRectWithGradient(Rect(rect.x, rect.y, rect.w, 100), COLOR_BLACK, Color(1.0f, 0.1f), Gradient::LeftToRight);
                rect.y += 100;
                dc.fillRectWithGradient(Rect(rect.x, rect.y, rect.w, 100), COLOR_BLACK, Color(0.2f, 0.7f, 0.9f), Gradient::TopToBottom);
                rect.y += 100;
            }

            { // Images and Textures
                // An Image is a Widget that draws a Texture.
                // A Texture is a bitmap of some graphic that is stored on the GPU.
                // If a particular Texture is only used in one place
                // you can just create an Image and it will automatically create the Texture.
                // Example: `Image *lena = new Image("lena.png");`
                // Later if you would like to reuse the same Texture for other images
                // you can access it with `image->_texture()`.
                //
                // However if the same Texture is going to be used in different places
                // and perhaps at different sizes and with different colors, you should create the Texture
                // manually, and pass that Texture to the Images.
                // Example:
                // `auto tex = std::make_shared<Texture>("lena.png");`
                // `Image *lena1 = new Image(tex);`
                // `Image *lena2 = new Image(tex);`
                // Here both Images use the exact same Texture but can draw it
                // at any size and color without affecting the other.
                Color colors[3] = { Color("#ffffff"), Color("#ffaa55"), Color("#55aaff") };
                Image *images[3] = { normal, flipped_h, flipped_v };
                Size sizes[3] = { Size(48, 48), Size(72, 72), Size(96, 96) };
                dc.fillRect(Rect(rect.x, rect.y, rect.w, 128), Color("#ffaa5555"));
                for (int i = 0; i < 3; i++) {
                    dc.drawTextureAligned(
                        Rect(rect.x, rect.y, rect.w, 128), sizes[i],
                        images[i]->_texture(),
                        images[i]->coords(),
                        h_align[i], v_align[i],
                        colors[i]
                    );
                }
                rect.y += 128;
                // Just like for text there is a drawTexture() variant wihout alignment
                // but similarly you probably want to use the aligned version most of the time.
                //
                // It is important to note that in order to draw a Texture at its original color
                // you need to pass `COLOR_WHITE` or `Color("#ffffff")` to the drawing function.
                // This is due to how the drawing works in the renderer.
                // However this also means that if a graphic is only or mostly composed of white
                // then you can change the white pixels on it to any color you want.
                // This can be great for icons.

                // In this class's constructor we override the minimum size of the `normal`
                // Image to be 24 by 24 pixels.
                // Below you can see that the Image expands into its given rectangle
                // and can even stretch when you disable maintaining the aspect ratio.
                normal->setExpand(true)->setMaintainAspectRatio(true);
                normal->draw(dc, Rect(rect.x, rect.y, rect.w, 256), normal->state());
                rect.y += 256;
                normal->setExpand(true)->setMaintainAspectRatio(false);
                normal->draw(dc, Rect(rect.x, rect.y, rect.w, 256), normal->state());
                rect.y += 256;
            }
        }

        Size sizeHint(DrawingContext &dc) {
            return Size(500, 1500);
        }
};

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->resize(600, 600);
        app->center();
        app->setTitle("Custom Widget Drawing");
        app->append(new CustomWidget(), Fill::Both);
    app->run();

    return 0;
}
