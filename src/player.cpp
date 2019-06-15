#include "player.h"
#include "game.h"
#include <algorithm>
#include <map>
#include <utility>
#include <typeinfo>
#include "blinky.h"
#include "pinky.h"
#include "inky.h"
#include "clyde.h"
#include "regularpellet.h"
#include "superpellet.h"

template <class myType>
typename std::vector<myType>::const_iterator findInVector(std::vector<myType> const& vector, void* itemToBeFound);

Player::Player(std::vector<Node*> const& nodes,
               Score& score,
               LivesCounter& livesCounter,
               std::vector<RegularPellet*>& regularPellets,
               std::vector<SuperPellet*>& superPellets,
               Game const& game,
               std::vector<Enemy*> const& enemies)
    : Character(nodes),
      score(score),
      livesCounter(livesCounter),
      regularPellets(regularPellets),
      superPellets(superPellets),
      game(game),
      enemies(enemies),
      initialDelay(1000),
      movementTime(9),
      animationTime(100)
{
    QObject::connect(&initialDelayTimer, SIGNAL(timeout()), this, SLOT(allowToMove()));
    QObject::connect(&movementTimer, SIGNAL(timeout()), this, SLOT(move()));
    QObject::connect(&animationTimer, SIGNAL(timeout()), this, SLOT(chompingAnimation()));
    init();
}

void Player::checkCollisionWithPelletsAndGhosts()
{
    // get a list of all the items currently colliding with all dots and enemies
    QList<QGraphicsItem*> allItems = collidingItems();

    // if one of the colliding items is Pac-Man, destroy that dot
    for(int i = 0; i < allItems.size(); ++i)
    {
        if(typeid(*(allItems[i])) == typeid(RegularPellet))
        {
            score.little_increase(); // increase the score by 10

            // remove from a vector
            regularPellets.erase(findInVector(regularPellets, reinterpret_cast<void*>(allItems[i])));

            // remove them from the scene (still on the heap)
            scene()->removeItem(allItems[i]);

            // delete them from the heap to save memory
            delete allItems[i];
        }
        else if(typeid(*(allItems[i])) == typeid(SuperPellet))
        {
            score.big_increase(); // increase the score by 50

            // remove from a vector
            superPellets.erase(findInVector(superPellets, reinterpret_cast<void*>(allItems[i])));

            // remove them from the scene (still on the heap)
            scene()->removeItem(allItems[i]);

            // delete them from the heap to save memory
            delete allItems[i];

            // checking if any of enemies is frightened (if not, that means that player will not get extra points
            if(!isAnyOfEnemiesFrightened())
            {
                score.resetMultiplier();
            }

            //frighten enemies - requires getting rid of const to call enableRunawayState
            std::for_each(const_cast<std::vector<Enemy*>&>(enemies).begin(),
                          const_cast<std::vector<Enemy*>&>(enemies).end(),
                          [](Enemy* ptrToEnemy){ptrToEnemy->enableRunawayState();});
        }
        else if(typeid(*(allItems[i])) == typeid(Blinky) || typeid(*(allItems[i])) == typeid(Pinky)
                || typeid(*(allItems[i])) == typeid(Inky) || typeid(*(allItems[i])) == typeid(Clyde))
        {
            if(!dynamic_cast<Enemy*>(allItems[i])->isFrightened())
            {
                livesCounter.decrease();
                if(livesCounter.getLives() == 0) prepareToEndGame(LOSS_OF_LIVES);
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
    if(!regularPellets.size() && !superPellets.size())
    {
        prepareToEndGame(VICTORY);
    }
}

void Player::checkPositionWithRespectToNodes()
{
    for(std::vector<Node*>::const_iterator it = nodes.cbegin();; it++)
    {
        if(it == nodes.cend()) /* there is the last iteration of a list,
        nothing happened but at least check along the player's movement line */
        {
            if(currentDirection == UP || currentDirection == DOWN)
            {
                if(pendingDirection == UP || pendingDirection == DOWN)
                    setMovement(pendingDirection);
            }
            else if(currentDirection == LEFT || currentDirection == RIGHT)
            {
                if(pendingDirection == LEFT || pendingDirection == RIGHT)
                    setMovement(pendingDirection);
            }
            break;
        }
        if(isInNode(**it)) //player is in a node
        {
            std::map<MovementDirection, bool> movementPossibleFromTheNode;

            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(UP, (*it)->possibleUpward));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(LEFT, (*it)->possibleLeftward));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(DOWN, (*it)->possibleDownward));
            movementPossibleFromTheNode.insert(std::pair<MovementDirection, bool>(RIGHT, (*it)->possibleRightward));

            if(movementPossibleFromTheNode.find(pendingDirection)->second) //check if a pending move can be performed
            {
                setMovement(pendingDirection);
            }
            else if(movementPossibleFromTheNode.find(currentDirection)->second) //check if Pac-Man can continue in his current direction
            {
                // do nothing
            }
            else setMovement(currentDirection, false); // Why currentMove? He's going to be stopped anyway

            break;
        }
    }
}

