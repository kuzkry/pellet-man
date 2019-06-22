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
#include "utils.h"

#include <QFont>
#include <QGraphicsScene>
#include <QKeyEvent>

#include <algorithm>
#include <map>
#include <utility>
#include <typeinfo>

template <class myType>
auto findInVector(std::vector<myType> const& vector, void* itemToBeFound) -> typename std::vector<myType>::const_iterator;

constexpr QPointF Player::initialPosition;

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
      enemies(enemies),
      initialDelay(1000),
      movementTime(9),
      animationTime(100)
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

void Player::checkPositionWithRespectToNodes()
{
    for (auto const& node : nodes)
    {
        if (isInNode(node)) //player is in a node
        {
            std::map<MovementDirection, bool> movementPossibleFromTheNode;

            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(MovementDirection::UP, node.possibleUpward));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(MovementDirection::LEFT, node.possibleLeftward));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(MovementDirection::DOWN, node.possibleDownward));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(MovementDirection::RIGHT, node.possibleRightward));

            if (movementPossibleFromTheNode.find(pendingDirection)->second) //check if a pending move can be performed
                setMovement(pendingDirection);
            else if (!movementPossibleFromTheNode.find(currentDirection)->second) //check if Pac-Man can not continue going in his current direction and must be stopped
                setMovement(currentDirection, false);
            return;
        }
    }

    if ((::contains({MovementDirection::UP, MovementDirection::DOWN}, currentDirection) &&
         ::contains({MovementDirection::UP, MovementDirection::DOWN}, pendingDirection))
        ||
        (::contains({MovementDirection::LEFT, MovementDirection::RIGHT}, currentDirection) &&
         ::contains({MovementDirection::LEFT, MovementDirection::RIGHT}, pendingDirection)))
    {
        setMovement(pendingDirection);
    }
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

void Player::setMovement(Player::MovementDirection const newDirection, bool movementPossibility)
{
    currentDirection = newDirection;
    isMoving = movementPossibility;
}

void Player::allowToMove()
{
    initialDelayTimer.stop();
    movementTimer.start(movementTime);
    isMoving = true;
}

void Player::move()
{
    checkPositionWithRespectToNodes(); //meaning: check collisions with enemies or being in nodes

    //moving Pac-Man
    if (isMoving)
    {
        switch (currentDirection)
        {
        case MovementDirection::LEFT:
            setPos(x() - 1, y());
            break;
        case MovementDirection::RIGHT:
            setPos(x() + 1, y());
            break;
        case MovementDirection::UP:
            setPos(x(), y() - 1);
            break;
        case MovementDirection::DOWN:
            setPos(x(), y() + 1);
            break;
        }
    }

    //teleporting on the edges of a map
    if (x() + pixmap().width() < 0)
        setPos(450, y());
    else if (x() > 450)
        setPos(-pixmap().width(), y());

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
