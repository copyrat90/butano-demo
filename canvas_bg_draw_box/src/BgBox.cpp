// SPDX-FileCopyrightText: Copyright 2024 copyrat90
// SPDX-License-Identifier: 0BSD

#include "BgBox.hpp"

#include <bit>

#include <bn_memory.h>
#include <bn_regular_bg_item.h>
#include <bn_regular_bg_map_cell_info.h>

#include "bn_regular_bg_tiles_items_test_numbers.h" // TEST

namespace demo
{

namespace
{

enum TileIdx
{
    EMPTY = 0,

    TOP_LEFT_OUTER,
    TOP_OUTER,
    TOP_RIGHT_OUTER,
    LEFT_OUTER,
    TOP_LEFT_INNER,
    TOP_INNER,
    TOP_RIGHT_INNER,
    RIGHT_OUTER,
    LEFT_INNER,
    MID_INNER,
    RIGHT_INNER,
    BOTTOM_LEFT_INNER,
    BOTTOM_INNER,
    BOTTOM_RIGHT_INNER,
    BOTTOM_LEFT_OUTER,
    BOTTOM_OUTER,
    BOTTOM_RIGHT_OUTER,

    TILE_IDX_COUNT
};

} // namespace

BgBox::BgBox(const bn::top_left_fixed_rect& boxRect, int borderThickness, bn::optional<bn::color> borderColor,
             bn::optional<bn::color> fillColor)
    : _borderThickness(borderThickness), _borderColorIdx([](int thickness, bool has_border_color) {
          if (thickness <= 0)
              return 2;
          if (!has_border_color)
              return 0;
          return 1;
      }(borderThickness, borderColor.has_value())),
      _fillColorIdx([](bool has_fill_color) { return has_fill_color ? 2 : 0; }(fillColor.has_value())), _cells{},
      _tiles{}, _colors{}, _mapItem(_cells[0], MAP_SIZE),
      _palette(bn::bg_palette_item(_colors, bn::bpp_mode::BPP_4).create_new_palette()),
      _tileset(bn::regular_bg_tiles_item(_tiles, bn::bpp_mode::BPP_4).create_tiles()),
      _map(_mapItem.create_new_map(_tileset, _palette)), _bg(bn::regular_bg_ptr::create(0, 0, _map))
{
    static_assert(UNIQUE_TILE_COUNT == TileIdx::TILE_IDX_COUNT);

    BN_ASSERT(0 <= borderThickness && borderThickness <= 8, "Invalid thickness: ", borderThickness);

    BN_ASSERT(!(borderThickness == 0 && !fillColor), "`BgBox` is always invisible");
    BN_ASSERT(!(borderThickness > 0 && !borderColor && !fillColor), "`BgBox` is always invisible");

    if (borderThickness > 0 && borderColor.has_value())
    {
        BN_ASSERT(_borderColorIdx == 1);
        _colors[_borderColorIdx] = *borderColor;
    }

    if (fillColor.has_value())
    {
        BN_ASSERT(_fillColorIdx == 2);
        _colors[_fillColorIdx] = *fillColor;
    }

    _palette.set_colors(bn::bg_palette_item(_colors, bn::bpp_mode::BPP_4));

    setRect(boxRect);
}

auto BgBox::getRect() const -> const bn::top_left_fixed_rect&
{
    return _rawRect;
}

auto BgBox::getPosition() const -> const bn::fixed_point&
{
    return _rawRect.position();
}

auto BgBox::getWidth() const -> bn::fixed
{
    return _rawRect.width();
}

auto BgBox::getHeight() const -> bn::fixed
{
    return _rawRect.height();
}

void BgBox::setRect(const bn::top_left_fixed_rect& boxRect)
{
    if (_rawRect == boxRect)
        return;

    _rawRect = boxRect;

    redrawAll();
}

void BgBox::setPosition(const bn::fixed_point& position)
{
    auto newRect = _rawRect;
    newRect.set_position(position);
    setRect(newRect);
}

void BgBox::setWidth(bn::fixed width)
{
    auto newRect = _rawRect;
    newRect.set_width(width);
    setRect(newRect);
}

void BgBox::setHeight(bn::fixed height)
{
    auto newRect = _rawRect;
    newRect.set_height(height);
    setRect(newRect);
}

void BgBox::redrawAll()
{
    redrawMap();
    redrawTiles();

    _map.reload_cells_ref();
    _tileset.reload_tiles_ref();
}

void BgBox::redrawMap()
{
    // clear map
    BN_ASSERT(sizeof(_cells) % 4 == 0);
    bn::memory::set_words(0, sizeof(_cells) / 4, _cells);

    const auto borderRect = convertToPositiveIntRect(getClampedRect());
    const auto fillRect = bn::top_left_rect{
        borderRect.x() + _borderThickness,
        borderRect.y() + _borderThickness,
        bn::max(0, borderRect.width() - 2 * _borderThickness),
        bn::max(0, borderRect.height() - 2 * _borderThickness),
    };

    // fill
    int xLo = fillRect.left() / TILE_LEN;
    int yLo = fillRect.top() / TILE_LEN;
    int xHi = (fillRect.right() - 1) / TILE_LEN;
    int yHi = (fillRect.bottom() - 1) / TILE_LEN;

    drawMapSides(false, xLo, xHi, yLo, yHi);

    for (int y = yLo + 1; y <= yHi - 1; ++y)
        for (int x = xLo + 1; x <= xHi - 1; ++x)
            setCell(x, y, TileIdx::MID_INNER);

    // border
    xLo = borderRect.left() / TILE_LEN;
    yLo = borderRect.top() / TILE_LEN;
    xHi = (borderRect.right() - 1) / TILE_LEN;
    yHi = (borderRect.bottom() - 1) / TILE_LEN;

    drawMapSides(true, xLo, xHi, yLo, yHi);
}

void BgBox::redrawTiles()
{
    // clear tiles
    BN_ASSERT(sizeof(_tiles) % 4 == 0);
    bn::memory::set_words(0, sizeof(_tiles) / 4, _tiles);

    // TODO

    // TEST: `test_numbers` tiles
    for (int i = 0; i < TILE_IDX_COUNT; ++i)
        _tiles[i] = bn::regular_bg_tiles_items::test_numbers.tiles_ref()[i];
    // END TEST
}

auto BgBox::getClampedRect() const -> bn::top_left_fixed_rect
{
    auto clamped = _rawRect;

    if (clamped.left() < -MAP_LEN.width() / 2)
    {
        clamped.set_width(bn::max(bn::fixed(0), clamped.width() - (-MAP_LEN.width() / 2 - clamped.x())));
        clamped.set_x(-MAP_LEN.width() / 2);
    }
    if (clamped.right() > MAP_LEN.width() / 2)
    {
        clamped.set_width(bn::max(bn::fixed(0), clamped.width() - (clamped.right() - MAP_LEN.width() / 2)));
    }
    if (clamped.top() < -MAP_LEN.height() / 2)
    {
        clamped.set_height(bn::max(bn::fixed(0), clamped.height() - (-MAP_LEN.height() / 2 - clamped.y())));
        clamped.set_y(-MAP_LEN.height() / 2);
    }
    if (clamped.bottom() > MAP_LEN.height() / 2)
    {
        clamped.set_height(bn::max(bn::fixed(0), clamped.height() - (clamped.bottom() - MAP_LEN.height() / 2)));
    }

    return clamped;
}

void BgBox::setCell(int x, int y, int tileIdx)
{
    // BN_LOG("BgBox::setCell(x=", x, ", y=", y, ", tileIdx=", tileIdx, ")");
    bn::regular_bg_map_cell& cell = _cells[_mapItem.cell_index(x, y)];
    bn::regular_bg_map_cell_info cellInfo(cell);
    cellInfo.set_tile_index(tileIdx);
    cell = cellInfo.cell();
}

void BgBox::drawMapSides(bool isOuter, int xLo, int xHi, int yLo, int yHi)
{
    // sides
    for (int x = xLo; x <= xHi; ++x)
    {
        setCell(x, yLo, isOuter ? TileIdx::TOP_OUTER : TileIdx::TOP_INNER);
        setCell(x, yHi, isOuter ? TileIdx::BOTTOM_OUTER : TileIdx::BOTTOM_INNER);
    }
    for (int y = yLo; y <= yHi; ++y)
    {
        setCell(xLo, y, isOuter ? TileIdx::LEFT_OUTER : TileIdx::LEFT_INNER);
        setCell(xHi, y, isOuter ? TileIdx::RIGHT_OUTER : TileIdx::RIGHT_INNER);
    }

    // corners
    setCell(xLo, yLo, isOuter ? TileIdx::TOP_LEFT_OUTER : TileIdx::TOP_LEFT_INNER);
    setCell(xHi, yLo, isOuter ? TileIdx::TOP_RIGHT_OUTER : TileIdx::TOP_RIGHT_INNER);
    setCell(xLo, yHi, isOuter ? TileIdx::BOTTOM_LEFT_OUTER : TileIdx::BOTTOM_LEFT_INNER);
    setCell(xHi, yHi, isOuter ? TileIdx::BOTTOM_RIGHT_OUTER : TileIdx::BOTTOM_RIGHT_INNER);
}

auto BgBox::getCanvas() -> bn::regular_bg_ptr&
{
    return _bg;
}

auto BgBox::getCanvas() const -> const bn::regular_bg_ptr&
{
    return _bg;
}

auto BgBox::convertToPositiveRect(const bn::top_left_fixed_rect& rawRect) -> bn::top_left_fixed_rect
{
    return bn::top_left_fixed_rect{
        rawRect.position().x() + MAP_LEN.width() / 2,
        rawRect.position().y() + MAP_LEN.height() / 2,
        rawRect.width(),
        rawRect.height(),
    };
}

auto BgBox::convertToPositiveIntRect(const bn::top_left_fixed_rect& rawRect) -> bn::top_left_rect
{
    return bn::top_left_rect{
        (rawRect.position().x() + MAP_LEN.width() / 2).floor_integer(),
        (rawRect.position().y() + MAP_LEN.height() / 2).floor_integer(),
        rawRect.width().round_integer(),
        rawRect.height().round_integer(),
    };
}

} // namespace demo
