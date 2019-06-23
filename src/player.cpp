#include "player.h"

#include "clyde.h"
#include "blinky.h"
#include "inky.h"
#include "lifecounter.h"
#include "node.h"
#include "pinky.h"
#include "regularpellet.h"
#include "score.h"
#include "superpellet.h"

#include <QFont>
#include <QGraphicsScene>
#include <QKeyEvent>

#include <algorithm>
#include <typeinfo>
#include <utility>
#include <vector>

template <class myType>
auto findInVector(std::vector<myType> const& vector, void* itemToBeFound) -> typename std::vector<myType>::const_iterator;

Player::Player(std::vector<Node> const& nodes,
               Score& score,
               LifeCounter& lifeCounter,
               std::vector<RegularPellet*>& regularPellets,
               std::vector<SuperPellet*>& superPellets,
               std::function<void()> quitCallback,
               std::vector<Enemy*> const& enemies)
    : Character(nodes, initialPosition),
      score(score),
      lifeCounter(lifeCounter),
      regularPellets(regularPellets),
      superPellets(superPellets),
      quitCallback(std::move(quitCallback)),
      enemies(enemies)
{
    QObject::connect(&initialDelayTimer, SIGNAL(timeout()), this, SLOT(allowToMove()));
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    QObject::connect(&animationTimer, SIGNAL(timeout()), this, SLOT(chompingAnimation()));
}

void Player::init()
{
    deinit();
    setPixmap(QPixmap(":/sprites/sprites/pacopenleft.png"));
    setInitialPosition();
    currentDirection = pendingDirection = MovementDirection::LEFT;
    isMoving = false;
    initialDelayTimer.start(initialDelay);
    animationTimer.start(animationTime);
}

void Player::deinit()
{
    initialDelayTimer.stop();
    animationTimer.stop();
    movementTimer.stop();
}

void Player::keyPressEvent(QKeyEvent* event)
{
    // move the player left and right
    switch (event->key())
    {
    case Qt::Key_Left:
        pendingDirection = MovementDirection::LEFT;
        break;
    case Qt::Key_Right:
        pendingDirection = MovementDirection::RIGHT;
        break;
    case Qt::Key_Up:
        pendingDirection = MovementDirection::UP;
        break;
    case Qt::Key_Down:
        pendingDirection = MovementDirection::DOWN;
        break;
    case Qt::Key_Escape:
        prepareToEndGame(QuitReason::PRESSED_ESC);
        break;
    }
}

void Player::checkCollisionWithPelletsAndGhosts()
{
    // get a list of all the items currently colliding with all dots and enemies
    QList<QGraphicsItem*> allItems = collidingItems();

    // if one of the colliding items is Pac-Man, destroy that dot
    for (int i = 0; i < allItems.size(); ++i)
    {
        if (typeid(*(allItems[i])) == typeid(RegularPellet))
        {
            score.little_increase(); // increase the score by 10

            // remove from a vector
            regularPellets.erase(findInVector(regularPellets, static_cast<void*>(allItems[i])));

            // remove them from the scene (still on the heap)
            scene()->removeItem(allItems[i]);

            // delete them from the heap to save memory
            delete allItems[i];
        }
        else if (typeid(*(allItems[i])) == typeid(SuperPellet))
        {
            score.big_increase(); // increase the score by 50

            // remove from a vector
            superPellets.erase(findInVector(superPellets, static_cast<void*>(allItems[i])));

            // remove them from the scene (still on the heap)
            scene()->removeItem(allItems[i]);

            // delete them from the heap to save memory
            delete allItems[i];

            // checking if any of enemies is frightened (if not, that means that player will not get extra points
            if (!isAnyOfEnemiesFrightened())
                score.resetMultiplier();

            //frighten enemies - requires getting rid of const to call enableRunawayState
            std::for_each(const_cast<std::vector<Enemy*>&>(enemies).begin(),
                          const_cast<std::vector<Enemy*>&>(enemies).end(),
                          [](Enemy* ptrToEnemy){ptrToEnemy->enableRunawayState();});
        }
        else if (typeid(*(allItems[i])) == typeid(Blinky) || typeid(*(allItems[i])) == typeid(Pinky)
                || typeid(*(allItems[i])) == typeid(Inky) || typeid(*(allItems[i])) == typeid(Clyde))
        {
            if (!dynamic_cast<Enemy*>(allItems[i])->isFrightened())
            {
                lifeCounter.decrease();
                if (lifeCounter.getLives() == 0)
                    prepareToEndGame(QuitReason::DEFEAT);
                else
                {
                    std::for_each(const_cast<std::vector<Enemy*>&>(enemies).begin(),
                                  const_cast<std::vector<Enemy*>&>(enemies).end(),
                                  [](Enemy* ptrToEnemy){ptrToEnemy->init();});
                    init();
                }
            }
            else
            {
                score.huge_increase();
                dynamic_cast<Enemy*>(allItems[i])->init();
            }
        }
    }
    if (!regularPellets.size() && !superPellets.size())
        prepareToEndGame(QuitReason::VICTORY);
}

