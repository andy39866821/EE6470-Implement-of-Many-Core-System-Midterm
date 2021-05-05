#include <systemc>
#include <sys/time.h> 
#include "Compile_def.h"

#include <cynw_p2p.h>

using namespace std;
using namespace sc_core;
using namespace sc_dt;

class FC_1: public sc_module {
private:
    int H, M;
    void MAC();
public:

	sc_in_clk i_clk;
	sc_in < bool > i_rst;
    
	cynw_p2p< sc_dt::sc_int<8> >::in i_source;
	cynw_p2p< sc_dt::sc_int<8> >::in i_weight;
	cynw_p2p< sc_dt::sc_int<24> >::out o_result;
    
    SC_HAS_PROCESS(FC_1);
    FC_1(sc_module_name n);
    ~FC_1();
   
};


