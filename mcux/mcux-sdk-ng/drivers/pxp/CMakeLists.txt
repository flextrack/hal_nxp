# Copyright 2024 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

if(CONFIG_MCUX_COMPONENT_driver.pxp)
    mcux_component_version(2.6.1)

    mcux_add_source(SOURCES fsl_pxp.c fsl_pxp.h)

    mcux_add_include(INCLUDES .)

endif()
