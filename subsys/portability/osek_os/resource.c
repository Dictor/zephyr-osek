/*
 * Copyright (c) 2024 Jeonghyun Kim <kimdictor@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <osek_os.h>
#include "wrapper.h"

// (ch 13.4.3.1)
// Not Standard:
//   1. Return E_OS_SYS when failed to convert zephyr thread to OSEK task
StatusType GetResource(ResourceType ResID)
{
	if (!osek_resource_is_valid(ResID)) {
		return E_OS_ID;
	}

	k_tid_t current_thread = k_current_get();
	TaskType current_task = osek_task_from_zephyr_thread(current_thread);
	if (current_task == INVALID_TASK) {
		return E_OS_SYS;
	}

	int current_prio = k_thread_priority_get(current_thread);
	// smaller prio is higher prio, below line means "is ResID's ceiling has lower priority than the current thread?"
	if (osek_resource[ResID].ceiling_prio > current_prio) {
		// the statically assigned priority of the calling task or interrupt routine
		// is higher than the calculated ceiling priority
		return E_OS_ACCESS;
	}

	if (k_sem_take(osek_resource[ResID].lock, K_NO_WAIT) == 0) {
		// task success
		osek_resource[ResID].occupier = current_thread;
		osek_resource[ResID].occupier_prio = current_prio;
		k_thread_priority_set(current_thread, osek_resource[ResID].ceiling_prio);
	} else {
		// take fail
		return E_OS_ACCESS;
	}

	return E_OK;
}

// (ch 13.4.3.2)
// Not Standard:
//   1. Return E_OS_SYS when failed to convert zephyr thread to OSEK task
StatusType ReleaseResource(ResourceType ResID)
{
	if (!osek_resource_is_valid(ResID)) {
		return E_OS_ID;
	}

	if (k_sem_count_get(osek_resource[ResID].lock) != 0) {
		return E_OS_NOFUNC;
	}

	k_tid_t current_thread = k_current_get();
	TaskRefType current_task = osek_task_from_zephyr_thread(current_thread);
	if (current_task == NULL) {
		return E_OS_SYS;
	}

	if (osek_resource[ResID].ceiling_prio > k_thread_priority_get(current_thread)) {
		// Attempt to release a resource which has a lower ceiling priority than the
		// statically assigned priority of the calling task or interrupt routine
		return E_OS_ACCESS;
	}

    k_sem_give(osek_resource[ResID].lock);
	k_thread_priority_set(current_thread, osek_resource[ResID].occupier_prio);
    return E_OK;
}