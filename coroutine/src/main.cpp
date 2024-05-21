// SPDX-FileCopyrightText: Copyright 2023-2024 copyrat90
// SPDX-License-Identifier: 0BSD

#include <bn_core.h>
#include <bn_keypad.h>
#include <bn_sprite_text_generator.h>

#include "TaskManager.hpp"
#include "WalkingNinja.hpp"

#include "common_info.h"
#include "common_variable_8x16_sprite_font.h"

int main()
{
    bn::core::init();

    bn::sprite_text_generator textGen(common::variable_8x16_sprite_font);

    static constexpr bn::string_view infoTextLines[] = {
        "A/R: change moving direction",
        "B/L: stop moving",
    };

    common::info info("C++20 Coroutine Task Demo", infoTextLines, textGen);

    task::TaskManager taskManager;

    WalkingNinja ninja1(1, {WalkingNinja::LEFT_MOST_X, 10}, taskManager, textGen);
    WalkingNinja ninja2(2, {WalkingNinja::LEFT_MOST_X, 40}, taskManager, textGen);

    while (true)
    {
        if (bn::keypad::a_pressed())
            ninja1.changeWalkDirection();
        if (bn::keypad::b_pressed())
            ninja1.stopWalk();

        if (bn::keypad::r_pressed())
            ninja2.changeWalkDirection();
        if (bn::keypad::l_pressed())
            ninja2.stopWalk();

        ninja1.update();
        ninja2.update();

        taskManager.update();

        info.update();
        bn::core::update();
    }
}
