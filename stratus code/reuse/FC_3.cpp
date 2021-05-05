
#include "stratus_hls.h"
#include "FC_3.h"


FC_3::FC_3(sc_module_name n):
    sc_module(n)
{
    SC_THREAD( MAC );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);

	i_source.clk_rst(i_clk, i_rst);
    i_weight.clk_rst(i_clk, i_rst);
    i_bias.clk_rst(i_clk, i_rst);
    o_result.clk_rst(i_clk, i_rst);

    H = FC3_H;
    M = FC3_M;
}

FC_3::~FC_3() {}

void FC_3::MAC(){
    
    sc_int<8> source, weight, bias;
    sc_int<24> acc[120];
    sc_int<24> zero = 0;
    
	{
		HLS_DEFINE_PROTOCOL("main_reset");
		i_source.reset();
		i_weight.reset();
        i_bias.reset();
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
            HLS_CONSTRAIN_LATENCY(0, 5, "BIAS");
            {
                HLS_DEFINE_PROTOCOL("bias_input");
                bias = i_bias.get();
                wait();
            }
            acc[j] += bias;
            {
                HLS_DEFINE_PROTOCOL("output");
                o_result.put(acc[j]);
                wait();
            }
		}
    }
    
}
