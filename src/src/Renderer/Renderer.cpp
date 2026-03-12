#include "include/Renderer/Renderer.hpp"

void Renderer::DrawScene(shared_ptr<Scene> scene) {
    PrepareDraw(scene->root, Transform());
    Draw(scene->root);
}

void Renderer::Draw(shared_ptr<Node> node) {
    node->Draw();
    for(auto& k : node->GetChildren()) {
        Draw(k);
    }
}

void Renderer::PrepareDraw(shared_ptr<Node> node, Transform t) {
    if(node->TestDraw()) {
        shared_ptr<VisualNode> nodeCast = static_pointer_cast<VisualNode>(node);
        ConfigureShader(nodeCast->GetShader());
        if(!nodeCast->TestIgnoreParent()) {
            if(nodeCast->TestTransformChanged())
                nodeCast->ApplyParentTransform(t);
        } else {
            nodeCast->ResetGlobal();
        }
        t = nodeCast->GetTransform();
    } 
    for(auto& k : node->GetChildren()) {
        PrepareDraw(k, t);
    }
} 

void Renderer::ConfigureShader(shared_ptr<Shader> shader) {

}