#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"

#include <QObject>

#include <chrono>
#include <optional>

class Enemy;
class QKeyEvent;
class RegularPellet;
class SuperPellet;

class Player : public QObject, public Character
{
    Q_OBJECT
public:
    Player(std::vector<Node> const& nodes,
           std::vector<RegularPellet*> const& regular_pellets,
           std::vector<SuperPellet*> const& super_pellets,
           std::vector<Enemy*> const& enemies);

    void deinit() override;
    void init() override;

    auto get_current_direction() const noexcept -> Direction;

signals:
    void enemy_hit(Enemy*);
    void interrupted() const;
    void regular_pellet_eaten(RegularPellet*);
    void super_pellet_eaten(SuperPellet*);

private:
    static constexpr QPointF InitialPosition = {210, 347};
    static constexpr std::chrono::milliseconds MovementTime{9};
    static constexpr std::chrono::milliseconds AnimationTime{100};
    static constexpr std::chrono::milliseconds InitialDelay{1000};

    void keyPressEvent(QKeyEvent* event) override;

    void check_collisions();
    auto direction() const -> std::optional<Direction>;
    auto is_any_of_enemies_frightened() const -> bool;
    auto get_sprites() -> SpriteMap<Direction>;
    auto try_direction_at_node(Node const& node) const -> std::optional<Direction>;
    auto try_opposite_direction() const -> std::optional<Direction>;

    std::vector<RegularPellet*> const& regular_pellets;
    std::vector<SuperPellet*> const& super_pellets;
    std::vector<Enemy*> const& enemies;
    Direction pending_direction;

private slots:
    void allow_to_move() override;
    void animate_movement() override;
    void animate_sprites() override;
};

inline auto Player::get_current_direction() const noexcept -> Direction
{
    return current_direction;
}

#endif // PLAYER_H
