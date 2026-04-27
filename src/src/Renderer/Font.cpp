#include "include/Renderer/Font.hpp"

void Font::CreateCharset(const ivec2& size) {
    int atlasWidth = 0;
    int atlasHeight = 0;

    int rowWidth = 0;
    int rowHeight = 0;
    const int MAX_ROW_WIDTH = 1024;

    FT_Set_Char_Size(face, 0, size.y * 64, 0, 0);
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            continue;

        int w = face->glyph->bitmap.width;
        int h = face->glyph->bitmap.rows;

        if (rowWidth + w >= MAX_ROW_WIDTH) {
            atlasWidth = std::max(atlasWidth, rowWidth);
            atlasHeight += rowHeight;

            rowWidth = 0;
            rowHeight = 0;
        }

        rowWidth += w;
        rowHeight = std::max(rowHeight, h);
    }

    atlasWidth = std::max(atlasWidth, rowWidth);
    atlasHeight += rowHeight;
    if (atlasWidth == 0 || atlasHeight == 0) {
        throw std::runtime_error("Font atlas has zero size");
    }
    GLuint atlasTex;
    glGenTextures(1, &atlasTex);
    glBindTexture(GL_TEXTURE_2D, atlasTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,atlasWidth,atlasHeight,0,
        GL_RED,GL_UNSIGNED_BYTE,nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int x = 0;
    int y = 0;
    rowHeight = 0;

    map<char, Character> charset;

    for(unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            continue;

        int w = face->glyph->bitmap.width;
        int h = face->glyph->bitmap.rows;

        if (x + w >= atlasWidth) {
            x = 0;
            y += rowHeight;
            rowHeight = 0;
        }

        if (w > 0 && h > 0) {
            glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h,
                GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        }

        Character ch;
        ch.uvOffset = vec2((float)x / atlasWidth, (float)y / atlasHeight);
        ch.uvSize   = vec2((float)w / atlasWidth, (float)h / atlasHeight);
        ch.size     = ivec2(w, h);
        ch.bearing  = ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        ch.advance  = face->glyph->advance.x;

        charset.insert({c, ch});

        x += w + 1;
        rowHeight = std::max(rowHeight, h);
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    charsets.push_back({charset,size,atlasTex});
}

Charset Font::GetCharset(const ivec2& size) noexcept {
    for(auto& c : charsets) {
        if(size == c.size) {
            return c;
        }
    }
    CreateCharset(size);
    return charsets[charsets.size()-1];
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
}