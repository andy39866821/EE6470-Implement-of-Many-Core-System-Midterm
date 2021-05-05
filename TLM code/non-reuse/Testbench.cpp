#include "testbench.h"
#include <systemc>

Testbench::Testbench(sc_module_name n):
    sc_module(n), 
    initiator("initiator") {

    SC_THREAD(do_DNN);
    //SC_THREAD(do_conv2);
}

void Testbench::read_weight(){

    FILE *fp_s = NULL; // source file handler
    string file_path;

    file_path = "../parameters/weights/conv1_weights.txt";
    fp_s = fopen(file_path.c_str(), "r");
    if(!fp_s)
        cout << "open file conv1 ERROR" << endl;
    for(int i=0;i<6;i++)
        for(int j=0;j<3;j++)
            for(int k=0;k<5;k++)
                for(int l=0;l<5;l++)
                    fscanf(fp_s, "%d",&conv1_weight[i][j][k][l]);
    fclose(fp_s);  
    
    file_path = "../parameters/weights/conv2_weights.txt";
    fp_s = fopen(file_path.c_str(), "r");  
    if(!fp_s)
        cout << "open file conv2 ERROR" << endl;
    for(int i=0;i<16;i++)
        for(int j=0;j<6;j++)
            for(int k=0;k<5;k++)
                for(int l=0;l<5;l++)
                    fscanf(fp_s, "%d",&conv2_weight[i][j][k][l]);
    fclose(fp_s);
      
    file_path = "../parameters/weights/fc1_weights.txt";
    fp_s = fopen(file_path.c_str(), "r");   
    if(!fp_s)
        cout << "open file fc1 ERROR" << endl;
    for(int i=0;i<120;i++)
        for(int j=0;j<400;j++)
            fscanf(fp_s, "%d",&fc1_weight[i][j]);
    fclose(fp_s);

    file_path = "../parameters/weights/fc2_weights.txt";
    fp_s = fopen(file_path.c_str(), "r");   
    if(!fp_s)
        cout << "open file fc2 ERROR" << endl;
    for(int i=0;i<84;i++)
        for(int j=0;j<120;j++)
            fscanf(fp_s, "%d",&fc2_weight[i][j]);
    fclose(fp_s);

    file_path = "../parameters/weights/fc3_weights.txt";
    fp_s = fopen(file_path.c_str(), "r");   
    if(!fp_s)
        cout << "open file fc3 ERROR" << endl;
    for(int i=0;i<10;i++)
        for(int j=0;j<84;j++)
            fscanf(fp_s, "%d",&fc3_weight[i][j]);
    fclose(fp_s);   
    
    file_path = "../parameters/weights/fc3_bias.txt";
    fp_s = fopen(file_path.c_str(), "r");   
    if(!fp_s)
        cout << "open file fc3 ERROR" << endl;
    for(int i=0;i<10;i++)
        fscanf(fp_s, "%d",&fc3_bias[i]);
    fclose(fp_s);  
}
void Testbench::read_golden(int index){
    char buf[30];
    sprintf(buf, "%d", index);
    string filename = "conv1_activation_" + string(buf) + ".txt";

    FILE *fp_s = NULL; // source file handler
    fp_s = fopen(("../dataset/" + filename).c_str(), "r");
    if(!fp_s)  
        cout << "open file conv1_activation ERROR" << endl;
    for(int i=0;i<6;i++)
        for(int j=0;j<14;j++)
            for(int k=0;k<14;k++)
                fscanf(fp_s, "%d",&conv1_activation_golden[i][j][k]);
    fclose(fp_s);     

    filename = "conv2_activation_" + string(buf) + ".txt";
    fp_s = fopen(("../dataset/" + filename).c_str(), "r");
    if(!fp_s)  
        cout << "open file conv2_activation ERROR" << endl;
    for(int i=0;i<16;i++)
        for(int j=0;j<5;j++)
            for(int k=0;k<5;k++)
                fscanf(fp_s, "%d",&conv2_activation_golden[i][j][k]);
    fclose(fp_s);    

    filename = "fc1_activation_" + string(buf) + ".txt";
    fp_s = fopen(("../dataset/" + filename).c_str(), "r");
    if(!fp_s)  
        cout << "open file fc1_activation ERROR" << endl;
    for(int i=0;i<FC1_M;i++)
        fscanf(fp_s, "%d",&fc1_activation_golden[i]);
    fclose(fp_s);      
    
    filename = "fc2_activation_" + string(buf) + ".txt";
    fp_s = fopen(("../dataset/" + filename).c_str(), "r");
    if(!fp_s)  
        cout << "open file fc2_activation ERROR" << endl;
    for(int i=0;i<FC2_M;i++)
        fscanf(fp_s, "%d",&fc2_activation_golden[i]);
    fclose(fp_s);     

}
void Testbench::read_input(int index){
    char buf[30];
    sprintf(buf, "%d", index);
    string input_filename = "data_" + string(buf)  + ".txt";
    sprintf(buf, "%d", index);
    string label_filename = "label_" + string(buf)  + ".txt";

    FILE *fp_s = NULL; // source file handler
    fp_s = fopen(("../dataset/" + input_filename).c_str(), "r");
    if (!fp_s)
        cout << "open file input ERROR" << endl;
    for(int i=0;i<3;i++)
        for(int j=0;j<32;j++)
            for(int k=0;k<32;k++) {
                fscanf(fp_s, "%lf",&input[i][j][k]);
                // read floating point input and quantization
                input_quantized[i][j][k] = input[i][j][k] * input_scale;
            }
    fclose(fp_s); 

    fp_s = fopen(("../dataset/" + label_filename).c_str(), "r");
    if (!fp_s)
        cout << "open file label ERROR" << endl;
    for(int i=0;i<10;i++){
        fscanf(fp_s, "%d",&label[i]);
    }
    fclose(fp_s); 
}
void Testbench::write(sc_int<24> value, int BASE, int ADDR){
    word data;
    unsigned char mask[4];
    unsigned char buffer[4];

    data.Integer = value;
    mask[0] = 0xff;
    mask[1] = 0xff;
    mask[2] = 0xff;
    mask[3] = 0xff;
    buffer[0] = data.Character[0];
    buffer[1] = data.Character[1];
    buffer[2] = data.Character[2];
    buffer[3] = data.Character[3];

    initiator.write_to_socket(BASE + ADDR, mask, buffer, 4);   
}

