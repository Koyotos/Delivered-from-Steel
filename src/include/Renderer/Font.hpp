#ifndef FE_FONT
#define FE_FONT

#include <map>
#include <vector>

#include "ft2build.h"

#include FT_FREETYPE_H

#include "Utils.hpp"

struct Character {
    vec2 uvOffset;   
    vec2 uvSize;     
    ivec2 size;
    ivec2 bearing;
    GLuint advance;
};

struct Charset {
    map<char, Character> chars;
    ivec2 size;
    GLuint atlasTexture;
};

class Font {
    private:
    vector<Charset> charsets;

    FT_Library ftLib;
    FT_Face face;

    inline void CreateCharset(const ivec2&);

    public:
    Charset GetCharset(const ivec2&) noexcept;

    Font();
    Font(const string&, const ivec2&);
    ~Font();
};


#endif
