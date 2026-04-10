#ifndef FE_LIGHT
#define FE_LIGHT

#include "include/Core/Node.hpp"
#include <glm/ext/vector_float3.hpp>

using namespace glm;

enum LightType {
    LIGHT_DIRECTIONAL = 0,
    LIGHT_POINT = 1,
    LIGHT_SPOT = 2
};

class Light : public Node {
    public:
    LightType type;

    vec3 colorDiffuse;
    vec3 colorSpecular;
    vec3 colorAmbient;

    /*
    Data components are used for different light types differently.
    Directional : data1 - direction, data2 - unused, data3 - unused, data4 - unused
    Point : data1 - position, data2 - attenuation factors, data3 - unused, data4 - unused
    Spot : data1 - position, data2 - direction, data3 - attenuation factors, data4 - cutoff
    */   
    vec3 data1;
    vec3 data2;
    vec3 data3;
    float data4;

    string Type();

    Light();
    Light(unordered_map<string,std::any>);
    ~Light();
};

#endif