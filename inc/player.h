#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"

#include <chrono>

class Enemy;
class LifeCounter;
class QKeyEvent;
class RegularPellet;
class Score;
class SuperPellet;

class Player : public Character
{
    Q_OBJECT
public:
    Player(std::vector<Node> const& nodes,
           Score& score,
           LifeCounter& life_counter,
           std::vector<RegularPellet*>& regular_pellets,
           std::vector<SuperPellet*>& super_pellets,
           std::function<void()> quit_callback,
           std::vector<Enemy*> const& enemies);
    //last two are const to avoid inattentively usages of this (have to const_cast though)

    void init() override;

    auto get_current_direction() const noexcept -> MovementDirection;

private:
    static constexpr QPointF InitialPosition = {210, 347};
    static constexpr std::chrono::milliseconds MovementTime{9};
    static constexpr std::chrono::milliseconds AnimationTime{100};
    static constexpr std::chrono::milliseconds InitialDelay{1000};

    enum class QuitReason{PRESSED_ESC, DEFEAT, VICTORY};

    void deinit() override;
    void keyPressEvent(QKeyEvent* event) override;

    void check_collision_with_pellets_and_ghosts();
    auto is_any_of_enemies_frightened() const -> bool;
    auto get_sprites() -> SpriteMap<MovementDirection>;
    void prepare_to_end_game(QuitReason reason) const;
    void set_movement(MovementDirection new_direction) noexcept;
    void set_movement_in_node(Node const& node);
    void stop() noexcept;
    void try_to_set_opposite_movement() noexcept;

    MovementDirection pending_direction;
    Score& score;
    LifeCounter& life_counter;
    std::vector<RegularPellet*>& regular_pellets;
    std::vector<SuperPellet*>& super_pellets;
    std::function<void()> quit_callback;
    std::vector<Enemy*> const& enemies;
    bool moving;

private slots:
    void allow_to_move() override;
    void change_sprite() override;
    void move() override;

    void end_game() const;
};

inline auto Player::get_current_direction() const noexcept -> MovementDirection
{
    return current_direction;
}

#endif // PLAYER_H
