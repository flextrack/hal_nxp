# Copyright 2024 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

if(CONFIG_MCUX_COMPONENT_driver.igpio)
    mcux_component_version(2.0.6)

    mcux_add_source(SOURCES fsl_gpio.h fsl_gpio.c)

    mcux_add_include(INCLUDES .)

endif()
