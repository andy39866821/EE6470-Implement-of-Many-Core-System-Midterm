
#include "stratus_hls.h"
#include "FC_1.h"


FC_1::FC_1(sc_module_name n):
    sc_module(n)
{
    SC_THREAD( MAC );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);

	i_source.clk_rst(i_clk, i_rst);
    i_weight.clk_rst(i_clk, i_rst);
    o_result.clk_rst(i_clk, i_rst);

    H = FC1_H;


}

FC_1::~FC_1() {}

void FC_1::MAC(){
    
    sc_int<8> source, weight;
    sc_int<24> acc;
    sc_int<24> zero = 0;
    
	{
		HLS_DEFINE_PROTOCOL("main_reset");
		i_source.reset();
		i_weight.reset();
        o_result.reset();
		wait();
	}

    while(true) {
        acc = 0;
        
        for(int i = 0 ; i < H ; i++){		
            {
                HLS_DEFINE_PROTOCOL("input");
                source = i_source.get();
                weight = i_weight.get();
                wait();
		    }

            acc += source * weight;
        }
        acc = (acc < zero ? zero : acc);
		{
			HLS_DEFINE_PROTOCOL("output");
            o_result.put(acc);
			wait();
		}
        
        
    }
    
}
