# SPDX-License-Identifier: Apache-2.0


add_library(OSEK INTERFACE)

target_include_directories(OSEK INTERFACE ${PROJECT_SOURCE_DIR}/include/zephyr/portability)

zephyr_library()
zephyr_library_sources_ifdef(
  CONFIG_OSEK_OS
  internal.c
  alarm.c
  event.c
  interrupt.c
  os.c
  resource.c
  task.c
)

zephyr_library_include_directories(
  ${ZEPHYR_BASE}/kernel/include
)

zephyr_library_link_libraries(OSEK)