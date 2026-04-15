#ifndef FE_BUTTON
#define FE_BUTTON

#include "include/Core/Object2D.hpp"
class Button :
    public Object2D
{
public:
    Button(const unordered_map<string, std::any>&);

};

#endif // FE_BUTTON
