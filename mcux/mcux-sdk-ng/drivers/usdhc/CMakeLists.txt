# Copyright 2024 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

if(CONFIG_MCUX_COMPONENT_driver.usdhc)
    mcux_component_version(2.8.4)

    mcux_add_source(SOURCES fsl_usdhc.c fsl_usdhc.h)

    mcux_add_include(INCLUDES .)

endif()
