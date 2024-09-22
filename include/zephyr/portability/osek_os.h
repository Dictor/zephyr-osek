/*
 * Copyright (c) 2024 Jeonghyun Kim <kimdictor@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_OSEK_OS_H_
#define ZEPHYR_INCLUDE_OSEK_OS_H_

#include <zephyr/kernel.h>
#include <zephyr/init.h>

#include <stdint.h>
#include <stdbool.h>

// TaskType identifies a task. (ch 13.2.1)
typedef int16_t TaskType;

// TaskRefType points to a variable of TaskType. (ch 13.2.1)
typedef TaskType *TaskRefType;

// TaskStateType identifies the state of a task. (ch 13.2.1)
typedef unsigned char TaskStateType;

// TaskStateRefType points to a variable of the data type TaskStateType. (ch 13.2.1)
typedef TaskStateType *TaskStateRefType;

// ResourceType is data type for a resource.(ch 13.4.1)
typedef int16_t ResourceType;

// EventMaskType is data type of the event mask. (ch 13.5.1)
typedef struct {
} EventMaskType;

// EventMaskRefType is reference to an event mask. (ch 13.5.1)
typedef EventMaskType *EventMaskRefType;

// TickType represents count values in ticks. (ch 13.6.1)
typedef k_ticks_t TickType;

// TickRefType points to the data type TickType. (ch 13.6.1)
typedef TickType *TickRefType;

// AlarmBaseType represents a structure for storage of counter characteristics. (ch 13.6.1)
typedef struct {
	TickType maxallowedvalue;
	TickType ticksperbase;
	TickType mincycle;
} AlarmBaseType;

// AlarmBaseRefType points to the data type AlarmBaseType. (ch 13.6.1)
typedef AlarmBaseType *AlarmBaseRefType;

// AlarmType represents an alarm object. (ch 13.6.1)
typedef int16_t AlarmType;

// AppModeType represents the application mode. (ch 13.7.1)
typedef unsigned char AppModeType;

// OSServiceIdType represents the identification of system services. (ch 13.8.1)
typedef struct {
} OSServiceIdType;

// StatusType means all status information the API services offer (ch 13.1)
typedef enum {
	E_OK = 0,
	E_OS_ACCESS = 1,
	E_OS_CALLEVEL = 2,
	E_OS_ID = 3,
	E_OS_LIMIT = 4,
	E_OS_NOFUNC = 5,
	E_OS_RESOURCE = 6,
	E_OS_STATE = 7,
	E_OS_VALUE = 8,
	E_OS_SYS = 9
} StatusType;

// (ch 13.2.4)
extern const TaskStateType RUNNING;
extern const TaskStateType WAITING;
extern const TaskStateType READY;
extern const TaskStateType SUSPENDED;
extern const TaskType INVALID_TASK;

// (ch 13.4.4)
extern const ResourceType RES_SCHEDULER;

// (ch 13.6.4)
#define OSEK_OSMAXALLOWEDVALUE(name) OSMAXALLOWEDVALUE_##name
#define OSEK_OSTICKSPERBASE(name)    OSTICKSPERBASE_##name
#define OSEK_OSMINCYCLE(name)        OSMINCYCLE_##name

// (ch 13.7.3)
extern const AppModeType OSDEFAULTAPPMODE;

// (ch.13.2.2.1)
#define DeclareTask(TaskIdentifier)                                                                \
	extern TaskType osek_##TaskIdentifier;                                                     \
	extern const k_tid_t thread_##TaskIdentifier

// (ch 13.2.3.1)
StatusType ActivateTask(TaskType TaskID);

// (ch 13.2.3.2)
StatusType TerminateTask(void);

// (ch 13.2.3.3)
StatusType ChainTask(TaskType TaskID);

// (ch 13.2.3.4)
StatusType Schedule(void);

// (ch 13.2.3.5)
StatusType GetTaskID(TaskRefType TaskID);

// (ch 13.2.3.6)
StatusType GetTaskState(TaskType TaskID, TaskStateRefType State);

// (ch 13.3.2.1)
void EnableAllInterrupts(void);

// (ch 13.3.2.2)
void DisableAllInterrupts(void);

// (ch 13.3.2.3)
void ResumeAllInterrupts(void);

// (ch 13.3.2.4)
void SuspendAllInterrupts(void);

// (ch 13.3.2.5)
void ResumeOSInterrupts(void);

// (ch 13.3.2.6)
void SuspendOSInterrupts(void);

// (ch 13.4.2.1)
#define DeclareResource(ResourceIdentifier) extern ResourceType osek_##ResourceIdentifier

// (ch 13.4.3.1)
StatusType GetResource(ResourceType ResID);

// (ch 13.4.3.2)
StatusType ReleaseResource(ResourceType ResID);

// (ch 13.5.2.1)
#define DeclareEvent(EventIdentifier) (EventIdentifier)

// (ch 13.5.3.1)
StatusType SetEvent(TaskType TaskID, EventMaskType Mask);

// (ch 13.5.3.2)
StatusType ClearEvent(EventMaskType Mask);

// (ch 13.5.3.3)
StatusType GetEvent(TaskType TaskID, EventMaskRefType Event);

// (ch 13.5.3.4)
StatusType WaitEvent(EventMaskType Mask);

// (ch 9.3)
#define ALARMCALLBACK(AlarmCallbackRoutineName) void AlarmCallbackRoutineName(void)

// (ch 13.6.2.1)
#define DeclareAlarm(AlarmIdentifier) extern AlarmType osek_##AlarmIdentifier

// (ch 13.6.3.1)
StatusType GetAlarmBase(AlarmType AlarmID, AlarmBaseRefType Info);

// (ch 13.6.3.2)
StatusType GetAlarm(AlarmType AlarmID, TickRefType Tick);

// (ch 13.6.3.3)
StatusType SetRelAlarm(AlarmType AlarmID, TickType increment, TickType cycle);

// (ch 13.6.3.4)
StatusType SetAbsAlarm(AlarmType AlarmID, TickType start, TickType cycle);

// (ch 13.6.3.5)
StatusType CancelAlarm(AlarmType AlarmID);

// (ch 13.7.2.1)
AppModeType GetActiveApplicationMode(void);

// (ch 13.7.2.2)
void StartOS(AppModeType Mode);

// (ch 13.7.2.3)
void ShutdownOS(StatusType Error);

// (ch 13.8.2.1)
extern void ErrorHook(StatusType Error);

// (ch 13.8.2.2)
extern void PreTaskHook(void);

// (ch 13.8.2.3)
extern void PostTaskHook(void);

// (ch 13.8.2.4)
extern void StartupHook(void);

// (ch 13.8.2.5)
extern void ShutdownHook(StatusType Error);

// internal object registering methods
typedef void (*osek_alarm_callback_t)(void);
extern void osek_alarm_timer_expiry_handler(struct k_timer *);

extern TaskType osek_task_register(k_tid_t thread_id, bool is_extended);
extern AlarmType osek_alarm_register(struct k_timer *timer, AlarmBaseRefType base,
				     bool is_autostart, bool is_single, TickType period);
extern void osek_alarm_attach_callback(AlarmType alarm, osek_alarm_callback_t callback);
extern void osek_alarm_attach_event(AlarmType alarm, void *event);
extern void osek_alarm_attach_activation(AlarmType alarm, TaskType task);
extern ResourceType osek_resource_register(struct k_sem *lock, int ceiling_prio);

// Macros for OSEK object definition
#define OSEK_TASK_DEFINE(name, stack_size, entry, prio, options, delay)                            \
	K_THREAD_DEFINE(thread_##name, stack_size, entry, NULL, NULL, NULL, prio, options, delay); \
	TaskType osek_##name

#define OSEK_TASK_REGISTER(name, is_extended)                                                      \
	osek_##name = osek_task_register(thread_##name, is_extended)

#define OSEK_ALARM_DEFINE(name)                                                                    \
	K_TIMER_DEFINE(timer_##name, osek_alarm_timer_expiry_handler, NULL);                       \
	AlarmType osek_##name

#define OSEK_ALARM_REGISTER(name, base, is_autostart, is_single, period)                           \
	osek_##name =                                                                              \
		osek_alarm_register(&timer_##name, &osek_##base, is_autostart, is_single, period)

#define OSEK_ALARM_ATTACH_CALLBACK(name, callback) osek_alarm_attach_callback(osek_##name, callback)

#define OSEK_ALARM_ATTACH_EVENT(name, event) osek_alarm_attach_event(osek_##name, event)

#define OSEK_ALARM_ATTACH_ACTIVATION(name, task) osek_alarm_attach_activation(osek_##name, osek_##task)

#define OSEK_RESOURCE_DEFINE(name)                                                                 \
	K_SEM_DEFINE(sem_##name, 1, 1);                                                            \
	ResourceType osek_##name

#define OSEK_RESOURCE_REGISTER(name, ceiling_prio)                                                 \
	osek_##name = osek_resource_register(&sem_##lock, ceiling_prio)

#define OSEK_ALARMBASE_DECLARE(name)                                                               \
	extern const TickType OSEK_OSMAXALLOWEDVALUE(name);                                        \
	extern const TickType OSEK_OSTICKSPERBASE(name);                                           \
	extern const TickType OSEK_OSMINCYCLE(name);

#define OSEK_ALARMBASE_DEFINE(name, maxval, tickper, mincyc)                                       \
	const TickType OSEK_OSMAXALLOWEDVALUE(name) = maxval;                                      \
	const TickType OSEK_OSTICKSPERBASE(name) = tickper;                                        \
	const TickType OSEK_OSMINCYCLE(name) = mincyc;                                             \
	AlarmBaseType osek_##name = {.maxallowedvalue = OSEK_OSMAXALLOWEDVALUE(name),              \
				     .ticksperbase = OSEK_OSTICKSPERBASE(name),                    \
				     .mincycle = OSEK_OSMINCYCLE(name)}

#define OSEK_INIT_START                                                                            \
	int osek_init()                                                                            \
	{

#define OSEK_INIT_END                                                                              \
	}                                                                                          \
	SYS_INIT(osek_init, APPLICATION, 0);

#endif