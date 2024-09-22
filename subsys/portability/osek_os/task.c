/*
 * Copyright (c) 2024 Jeonghyun Kim <kimdictor@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <osek_os.h>

#include "wrapper.h"

// (ch 13.2.3.1)
StatusType ActivateTask(TaskType TaskID)
{
    if (!osek_task_is_valid(TaskID)) {
        return E_OS_ID;
    }
    
    if (osek_task_is_activated(TaskID)) {
        return E_OS_LIMIT;
    }

    k_thread_resume(osek_task[TaskID].id);
    return E_OK;
}

// (ch 13.2.3.2)
// Not Standard:
//   1. Return E_OS_SYS when failed to convert zephyr thread to OSEK task
// Not implemented:
//   1. (Particularities) An internal resource assigned to the calling task is automatically
//   released.
StatusType TerminateTask(void)
{
    if (k_is_in_isr()) {
        return E_OS_CALLEVEL;
    }

    k_tid_t current_thread = k_current_get();
    TaskType task = osek_task_from_zephyr_thread(current_thread);
    if (task == NULL) {
        return E_OS_SYS;
    }

    if (osek_task_occupied_resource(task)) {
        return E_OS_RESOURCE;
    }

    k_thread_suspend(current_thread);

    // this function return shouldn't be evaluated, but return E_OK for activation of task
    return E_OK;
}

// (ch 13.2.3.3)
StatusType ChainTask(TaskType TaskID)
{
    if (k_is_in_isr()) {
        return E_OS_CALLEVEL;
    }

    if (!osek_task_is_valid(TaskID)) {
        return E_OS_ID;
    }

    k_tid_t current_thread = k_current_get();
    if (current_thread == osek_task[TaskID].id) {
        // (Particularities) If the succeeding task is identical with the current task, The
        // task will immediately become ready again. So, returns E_OK for going back current
        // thread's code.
        return E_OK;
    }

    if (osek_task_is_activated(TaskID)) {
        k_thread_suspend(current_thread);
        return E_OS_LIMIT;
    }

    if (osek_task_occupied_resource(TaskID)) {
        return E_OS_RESOURCE;
    }

    k_thread_resume(osek_task[TaskID].id);
    k_thread_suspend(current_thread);

    // this function return shouldn't be evaluated, but return E_OK for activation of task
    return E_OK;
}

// (ch 13.2.3.4)
// Not Standard:
//   1. Return E_OS_SYS when failed to convert zephyr thread to OSEK task
StatusType Schedule(void)
{

    if (k_is_in_isr()) {
        return E_OS_CALLEVEL;
    }

    k_tid_t current_thread = k_current_get();
    TaskRefType task = osek_task_from_zephyr_thread(current_thread);
    if (task == NULL) {
        return E_OS_SYS;
    }

    if (osek_task_occupied_resource(task)) {
        return E_OS_RESOURCE;
    }

    k_yield();
    return E_OK;
}

// (ch 13.2.3.5)
// Not Standard:
//   1. Return E_OS_SYS when failed to convert zephyr thread to OSEK task
StatusType GetTaskID(TaskRefType TaskID)
{
    k_tid_t current_thread = k_current_get();
    TaskType current_task = osek_task_from_zephyr_thread(current_thread);
    if (current_task == INVALID_TASK) {
        return E_OS_SYS;
    }

    *TaskID = current_task;
    return E_OK;
}

// (ch 13.2.3.6)
StatusType GetTaskState(TaskType TaskID, TaskStateRefType State)
{
    if (osek_task[TaskID].id == k_current_get()) {
        *State = RUNNING;
        return E_OK;
    }

    uint8_t state = osek_task[TaskID].id->base.thread_state;
    switch (state) {
    case _THREAD_DUMMY:
        // illegal thread state
        return E_OS_SYS;
    case _THREAD_DEAD:
    case _THREAD_ABORTING:
        // OSEK doesn't accapt thread terminations
        return E_OS_SYS;
    case _THREAD_PENDING:
    case _THREAD_PRESTART:
        *State = WAITING;
        break;
    case _THREAD_QUEUED:
        *State = READY;
        break;
    case _THREAD_SUSPENDING:
    case _THREAD_SUSPENDED:
        *State = SUSPENDED;
        break;
    }
    return E_OK;
}