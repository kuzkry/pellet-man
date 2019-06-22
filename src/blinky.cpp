#include "blinky.h"

#include "player.h"

#include <cmath>
#include <cstdlib>

constexpr std::chrono::milliseconds Blinky::delayToLeaveHideout;

Blinky::Blinky(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes, getRegularSprites(), delayToLeaveHideout) {}

auto Blinky::getRegularSprites() -> SpriteMap<MovementDirection>
{
    return {{MovementDirection::LEFT, {QPixmap(":/sprites/sprites/rghostL1.png"), QPixmap(":/sprites/sprites/rghostL2.png")}},
            {MovementDirection::RIGHT, {QPixmap(":/sprites/sprites/rghost1.png"), QPixmap(":/sprites/sprites/rghost2.png")}},
            {MovementDirection::UP, {QPixmap(":/sprites/sprites/rghostU1.png"), QPixmap(":/sprites/sprites/rghostU2.png")}},
            {MovementDirection::DOWN, {QPixmap(":/sprites/sprites/rghostD1.png"), QPixmap(":/sprites/sprites/rghostD2.png")}}};
}

auto Blinky::makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection
{
    unsigned playerEnemyOffsetX = std::abs(player.x() - x()),
             playerEnemyOffsetY = std::abs(player.y() - y());
    DistanceAndDirectionBinder binder[4] = {
        {std::pow((player.y() > y() ? playerEnemyOffsetY + 1 : playerEnemyOffsetY - 1), 2) + std::pow(playerEnemyOffsetX, 2), MovementDirection::UP},
        {std::pow((player.x() > x() ? playerEnemyOffsetX + 1 : playerEnemyOffsetX - 1), 2) + std::pow(playerEnemyOffsetY, 2), MovementDirection::LEFT},
        {std::pow((player.y() > y() ? playerEnemyOffsetY - 1 : playerEnemyOffsetY + 1), 2) + std::pow(playerEnemyOffsetX, 2), MovementDirection::DOWN},
        {std::pow((player.x() > x() ? playerEnemyOffsetX - 1 : playerEnemyOffsetX + 1), 2) + std::pow(playerEnemyOffsetY, 2), MovementDirection::RIGHT}};
    /* those directions are in the following order: up, left, down, right */

    if (!frightened)
        std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                   sortDistanceAndDirectionBindersInAscendingOrder);
    else
        std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                   sortDistanceAndDirectionBindersInDescendingOrder);

    return chooseMostSuitableTurnOption(possibleMovements, binder);
}

void Blinky::setInitialPixmap()
{
    setPixmap(QPixmap(":/sprites/sprites/rghostU1.png").scaled(26, 26));
}