void Player::init()
{
    disable();
    setPixmap(QPixmap(":/sprites/sprites/pacopenleft.png"));
    setPos(210, 347);
    currentDirection = pendingDirection = LEFT;
    moving = false;
    initialDelayTimer.start(initialDelay);
    animationTimer.start(animationTime);
}

bool Player::isAnyOfEnemiesFrightened() const
{
    for(std::vector<Enemy*>::const_iterator it = enemies.cbegin(); it != enemies.cend(); ++it)
    {
        if((*it)->isFrightened()) return true;
    }
    return false;
}

void Player::keyPressEvent(QKeyEvent* event)
{
    // move the player left and right
    if(event->key() == Qt::Key_Left) pendingDirection = LEFT;
    else if(event->key() == Qt::Key_Right) pendingDirection = RIGHT;
    else if(event->key() == Qt::Key_Up) pendingDirection = UP;
    else if(event->key() == Qt::Key_Down) pendingDirection = DOWN;
    else if(event->key() == Qt::Key_Escape) prepareToEndGame(PRESSED_ESC);
}

void Player::prepareToEndGame(Player::QuitReason reason) const
{
    if(reason == PRESSED_ESC)
    {
        endGame();
        return;
    }
    else if(reason == VICTORY)
    {
        QGraphicsTextItem* text = new QGraphicsTextItem("YOU WIN!");
        text->setPos(120, 210);
        text->setDefaultTextColor(Qt::red);
        text->setFont(QFont("times", 34));
        scene()->addItem(text);
    }
    else if(reason == LOSS_OF_LIVES)
    {
        QGraphicsTextItem*  text = new QGraphicsTextItem("YOU LOSE!");
        text->setPos(120, 210);
        text->setDefaultTextColor(Qt::red);
        text->setFont(QFont("times", 34));
        scene()->addItem(text);
    }

    // disable all timers (disablesMovements)
    const_cast<Player*>(this)->disable(); // another way is to make timers mutable
    std::for_each(const_cast<std::vector<Enemy*>&>(enemies).begin(),
                  const_cast<std::vector<Enemy*>&>(enemies).end(),
                  [](Enemy* ptrToEnemy){ptrToEnemy->disable();});

    QTimer::singleShot(3000, this, SLOT(endGame()));
}

void Player::setMovement(Player::MovementDirection const newDirection, bool movementPossibility)
{
    currentDirection = newDirection;
    moving = movementPossibility;
}

void Player::allowToMove()
{
    initialDelayTimer.stop();
    movementTimer.start(movementTime);
    moving = true;
}

void Player::chompingAnimation()
{
    static bool phase = false;

    if(currentDirection == LEFT)
    {
        if(!phase) setPixmap(QPixmap(":/sprites/sprites/pacopenleft.png"));
        else setPixmap(QPixmap(":/sprites/sprites/pacmidleft.png"));
    }
    else if(currentDirection == RIGHT)
    {
        if(!phase) setPixmap(QPixmap(":/sprites/sprites/pacopenright.png"));
        else setPixmap(QPixmap(":/sprites/sprites/pacmidright.png"));
    }
    else if(currentDirection == UP)
    {
        if(!phase) setPixmap(QPixmap(":/sprites/sprites/pacopenup.png"));
        else setPixmap(QPixmap(":/sprites/sprites/pacmidup.png"));

    }
    else if(currentDirection == DOWN)
    {
        if(!phase) setPixmap(QPixmap(":/sprites/sprites/pacopendown.png"));
        else setPixmap(QPixmap(":/sprites/sprites/pacmiddown.png"));
    }

    phase =! phase;
}

void Player::endGame() const
{
    const_cast<Game&>(game).close();
}

void Player::move()
{
    checkPositionWithRespectToNodes(); //meaning: check collisions with enemies or being in nodes

    //moving Pac-Man
    if(moving)
    {
        switch(currentDirection)
        {
        case LEFT:
            setPos(x() - 1, y());
            break;
        case RIGHT:
            setPos(x() + 1, y());
            break;
        case UP:
            setPos(x(), y() - 1);
            break;
        case DOWN:
            setPos(x(), y() + 1);
            break;
        }
    }

    //teleporting on the edges of a map
    if(x() + this->pixmap().width() < 0) setPos(450, y());
    else if(x() > 450) setPos(-this->pixmap().width(), y());

    checkCollisionWithPelletsAndGhosts();
}

template <class myType>
typename std::vector<myType>::const_iterator findInVector(std::vector<myType> const& vector, void* itemToBeFound)
{
    typename std::vector<myType>::const_iterator it = vector.begin();
    while(*it != itemToBeFound)
    {
        it++;
    }
    return it;
}
