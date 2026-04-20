#ifndef FE_OBJECT_3D
#define FE_OBJECT_3D

#include "include/Core/Object2D.hpp"
#include "include/Core/VisualNode.hpp"
#include "include/PhysicsManager/PhysicsNode.hpp"


/*
@brief Base class for every 3D game object. It combines physics and visuals. This class is on it's own
fully used by engine. It satisfies all requiements and allows to call every standard Node method.
*/
class Object3D : public PhysicsNode {
    private:
    shared_ptr<Model> model;

    public:

    /*
    @brief Override of Draw() function. It renders object's model with given shader.
    @return void
    */
    void Draw(shared_ptr<Shader> sh = nullptr) override final;

    string Type() override;
    string RenderType() override final;

    /*
    @brief Returns model used by object.
    @return shared_ptr<Model> - object's model
    */
    shared_ptr<Model> GetModel();

    /*
    @brief Sets model used by object.
    @param1 shared_ptr<Model> - new model
    @return void
    */
    void SetModel(shared_ptr<Model>);

    /*
    @brief Basic constructor. Creates empty Object3D.
    @return Object3D
    */
    Object3D();

    /*
    @brief Constructor from json data. Creates Object3D from input loaded in scene
    specific format. 
    @param1 const unordered_map<string, std::any>& - data from json
    @return Object3D
    */
    Object3D(const unordered_map<string, std::any>&);
};

#endif