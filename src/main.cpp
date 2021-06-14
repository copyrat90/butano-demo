#include "bn_core.h"

#include "bn_affine_bg_items_image_file.h"
#include "bn_affine_bg_map_cell.h"
#include "bn_affine_bg_map_ptr.h"
#include "bn_affine_bg_ptr.h"
#include "bn_fixed.h"
#include "bn_format.h"
#include "bn_log.h"
#include "bn_optional.h"
#include "bn_span.h"

int main()
{
    bn::core::init();

    bn::affine_bg_ptr bg = bn::affine_bg_items::image_file.create_bg(0, 0);
    // bn::affine_bg_map_cell is the unique id number,
    // which is just a type alias of uint8_t.
    bn::span<const bn::affine_bg_map_cell> cells = bg.map().cells_ref().value();
    // You can get unique id by indexing `cells` like this:
    int x = 3, y = 7;
    // COLUMNS: how many tiles in a row?
    constexpr int COLUMNS = bn::affine_bg_items::image_file.map_item().dimensions().width();
    // Get the unique id
    auto unique_id_of_x_by_y = cells[x + y * COLUMNS];

    BN_LOG(bn::format<32>("({},{}) is {}", x, y, unique_id_of_x_by_y));

    while (true)
    {
        bn::core::update();
    }
}
