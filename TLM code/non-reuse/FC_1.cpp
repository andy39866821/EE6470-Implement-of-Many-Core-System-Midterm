#include "FC_1.h"


FC_1::FC_1(sc_module_name n):
    sc_module(n)
{
    SC_THREAD(Fully_Connect);

    H = FC1_H;
    M = FC1_M;

    t_skt.register_b_transport(this, &FC_1::blocking_transport);

}

FC_1::~FC_1() {}

void FC_1::Fully_Connect(){
    
    sc_int<24> source, weight, acc;
    sc_int<24> zero = 0;

    while(true) {
        acc = 0;
        
        for(int i = 0 ; i < H ; i++){		
            source = i_source.read();
            weight = i_weight.read();

            acc += source * weight;
        }
        acc = (acc < zero ? zero : acc);
        
        o_result.write(acc);
        
        
    }
    
}


void FC_1::blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay) {
    wait(delay);
    sc_dt::uint64 addr = payload.get_address();
    //addr = addr - base_offset;
    unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    unsigned char *data_ptr = payload.get_data_ptr();
    sc_int<24> result, source, weight;
    switch (payload.get_command()) {
        case tlm::TLM_READ_COMMAND:
            switch (addr) {
                case FC1_RESULT_ADDR:
                    result = o_result.read();
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
                case FC1_SOURCE_ADDR:
                    source.range(7,0) = data_ptr[0] & mask_ptr[0];
                    source.range(15,8) = data_ptr[1] & mask_ptr[1];
                    source.range(23,16) = data_ptr[2] & mask_ptr[2];
                    i_source.write(source);
                    break;
                case FC1_WEIGHT_ADDR:
                    weight.range(7,0) = data_ptr[0] & mask_ptr[0];
                    weight.range(15,8) = data_ptr[1] & mask_ptr[1];
                    weight.range(23,16) = data_ptr[2] & mask_ptr[2];
                    i_weight.write(weight);
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