sc_int<24> Testbench::read(int BASE, int ADDR){
    sc_int<24> result;
    unsigned char buffer[4];
    unsigned char mask[4];
    initiator.read_from_socket(BASE + ADDR, mask, buffer, 4);
    result.range(7,0) = buffer[0];
    result.range(15,8) = buffer[1];
    result.range(23,16) = buffer[2];

    return result;
}
void Testbench::do_DNN(){
    do_conv1();
    do_conv2();
    do_fc1();
    do_fc2();
    do_fc3();

    sc_stop();
}

void Testbench::do_conv1(){
    bool pass = true;

    conv1_start_time = sc_time_stamp();
    for (int m = 0; m != 6; m++) {
        for (int p_mp = 0; p_mp != 14; p_mp++) {
            for(int q_mp = 0; q_mp != 14 ; q_mp ++) {
                for(int i = 0 ; i < 2 ; i++){
                    for (int j = 0 ; j < 2 ; j++){
                        int p = p_mp * 2 + i;
                        int q = q_mp * 2 + j;
                        for (int c = 0; c != 3; c++) {
                            for (int r = 0; r != 5; r++) {
                                for(int s = 0; s != 5 ; s++) {
                                    write(input_quantized[c][p + r][q + s], CONV1_MM_BASE, CONV1_SOURCE_ADDR);
                                    write(conv1_weight[m][c][r][s], CONV1_MM_BASE, CONV1_WEIGHT_ADDR);
                                   
                                }
                            }
                        }
                        
                    }
                }
                 conv1_activation[m][p_mp][q_mp] = read(CONV1_MM_BASE, CONV1_RESULT_ADDR);
               // cout << "Get data: " << result << endl;
                if(conv1_activation[m][p_mp][q_mp] != conv1_activation_golden[m][p_mp][q_mp]){
                    
                    cout << "ERROR: m" << " " << p_mp << " " << q_mp << " :" <<  conv1_activation[m][p_mp][q_mp] 
                         << "\n   Correct should be: " << conv1_activation_golden[m][p_mp][q_mp] << endl;
                    pass = false;
                }
                conv1_activation_quantized[m][p_mp][q_mp] = round(((double)conv1_activation[m][p_mp][q_mp] / conv1_output_scale));
                
                wait(CONV1_EACH_ROW_TIMES, SC_NS);

            }
        }
    }

    cout << "   Conv1 activation simulated time: " << sc_time_stamp() - conv1_start_time << endl;
    if(pass == true){
        cout << "=====> Conv1 activation PASS" << endl;
    }
    else{
        cout << "=====> Conv1 activation FAILED" << endl;
    }

}


