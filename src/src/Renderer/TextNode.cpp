#include "include/Renderer/TextNode.hpp"
#include "include/Profiler/ProfilerNode.hpp"

void TextNode::SetContent(const string& content) {
    this->content = content;
}

void TextNode::SetColor(const vec3& color) {
    this->color = color;
}

void TextNode::SetPos(const ivec2& pos) {
    this->pos = pos;
}

void TextNode::SetScale(const float& scale) {
    this->scale = scale;
}

void TextNode::SetSize(const ivec2& size) {
    this->size = size;
}

string TextNode::Type() {
    return "TextNode";
}

void TextNode::Draw() {
    shader->SetVec3("color", color);
	shader->SetMat4("M", mat4(1.0f));
    shader->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    map<char, Character> charset = Globals::GetGlobals().GetGameFont().GetCharset(this->size);
    float localPos = pos.x;
	float currentY = pos.y;
    for(string::const_iterator c = content.begin(); c != content.end(); c++) {
        if (*c == '\n') {
            currentY += size.y * scale * 1.2f;
            localPos = pos.x;
            continue;
        }
        Character ch = charset[*c];
        float xpos = localPos + ch.bearing.x*scale;
        float ypos = currentY - (ch.size.y - ch.bearing.y)*scale;

        float w = ch.size.x*scale;
        float h = ch.size.y*-scale;
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        PROFILER_ADD_DRAW_CALL(2);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        localPos+= (ch.advance >> 6) * scale;
        }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextNode::TextNode() : VisualNode(){
    VAO = 0;
    VBO = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        4,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

TextNode::TextNode(const unordered_map<string, std::any>& data) : VisualNode(data) {
    vector<std::any> values = fromMap(vector<std::any>, "color", data);
    color.x = any_cast<float>(values[0]);
    color.y = any_cast<float>(values[1]);
    color.z = any_cast<float>(values[2]);

    values = fromMap(vector<std::any>, "size", data);
    size.x = any_cast<int64_t>(values[0]);
    size.y = any_cast<int64_t>(values[1]);

    values = fromMap(vector<std::any>, "pos", data);
    pos.x = any_cast<int64_t>(values[0]);
    pos.y = any_cast<int64_t>(values[1]);

    scale = fromMap(float, "scale", data);
    content = fromMap(string, "content", data);

    VAO = 0;
    VBO = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        4,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

TextNode::~TextNode() {
    if (VBO) {
        glDeleteBuffers(1, &VBO);
    }
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
    }
}