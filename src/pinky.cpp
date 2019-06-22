#include "pinky.h"

#include "distancecalculator.h"
#include "player.h"

Pinky::Pinky(Player const& player, std::vector<Node> const& nodes)
    : Enemy(player, nodes, getRegularSprites(), delayToLeaveHideout) {}

auto Pinky::getRegularSprites() -> SpriteMap<MovementDirection>
{
    return {{MovementDirection::LEFT, {QPixmap(":/sprites/sprites/pghostL1.png"), QPixmap(":/sprites/sprites/pghostL2.png")}},
            {MovementDirection::RIGHT, {QPixmap(":/sprites/sprites/pghost1.png"), QPixmap(":/sprites/sprites/pghost2.png")}},
            {MovementDirection::UP, {QPixmap(":/sprites/sprites/pghostU1.png"), QPixmap(":/sprites/sprites/pghostU2.png")}},
            {MovementDirection::DOWN, {QPixmap(":/sprites/sprites/pghostD1.png"), QPixmap(":/sprites/sprites/pghostD2.png")}}};
}

auto Pinky::makeTurnDecision(std::vector<MovementDirection> const& possibleMovements) -> MovementDirection
{
    if (!isFrightened())
    {
        constexpr auto tilesAhead = 4;
        qreal offsetPlayerPositionOffset = player.pixmap().width() * tilesAhead;
        auto const futurePlayerPosition = DistanceCalculator::generateNextPosition(player.pos(), player.getCurrentDirection(), offsetPlayerPositionOffset);

        DistanceCalculator const distanceCalculator(possibleMovements, pos(), futurePlayerPosition);
        return distanceCalculator.calculateShortestDirection();
    }
    return DistanceCalculator(possibleMovements, pos(), player.pos()).calculateLongestDirection();
}
