/*
 * Copyright (c) 2024 Jeonghyun Kim <kimdictor@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <osek_os.h>

// (ch 13.5.3.1)
StatusType SetEvent(TaskType TaskID, EventMaskType Mask)
{
}

// (ch 13.5.3.2)
StatusType ClearEvent(EventMaskType Mask)
{
}

// (ch 13.5.3.3)
StatusType GetEvent(TaskType TaskID, EventMaskRefType Event)
{
}

// (ch 13.5.3.4)
StatusType WaitEvent(EventMaskType Mask)
{
}
