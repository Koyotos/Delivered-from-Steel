#ifndef FE_TRANSFORM
#define FE_TRANSFORM

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/matrix.hpp>
#include <vector>
#include <any>

using namespace glm;
using namespace std;

/*
@brief Transform is responsible for stoing all information realted to position.
It holds every partial matrix as well as combined local and global.
*/
class Transform {
    private:
    vec3 translation;
    quat rotation;
    vec3 scale;

    mat4 local;
    mat4 global;

    inline void UpdateLocal();

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

    void SetGlobal(mat4);
    /*
    @brief Combines global matrix of parent with local matrix.
    @param1 const mat4& - parent global matrix
    @return void
    */
    void Combine(const Transform&);

    /*
    @brief Returns translation component of transform.
    @return const vec3& - translation vector
    */
    const vec3& GetTranslation() const;

    /*
    @brief Returns rotation component of transform.
    @return const ve3& - rotation vector, in radians
    */
    const vec3 GetRotation() const;

    /*
    @brief Returns scale component of transform.
    @return const vec3& - scale vector
    */
    const vec3& GetScale() const;

    /*
    @brief Sets translation component of transform.
    @param1 const vec3& - position in a local coordinate space
    @return void
    */
    void SetTranslation(const vec3&);

    /*
    @brief Sets rotation component of transform.
    @param1 const vec3& - rotation vector in local coordinate space in radians
    @return void
    */
    void SetRotation(const vec3&);

    /*
    @brief Sets scale component of transform.
    @param1 const vec3& - scaling vector
    @return void
    */
    void SetScale(const vec3&);

    /*
    @brief Sets global transform to be equal to local
    @return void
    */
    void ResetGlobal();

    /*
    @brief Generic constructor. It creates transform with scale of 1, no rotation
    and position of 0,0,0.
    @return Transform
    */
    Transform();

    /*
    @brief Parametrized constructor. It creates transform with given component vectors
    and performs local matrix calculation.
    @param1 const vec3& - translation vector
    @param2 const vec3& - rotation vector (radians)
    @param3 const vec3& - scale vector
    @return Transform
    */
    Transform(const vec3&, const vec3&, const vec3&);

    /*
    @brief Constructor from json files. It takes vector extracted from json.
    @param1 const vector<std::any>& - data vector
    @return Transform 
    */
    Transform(const vector<std::any>&);

    /*
    @brief Generic destructor.
    */
    ~Transform();
};

#endif