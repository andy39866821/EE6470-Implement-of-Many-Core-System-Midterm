#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <string>
#include <systemc>
#include <iostream>
#include <cmath>
#include "Compile_def.h"
#include <cynw_p2p.h>

using namespace std;
using namespace sc_core;
using namespace sc_dt;

class Testbench: public sc_module {
private:
    sc_time conv1_start_time;
    sc_time conv2_start_time;
    sc_time fc1_start_time;
    sc_time fc2_start_time;
    sc_time fc3_start_time;
    sc_time conv1_each_row_start_time;
    sc_time conv2_each_row_start_time;
    sc_time fc1_each_row_start_time;
    sc_time fc2_each_row_start_time;
    sc_time fc3_each_row_start_time;

    int conv1_weight[6][3][5][5];
    int conv2_weight[16][6][5][5];
    int fc1_weight[120][400];
    int fc2_weight[84][120];
    int fc3_weight[10][84];
    int fc3_bias[10];
    int label[10];
    double input[3][32][32];

    int input_quantized[3][32][32];
    int conv1_activation[6][14][14];
    int conv1_activation_quantized[6][14][14];
    int conv2_activation[16][5][5];
    int fc1_input[400];
    int fc1_activation[120];
    int fc1_activation_quantized[120];
    int fc2_activation[84];
    int fc2_activation_quantized[84];
    int fc3_activation[10];

    int conv1_activation_golden[6][14][14];
    int conv2_activation_golden[16][5][5];
    int fc1_activation_golden[120];
    int fc2_activation_golden[84];

    bool conv1_finish;
    bool conv2_finish;
    bool fc1_finish;
    bool fc2_finish;


    void conv1_feed_input();
    void conv1_fetch_result();
    void conv2_feed_input();
    void conv2_fetch_result();
    void fc1_feed_input();
    void fc1_fetch_result();
    void fc2_feed_input();
    void fc2_fetch_result();
    void fc3_feed_input();
    void fc3_fetch_result();
    

public:

	sc_in_clk i_clk;
	sc_out < bool >  o_rst;

    cynw_p2p< sc_dt::sc_int<8> >::base_out o_conv1_input;
	cynw_p2p< sc_dt::sc_int<8> >::base_out o_conv1_weight;
	cynw_p2p< sc_dt::sc_int<24> >::base_in i_conv1_activation;

    
    cynw_p2p< sc_dt::sc_int<8> >::base_out o_conv2_input;
	cynw_p2p< sc_dt::sc_int<8> >::base_out o_conv2_weight;
	cynw_p2p< sc_dt::sc_int<24> >::base_in i_conv2_activation;
    
    cynw_p2p< sc_dt::sc_int<8> >::base_out o_fc1_input;
	cynw_p2p< sc_dt::sc_int<8> >::base_out o_fc1_weight;
	cynw_p2p< sc_dt::sc_int<24> >::base_in i_fc1_activation;

    cynw_p2p< sc_dt::sc_int<8> >::base_out o_fc2_input;
	cynw_p2p< sc_dt::sc_int<8> >::base_out o_fc2_weight;
	cynw_p2p< sc_dt::sc_int<24> >::base_in i_fc2_activation;

    cynw_p2p< sc_dt::sc_int<8> >::base_out o_fc3_input;
	cynw_p2p< sc_dt::sc_int<8> >::base_out o_fc3_weight;
	cynw_p2p< sc_dt::sc_int<8> >::base_out o_fc3_bias;
	cynw_p2p< sc_dt::sc_int<24> >::base_in i_fc3_activation;

    SC_HAS_PROCESS(Testbench);
    Testbench(sc_module_name n);
    ~Testbench();
    void read_weight();
    void read_input(int);
    void read_golden(int);
};

#endif