#include "font.hpp"
#include "../application.hpp"

Font::Font(Application *app, String file_path, u32 pixel_size, Font::Type type)
: file_path{file_path}, loaded_from_file{true}, pixel_size{pixel_size}, type{type}, app{app} {
    if (FT_New_Face(app->ft, file_path.data(), 0, &face)) {
        fail("FAILED_TO_LOAD_FONT", file_path.data());
        if (FT_Select_Charmap(face, FT_ENCODING_UNICODE)) {
            fail("FAILED_TO_SET_UNICODE_CHARMAP_FOR_FONT", file_path.data());
        }
    }
    load(face);
}

Font::Font(Application *app, const u8 *data, i64 length, u32 pixel_size, Font::Type type)
: file_path{":memory:"}, data{data}, data_size{length}, pixel_size{pixel_size}, type{type}, app{app} {
    if (FT_New_Memory_Face(app->ft, data, length, 0, &face)) {
        fail("FAILED_TO_LOAD_FONT", file_path.data());
        if (FT_Select_Charmap(face, FT_ENCODING_UNICODE)) {
            fail("FAILED_TO_SET_UNICODE_CHARMAP_FOR_FONT", file_path.data());
        }
    }
    load(face);
}

Font::~Font() {
    FT_Done_Face(face);
    glDeleteTextures(1, &atlas_ID);
}

u32 Font::maxHeight() {
    return atlas_height;
}

Font::Character Font::get(u32 codepoint) {
    auto search = characters.find(codepoint);
    if (!search) {
        loadGlyph(codepoint, true);
        return characters[codepoint];
    }
    return search.value;
}

void Font::load(FT_Face face) {
    FT_Set_Pixel_Sizes(face, pixel_size, pixel_size);

    atlas_width = app->currentWindow()->dc->renderer->max_texture_size;
    atlas_height = pixel_size; // Note: that this is not guaranteed and needs to be verified when loading characters.
    atlas_depth = 1;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &atlas_ID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, atlas_ID);
#ifdef __EMSCRIPTEN__
    u8 *fuck_webgl = new u8[atlas_width * atlas_height * atlas_depth];
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0, // mipmap level
        GL_R8,
        atlas_width,
        atlas_height,
        atlas_depth, // layers
        0, // border (must be zero... dont ask)
        GL_RED,
        GL_UNSIGNED_BYTE,
        fuck_webgl // texture data
    );
    delete[] fuck_webgl;
#else
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0, // mipmap level
        GL_R8,
        atlas_width,
        atlas_height,
        atlas_depth, // layers
        0, // border (must be zero... dont ask)
        GL_RED,
        GL_UNSIGNED_BYTE,
        NULL // texture data
    );
#endif
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Pre-load ascii
    for (u8 c = 32; c < 127; c++) {
        loadGlyph((u32)c, false);
    }
}

// Note: this should NOT be used to initialise a texture
// but only to EXPAND an existing one.
void Font::grow(u32 width, u32 height, u32 depth) {
    u32 new_texture;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &new_texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, new_texture);
#ifdef __EMSCRIPTEN__
    u8 *fuck_webgl = new u8[atlas_width * atlas_height * atlas_depth];
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_R8,
        width,
        height,
        depth,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        fuck_webgl
    );
    delete[] fuck_webgl;
#else
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        GL_R8,
        width,
        height,
        depth,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        NULL
    );
#endif
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glDeleteTextures(1, &atlas_ID);
    atlas_width = width;
    atlas_height = height;
    atlas_depth = depth;
    atlas_ID = new_texture;
    next_slot = 0;
    next_depth = 0;

    characters.clear(); // This forces us to re-rasterize ascii glyphs and potentially more
    // but I couldn't quite figure out how to copy the old texture into the new one when it grew in width.
}

void Font::loadGlyph(u32 codepoint, bool bind_texture) {
    if (bind_texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, atlas_ID);
    }

    FT_GlyphSlot g = face->glyph;
    if (FT_Load_Char(face, codepoint, FT_LOAD_RENDER)) {
        // TODO would be helpful to print what the codepoint should translate to
        fail("FAILED_TO_LOAD_CHAR", toString(codepoint));
    }

    bool should_grow = false;
    u32 new_atlas_width = atlas_width;
    u32 new_atlas_height = atlas_height;
    u32 new_atlas_depth = atlas_depth;

    if (next_slot + g->bitmap.width > atlas_width) {
        next_depth++;
        next_slot = 0;
        if (next_depth == atlas_depth) {
            info("Font grow depth:", file_path);
            new_atlas_depth *= 2;
            if (new_atlas_depth > (u32)app->currentWindow()->dc->renderer->max_texture_depth) {
                fail("Exceeded max_texture_depth:", String::format("new_atlas_depth: %u, max: %d", new_atlas_depth, app->currentWindow()->dc->renderer->max_texture_depth));
            }
            should_grow = true;
        }
    }
    if (g->bitmap.rows > atlas_height) {
        info("Font grow height:", file_path);
        new_atlas_height = g->bitmap.rows;
        should_grow = true;
        {
            SDL_Event event;
            SDL_UserEvent userevent;
            userevent.type = SDL_USEREVENT;
            userevent.code = LAYOUT_FONT;
            userevent.data1 = this;
            userevent.data2 = NULL;
            event.type = SDL_USEREVENT;
            event.user = userevent;
            SDL_PushEvent(&event);
        }
    }
    if (should_grow) {
        grow(new_atlas_width, new_atlas_height, new_atlas_depth);
        info(String::format("grew to: w%u, h%u, d%u\n", new_atlas_width, new_atlas_height, new_atlas_depth));
        should_grow = false;
    }

#ifdef __EMSCRIPTEN__
    u8 *fuck_webgl = new u8[g->bitmap.width * g->bitmap.rows];
    glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        next_slot,
        0,
        next_depth,
        g->bitmap.width,
        g->bitmap.rows,
        1, // This is the depth of the texture so you could submit an array of textures instead of just one at a time.
        GL_RED,
        GL_UNSIGNED_BYTE,
        g->bitmap.buffer ? g->bitmap.buffer : fuck_webgl
    );
    delete[] fuck_webgl;
#else
    glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,
        next_slot,
        0,
        next_depth,
        g->bitmap.width,
        g->bitmap.rows,
        1, // This is the depth of the texture so you could submit an array of textures instead of just one at a time.
        GL_RED,
        GL_UNSIGNED_BYTE,
        g->bitmap.buffer
    );
#endif
    characters.insert(
        codepoint,
        Character(
            Point(g->bitmap_left, g->bitmap_top),
            Size(g->bitmap.width, g->bitmap.rows),
            (i32)(g->advance.x >> 6),
            next_slot / (f32)atlas_width,
            next_depth
        )
    );
    if (g->bitmap_top > max_bearing) { max_bearing = g->bitmap_top; }
    next_slot += g->bitmap.width;
}

Font* Font::reload(i64 new_pixel_size) {
    if (this->loaded_from_file) {
        return new Font(this->app, this->file_path, new_pixel_size, this->type);
    }
    return new Font(this->app, this->data, this->data_size, new_pixel_size, this->type);
}
