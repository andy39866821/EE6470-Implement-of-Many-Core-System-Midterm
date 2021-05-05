
#include "stratus_hls.h"
#include "FC_2.h"


FC_2::FC_2(sc_module_name n):
    sc_module(n)
{
    SC_THREAD( MAC );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);

	i_source.clk_rst(i_clk, i_rst);
    i_weight.clk_rst(i_clk, i_rst);
    o_result.clk_rst(i_clk, i_rst);

    H = FC2_H;
    M = FC2_M;
}

FC_2::~FC_2() {}

void FC_2::MAC(){
    
    sc_int<8> source, weight;
    sc_int<24> acc[84];
    sc_int<24> zero = 0;
    
	{
		HLS_DEFINE_PROTOCOL("main_reset");
		i_source.reset();
		i_weight.reset();
        o_result.reset();
		wait();
	}

    while(true) {
        
        for(int j = 0 ; j < M ; j++){
            acc[j] = 0;
        }
        for(int i = 0 ; i < H ; i++){		
            {
                HLS_DEFINE_PROTOCOL("source_input");
                source = i_source.get();
                wait();
		    }
            for(int j = 0 ; j < M ; j++){
                HLS_CONSTRAIN_LATENCY(0, 3, "MAC");
            
                {
                    HLS_DEFINE_PROTOCOL("weight_input");
                    weight = i_weight.get();
                    wait();
                }
                acc[j] += source * weight;
            }

        }
        
        
        for(int j = 0 ; j < M ; j++){
            HLS_CONSTRAIN_LATENCY(0, 3, "RELU");
            
            acc[j] = (acc[j] > zero ? acc[j] : zero);
            {
                HLS_DEFINE_PROTOCOL("output");
                o_result.put(acc[j]);
                wait();
            }
		}
    }
    
}
