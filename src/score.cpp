#include "score.h"

#include "enemy.h"
#include "gameconstants.h"

#include <QFont>

#include <algorithm>
#include <cstddef>
#include <limits>

namespace {

// needs to provide a constexpr function (std::pow is not constexpr for the time being)
constexpr auto pow(unsigned const base, unsigned const exp) noexcept -> unsigned
{
    return exp != 0 ? base * pow(base, exp - 1) : 1;
}

constexpr auto calculate_bonus_points(unsigned const multiplier) noexcept -> unsigned
{
    return 200 * pow(2, multiplier - 1);
}

constexpr auto calculate_max_points_during_single_pellet_time() noexcept -> std::uintmax_t
{
    std::uintmax_t max = 0;
    for (unsigned multiplier = 1; multiplier <= EnemyCount; ++multiplier)
        max += calculate_bonus_points(multiplier);
    return max;
}

constexpr auto calculate_max_points_for_max_survivability_of_multiplier() noexcept -> std::uintmax_t
{
    std::uintmax_t max = 0;
    for (unsigned multiplier = 1; multiplier <= (SuperPelletCount - 1) * (EnemyCount - 1) + EnemyCount; ++multiplier)
        max += calculate_bonus_points(multiplier);
    return max;
}

constexpr auto calculate_max_earnable_bonus() noexcept -> std::uintmax_t
{
    return std::max(SuperPelletCount * calculate_max_points_during_single_pellet_time(), calculate_max_points_for_max_survivability_of_multiplier());
}

} // namespace

Score::Score(std::vector<Enemy*> const& enemies) : enemies(enemies)
{
    constexpr std::uintmax_t max_earnable_bonus = calculate_max_earnable_bonus();
    constexpr std::uintmax_t max_earnable_score = max_earnable_bonus + RegularPelletCount * RegularPelletScore + SuperPelletCount * SuperPelletCount;
    static_assert(std::numeric_limits<decltype(score)>::max() >= max_earnable_score, "given score parameters may cause overflow of the score variable");

    setDefaultTextColor(Qt::white);
    setFont(QFont("times", 12));
    set_score();
}

void Score::increase(IncrementCause const cause)
{
    switch (cause)
    {
    case IncrementCause::REGULAR_PELLET:
        score += RegularPelletScore;
        break;
    case IncrementCause::SUPER_PELLET:
        score += SuperPelletScore;
        break;
    case IncrementCause::ENEMY_EATEN:
        score += calculate_bonus_points(multiplier++);
        break;
    }
    set_score();
}

void Score::try_to_reset_multiplier()
{
    if (std::none_of(enemies.cbegin(), enemies.cend(), [](Enemy* const enemy) { return enemy->is_frightened(); }))
        multiplier = 1;
}

void Score::set_score()
{
    setPlainText("Score: " + QString::number(score));
}
