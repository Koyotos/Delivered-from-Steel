#include "include/Renderer/TextNode.hpp"
#include "include/Profiler/Profiler.hpp"

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

void TextNode::Draw(shared_ptr<Shader> sh) {
    if (sh == nullptr) {
        sh = shader;
    }

    sh->Use();
    sh->SetVec3("color", color);
    sh->SetMat4("M", mat4(1.0f));
    sh->SetInt("text", 0);

    glActiveTexture(GL_TEXTURE0);

    Charset charset = Globals::GetGlobals().GetGameFont().GetCharset(this->size);
    glBindTexture(GL_TEXTURE_2D, charset.atlasTexture);

    glBindVertexArray(VAO);

    float localPos = pos.x;
    float currentY = pos.y;

    vector<float> vertices;
    vertices.reserve(content.size() * 6 * 4);

    for (char c : content) {
        if (c == '\n') {
            currentY += size.y * scale * 1.2f;
            localPos = pos.x;
            continue;
        }
        Character ch = charset.chars[c];

        float xpos = localPos + ch.bearing.x * scale;
        float ypos = currentY - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        float u0 = ch.uvOffset.x;
        float v0 = ch.uvOffset.y;
        float u1 = u0 + ch.uvSize.x;
        float v1 = v0 + ch.uvSize.y;

        vertices.insert(vertices.end(), {
            xpos,     ypos + h,   u0, v1,
            xpos,     ypos,       u0, v0,
            xpos + w, ypos,       u1, v0,
            xpos,     ypos + h,   u0, v1,
            xpos + w, ypos,       u1, v0,
            xpos + w, ypos + h,   u1, v1
        });

        localPos += (ch.advance >> 6) * scale;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());

    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 4);

    PROFILER_ADD_DRAW_CALL(1);

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