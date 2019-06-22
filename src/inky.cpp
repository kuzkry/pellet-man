#include "inky.h"

#include "player.h"

#include <cmath>
#include <cstdlib>

constexpr std::chrono::milliseconds Inky::delayToLeaveHideout;

Inky::Inky(Player const& player, std::vector<Node> const& nodes, Blinky const& blinky)
    : Enemy(player, nodes, getRegularSprites(), delayToLeaveHideout),
      blinky(blinky) {}

auto Inky::getRegularSprites() -> SpriteMap<MovementDirection>
{
    return {{MovementDirection::LEFT, {QPixmap(":/sprites/sprites/cghostL1.png"), QPixmap(":/sprites/sprites/cghostL2.png")}},
            {MovementDirection::RIGHT, {QPixmap(":/sprites/sprites/cghost1.png"), QPixmap(":/sprites/sprites/cghost2.png")}},
            {MovementDirection::UP, {QPixmap(":/sprites/sprites/cghostU1.png"), QPixmap(":/sprites/sprites/cghostU2.png")}},
            {MovementDirection::DOWN, {QPixmap(":/sprites/sprites/cghostD1.png"), QPixmap(":/sprites/sprites/cghostD2.png")}}};
}

auto Inky::makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection
{
    int relativePlayerX = player.x(),
        relativePlayerY = player.y(),
        playerEnemyOffsetX, playerEnemyOffsetY;

    if (!frightened)
    {
        unsigned futurePlayerPositionOffset = player.pixmap().width() * 2;
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

        playerEnemyOffsetX = std::abs(relativePlayerX - blinky.x()) * 2;
        playerEnemyOffsetY = std::abs(relativePlayerY - blinky.y()) * 2;
    }
    else
    {
        playerEnemyOffsetX = std::abs(relativePlayerX - x());
        playerEnemyOffsetY = std::abs(relativePlayerY - y());
    }
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

void Inky::setInitialPixmap()
{
    setPixmap(QPixmap(":/sprites/sprites/cghostU1.png").scaled(26, 26));
}
