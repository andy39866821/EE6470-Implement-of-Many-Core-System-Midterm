#*******************************************************************************
# Copyright 2015 Cadence Design Systems, Inc.
# All Rights Reserved.
#
#*******************************************************************************
#
# Stratus Project File
#
############################################################
# Project Parameters
############################################################
#
# Technology Libraries
#
set LIB_PATH "[get_install_path]/share/stratus/techlibs/GPDK045/gsclib045_svt_v4.4/gsclib045/timing"
set LIB_LEAF "slow_vdd1v2_basicCells.lib"
use_tech_lib    "$LIB_PATH/$LIB_LEAF"

#
# Global synthesis attributes.
#
set_attr clock_period           10.0
set_attr message_detail         3 
#set_attr default_input_delay    0.1
#set_attr sched_aggressive_1 on
#set_attr unroll_loops on
#set_attr flatten_arrays all 
#set_attr timing_aggression 0
#set_attr default_protocol true

#
# Simulation Options
#
set_attr cc_options             "-DCLOCK_PERIOD=10.0 -g"
#enable_waveform_logging -vcd
set_attr end_of_sim_command "make saySimPassed"

#
# Testbench or System Level Modules
#
define_system_module ../main.cpp
define_system_module ../Testbench.cpp
define_system_module ../System.cpp

#
# SC_MODULEs to be synthesized
#
define_hls_module Convolution_1 ../Convolution_1.cpp
define_hls_config Convolution_1 BASIC
define_hls_config Convolution_1 DPA       --dpopt_auto=op,expr

define_hls_module Convolution_2 ../Convolution_2.cpp
define_hls_config Convolution_2 BASIC
define_hls_config Convolution_2 DPA       --dpopt_auto=op,expr

define_hls_module FC_1 ../FC_1.cpp
define_hls_config FC_1 BASIC
define_hls_config FC_1 DPA       --dpopt_auto=op,expr

define_hls_module FC_2 ../FC_2.cpp
define_hls_config FC_2 BASIC
define_hls_config FC_2 DPA       --dpopt_auto=op,expr

define_hls_module FC_3 ../FC_3.cpp
define_hls_config FC_3 BASIC
define_hls_config FC_3 DPA       --dpopt_auto=op,expr


define_sim_config B 
foreach cfg { BASIC DPA } {
	define_sim_config V_${cfg} \
	"Convolution_1 RTL_V ${cfg}" \
	"Convolution_2 RTL_V ${cfg}" \
	"FC_1 RTL_V ${cfg}" \
	"FC_2 RTL_V ${cfg}" \
	"FC_3 RTL_V ${cfg}"
}
