#include <iostream>
#include <string>
#include <systemc>
#include <sys/time.h>
#include "Convolution_1.h"
#include "Convolution_2.h"
#include "FC_1.h"
#include "FC_2.h"
#include "FC_3.h"
#include "Testbench.h"
#include "SimpleBus.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

int sc_main(int argc, char *argv[]) {
    Testbench tb("tb");
    Convolution_1 conv1("conv1");
    Convolution_2 conv2("conv2");
    FC_1 fc1("fc1");
    FC_2 fc2("fc2");
    FC_3 fc3("fc3");
    SimpleBus<1, 5> bus("bus");

    tb.initiator.i_skt(bus.t_skt[0]);
    bus.set_clock_period(sc_time(CLOCK_PERIOD, SC_NS));
    bus.setDecode(0, CONV1_MM_BASE, CONV1_MM_BASE + CONV1_MM_SIZE - 1);
    bus.setDecode(1, CONV2_MM_BASE, CONV2_MM_BASE + CONV2_MM_SIZE - 1);
    bus.setDecode(2, FC1_MM_BASE, FC1_MM_BASE + FC1_MM_SIZE - 1);
    bus.setDecode(3, FC2_MM_BASE, FC2_MM_BASE + FC2_MM_SIZE - 1);
    bus.setDecode(4, FC3_MM_BASE, FC3_MM_BASE + FC3_MM_SIZE - 1);
    bus.i_skt[0](conv1.t_skt); 
    bus.i_skt[1](conv2.t_skt);  
    bus.i_skt[2](fc1.t_skt);  
    bus.i_skt[3](fc2.t_skt);  
    bus.i_skt[4](fc3.t_skt);  

    int index = 1;
    tb.read_weight();
    cout << "Loading weight complteted" << endl;
    tb.read_input(index);
    cout << "Loading input complteted" << endl;
    tb.read_golden(index);
    cout << "Loading golden complteted" << endl;
    
    sc_start();
    
    cout << "Total Simulation Times: " << sc_time_stamp() << endl;
    return 0;
}