auto Player::isAnyOfEnemiesFrightened() const -> bool
{
    return std::any_of(enemies.begin(), enemies.end(), [](Enemy const* enemy) { return enemy->isFrightened(); });
}

void Player::prepareToEndGame(Player::QuitReason reason) const
{
    QGraphicsTextItem* text = nullptr;
    switch (reason)
    {
    case QuitReason::PRESSED_ESC:
        endGame();
        return;
    case QuitReason::VICTORY:
        text = new QGraphicsTextItem("YOU WIN!");
        break;
    case QuitReason::DEFEAT:
        text = new QGraphicsTextItem("YOU LOSE!");
    }
    text->setPos(120, 210);
    text->setDefaultTextColor(Qt::red);
    text->setFont(QFont("times", 34));
    scene()->addItem(text);

    // disable all timers (disablesMovements)
    const_cast<Player*>(this)->deinit(); // another way is to make timers mutable
    std::for_each(const_cast<std::vector<Enemy*>&>(enemies).begin(),
                  const_cast<std::vector<Enemy*>&>(enemies).end(),
                  [](Enemy* ptrToEnemy){ptrToEnemy->deinit();});

    QTimer::singleShot(3000, this, SLOT(endGame()));
}

void Player::setMovement(MovementDirection const newDirection) noexcept
{
    currentDirection = newDirection;
    isMoving = true;
}

void Player::setMovementInNode(Node const& node)
{
    std::vector<MovementDirection> possibleDirections;
    possibleDirections.reserve(node.movementPossibilities.size());
    for (auto const& [direction, isDirectionValid] : node.movementPossibilities)
    {
        if (isDirectionValid)
            possibleDirections.push_back(direction);
    }

    if (std::find(possibleDirections.cbegin(), possibleDirections.cend(), pendingDirection) != possibleDirections.cend()) // check if a pending move can be performed
        setMovement(pendingDirection);
    else if (std::find(possibleDirections.cbegin(), possibleDirections.cend(), currentDirection) != possibleDirections.cend()) // check if Pac-Man can continue going in his current direction
        setMovement(currentDirection);
    else // otherwise Pac-Man hits a wall
        stop();
}

void Player::stop() noexcept
{
    isMoving = false;
}

void Player::tryToSetOppositeMovement() noexcept
{
    if (pendingDirection == opposite(currentDirection))
        setMovement(pendingDirection);
}

void Player::allowToMove()
{
    initialDelayTimer.stop();
    movementTimer.start(movementTime);
    isMoving = true;
}

void Player::move()
{
    auto const it = findCurrentNode();
    if (it != nodes.cend())
        setMovementInNode(*it);
    else
        tryToSetOppositeMovement();

    if (isMoving)
        animate();

    checkCollisionWithPelletsAndGhosts();
}

void Player::chompingAnimation()
{
    static bool phase = false;

    switch (currentDirection)
    {
    case MovementDirection::LEFT:
        if (!phase)
            setPixmap(QPixmap(":/sprites/sprites/pacopenleft.png"));
        else
            setPixmap(QPixmap(":/sprites/sprites/pacmidleft.png"));
        break;
    case MovementDirection::RIGHT:
        if (!phase)
            setPixmap(QPixmap(":/sprites/sprites/pacopenright.png"));
        else
            setPixmap(QPixmap(":/sprites/sprites/pacmidright.png"));
        break;
    case MovementDirection::UP:
        if (!phase)
            setPixmap(QPixmap(":/sprites/sprites/pacopenup.png"));
        else
            setPixmap(QPixmap(":/sprites/sprites/pacmidup.png"));
        break;
    case MovementDirection::DOWN:
        if (!phase)
            setPixmap(QPixmap(":/sprites/sprites/pacopendown.png"));
        else
            setPixmap(QPixmap(":/sprites/sprites/pacmiddown.png"));
        break;
    }

    phase =! phase;
}

void Player::endGame() const
{
    quitCallback();
}

template <class myType>
auto findInVector(std::vector<myType> const& vector, void* itemToBeFound) -> typename std::vector<myType>::const_iterator
{
    auto it = vector.cbegin();
    while (*it != itemToBeFound)
        it++;

    return it;
}
