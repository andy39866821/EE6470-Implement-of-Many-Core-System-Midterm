#include "FC_2.h"


FC_2::FC_2(sc_module_name n):
    sc_module(n)
{
    SC_THREAD(Fully_Connect);

    H = FC2_H;
    M = FC2_M;

    t_skt.register_b_transport(this, &FC_2::blocking_transport);

}

FC_2::~FC_2() {}

void FC_2::Fully_Connect(){
    
    sc_int<8> source, weight;
    sc_int<24> acc[84];
    sc_int<24> zero = 0;

    while(true) {
        
        for(int j = 0 ; j < M ; j++){
            acc[j] = 0;
        }
        for(int i = 0 ; i < H ; i++){	
            source = i_source.read();
            //cout << "FC2 get source: " << i << endl;
            for(int j = 0 ; j < M ; j++){
                weight = i_weight.read();
                acc[j] += source * weight;
                
            }
        }
        
        
        for(int j = 0 ; j < M ; j++){
            acc[j] = (acc[j] > zero ? acc[j] : zero);
            o_result.write(acc[j]);
            //cout << "FC2 write result: " << j << endl;
		}
    }
    
}


void FC_2::blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay) {
    wait(delay);
    sc_dt::uint64 addr = payload.get_address();
    //addr = addr - base_offset;
    unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    unsigned char *data_ptr = payload.get_data_ptr();
    sc_int<24> result;
    sc_int<8> source, weight;
    switch (payload.get_command()) {
        case tlm::TLM_READ_COMMAND:
            switch (addr) {
                case FC2_RESULT_ADDR:
                    result = o_result.read();
                    //cout << "   FC2 socket get result : " << result << endl;
                    break;
                default:
                    std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                            << std::setfill('0') << std::setw(8) << std::hex << addr
                            << std::dec << " is not valid" << std::endl;
                    break;
            }
            word buffer;
            buffer.Integer = result;
            data_ptr[0] = buffer.Character[0];
            data_ptr[1] = buffer.Character[1];
            data_ptr[2] = buffer.Character[2];
            data_ptr[3] = buffer.Character[3];
            
            break;
        case tlm::TLM_WRITE_COMMAND:
            switch (addr) {
                case FC2_SOURCE_ADDR:
                    source = data_ptr[0] & mask_ptr[0];
                    i_source.write(source);
                    //cout << "   FC2 socket write source : " << source << endl;
                    break;
                case FC2_WEIGHT_ADDR:
                    weight = data_ptr[0] & mask_ptr[0];
                    i_weight.write(weight);
                    //cout << "   FC2 socket get weight : " << weight << endl;
                    break;
                default:
                    std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                            << std::setfill('0') << std::setw(8) << std::hex << addr
                            << std::dec << " is not valid" << std::endl;
                    break;
            }
            break;
        case tlm::TLM_IGNORE_COMMAND:
            payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
            return;
        default:
            payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
            return;
    }
    payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
}

