// SPDX-FileCopyrightText: Copyright 2023-2024 copyrat90
// SPDX-License-Identifier: 0BSD

#include "TaskManager.hpp"

#include "TaskSignal.hpp"

namespace task
{

void TaskManager::update()
{
    auto beforeIt = _tasks.before_begin();
    auto it = _tasks.begin();

    while (it != _tasks.end())
    {
        if (it->done())
        {
            it = _tasks.erase_after(beforeIt);
        }
        else
        {
            auto& signal = it->getTaskSignal();

            // Resume coroutine that awaits `TimeAwaiter` when there's no remaining ticks to wait
            if (signal.kind == task::TaskSignal::Kind::TIME)
            {
                BN_ASSERT(signal.argument.type() == bn::type_id<int>());

                if (--signal.argument.value<int>() <= 0)
                    it->resume();
            }

            beforeIt = it;
            ++it;
        }
    }
}

void TaskManager::addTask(task::Task&& task)
{
    _tasks.push_front(std::move(task));
}

void TaskManager::onSignal(const task::TaskSignal& received)
{
    using SigKind = task::TaskSignal::Kind;

    // All tasks are cancelled when scene is destroyed
    if (received.kind == SigKind::SCENE_DESTROYED)
    {
        _tasks.clear();
    }
    else
    {
        for (auto& task : _tasks)
        {
            auto& signal = task.getTaskSignal();

            if (signal.kind != received.kind)
                continue;

            switch (received.kind)
            {
            case SigKind::NPC_WALK_END:
                // NPC id is stored in `TaskSignal::argument` as int
                BN_ASSERT(signal.argument.type() == bn::type_id<int>());
                BN_ASSERT(received.argument.type() == bn::type_id<int>());

                // Resume the task of the same NPC id.
                if (signal.argument.value<int>() == received.argument.value<int>())
                {
                    // Pass the moved distance of NPC.
                    // It will be a return value of `co_await NPCWalkEndAwaiter(..)`
                    signal.result = received.result;

                    // Resume the task.
                    task.resume();
                }
                break;

            default:
                BN_ERROR("Invalid SigKind=", (int)received.kind);
            }
        }
    }
}

} // namespace task
