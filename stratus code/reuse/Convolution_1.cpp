
#include "stratus_hls.h"
#include "Convolution_1.h"


Convolution_1::Convolution_1(sc_module_name n):
    sc_module(n)
{
    SC_THREAD( MAC );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);

	i_source.clk_rst(i_clk, i_rst);
    i_weight.clk_rst(i_clk, i_rst);
    o_result.clk_rst(i_clk, i_rst);

    C = conv1_C;
    R = conv1_R;
    S = conv1_S;


}

Convolution_1::~Convolution_1() {}

void Convolution_1::MAC(){
    
    
    sc_int<8> input_buffer[3][5][32];
    sc_int<8> weight_buffer[3][5][5];
    sc_int<24> activation_buffer[2][28];
    sc_int<24> acc, max_value;
    bool even_row;
    
	{
		HLS_DEFINE_PROTOCOL("main_reset");
		i_source.reset();
		i_weight.reset();
        o_result.reset();
		wait();
	}

    while(true) { // m loop
        
        for (int c = 0; c != C; c++) {// read 3D weight
            for(int r = 0 ; r < R ; r++){
                for(int s = 0 ; s < S ; s++){
                    HLS_DEFINE_PROTOCOL("weight_input");
                    weight_buffer[c][r][s] = i_weight.get();
                }
            }
        }

        for(int i = 0 ; i < R-1 ; i++) {// read inital 4-row input
            for(int c = 0;  c != C ; c++) {
                for(int q = 0;  q != 32 ; q ++) {
                    HLS_DEFINE_PROTOCOL("initial_input");
                    input_buffer[c][i][q] = i_source.get(); 
                }
            }
        }

        for(int p = 4 ; p<32 ; p++) {

            for(int c = 0;  c != C ; c++) {
                for(int q = 0;  q != 32 ; q ++) {
                    HLS_DEFINE_PROTOCOL("input");
                    input_buffer[c][4][q] = i_source.get(); 
                }
            }
            for(int index = 0 ; index < 28 ; index++) {
                acc = 0;
                for(int c = 0 ; c != C ; c++){ 
                    for(int r = 0 ; r != R ; r++){
                        for(int s = 0 ; s != S ; s++){
                            HLS_CONSTRAIN_LATENCY(0, 3, "MAC");
                            acc += weight_buffer[c][r][s] *  input_buffer[c][r][index + s];
                        }
                    }
                }
                activation_buffer[even_row][index] = acc;
            }
            for(int c = 0 ; c != C ; c++){
                for(int i = 0 ; i < R-1 ; i++) {// shift 4-upper-row input
                    for(int index = 0 ; index < 32 ; index++){
                        
                        HLS_CONSTRAIN_LATENCY(0, 3, "SHIFT"); 
                        input_buffer[c][i][index] = input_buffer[c][i+1][index];
                    }
                }
            }

            if(even_row == true){
                for(int index = 0 ; index < 14 ; index++){
                    
                    max_value = 0;
                    for(int i = 0; i < 2 ; i++){
                        for(int j = 0 ; j < 2 ; j++){
                            HLS_CONSTRAIN_LATENCY(0, 1, "MP"); 
                            max_value = (max_value > activation_buffer[i][index*2+j] ? max_value : activation_buffer[i][index*2+j]);
                        }
                    }
                    {
                        HLS_DEFINE_PROTOCOL("output");
                        o_result.put(max_value);
                        wait();
                    }
                }
            }
            even_row = !even_row;
        }
    }
    
}
