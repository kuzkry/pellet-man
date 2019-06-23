#include "clyde.h"

#include "distancecalculator.h"
#include "player.h"

Clyde::Clyde(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes, getRegularSprites(), delayToLeaveHideout) {}

auto Clyde::makeTurnDecision(std::vector<MovementDirection> const& possibleMovements) const -> MovementDirection
{
    DistanceCalculator const distanceCalculator(possibleMovements, pos(), player.pos());

    constexpr auto tilesAhead = 8;
    qreal const nonChasingArea = player.pixmap().width() * tilesAhead,
                distanceFromPlayer = distanceCalculator.calculateDistance(pos(), player.pos());

    return nonChasingArea < distanceFromPlayer && !isFrightened() ? distanceCalculator.calculateShortestDirection() : distanceCalculator.calculateLongestDirection();
}

auto Clyde::getRegularSprites() -> SpriteMap<MovementDirection>
{
    return {{MovementDirection::LEFT, {QPixmap(":/sprites/sprites/oghostL1.png"), QPixmap(":/sprites/sprites/oghostL2.png")}},
            {MovementDirection::RIGHT, {QPixmap(":/sprites/sprites/oghost1.png"), QPixmap(":/sprites/sprites/oghost2.png")}},
            {MovementDirection::UP, {QPixmap(":/sprites/sprites/oghostU1.png"), QPixmap(":/sprites/sprites/oghostU2.png")}},
            {MovementDirection::DOWN, {QPixmap(":/sprites/sprites/oghostD1.png"), QPixmap(":/sprites/sprites/oghostD2.png")}}};
}
