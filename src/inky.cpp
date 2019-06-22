#include "inky.h"

#include "distancecalculator.h"
#include "player.h"

#include <cmath>
#include <utility>

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

auto Inky::makeTurnDecision(std::vector<MovementDirection> const& possibleMovements) -> MovementDirection
{
    if (!isFrightened())
    {
        constexpr auto tilesAhead = 2;
        qreal offsetPlayerPositionOffset = player.pixmap().width() * tilesAhead;
        auto const futurePlayerPosition = DistanceCalculator::generateNextPosition(player.pos(), player.getCurrentDirection(), offsetPlayerPositionOffset);
        auto const blinkyPosition = blinky.pos();
        auto const euclideanVector = std::make_pair(std::abs(futurePlayerPosition.x() - blinkyPosition.x()) * 2,
                                                    std::abs(futurePlayerPosition.y() - blinkyPosition.y()) * 2);
        qreal const targetPositionX = blinkyPosition.x() + (blinkyPosition.x() < futurePlayerPosition.x() ? euclideanVector.first : -euclideanVector.first);
        qreal const targetPositionY = blinkyPosition.y() + (blinkyPosition.y() < futurePlayerPosition.y() ? euclideanVector.second : -euclideanVector.second);
        QPointF const targetPosition(targetPositionX, targetPositionY);

        DistanceCalculator const distanceCalculator(possibleMovements, pos(), targetPosition);
        return distanceCalculator.calculateShortestDirection();
    }
    return DistanceCalculator(possibleMovements, pos(), player.pos()).calculateLongestDirection();
}
