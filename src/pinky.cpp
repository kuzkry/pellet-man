#include "pinky.h"

#include "player.h"

#include <cmath>
#include <cstdlib>

constexpr std::chrono::milliseconds Pinky::delayToLeaveHideout;

Pinky::Pinky(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes, getRegularSprites(), delayToLeaveHideout)
{
    init();
}

auto Pinky::getRegularSprites() -> SpriteMap<MovementDirection>
{
    return {{MovementDirection::LEFT, {QPixmap(":/sprites/sprites/pghostL1.png"), QPixmap(":/sprites/sprites/pghostL2.png")}},
            {MovementDirection::RIGHT, {QPixmap(":/sprites/sprites/pghost1.png"), QPixmap(":/sprites/sprites/pghost2.png")}},
            {MovementDirection::UP, {QPixmap(":/sprites/sprites/pghostU1.png"), QPixmap(":/sprites/sprites/pghostU2.png")}},
            {MovementDirection::DOWN, {QPixmap(":/sprites/sprites/pghostD1.png"), QPixmap(":/sprites/sprites/pghostD2.png")}}};
}

auto Pinky::makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection
{
    int relativePlayerX = player.x(),
        relativePlayerY = player.y();

    if (!frightened)
    {
        unsigned futurePlayerPositionOffset = player.pixmap().width() * 4;
        switch (player.getCurrentDirection()) {
        case MovementDirection::LEFT:
            relativePlayerX -= futurePlayerPositionOffset;
            break;
        case MovementDirection::RIGHT:
            relativePlayerX += futurePlayerPositionOffset;
            break;
        case MovementDirection::UP:
            relativePlayerY -= futurePlayerPositionOffset;
            break;
        case MovementDirection::DOWN:
            relativePlayerY += futurePlayerPositionOffset;
            break;
        }
    }

    unsigned playerEnemyOffsetX = std::abs(relativePlayerX - x()),
             playerEnemyOffsetY = std::abs(relativePlayerY - y());
    DistanceAndDirectionBinder binder[4] = {
        {std::pow((relativePlayerY > y() ? playerEnemyOffsetY + 1 : playerEnemyOffsetY - 1), 2) + std::pow(playerEnemyOffsetX, 2), MovementDirection::UP},
        {std::pow((relativePlayerX > x() ? playerEnemyOffsetX + 1 : playerEnemyOffsetX - 1), 2) + std::pow(playerEnemyOffsetY, 2), MovementDirection::LEFT},
        {std::pow((relativePlayerY > y() ? playerEnemyOffsetY - 1 : playerEnemyOffsetY + 1), 2) + std::pow(playerEnemyOffsetX, 2), MovementDirection::DOWN},
        {std::pow((relativePlayerX > x() ? playerEnemyOffsetX - 1 : playerEnemyOffsetX + 1), 2) + std::pow(playerEnemyOffsetY, 2), MovementDirection::RIGHT}};
    /* those directions are in the following order: up, left, down, right */

    if (!frightened)
        std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                   sortDistanceAndDirectionBindersInAscendingOrder);
    else
        std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                   sortDistanceAndDirectionBindersInDescendingOrder);

    return chooseMostSuitableTurnOption(possibleMovements, binder);
}

void Pinky::setInitialPixmap()
{
    setPixmap(QPixmap(":/sprites/sprites/pghostU1.png").scaled(26, 26));
}
