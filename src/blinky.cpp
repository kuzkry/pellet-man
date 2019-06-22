#include "blinky.h"

#include "distancecalculator.h"
#include "player.h"

Blinky::Blinky(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes, getRegularSprites(), delayToLeaveHideout) {}

auto Blinky::getRegularSprites() -> SpriteMap<MovementDirection>
{
    return {{MovementDirection::LEFT, {QPixmap(":/sprites/sprites/rghostL1.png"), QPixmap(":/sprites/sprites/rghostL2.png")}},
            {MovementDirection::RIGHT, {QPixmap(":/sprites/sprites/rghost1.png"), QPixmap(":/sprites/sprites/rghost2.png")}},
            {MovementDirection::UP, {QPixmap(":/sprites/sprites/rghostU1.png"), QPixmap(":/sprites/sprites/rghostU2.png")}},
            {MovementDirection::DOWN, {QPixmap(":/sprites/sprites/rghostD1.png"), QPixmap(":/sprites/sprites/rghostD2.png")}}};
}

auto Blinky::makeTurnDecision(std::vector<MovementDirection> const& possibleMovements) -> MovementDirection
{
    DistanceCalculator const distanceCalculator(possibleMovements, pos(), player.pos());

    return !isFrightened() ? distanceCalculator.calculateShortestDirection() : distanceCalculator.calculateLongestDirection();
}
