#ifndef _CONVOLUTION_2_H_
#define _CONVOLUTION_2_H_
#include <systemc>
#include <sys/time.h>
#include <cmath>
#include <iomanip>
#include "Compile_def.h"
#include "tlm"
#include "tlm_utils/simple_target_socket.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

class Convolution_2: public sc_module {
private:
    int C, R, S;
    void MAC();
    void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay);
    
public:
    tlm_utils::simple_target_socket<Convolution_2> t_skt;
	sc_fifo< sc_dt::sc_int<24> > i_source;
	sc_fifo< sc_dt::sc_int<24> > i_weight;
	sc_fifo< sc_dt::sc_int<24> > o_result;
    
    SC_HAS_PROCESS(Convolution_2);
    Convolution_2(sc_module_name n);
    ~Convolution_2();
   
};
#endif

