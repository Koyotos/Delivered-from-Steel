#ifndef FE_OBJECT_3D
#define FE_OBJECT_3D

#include "include/Core/VisualNode.hpp"
#include "include/PhysicsManager/PhysicsNode3D.hpp"


/*
@brief Base class for every game object. It combines physics and visuals. This class is on it's own
fully used by engine. It satisfies all requiements and allows to call every standard Node method.
*/
class Object3D : public VisualNode, public PhysicsNode3D {
    private:
    shared_ptr<Model> model;

    public:

    /*
    @brief Override of Draw() function. It renders object's model with given shader.
    @return void
    */
    void Draw() override final;

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
};

#endif