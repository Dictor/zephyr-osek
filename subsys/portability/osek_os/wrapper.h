/*
 * Copyright (c) 2024 Jeonghyun Kim <kimdictor@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __OSEK_WRAPPER_H__
#define __OSEK_WRAPPER_H__

#include <zephyr/kernel.h>
#include <osek_os.h>
#include <stdbool.h>

#include "type.h"

// internal object storage
extern osek_task_t osek_task[CONFIG_OSEK_OS_MAX_TASK_COUNT];
extern osek_alarm_t osek_alarm[CONFIG_OSEK_OS_MAX_ALARM_COUNT];
extern osek_resource_t osek_resource[CONFIG_OSEK_OS_MAX_RESOURCE_COUNT];

// check data type's valid
bool osek_task_is_valid(TaskType task_id);
bool osek_resource_is_valid(ResourceType resource_id);
bool osek_alarm_is_valid(AlarmType alarm_id);

// every below function doesn't check data type's valid.
// checking responsibility is on caller.
bool osek_task_occupied_resource(TaskType task_id);
bool osek_task_is_activated(TaskType task_id);
TaskType osek_task_from_zephyr_thread(k_tid_t thread_id);
void osek_alarm_timer_expiry_handler(struct k_timer *timer);

// internal object registering methods
TaskType osek_task_register(k_tid_t thread_id, bool is_extended);
AlarmType osek_alarm_register(struct k_timer *timer, AlarmBaseRefType base, bool is_autostart,
			      bool is_single, TickType period);
void osek_alarm_attach_callback(AlarmType alarm, osek_alarm_callback_t callback);
void osek_alarm_attach_event(AlarmType alarm, void *event);
void osek_alarm_attach_activation(AlarmType alarm, TaskType task);
ResourceType osek_resource_register(struct k_sem *lock, int ceiling_prio);

#endif