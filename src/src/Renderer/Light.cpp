#include "include/Renderer/Light.hpp"

Light::Light() {

}

Light::Light(unordered_map<string,any> data) : Node(data) {

    type = LightType(fromMap(int64_t,"type",data));
    data4 = fromMap(float,"data4",data);

    vector<any> dataVec = fromMap(vector<any>,"colorAmbient",data);
    colorAmbient.x = any_cast<float>(dataVec[0]);
    colorAmbient.y = any_cast<float>(dataVec[1]);
    colorAmbient.z = any_cast<float>(dataVec[2]);

    dataVec = fromMap(vector<any>,"colorDiffuse",data);
    colorDiffuse.x = any_cast<float>(dataVec[0]);
    colorDiffuse.y = any_cast<float>(dataVec[1]);
    colorDiffuse.z = any_cast<float>(dataVec[2]);

    dataVec = fromMap(vector<any>,"colorSpecular",data);
    colorSpecular.x = any_cast<float>(dataVec[0]);
    colorSpecular.y = any_cast<float>(dataVec[1]);
    colorSpecular.z = any_cast<float>(dataVec[2]);

    dataVec = fromMap(vector<any>,"data1",data);
    data1.x = any_cast<float>(dataVec[0]);
    data1.y = any_cast<float>(dataVec[1]);
    data1.z = any_cast<float>(dataVec[2]);

    dataVec = fromMap(vector<any>,"data2",data);
    data2.x = any_cast<float>(dataVec[0]);
    data2.y = any_cast<float>(dataVec[1]);
    data2.z = any_cast<float>(dataVec[2]);

    dataVec = fromMap(vector<any>,"data3",data);
    data3.x = any_cast<float>(dataVec[0]);
    data3.y = any_cast<float>(dataVec[1]);
    data3.z = any_cast<float>(dataVec[2]);
}

Light::~Light() {

}