#include "osek_os.h"

// definition of osek task associated type and macros
typedef struct {
	k_tid_t id;
	bool is_extended;
} osek_task_t;

typedef enum {
	ALARM_ACTION_NOTASSIGNED = 0,
	ALARM_ACTION_CALLBACK = 1,
	ALARM_ACTION_ACTIVATE_TASK = 2,
	ALARM_ACTION_EVENT = 3
} osek_alarm_action_t;

// definition of osek alarm associated type and macros
typedef struct {
	struct k_timer *timer;
	osek_alarm_action_t action_type;
	osek_alarm_callback_t callback;
	void *event;
	TaskType task;
	AlarmBaseRefType base;
	bool is_autostart;
	bool is_single;
	TickType period;
} osek_alarm_t;

// definition of osek resource associated type and macros
typedef struct {
	struct k_sem *lock;
	k_tid_t occupier;
	int ceiling_prio;
	int occupier_prio;
} osek_resource_t;