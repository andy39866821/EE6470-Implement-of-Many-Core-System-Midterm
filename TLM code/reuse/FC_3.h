#include <systemc>
#include <sys/time.h> 
#include "Compile_def.h"
#include <cmath>
#include <iomanip>
#include "tlm"
#include "tlm_utils/simple_target_socket.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

class FC_3: public sc_module {
private:
    int H, M;
    void Fully_Connect();
    void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay);
public:

    tlm_utils::simple_target_socket<FC_3> t_skt;
    
	sc_fifo< sc_dt::sc_int<8> > i_source;
	sc_fifo< sc_dt::sc_int<8> > i_weight;
	sc_fifo< sc_dt::sc_int<8> > i_bias;
	sc_fifo< sc_dt::sc_int<24> > o_result;
    
    SC_HAS_PROCESS(FC_3);
    FC_3(sc_module_name n);
    ~FC_3();
   
};


