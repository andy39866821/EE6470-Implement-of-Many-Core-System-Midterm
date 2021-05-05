#include "System.h"
System::System( sc_module_name n): sc_module( n ), 
	tb("tb"), 
    conv1("conv1"), 
    conv2("conv2"), 
    fc1("fc1"), 
    fc2("fc2"), 
    fc3("fc3"), 
    clk("clk", CLOCK_PERIOD, SC_NS), 
    rst("rst")
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	tb.o_conv1_input(conv1_input);
    tb.o_conv1_weight(conv1_weight);
    tb.i_conv1_activation(conv1_activation);
	tb.o_conv2_input(conv2_input);
    tb.o_conv2_weight(conv2_weight);
    tb.i_conv2_activation(conv2_activation);
	tb.o_fc1_input(fc1_input);
    tb.o_fc1_weight(fc1_weight);
    tb.i_fc1_activation(fc1_activation);
	tb.o_fc2_input(fc2_input);
    tb.o_fc2_weight(fc2_weight);
    tb.i_fc2_activation(fc2_activation);
	tb.o_fc3_input(fc3_input);
    tb.o_fc3_weight(fc3_weight);
    tb.o_fc3_bias(fc3_bias);
    tb.i_fc3_activation(fc3_activation);

	conv1.i_clk(clk);
	conv1.i_rst(rst);
    conv1.i_source(conv1_input);
    conv1.i_weight(conv1_weight);
    conv1.o_result(conv1_activation);

	conv2.i_clk(clk);
	conv2.i_rst(rst);
    conv2.i_source(conv2_input);
    conv2.i_weight(conv2_weight);
    conv2.o_result(conv2_activation);

	fc1.i_clk(clk);
	fc1.i_rst(rst);
    fc1.i_source(fc1_input);
    fc1.i_weight(fc1_weight);
    fc1.o_result(fc1_activation);

	fc2.i_clk(clk);
	fc2.i_rst(rst);
    fc2.i_source(fc2_input);
    fc2.i_weight(fc2_weight);
    fc2.o_result(fc2_activation);

	fc3.i_clk(clk);
	fc3.i_rst(rst);
    fc3.i_source(fc3_input);
    fc3.i_weight(fc3_weight);
    fc3.i_bias(fc3_bias);
    fc3.o_result(fc3_activation);

    int index = 1;
    tb.read_weight();
    cout << "Loading weight complteted" << endl;
    tb.read_input(index);
    cout << "Loading input complteted" << endl;
    tb.read_golden(index);
    cout << "Loading golden complteted" << endl;
    
    
}

System::~System() {
    
    
}

    
    