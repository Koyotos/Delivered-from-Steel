#ifndef FE_TRANSFORM
#define FE_TRANSFORM

#include <glm/matrix.hpp>

using namespace glm;

/*
@brief Transform is responsible for stoing all information realted to position.
It holds every partial matrix as well as combined local and global.
*/
class Transform {
    private:
    mat4 translation;
    mat4 rotation;
    mat4 scale;

    mat4 local;
    mat4 global;

    public:

    /*
    @brief Returns local matrix.
    @return const mat4& - local matrix
    */
    const mat4& GetLocal() const;

    /*
    @brief Returns global matrix (matrix in relation to graph root coordinate system).
    @return const mat4& - global matrix
    */
    const mat4& GetGlobal() const;

    /*
    @brief Combines global matrix of parent with local matrix.
    @param1 const mat4& - parent global matrix
    @return void
    */
    void Combine(const Transform&);

    /*
    @brief Returns translation component of transform.
    @return const mat4& - translation matrix
    */
    const mat4& GetTranslation() const;

    /*
    @brief Returns rotation component of transform.
    @return const mat4& - rotation matrix
    */
    const mat4& GetRotation() const;

    /*
    @brief Returns scale component of transform.
    @return const mat4& - scale matrix
    */
    const mat4& GetScale() const;

    /*
    @brief Sets translation component of transform.
    @param1 const mat4& - translation matrix
    @return void
    */
    void SetTranslation(const mat4&);

    /*
    @brief Sets rotation component of transform.
    @param1 const mat4& - rotation matrix
    @return void
    */
    void SetRotation(const mat4&);

    /*
    @brief Sets scale component of transform.
    @param1 const mat4& - scale matrix
    @return void
    */
    void SetScale(const mat4&);

    /*
    @brief Generic constructor. It creates transform with scale of 1, no rotation
    and position of 0,0,0.
    @return Transform
    */
    Transform();

    /*
    @brief Parametrized constructor. It creates transform with given component matrices
    and performs local matrix calculation.
    @param1 const mat4& - translation matrix
    @param2 const mat4& - rotation matrix
    @param3 const mat4& - scale matrix
    @return Transform
    */
    Transform(const mat4&, const mat4&, const mat4&);

    /*
    @brief Generic destructor.
    */
    ~Transform();
};

#endif