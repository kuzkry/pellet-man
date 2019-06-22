#ifndef DIRECTIONS_H
#define DIRECTIONS_H

enum class MovementDirection {UP, DOWN, RIGHT, LEFT};

inline MovementDirection opposite(MovementDirection const direction) {
    switch (direction)
    {
    case MovementDirection::UP: return MovementDirection::DOWN;
    case MovementDirection::DOWN: return MovementDirection::UP;
    case MovementDirection::RIGHT: return MovementDirection::LEFT;
    case MovementDirection::LEFT: return MovementDirection::RIGHT;
    }
    return MovementDirection::UP;
}

#endif // DIRECTIONS_H
