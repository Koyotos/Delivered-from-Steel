#include "include/Renderer/Font.hpp"
#include "freetype/freetype.h"
#include <filesystem>

void Font::CreateCharset(const ivec2& size) {
    FT_Set_Pixel_Sizes(face, size.x, size.y);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    map<char, Character> charset;

    for (unsigned char c = 0; c < 128; c++) {

    if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        throw runtime_error("Can't load glyph : " + to_string(c));
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows,
        0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character character = {texture, ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), (GLuint)face->glyph->advance.x};
    
    charset.insert({c,character});
    }
    charsets.push_back({charset, size});
}

map<char, Character> Font::GetCharset(const ivec2& size) noexcept {
    for(auto& f : charsets) {
        if(size == f.second) {
            return f.first;
        }
    }
    CreateCharset(size);
    return charsets[charsets.size()-1].first;
}

Font::Font() {

}

Font::Font(const string& path, const ivec2& size) {
    if(FT_Init_FreeType(&ftLib)) {
        throw runtime_error("Can't initialize FreeType.");
    }
    
    int ercode = FT_New_Face(ftLib,path.c_str(),0,&face);
    if(ercode) {
        throw runtime_error("Can't load font from path : " + path);
    }
    CreateCharset(size);
}

Font::~Font() {
    charsets.clear();
    FT_Done_Face(face);
    FT_Done_FreeType(ftLib);
}