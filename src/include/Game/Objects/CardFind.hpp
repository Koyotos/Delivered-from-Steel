#ifndef FE_CARD_FIND
#define FE_CARD_FIND

#include "include/Core/Object2D.hpp"
#include "include/Game/CardType.hpp"
#include "include/Game/Objects/Player.hpp"

class CardFind : public Object2D, public ISerializable {
private:
	shared_ptr<Player> player;       
    CardType cardTypeToUnlock = CardType::WallJump;
    bool isCollected = false;

public:
    CardFind();
    CardFind(const unordered_map<string, std::any>& data);
    virtual ~CardFind();

    void Init(shared_ptr<Scene> scene) override;

    void OnCollisionEnter(std::shared_ptr<Collider> other) override;

    std::string GetSerializeKey() const override;
    nlohmann::json Serialize() const override;
    void Deserialize(const nlohmann::json& data) override;
};
#endif // FE_CARD_FIND