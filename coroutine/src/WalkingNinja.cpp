// SPDX-FileCopyrightText: Copyright 2023-2024 copyrat90
// SPDX-License-Identifier: 0BSD

#include "WalkingNinja.hpp"

#include <bn_string.h>

#include "TaskManager.hpp"

namespace
{

constexpr const bn::sprite_item& SPR_ITEM = bn::sprite_items::ninja;

constexpr bn::array<uint16_t, 4> LEFT_GFX_IDXES = {8, 9, 10, 11};
constexpr bn::array<uint16_t, 4> RIGHT_GFX_IDXES = {12, 13, 14, 15};
constexpr uint16_t STOP_GFX_IDX = 0;

constexpr bn::fixed_point TEXT_DIFF{0, -15};

constexpr int ANIM_WAIT_UPDATES = 15;
constexpr int MOVE_DURATION_UPDATES = 120;

auto clampInitPos(bn::fixed_point pos) -> bn::fixed_point
{
    pos.set_x(bn::clamp(pos.x(), WalkingNinja::LEFT_MOST_X, WalkingNinja::RIGHT_MOST_X));
    return pos;
}

} // namespace

WalkingNinja::WalkingNinja(int npcId, const bn::fixed_point& initPos, task::TaskManager& taskManager,
                           bn::sprite_text_generator& textGen)
    : _npcId(npcId), _taskManager(taskManager), _textGen(textGen), _prevDirection(Direction::NONE),
      _prevX(clampInitPos(initPos).x()), _sprite(SPR_ITEM.create_sprite(clampInitPos(initPos), STOP_GFX_IDX))
{
}

void WalkingNinja::update()
{
    if (_spriteAnimAction && !_spriteAnimAction->done())
        _spriteAnimAction->update();

    if (_spriteMoveAction && !_spriteMoveAction->done())
    {
        _spriteMoveAction->update();

        if (_spriteMoveAction->done())
            stopWalk();
    }

    for (auto& textMove : _distanceTextMoveActions)
        if (!textMove.done())
            textMove.update();
}

void WalkingNinja::changeWalkDirection()
{
    stopWalk();

    // start new ninja move task
    auto task = walk();
    if (!task.done())
        _taskManager.addTask(std::move(task));
}

void WalkingNinja::stopWalk()
{
    // end walking by sending `NPC_WALK_END` signal.
    // this resume to the ninja move task to finish it, if this ninja is moving.
    _taskManager.onSignal(task::TaskSignal{
        .kind = task::TaskSignal::Kind::NPC_WALK_END,
        .argument = decltype(task::TaskSignal::argument)(_npcId),
        .result = decltype(task::TaskSignal::result)(_sprite.x() - _prevX),
    });
}

auto WalkingNinja::walk() -> task::Task
{
    _prevX = _sprite.x();

    // start moving ninja to `direction` w/ texts
    auto curDirection = (_prevDirection == Direction::RIGHT) ? Direction::LEFT : Direction::RIGHT;

    const auto& gfxIdxes = (curDirection == Direction::LEFT) ? LEFT_GFX_IDXES : RIGHT_GFX_IDXES;
    const auto& destination = (curDirection == Direction::LEFT) ? leftMostPos() : rightMostPos();

    _spriteAnimAction =
        decltype(_spriteAnimAction)::value_type::forever(_sprite, ANIM_WAIT_UPDATES, SPR_ITEM.tiles_item(), gfxIdxes);
    _spriteMoveAction = bn::sprite_move_to_action(_sprite, MOVE_DURATION_UPDATES, destination);

    const bn::fixed_point diff = destination - _sprite.position();

    BN_ASSERT(_distanceTextMoveActions.empty());
    for (auto& text : _distanceTexts)
        _distanceTextMoveActions.emplace_back(text, MOVE_DURATION_UPDATES, text.position() + diff);

    // await for the walk to end
    task::NpcWalkEndAwaiter walkEndAwaiter(_npcId);
    const bn::fixed movedDistance = co_await walkEndAwaiter;

    // stop moving
    _spriteAnimAction.reset();
    _spriteMoveAction.reset();
    _distanceTextMoveActions.clear();
    _sprite.set_tiles(SPR_ITEM.tiles_item(), STOP_GFX_IDX);

    // update distance text with new moved distance
    _distanceTexts.clear();

    const auto prevAlign = _textGen.alignment();
    _textGen.set_center_alignment();

    _textGen.generate(_sprite.position() + TEXT_DIFF, bn::to_string<TEXT_SPR_CNT * 4>(movedDistance), _distanceTexts);
    for (auto& text : _distanceTexts)
        text.set_bg_priority(3);

    _textGen.set_alignment(prevAlign);

    _prevDirection = curDirection;
    co_return;
}

auto WalkingNinja::leftMostPos() -> bn::fixed_point
{
    return {LEFT_MOST_X, _sprite.y()};
}

auto WalkingNinja::rightMostPos() -> bn::fixed_point
{
    return {RIGHT_MOST_X, _sprite.y()};
}
