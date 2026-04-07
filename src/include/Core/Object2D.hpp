#ifndef FE_OBJECT_2D
#define FE_OBJECT_2D

#include "include/Core/VisualNode.hpp"
#include "include/PhysicsManager/PhysicsNode2D.hpp"
#include "include/Renderer/Sprite.hpp"

/*
@brief Base class for every 2D game object. It combines physics and visuals. This class is on it's own
fully used by engine. It satisfies all requiements and allows to call every standard Node method.
*/
class Object2D : public VisualNode, public PhysicsNode2D {
    private:
    shared_ptr<Sprite> sprite;
    bool reqPerspecive;

    public:

    /*
    @brief Sets flag that informs renderer to use perspective matrix instead of orthogonal.
    @param1 const bool& - flag state
    @return void
    */
    void SetReqPerspective(const bool&);

    /*
    @brief Returns flag that informs renderer to use perspective matrix instead of orthogonal.
    @return void
    */
    const bool& GetReqPerspective();

    /*
    @brief Override of Draw() function. It renders object's sprite with given shader.
    @return void
    */
    void Draw() override final;

    /*
    @brief Returns sprite used by object.
    @return shared_ptr<Sprite> - object's sprite
    */
    shared_ptr<Sprite> GetSprite();

    /*
    @brief Sets sprite used by object.
    @param1 shared_ptr<Sprite> - new model
    @return void
    */
    void SetSprite(shared_ptr<Sprite>);

    /*
    @brief Basic constructor. Creates empty Object2D.
    @return Object2D
    */
    Object2D();

    /*
    @brief Constructor from json data. Creates Object2D from input loaded in scene
    specific format. 
    @param1 const unordered_map<string, std::any>& - data from json
    @return Object2D
    */
    Object2D(const unordered_map<string, std::any>&);
};

#endif