
#include "stratus_hls.h"
#include "Convolution_2.h"


Convolution_2::Convolution_2(sc_module_name n):
    sc_module(n)
{
    SC_THREAD( MAC );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);

	i_source.clk_rst(i_clk, i_rst);
    i_weight.clk_rst(i_clk, i_rst);
    o_result.clk_rst(i_clk, i_rst);

    C = conv2_C;
    R = conv2_R;
    S = conv2_S;


}

Convolution_2::~Convolution_2() {}

void Convolution_2::MAC(){
    
    sc_int<8> source, weight;
    sc_int<24> acc, max_vaule;
    
	{
		HLS_DEFINE_PROTOCOL("main_reset");
		i_source.reset();
		i_weight.reset();
        o_result.reset();
		wait();
	}

    while(true) {
        max_vaule = 0;
        for(int i = 0 ; i < 2 ; i++){
            for(int j = 0 ; j < 2 ; j++){
                acc = 0;
                for(int c = 0 ; c < C ; c++){
                    for(int r = 0 ; r < R ; r++){
                        for(int s = 0 ; s < S ; s++){
                            HLS_PIPELINE_LOOP(SOFT_STALL, 1, "main_loop");
                            {
                                HLS_DEFINE_PROTOCOL("input");
                                source = i_source.get();
                                weight = i_weight.get();
                                wait();
                            }
                            //cout << "Get: " << c << "," << r << "," << s << " : " << source << " " << weight << endl;
                            
                            acc +=  source * weight;
                            

                        }
                    }
                }
                max_vaule = (max_vaule > acc ? max_vaule : acc);
            }
        }
        

		{
			HLS_DEFINE_PROTOCOL("output");
            o_result.put(max_vaule);
			wait();
		}
        //cout << "Send data: " << acc << endl;
        
    }
    
}
