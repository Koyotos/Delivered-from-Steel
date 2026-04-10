#ifndef FE_FONT
#define FE_FONT

#include <map>
#include <vector>

#include "freetype2/ft2build.h"
#include FT_FREETYPE_H

#include "Utils.hpp"

struct Character {
    GLuint TextureID;
    ivec2 size;
    ivec2 bearing;
    GLuint advance;
};

class Font {
    private:
    vector<pair<map<char,Character>,ivec2>> charsets;

    FT_Library ftLib;
    FT_Face face;

    inline void CreateCharset(const ivec2&);

    public:
    map<char, Character> GetCharset(const ivec2&) noexcept;

    Font();
    Font(const string&, const ivec2&);
    ~Font();
};


#endif