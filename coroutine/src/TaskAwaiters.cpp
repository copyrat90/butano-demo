// SPDX-FileCopyrightText: Copyright 2023-2024 copyrat90
// SPDX-License-Identifier: 0BSD

#include "TaskAwaiters.hpp"

#include <bn_assert.h>

namespace task
{

SignalAwaiter::SignalAwaiter(const task::TaskSignal& signal) : _signal(signal)
{
    BN_ASSERT(signal.kind != task::TaskSignal::Kind::TIME, "`TIME` signal must use `TimeAwaiter` instead");
    BN_ASSERT(signal.kind != task::TaskSignal::Kind::NPC_WALK_END,
              "`NPC_WALK_END` signal must use `DialogChoiceAwaiter` instead");
}

bool SignalAwaiter::await_ready() const
{
    return false;
}

void SignalAwaiter::await_suspend(task::Task::CoHandle coHandle)
{
    coHandle.promise().taskSignal = _signal;
}

void SignalAwaiter::await_resume()
{
}

TimeAwaiter::TimeAwaiter(int ticks) : _ticks(ticks)
{
}

bool TimeAwaiter::await_ready() const
{
    return _ticks <= 0;
}

void TimeAwaiter::await_suspend(task::Task::CoHandle coHandle)
{
    auto& signal = coHandle.promise().taskSignal;

    signal.kind = TaskSignal::Kind::TIME;
    signal.argument = _ticks;
}

void TimeAwaiter::await_resume()
{
}

NpcWalkEndAwaiter::NpcWalkEndAwaiter(int npcId) : _npcId(npcId)
{
}

bool NpcWalkEndAwaiter::await_ready() const
{
    return false;
}

void NpcWalkEndAwaiter::await_suspend(task::Task::CoHandle coHandle)
{
    _coHandle = coHandle;

    auto& signal = coHandle.promise().taskSignal;
    signal.kind = TaskSignal::Kind::NPC_WALK_END;
    signal.argument = _npcId;
}

auto NpcWalkEndAwaiter::await_resume() -> bn::fixed
{
    auto& result = _coHandle.promise().taskSignal.result;
    BN_ASSERT(result.type() == bn::type_id<bn::fixed>());
    return result.value<bn::fixed>();
}

} // namespace task
