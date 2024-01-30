// SPDX-FileCopyrightText: Copyright 2024 copyrat90
// SPDX-License-Identifier: 0BSD

#include <bn_bg_palettes.h>
#include <bn_core.h>
#include <bn_display.h>
#include <bn_format.h>
#include <bn_keypad.h>
#include <bn_sprite_text_generator.h>
#include <bn_unique_ptr.h>

#include "BgBox.hpp"

#include "common_info.h"
#include "common_variable_8x16_sprite_font.h"

namespace
{

void updateCpuUsageText(bn::sprite_text_generator& textGen, int& cpuUpdateCounter, bn::fixed& maxCpuUsage,
                        bn::ivector<bn::sprite_ptr>& cpuSprites)
{
    constexpr int UPDATE_INTERVAL = 30;

    maxCpuUsage = bn::max(maxCpuUsage, bn::core::last_cpu_usage());

    if (++cpuUpdateCounter >= UPDATE_INTERVAL)
    {
        cpuSprites.clear();
        textGen.set_right_alignment();
        textGen.generate(112, -65, bn::format<11>("{}%", (maxCpuUsage * 100).round_integer()), cpuSprites);
        maxCpuUsage = 0;
        cpuUpdateCounter = 0;
    }
}

void moveAndScaleBgBoxScene(bn::sprite_text_generator& textGen, int& cpuUpdateCounter, bn::fixed& maxCpuUsage,
                            bn::ivector<bn::sprite_ptr>& cpuSprites)
{
    constexpr bn::string_view infoTextLines[] = {
        "PAD: move box",
        "A + PAD: enlarge box",
        "B + PAD: shrink box",
    };

    common::info info("Move & Scale BgBox", infoTextLines, textGen);

    bn::unique_ptr<demo::BgBox> box(
        new demo::BgBox({-16 - 1, -16 - 1, 32 + 2, 32 + 2}, 2, bn::colors::white, bn::colors::black));

    int cpuUsageLogTimer = 30;

    while (!bn::keypad::start_pressed())
    {
        constexpr bn::fixed MOVE_SPEED = 1.0f;

        auto rect = box->getRect();

        if (--cpuUsageLogTimer <= 0)
        {
            cpuUsageLogTimer = 30;
        }

        // enlarge
        if (bn::keypad::a_held())
        {
            if (bn::keypad::up_held())
            {
                rect.set_y(rect.y() - MOVE_SPEED);
                rect.set_height(bn::max(bn::fixed(0), rect.height() + MOVE_SPEED));
            }
            else if (bn::keypad::down_held())
            {
                rect.set_height(bn::max(bn::fixed(0), rect.height() + MOVE_SPEED));
            }
            if (bn::keypad::left_held())
            {
                rect.set_x(rect.x() - MOVE_SPEED);
                rect.set_width(bn::max(bn::fixed(0), rect.width() + MOVE_SPEED));
            }
            else if (bn::keypad::right_held())
            {
                rect.set_width(bn::max(bn::fixed(0), rect.width() + MOVE_SPEED));
            }
        }
        // shrink
        else if (bn::keypad::b_held())
        {
            if (bn::keypad::up_held())
            {
                rect.set_height(bn::max(bn::fixed(0), rect.height() - MOVE_SPEED));
            }
            else if (bn::keypad::down_held())
            {
                rect.set_y(rect.y() + MOVE_SPEED);
                rect.set_height(bn::max(bn::fixed(0), rect.height() - MOVE_SPEED));
            }
            if (bn::keypad::left_held())
            {
                rect.set_width(bn::max(bn::fixed(0), rect.width() - MOVE_SPEED));
            }
            else if (bn::keypad::right_held())
            {
                rect.set_x(rect.x() + MOVE_SPEED);
                rect.set_width(bn::max(bn::fixed(0), rect.width() - MOVE_SPEED));
            }
        }
        // move
        else
        {
            if (bn::keypad::up_held())
            {
                rect.set_y(rect.y() - MOVE_SPEED);
            }
            else if (bn::keypad::down_held())
            {
                rect.set_y(rect.y() + MOVE_SPEED);
            }
            if (bn::keypad::left_held())
            {
                rect.set_x(rect.x() - MOVE_SPEED);
            }
            else if (bn::keypad::right_held())
            {
                rect.set_x(rect.x() + MOVE_SPEED);
            }
        }

        box->setRect(rect);

        info.update();
        updateCpuUsageText(textGen, cpuUpdateCounter, maxCpuUsage, cpuSprites);
        bn::core::update();
    }
}

} // namespace

int main()
{
    bn::core::init();

    int cpuUpdateCounter = 0;
    bn::fixed maxCpuUsage = 0;
    bn::vector<bn::sprite_ptr, 4> cpuSprites;

    bn::sprite_text_generator textGen(common::variable_8x16_sprite_font);
    bn::bg_palettes::set_transparent_color(bn::color(16, 16, 16));

    while (true)
    {
        moveAndScaleBgBoxScene(textGen, cpuUpdateCounter, maxCpuUsage, cpuSprites);
        bn::core::update();
    }
}
