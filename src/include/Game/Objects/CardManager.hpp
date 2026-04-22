#ifndef CARD_MANAGER
#define CARD_MANAGER

#include "include/Core/Node.hpp"

class CardManager : public Node
{

private:
	

public:
	CardManager();
	CardManager(const unordered_map<string, std::any>& data);
	~CardManager();



};



#endif // CARD_MANAGER
