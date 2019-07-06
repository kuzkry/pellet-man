#ifndef ENEMY_H
#define ENEMY_H

#include "character.h"

#include <QObject>
#include <QSize>

#include <chrono>

class Player;

class Enemy : public QObject, public Character
{
    Q_OBJECT
public:
    Enemy(Player const& player, std::vector<Node> const& nodes, SpriteMap<MovementDirection> regular_sprites, std::chrono::milliseconds delay_to_leave_hideout);

    void deinit() override;
    void init() override;

    void enable_runaway_state();
    auto is_frightened() const noexcept -> bool;

signals:
    void entered_chase_mode();

protected:
    enum class FrightState {INITIAL_BLUE, TRANSFORMING_WHITE};

    virtual auto make_turn_decision(std::vector<MovementDirection> const& possibleMovements) const -> MovementDirection = 0;

    Player const& player;

private:
    static constexpr QPointF InitialPosition = {210, 210};
    static constexpr QPointF InitialChasePoint = {210, 168};
    static constexpr QSize PixmapScaling = {26, 26};
    static constexpr std::chrono::milliseconds MovementTime{10};
    static constexpr std::chrono::milliseconds AnimationTime{10};
    static constexpr std::chrono::milliseconds SingleBlinkTime{20 * MovementTime};
    static constexpr std::chrono::milliseconds BlinkingInterval{2000};
    static constexpr std::chrono::milliseconds RunawayTime{8000};

    static auto get_frightened_sprites() -> SpriteMap<FrightState>;
    auto next_direction(Node const& node) const -> MovementDirection;
    auto next_fright_state() const noexcept -> FrightState;
    template <typename Key>
    static auto rescale_pixmaps(SpriteMap<Key> sprite_map) -> SpriteMap<Key>;

    QTimer blinking_mode_timer, frightened_mode_timer;
    SpriteMap<FrightState> const frightened_sprites;
    std::chrono::milliseconds const delay_to_leave_hideout;
    FrightState fright_state;
    bool frightened;

private slots:
    void allow_to_move() override;
    void change_sprite() override;
    void move() override;

    void blink();
    void disable_runaway_state();
    void release_from_hideout();
};

inline auto Enemy::is_frightened() const noexcept -> bool
{
    return frightened;
}

#endif // ENEMY_H
