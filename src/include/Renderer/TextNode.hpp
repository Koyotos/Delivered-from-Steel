#ifndef FE_TEXTNODE
#define FE_TEXTNODE

#include "include/Core/VisualNode.hpp"
#include "include/Globals/Globals.hpp"

class TextNode : public Node {
    private:
    vec3 color;
    string content;
    ivec2 size;
    ivec2 pos;
    float scale;
    shared_ptr<Shader> shader;
    GLuint VAO,VBO;

    public:
    inline bool TestDraw() noexcept override final {return flags[3];};
    inline bool TestIgnoreParent() noexcept override final {return flags[4];};
    inline bool TestTransformChanged() noexcept override final {return flags[5];};

    /*
    @brief Sets Draw flag state.
    @param1 const bool& - state to be set
    @return void
    */
    inline void SetDraw(const bool& state) noexcept override final {flags[3] = state;};

    /*
    @brief Sets IgnoreParent flag state.
    @param1 const bool& - state to be set
    @return void
    */
    inline void SetIgnoreParent(const bool& state) noexcept override final {flags[4] = state;};

    void SetContent(const string&);
    void SetColor(const vec3&);
    void SetPos(const ivec2&);
    void SetScale(const float&);
    void SetSize(const ivec2&);

    void SetShader(shared_ptr<Shader>);

    void Draw() override final;

    TextNode();
    TextNode(const unordered_map<string, std::any>&);
    ~TextNode();
};

#endif