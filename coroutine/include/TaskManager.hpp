// SPDX-FileCopyrightText: Copyright 2023-2024 copyrat90
// SPDX-License-Identifier: 0BSD

#pragma once

#include <functional>

#include <bn_forward_list.h>

#include "Task.hpp"
#include "TaskAwaiters.hpp"

namespace task
{

class TaskManager
{
public:
    void update();

public:
    void addTask(task::Task&&);

    void onSignal(const task::TaskSignal&);

private:
    bn::forward_list<task::Task, 8> _tasks;
};

} // namespace task
