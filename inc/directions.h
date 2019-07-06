#ifndef DIRECTIONS_H
#define DIRECTIONS_H

enum class Direction {UP, DOWN, RIGHT, LEFT};

constexpr Direction opposite(Direction const direction) noexcept {
    switch (direction)
    {
    case Direction::UP: return Direction::DOWN;
    case Direction::DOWN: return Direction::UP;
    case Direction::RIGHT: return Direction::LEFT;
    case Direction::LEFT: return Direction::RIGHT;
    }
    return Direction::UP;
}

#endif // DIRECTIONS_H
