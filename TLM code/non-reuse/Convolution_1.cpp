
#include "Convolution_1.h"


Convolution_1::Convolution_1(sc_module_name n):
    sc_module(n), 
    t_skt("t_skt")
{
    SC_THREAD( MAC );

    C = conv1_C;
    R = conv1_R;
    S = conv1_S;

    t_skt.register_b_transport(this, &Convolution_1::blocking_transport);
}

Convolution_1::~Convolution_1() {}

void Convolution_1::MAC(){
    
    
    sc_int<24> source, weight, acc, max_vaule;
     
    while(true) { // m loop
        max_vaule = 0;
        for(int i = 0 ; i < 2 ; i++){
            for(int j = 0 ; j < 2 ; j++){
                acc = 0;
                for(int c = 0 ; c < C ; c++){
                    for(int r = 0 ; r < R ; r++){
                        for(int s = 0 ; s < S ; s++){
                        
                            source = i_source.read();
                            weight = i_weight.read();
                            //cout << "Get: " << c << "," << r << "," << s << " : " << source << " " << weight << endl;
                            acc +=  source * weight;
                        }
                    }
                }
                max_vaule = (max_vaule > acc ? max_vaule : acc);
            }
        }
        
        o_result.write(max_vaule);
        //cout << "Send data: " << acc << endl;
        
    }
    
}


void Convolution_1::blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay) {
    wait(delay);
    sc_dt::uint64 addr = payload.get_address();
    //addr = addr - base_offset;
    unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    unsigned char *data_ptr = payload.get_data_ptr();
    sc_int<24> result, source, weight;
    switch (payload.get_command()) {
        case tlm::TLM_READ_COMMAND:
            switch (addr) {
                case CONV1_RESULT_ADDR:
                    //cout << "conv1 socket try read result: " << endl;
                    result = o_result.read();
                   // cout << "conv1 socket read result: " << result << endl;
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
            
            //cout << "conv socket set result: " << result << endl;
            break;
        case tlm::TLM_WRITE_COMMAND:
            switch (addr) {
                case CONV1_SOURCE_ADDR:
                    source.range(7,0) = data_ptr[0] & mask_ptr[0];
                    source.range(15,8) = data_ptr[1] & mask_ptr[1];
                    source.range(23,16) = data_ptr[2] & mask_ptr[2];
                    i_source.write(source);
                    //cout << "conv1 socket get source: " << source << endl;
                    break;
                case CONV1_WEIGHT_ADDR:
                    weight.range(7,0) = data_ptr[0] & mask_ptr[0];
                    weight.range(15,8) = data_ptr[1] & mask_ptr[1];
                    weight.range(23,16) = data_ptr[2] & mask_ptr[2];
                    i_weight.write(weight);
                    //cout << "conv1 socket get weight: " << weight << endl;
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