void Testbench::do_conv2(){
    bool pass = true;
    int cnt = 0;

    conv2_start_time = sc_time_stamp();
    for (int m = 0; m != 16; m++) {
        for (int p_mp = 0; p_mp != 5; p_mp++) {
            for(int q_mp = 0; q_mp != 5 ; q_mp ++) {
                for(int i = 0 ; i < 2 ; i++){
                    for (int j = 0 ; j < 2 ; j++){
                        int p = p_mp * 2 + i;
                        int q = q_mp * 2 + j;
                        for (int c = 0; c != conv2_C; c++) {
                            for (int r = 0; r != conv2_R; r++) {
                                for(int s = 0; s != conv2_S ; s++) {
                                    write(conv1_activation_quantized[c][p + r][q + s], CONV2_MM_BASE, CONV2_SOURCE_ADDR);
                                    write(conv2_weight[m][c][r][s], CONV2_MM_BASE, CONV2_WEIGHT_ADDR);
                                    
                                }
                            }
                        }
                        
                    }
                }

                conv2_activation[m][p_mp][q_mp] = read(CONV2_MM_BASE, CONV2_RESULT_ADDR);
               // cout << "Get data: " << result << endl;
                if(conv2_activation[m][p_mp][q_mp] != conv2_activation_golden[m][p_mp][q_mp]){
                    
                    cout << "ERROR: m" << " " << p_mp << " " << q_mp << " :" <<  conv2_activation[m][p_mp][q_mp] 
                         << "\n   Correct should be: " << conv2_activation_golden[m][p_mp][q_mp] << endl;
                    pass = false;
                }
                fc1_input[cnt++] = round(((double)conv2_activation[m][p_mp][q_mp] / conv2_output_scale));
                
                wait(CONV2_EACH_ROW_TIMES, SC_NS);

            }
        }
    }

    cout << "   Conv2 activation simulated time: " << sc_time_stamp() - conv2_start_time << endl;
    if(pass == true){
        cout << "=====> Conv2 activation PASS" << endl;
    }
    else{
        cout << "=====> Conv2 activation FAILED" << endl;
    }

}

void Testbench::do_fc1(){
    bool pass = true;
	sc_int<24> result;
    fc1_start_time = sc_time_stamp();
    word data;
    unsigned char mask[4];
    unsigned char buffer[4];

    for (int m = 0; m != FC1_M; m++) {
        for (int h = 0; h != FC1_H; h++) {
            write(fc1_input[h], FC1_MM_BASE, FC1_SOURCE_ADDR);
            write(fc1_weight[m][h], FC1_MM_BASE, FC1_WEIGHT_ADDR);
        }

        result = read(FC1_MM_BASE, FC1_RESULT_ADDR);     
        if(result != fc1_activation_golden[m]){
            
            cout << "ERROR: " << m << " :" <<  result
                    << "\n   Correct should be: " << fc1_activation_golden[m] << endl;
            pass = false;
        }
        fc1_activation_quantized[m] = round(((double)result / fc1_output_scale));
        wait(FC1_EACH_ROUND_TIMES, SC_NS); 
    }

    cout << "     FC1 activation simulated time: " << sc_time_stamp() - fc1_start_time << endl;
    if(pass == true){
        cout << "=====> FC1 activation PASS" << endl;
    }
    else{
        cout << "=====> FC1 activation FAILED" << endl;
    }
}

void Testbench::do_fc2(){
    bool pass = true;
	sc_int<24> result;

    fc2_start_time = sc_time_stamp();
    word data;
    unsigned char mask[4];
    unsigned char buffer[4];

    for (int m = 0; m != FC2_M; m++) {
        for (int h = 0; h != FC2_H; h++) {
            write(fc1_activation_quantized[h], FC2_MM_BASE, FC2_SOURCE_ADDR);
            write(fc2_weight[m][h], FC2_MM_BASE, FC2_WEIGHT_ADDR);
        }

        result = read(FC2_MM_BASE, FC2_RESULT_ADDR);     
        if(result != fc2_activation_golden[m]){
            
            cout << "ERROR: " << m << " :" <<  result
                    << "\n   Correct should be: " << fc2_activation_golden[m] << endl;
            pass = false;
        }
        fc2_activation_quantized[m] = round(((double)result / fc2_output_scale));
        wait(FC2_EACH_ROUND_TIMES, SC_NS); 
    }
    
    cout << "     FC2 activation simulated time: " << sc_time_stamp() - fc2_start_time << endl;
    if(pass == true){
        cout << "=====> FC2 activation PASS" << endl;
    }
    else{
        cout << "=====> FC2 activation FAILED" << endl;
    }
}

void Testbench::do_fc3(){
    bool pass = true;
	sc_int<24> result;

    fc3_start_time = sc_time_stamp();
    word data;
    unsigned char mask[4];
    unsigned char buffer[4];

    for (int m = 0; m != FC3_M; m++) {
        for (int h = 0; h != FC3_H; h++) {
            write(fc2_activation_quantized[h], FC3_MM_BASE, FC3_SOURCE_ADDR);
            write(fc3_weight[m][h], FC3_MM_BASE, FC3_WEIGHT_ADDR);
        }

        write(fc3_bias[m], FC3_MM_BASE, FC3_BIAS_ADDR);
        result = read(FC3_MM_BASE, FC3_RESULT_ADDR);  
        result = round(((double)result / fc3_output_scale));   
        if(result != label[m]){
            
            cout << "ERROR: " << m << " :" <<  result
                    << "\n   Correct should be: " << label[m] << endl;
            pass = false;
        }
        wait(FC3_EACH_ROUND_TIMES, SC_NS); 
    }

        wait(FC3_EACH_ROUND_TIMES, SC_NS); 
    cout << "     FC3 activation simulated time: " << sc_time_stamp() - fc3_start_time << endl;
    if(pass == true){
        cout << "=====> FC3 activation PASS" << endl;
    }
    else{
        cout << "=====> FC3 activation FAILED" << endl;
    }
}