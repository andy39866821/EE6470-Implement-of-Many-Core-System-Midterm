#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <systemc>
#include <string>
#include "Compile_def.h"
#include "Testbench.h"

#include "Convolution_1_wrap.h"
#include "Convolution_2_wrap.h"
#include "FC_1_wrap.h"
#include "FC_2_wrap.h"
#include "FC_3_wrap.h"

using namespace sc_core;


class System: public sc_module
{
public:
	SC_HAS_PROCESS( System );
	System( sc_module_name n);
	~System();
private:

  	Testbench tb;
	Convolution_1_wrapper conv1;
	Convolution_2_wrapper conv2;
	FC_1_wrapper fc1;
	FC_2_wrapper fc2;
	FC_3_wrapper fc3;
	
	sc_clock clk;
	sc_signal<bool> rst;
    

	cynw_p2p< sc_dt::sc_int<8> > conv1_input;
	cynw_p2p< sc_dt::sc_int<8> > conv1_weight;
	cynw_p2p< sc_dt::sc_int<24> > conv1_activation;

	cynw_p2p< sc_dt::sc_int<8> > conv2_input;
	cynw_p2p< sc_dt::sc_int<8> > conv2_weight;
	cynw_p2p< sc_dt::sc_int<24> > conv2_activation;
	
	cynw_p2p< sc_dt::sc_int<8> > fc1_input;
	cynw_p2p< sc_dt::sc_int<8> > fc1_weight;
	cynw_p2p< sc_dt::sc_int<24> > fc1_activation;
	
	cynw_p2p< sc_dt::sc_int<8> > fc2_input;
	cynw_p2p< sc_dt::sc_int<8> > fc2_weight;
	cynw_p2p< sc_dt::sc_int<24> > fc2_activation;
	
	cynw_p2p< sc_dt::sc_int<8> > fc3_input;
	cynw_p2p< sc_dt::sc_int<8> > fc3_weight;
	cynw_p2p< sc_dt::sc_int<8> > fc3_bias;
	cynw_p2p< sc_dt::sc_int<24> > fc3_activation;
};
#endif
