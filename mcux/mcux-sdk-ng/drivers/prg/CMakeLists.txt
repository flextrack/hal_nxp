# Copyright 2024 NXP
#
# SPDX-License-Identifier: BSD-3-Clause

if(CONFIG_MCUX_COMPONENT_driver.prg)
    mcux_component_version(2.0.1)

    mcux_add_source(SOURCES fsl_prg.c fsl_prg.h)

    mcux_add_include(INCLUDES .)

endif()
