// SPDX-FileCopyrightText: Copyright 2023-2024 copyrat90
// SPDX-License-Identifier: 0BSD

#pragma once

#include <bn_optional.h>
#include <bn_sprite_actions.h>
#include <bn_sprite_animate_actions.h>
#include <bn_sprite_text_generator.h>
#include <bn_vector.h>

#include "Task.hpp"

#include "bn_sprite_items_ninja.h"

namespace task
{
class TaskManager;
}

enum class Direction
{
    NONE,

    LEFT,
    RIGHT
};

class WalkingNinja
{
public:
    static constexpr bn::fixed LEFT_MOST_X = -80;
    static constexpr bn::fixed RIGHT_MOST_X = +80;

public:
    WalkingNinja(int npcId, const bn::fixed_point& initPos, task::TaskManager&, bn::sprite_text_generator&);

    WalkingNinja(const WalkingNinja&) = delete;
    WalkingNinja& operator=(const WalkingNinja&) = delete;

public:
    void update();

    void changeWalkDirection();
    void stopWalk();

private:
    // coroutine function to be suspended & resumed
    auto walk() -> task::Task;

    auto leftMostPos() -> bn::fixed_point;
    auto rightMostPos() -> bn::fixed_point;

private:
    static constexpr int TEXT_SPR_CNT = 16;

private:
    const int _npcId;
    task::TaskManager& _taskManager;

    bn::sprite_text_generator& _textGen;

    Direction _prevDirection;
    bn::fixed _prevX;

    bn::sprite_ptr _sprite;
    bn::optional<bn::sprite_animate_action<4>> _spriteAnimAction;
    bn::optional<bn::sprite_move_to_action> _spriteMoveAction;

    bn::vector<bn::sprite_ptr, 16> _distanceTexts;
    bn::vector<bn::sprite_move_to_action, 16> _distanceTextMoveActions;
};
