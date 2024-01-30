// SPDX-FileCopyrightText: Copyright 2024 copyrat90
// SPDX-License-Identifier: 0BSD

#pragma once

#include <bn_bg_palette_item.h>
#include <bn_bg_palette_ptr.h>
#include <bn_colors.h>
#include <bn_optional.h>
#include <bn_regular_bg_map_item.h>
#include <bn_regular_bg_map_ptr.h>
#include <bn_regular_bg_ptr.h>
#include <bn_regular_bg_tiles_ptr.h>
#include <bn_size.h>
#include <bn_tile.h>
#include <bn_top_left_fixed_rect.h>

namespace demo
{

/**
 * @brief Box drawn on top of a 4bpp `regular_bg` canvas.
 *
 * This owns a `bn::regular_bg_ptr` as a canvas, along with run-time tiles, map and palette.
 */
class BgBox
{
public:
    BgBox(const bn::top_left_fixed_rect& boxRect, int borderThickness = 2,
          bn::optional<bn::color> borderColor = bn::colors::white,
          bn::optional<bn::color> fillColor = bn::colors::black);

public:
    auto getRect() const -> const bn::top_left_fixed_rect&;
    auto getPosition() const -> const bn::fixed_point&;
    auto getWidth() const -> bn::fixed;
    auto getHeight() const -> bn::fixed;

    void setRect(const bn::top_left_fixed_rect& boxRect);
    void setPosition(const bn::fixed_point& position);
    void setWidth(bn::fixed width);
    void setHeight(bn::fixed height);

    auto getCanvas() -> bn::regular_bg_ptr&;
    auto getCanvas() const -> const bn::regular_bg_ptr&;

private:
    void redrawAll();
    void redrawMap();
    void redrawTiles();

    auto getClampedRect() const -> bn::top_left_fixed_rect;

private:
    void setCell(int x, int y, int tileIdx);
    void drawMapSides(bool isOuter, int xLo, int xHi, int yLo, int yHi);

private:
    static auto convertToPositiveRect(const bn::top_left_fixed_rect& rawRect) -> bn::top_left_fixed_rect;
    static auto convertToPositiveIntRect(const bn::top_left_fixed_rect& rawRect) -> bn::top_left_rect;

private:
    static constexpr bn::size MAP_LEN = {256, 256};
    static constexpr int TILE_LEN = 8;
    static constexpr bn::size MAP_SIZE = MAP_LEN / TILE_LEN;

    static constexpr int UNIQUE_TILE_COUNT = 18;

private:
    const int _borderThickness;

    const int _borderColorIdx;
    const int _fillColorIdx;

    bn::top_left_fixed_rect _rawRect;

    alignas(4) bn::regular_bg_map_cell _cells[MAP_SIZE.width() * MAP_SIZE.height()];
    alignas(4) bn::tile _tiles[UNIQUE_TILE_COUNT];
    alignas(4) bn::color _colors[16];

    bn::regular_bg_map_item _mapItem;

    bn::bg_palette_ptr _palette;
    bn::regular_bg_tiles_ptr _tileset;
    bn::regular_bg_map_ptr _map;

    bn::regular_bg_ptr _bg;
};

} // namespace demo
