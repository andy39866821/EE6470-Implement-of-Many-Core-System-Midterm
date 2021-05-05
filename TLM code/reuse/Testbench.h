#ifndef _TESTBENCH_H_
#define _TESTBENCH_H_
#include <string>
#include <systemc>
#include <iostream>
#include "Compile_def.h"
#include "initiator.h"
#include "MemoryMap.h"
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

    int conv1_weight[6][3][5][5];
    int conv2_weight[16][6][5][5];
    int fc1_weight[120][400];
    int fc2_weight[84][120];
    int fc3_weight[10][84];
    int fc3_bias[10];
    int label[10];
    double input[3][32][32];

    int input_quantized[3][32][32];
    int conv1_activation[6][28][28];
    int conv1_activation_quantized[6][14][14];
    int conv2_activation[16][10][10];
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


    void do_DNN();
    void do_conv1();
    void do_conv2();
    void do_fc1();
    void do_fc2();
    void do_fc3();
    void write(sc_int<8>, int, int);
    sc_int<24> read(int,int);

public:
    Initiator initiator;

    SC_HAS_PROCESS(Testbench);
    Testbench(sc_module_name n);
    ~Testbench() = default;
    void read_weight();
    void read_input(int);
    void read_golden(int);

};
#endif