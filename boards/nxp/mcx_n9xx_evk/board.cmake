#
# Copyright 2024-2025 NXP
#
# SPDX-License-Identifier: Apache-2.0
#

if(CONFIG_SOC_MCXN947_CPU0 OR CONFIG_SECOND_CORE_MCUX)
  board_runner_args(jlink "--device=MCXN947_M33_0" "--reset-after-load")
  board_runner_args(linkserver  "--device=MCXN947:MCX-N9XX-EVK")
  if(CONFIG_SECOND_CORE_MCUX)
    board_runner_args(linkserver  "--core=all")
  else()
    board_runner_args(linkserver  "--core=cm33_core0")
  endif()
  board_runner_args(linkserver  "--override=/device/memory/1/flash-driver=MCXN9xx_S.cfx")
  board_runner_args(linkserver  "--override=/device/memory/1/location=0x10000000")
  # Linkserver v1.4.85 and earlier do not include the secure regions in the
  # MCXN947 memory map, so we add them here
  board_runner_args(linkserver "--override=/device/memory/-=\{\"location\":\"0x30000000\",\
                               \"size\":\"0x00060000\",\"type\":\"RAM\"\}")
  board_runner_args(linkserver "--override=/device/memory/-=\{\"location\":\"0x30060000\",\
                               \"size\":\"0x00008000\",\"type\":\"RAM\"\}")
  # Define region for peripherals
  board_runner_args(linkserver "--override=/device/memory/-=\{\"location\":\"0x50000000\",\
                               \"size\":\"0x00140000\",\"type\":\"RAM\"\}")
elseif(CONFIG_SOC_MCXN947_CPU1)
  board_runner_args(jlink "--device=MCXN947_M33_1" "--reset-after-load")
  board_runner_args(linkserver  "--device=MCXN947:MCX-N9XX-EVK")
  board_runner_args(linkserver  "--core=cm33_core1")
endif()

# Pyocd support added with the NXP.MCXN947_DFP.17.0.0.pack CMSIS Pack
board_runner_args(pyocd "--target=mcxn947")

include(${ZEPHYR_BASE}/boards/common/linkserver.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
