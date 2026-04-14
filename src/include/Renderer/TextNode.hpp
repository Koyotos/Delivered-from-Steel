#ifndef FE_TEXTNODE
#define FE_TEXTNODE

#include "include/Core/VisualNode.hpp"
#include "include/Globals/Globals.hpp"

class TextNode : public VisualNode {
    private:
    vec3 color;
    string content;
    ivec2 size;
    ivec2 pos;
    float scale;
    GLuint VAO,VBO;

	int testNumber = 10;

    public:

    string Type() override;

    void SetContent(const string&);
    void SetColor(const vec3&);
    void SetPos(const ivec2&);
    void SetScale(const float&);
    void SetSize(const ivec2&);

    void Draw() override final;
	void Process() override;


    TextNode();
    TextNode(const unordered_map<string, std::any>&);
    ~TextNode();
};

#endif