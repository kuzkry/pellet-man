#include "clyde.h"

#include "player.h"

#include <cmath>
#include <cstdlib>

constexpr std::chrono::milliseconds Clyde::delayToLeaveHideout;

Clyde::Clyde(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes, getRegularSprites(), delayToLeaveHideout)
{
    init();
}

auto Clyde::getRegularSprites() -> SpriteMap<MovementDirection>
{
    return {{MovementDirection::LEFT, {QPixmap(":/sprites/sprites/oghostL1.png"), QPixmap(":/sprites/sprites/oghostL2.png")}},
            {MovementDirection::RIGHT, {QPixmap(":/sprites/sprites/oghost1.png"), QPixmap(":/sprites/sprites/oghost2.png")}},
            {MovementDirection::UP, {QPixmap(":/sprites/sprites/oghostU1.png"), QPixmap(":/sprites/sprites/oghostU2.png")}},
            {MovementDirection::DOWN, {QPixmap(":/sprites/sprites/oghostD1.png"), QPixmap(":/sprites/sprites/oghostD2.png")}}};
}

auto Clyde::makeTurnDecision(std::map<MovementDirection, bool>& possibleMovements, bool frightened) -> MovementDirection
{
    int playerX = player.x(),
        playerY = player.y();
    unsigned playerEnemyOffsetX = std::abs(playerX - x()),
             playerEnemyOffsetY = std::abs(playerY - y());
    DistanceAndDirectionBinder binder[4] = {
        {std::pow((playerY > y() ? playerEnemyOffsetY + 1 : playerEnemyOffsetY - 1), 2) + std::pow(playerEnemyOffsetX, 2), MovementDirection::UP},
        {std::pow((playerX > x() ? playerEnemyOffsetX + 1 : playerEnemyOffsetX - 1), 2) + std::pow(playerEnemyOffsetY, 2), MovementDirection::LEFT},
        {std::pow((playerY > y() ? playerEnemyOffsetY - 1 : playerEnemyOffsetY + 1), 2) + std::pow(playerEnemyOffsetX, 2), MovementDirection::DOWN},
        {std::pow((playerX > x() ? playerEnemyOffsetX - 1 : playerEnemyOffsetX + 1), 2) + std::pow(playerEnemyOffsetY, 2), MovementDirection::RIGHT}};
    /* those directions are in the following order: up, left, down, right */

    {
        unsigned nonChasingAreaLimiter = player.pixmap().width() * 8,
                 distanceFromPlayer = sqrt(std::pow(std::abs(playerX - x()), 2) + std::pow(std::abs(playerY - y()), 2));
        if (!frightened && nonChasingAreaLimiter < distanceFromPlayer)
            std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                       sortDistanceAndDirectionBindersInAscendingOrder);
        else
            std::qsort(binder, 4, sizeof(DistanceAndDirectionBinder),
                       sortDistanceAndDirectionBindersInDescendingOrder);
    }

    return chooseMostSuitableTurnOption(possibleMovements, binder);
}

void Clyde::setInitialPixmap()
{
    setPixmap(QPixmap(":/sprites/sprites/oghostU1.png").scaled(26, 26));
}
