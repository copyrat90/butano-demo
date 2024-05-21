// SPDX-FileCopyrightText: Copyright 2023-2024 copyrat90
// SPDX-License-Identifier: 0BSD

#pragma once

#include <bn_fixed.h>

#include "Task.hpp"

namespace task
{

class SignalAwaiter
{
public:
    SignalAwaiter(const task::TaskSignal& signal);

public:
    bool await_ready() const;
    void await_suspend(task::Task::CoHandle coHandle);
    void await_resume();

private:
    task::TaskSignal _signal;
};

class TimeAwaiter
{
public:
    TimeAwaiter(int ticks);

public:
    bool await_ready() const;
    void await_suspend(task::Task::CoHandle coHandle);
    void await_resume();

private:
    int _ticks;
};

class NpcWalkEndAwaiter
{
public:
    NpcWalkEndAwaiter(int npcId);

public:
    bool await_ready() const;
    void await_suspend(task::Task::CoHandle coHandle);

    /// @return the distance NPC walked
    auto await_resume() -> bn::fixed;

private:
    int _npcId;
    task::Task::CoHandle _coHandle;
};

} // namespace task
