// SPDX-FileCopyrightText: Copyright 2023-2024 copyrat90
// SPDX-License-Identifier: 0BSD

#pragma once

#include <bn_any.h>

namespace task
{

struct TaskSignal
{
    enum class Kind;

    Kind kind;
    bn::any<16> argument;
    bn::any<16> result;

    enum class Kind
    {
        SCENE_DESTROYED,
        TIME,
        NPC_WALK_END,
    };
};

} // namespace task
