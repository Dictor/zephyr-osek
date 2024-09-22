/*
 * Copyright (c) 2024 Jeonghyun Kim <kimdictor@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <osek_os.h>

#include "wrapper.h"

// (ch 13.6.3.1)
StatusType GetAlarmBase(AlarmType AlarmID, AlarmBaseRefType Info)
{
    Info->maxallowedvalue = osek_alarm[AlarmID].base->maxallowedvalue;
    Info->ticksperbase = osek_alarm[AlarmID].base->ticksperbase;
    Info->mincycle = osek_alarm[AlarmID].base->mincycle;
    return E_OK;
}

// (ch 13.6.3.2)
StatusType GetAlarm(AlarmType AlarmID, TickRefType Tick)
{
    if (!osek_alarm_is_valid(AlarmID)) {
        return E_OS_ID;
    }

    k_ticks_t remaining = k_timer_remaining_ticks(osek_alarm[AlarmID].timer);
    if (remaining == 0) {
        return E_OS_NOFUNC;
    }

    *Tick = remaining;
    return E_OK;
}

// (ch 13.6.3.3)
StatusType SetRelAlarm(AlarmType AlarmID, TickType increment, TickType cycle)
{
    if (!osek_alarm_is_valid(AlarmID)) {
        return E_OS_ID;
    }

    TickType remaining;
    if (GetAlarm(AlarmID, &remaining) == E_OS_NOFUNC) return E_OS_STATE;

    AlarmBaseType base;
    GetAlarmBase(AlarmID, &base);
    if (increment < 0 || increment > base.maxallowedvalue) return E_OS_VALUE;
    if (cycle < base.mincycle || cycle > base.maxallowedvalue) return E_OS_VALUE;

    k_timer_start(osek_alarm[AlarmID].timer, K_TICKS(increment), K_TICKS(cycle));
    return E_OK; 
}

// (ch 13.6.3.4)
StatusType SetAbsAlarm(AlarmType AlarmID, TickType start, TickType cycle)
{
    if (!osek_alarm_is_valid(AlarmID)) {
        return E_OS_ID;
    }

    TickType remaining;
    if (GetAlarm(AlarmID, &remaining) == E_OS_NOFUNC) return E_OS_STATE;

    AlarmBaseType base;
    GetAlarmBase(AlarmID, &base);
    if (start < 0 || start > base.maxallowedvalue) return E_OS_VALUE;
    if (cycle < base.mincycle || cycle > base.maxallowedvalue) return E_OS_VALUE;

    k_timer_start(osek_alarm[AlarmID].timer, K_TICKS(start - k_uptime_ticks()), K_TICKS(cycle));
    return E_OK;
}

// (ch 13.6.3.5)
StatusType CancelAlarm(AlarmType AlarmID)
{
    if (!osek_alarm_is_valid(AlarmID)) {
        return E_OS_ID;
    }

    TickType remaining;
    if (GetAlarm(AlarmID, &remaining) == E_OS_NOFUNC) return E_OS_STATE;

    k_timer_stop(osek_alarm[AlarmID].timer);
    return E_OK;
}