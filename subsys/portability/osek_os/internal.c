/*
 * Copyright (c) 2024 Jeonghyun Kim <kimdictor@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/kernel_structs.h>
#include <osek_os.h>

#include "wrapper.h"

const TaskStateType RUNNING = 0;
const TaskStateType WAITING = 1;
const TaskStateType READY = 2;
const TaskStateType SUSPENDED = 3;
const TaskType INVALID_TASK = -1;
const ResourceType RES_SCHEDULER;
const AppModeType OSDEFAULTAPPMODE;

// for internal object count management
static int osek_task_count = 0, osek_alarm_count = 0, osek_resource_count = 0;

// internal object storage
osek_task_t osek_task[CONFIG_OSEK_OS_MAX_TASK_COUNT];
osek_alarm_t osek_alarm[CONFIG_OSEK_OS_MAX_ALARM_COUNT];
osek_resource_t osek_resource[CONFIG_OSEK_OS_MAX_RESOURCE_COUNT];

bool osek_task_is_valid(TaskType task_id)
{
	if (task_id < 0) {
		return false;
	}
	if (task_id >= CONFIG_OSEK_OS_MAX_TASK_COUNT) {
		return false;
	}
	if (task_id >= osek_task_count) {
		return false;
	}
	return true;
}

bool osek_resource_is_valid(ResourceType resource_id)
{
	if (resource_id < 0) {
		return false;
	}
	if (resource_id >= CONFIG_OSEK_OS_MAX_RESOURCE_COUNT) {
		return false;
	}
	if (resource_id >= osek_resource_count) {
		return false;
	}
	return true;
}

bool osek_alarm_is_valid(AlarmType alarm_id)
{
	if (alarm_id < 0) {
		return false;
	}
	if (alarm_id >= CONFIG_OSEK_OS_MAX_ALARM_COUNT) {
		return false;
	}
	if (alarm_id >= osek_alarm_count) {
		return false;
	}
	return true;
}

bool osek_task_occupied_resource(TaskType task_id)
{
	for (uint8_t i = 0; i < osek_resource_count; i++) {
		if (osek_resource[i].occupier == osek_task[task_id].id) {
			return true;
		}
	}
	return false;
}

bool osek_task_is_activated(TaskType task_id)
{
	uint8_t state = osek_task[task_id].id->base.thread_state;
	static const uint8_t inactivate = _THREAD_ABORTING | _THREAD_DEAD | _THREAD_SUSPENDED |
					  _THREAD_SUSPENDING | _THREAD_PRESTART;
	return (state & inactivate) > 0 ? false : true;
}

TaskType osek_task_from_zephyr_thread(k_tid_t thread_id)
{
	for (uint8_t i = 0; i < osek_task_count; i++) {
		if (osek_task[i].id == thread_id) {
			return true;
		}
	}
	return INVALID_TASK;
}

void osek_alarm_timer_expiry_handler(struct k_timer *timer)
{
	osek_alarm_t *alarm_ref = (osek_alarm_t *)k_timer_user_data_get(timer);
	switch (alarm_ref->action_type) {
	case ALARM_ACTION_ACTIVATE_TASK:
		ActivateTask(alarm_ref->task);
		break;
	case ALARM_ACTION_CALLBACK:
		alarm_ref->callback();
		break;
	case ALARM_ACTION_EVENT:
		break;
	}
}

TaskType osek_task_register(k_tid_t thread_id, bool is_extended)
{
	osek_task[osek_task_count].id = thread_id;
	osek_task[osek_task_count].is_extended = is_extended;
	return osek_task_count++;
}

AlarmType osek_alarm_register(struct k_timer *timer, AlarmBaseRefType base, bool is_autostart,
			      bool is_single, TickType period)
{
	osek_alarm[osek_alarm_count].timer = timer;
	osek_alarm[osek_alarm_count].base = base;
	osek_alarm[osek_alarm_count].is_autostart = is_autostart;
	osek_alarm[osek_alarm_count].is_single = is_single;
	osek_alarm[osek_alarm_count].period = period;
	osek_alarm[osek_alarm_count].action_type = ALARM_ACTION_NOTASSIGNED;

	k_timer_init(timer, osek_alarm_timer_expiry_handler, NULL);
	k_timer_user_data_set(timer, (void *)&osek_alarm[osek_alarm_count]);
	if (is_autostart) k_timer_start(timer, K_TICKS(period), is_single ? K_NO_WAIT : K_TICKS(period)); 
	return osek_alarm_count++;
}

void osek_alarm_attach_callback(AlarmType alarm, osek_alarm_callback_t callback)
{
	osek_alarm[alarm].action_type = ALARM_ACTION_CALLBACK;
	osek_alarm[alarm].callback = callback;
}

void osek_alarm_attach_event(AlarmType alarm, void *event)
{
	osek_alarm[alarm].action_type = ALARM_ACTION_EVENT;
	osek_alarm[alarm].event = event;
}

void osek_alarm_attach_activation(AlarmType alarm, TaskType task)
{
	osek_alarm[alarm].action_type = ALARM_ACTION_ACTIVATE_TASK;
	osek_alarm[alarm].task = task;
}

ResourceType osek_resource_register(struct k_sem *lock, int ceiling_prio)
{
	osek_resource[osek_resource_count].lock = lock;
	osek_resource[osek_resource_count].ceiling_prio = ceiling_prio;
	return osek_resource_count++;
}