#ifndef CHARACTER_H
#define CHARACTER_H

#include "directions.h"

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPointF>
#include <QTimer>

#include <array>
#include <unordered_map>
#include <vector>

struct Node;

class Character : public QObject, public QGraphicsPixmapItem
{
protected:
    static constexpr std::size_t spriteCount = 2;

    template <typename Key>
    using SpriteMap = std::unordered_map<Key, std::array<QPixmap, spriteCount>>;

public:
    Character(std::vector<Node> const& nodes, SpriteMap<MovementDirection> regularSprites, QPointF initialPosition);
    ~Character() override = default;

    virtual void init() = 0;

protected:
    virtual void deinit() = 0;

    void animate();
    template <typename Key>
    void setSprite(SpriteMap<Key> const& spriteMap, Key key);
    auto findCurrentNode() const -> std::vector<Node>::const_iterator;
    void setInitialPixmap(MovementDirection direction);
    void setInitialPosition();

    std::vector<Node> const& nodes;
    SpriteMap<MovementDirection> const regularSprites;
    MovementDirection currentDirection;
    QTimer movementTimer, animationTimer, initialDelayTimer;
    QPointF const initialPosition;

protected slots:
    virtual void allowToMove() = 0;
    virtual void changeSprite() = 0;
    virtual void move() = 0;

private:
    auto isInNode(Node const& node) const -> bool;
    auto nextSpriteIndex() const noexcept -> std::size_t;
    void teleportOnMapEdge();

    std::size_t spriteIndex = 0;
};

template <typename Key>
inline void Character::setSprite(SpriteMap<Key> const& spriteMap, Key const key)
{
    setPixmap(spriteMap.find(key)->second[spriteIndex]);
    spriteIndex = nextSpriteIndex();
}

#endif // CHARACTER_H
