// SPDX-FileCopyrightText: Copyright 2021-2022 copyrat90
// SPDX-License-Identifier: 0BSD

#include "bn_core.h"

// While the bn::regular_bg_map_cell is an *unique tile id* of the regular bg, it is not directly a *tile index*.
// This is because flipped tile shares the same tile index.
// So, you need to convert it to the tile index by using the `bn::regular_bg_map_cell_info` helper class.
#include "bn_regular_bg_map_cell_info.h"

#include "bn_regular_bg_items_image_file.h"
#include "bn_regular_bg_map_cell.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_ptr.h"

#include "bn_fixed.h"
#include "bn_format.h"
#include "bn_log.h"
#include "bn_optional.h"
#include "bn_span.h"

void log_each_tile_index(const bn::regular_bg_ptr& bg, const int COLUMNS);

int main()
{
    bn::core::init();

    bn::regular_bg_ptr bg = bn::regular_bg_items::image_file.create_bg(0, 0);
    // bn::regular_bg_map_cell is the unique id number,
    // which is just a type alias of uint16_t.
    bn::span<const bn::regular_bg_map_cell> cells = bg.map().cells_ref().value();
    // You can get unique id by indexing `cells` like this:
    int x = 3, y = 7;
    // COLUMNS: how many tiles in a row?
    constexpr int COLUMNS = bn::regular_bg_items::image_file.map_item().dimensions().width();
    // Get the unique id (which is not a tile index, yet)
    bn::regular_bg_map_cell unique_id_of_x_by_y = cells[x + y * COLUMNS];
    // Convert the unique id to a tile index
    bn::regular_bg_map_cell_info cell_info(unique_id_of_x_by_y);
    int tile_index = cell_info.tile_index();

    // Log each tile index of the regular bg, generated with `image_file.bmp`.
    log_each_tile_index(bg, COLUMNS);

    BN_LOG(bn::format<32>("({},{}) is {}", x, y, tile_index));

    while (true)
    {
        bn::core::update();
    }
}

void log_each_tile_index(const bn::regular_bg_ptr& bg, const int COLUMNS)
{
    BN_LOG("Each tile_index() for the regular background generated with 'image_file.bmp'.");
    BN_LOG("");
    BN_LOG("Unfortunately, you can't do exactly same thing I wrote on itch.io with regular bg,");
    BN_LOG("as flipped tiles share the same tile index for the regular bg.");
    BN_LOG("See Butano Docs for 'bn::regular_bg_map_cell_info' to come up with your own workaround.");

    bn::span<const bn::regular_bg_map_cell> cells = bg.map().cells_ref().value();
    bn::regular_bg_map_cell_info cell_info;

    for (int y = 0; y < 39; ++y)
    {
        char _bn_string[BN_CFG_LOG_MAX_SIZE];
        bn::istring_base _bn_istring(_bn_string);
        bn::ostringstream _bn_string_stream(_bn_istring);

        for (int x = 0; x < 39; ++x)
        {
            cell_info.set_cell(cells[x + y * COLUMNS]);
            int tile_index = cell_info.tile_index();
            _bn_string_stream.append_args(tile_index, " ");
        }

        bn::log(_bn_istring);
    }
}
