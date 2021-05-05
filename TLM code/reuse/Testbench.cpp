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

void Testbench::write(sc_int<8> value, int BASE, int ADDR){
    unsigned char mask[4];
    unsigned char buffer[4];

    mask[0] = 0xff;
    mask[1] = 0x00;
    mask[2] = 0x00;
    mask[3] = 0x00;
    buffer[0] = value;
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = 0;

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
    sc_int<24> result;
    int failed_cnt = 0;
    wait(5 * CLOCK_PERIOD, SC_NS);
    conv1_start_time = sc_time_stamp();
    for (int m = 0; m != 6; m++) {
        for (int c = 0; c != 3; c++) {
            for (int r = 0; r != 5; r++) {
                for(int s = 0; s != 5 ; s++) {
                    write(conv1_weight[m][c][r][s], CONV1_MM_BASE, CONV1_WEIGHT_ADDR);
                }
            }
        }

        for (int p = 0; p != 32; p++) {
            for(int c = 0 ; c != 3; c++) {
                for(int q = 0;  q != 32 ; q ++) {
                    write(input_quantized[c][p][q], CONV1_MM_BASE, CONV1_SOURCE_ADDR);
                    //cout << "feed data: " << input_quantized[c][p][q] << endl;
                }
            }
            if(p % 2 == 1 && p >= 5){
                for(int q = 0 ; q < 14 ; q++){
                    int P = (p-5)/2;
                    result = read(CONV1_MM_BASE, CONV1_RESULT_ADDR);
                    if(result != conv1_activation_golden[m][P][q]){
                        cout << "ERROR: m" << " " << P << " " << q << " :" <<  result 
                                << "\n   Correct should be: " << conv1_activation_golden[m][P][q] << endl;
                        pass = false;
                        failed_cnt++;
                    }
                    //cout << "get data: " << result << endl;
                    conv1_activation_quantized[m][P][q] = round(((double)result / conv1_output_scale));             
                }
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
        cout << "       Error count: " << failed_cnt << endl;
    }

}


void Testbench::do_conv2(){
    bool pass = true;
    sc_int<24> result;
    int failed_cnt = 0;
    int cnt = 0;

    wait(5 * CLOCK_PERIOD, SC_NS);
    conv2_start_time = sc_time_stamp();
    for (int m = 0; m != 16; m++) {
        for (int c = 0; c != 6; c++) {
            for (int r = 0; r != 5; r++) {
                for(int s = 0; s != 5 ; s++) {
                    write(conv2_weight[m][c][r][s], CONV2_MM_BASE, CONV2_WEIGHT_ADDR);
                    //cout << "sent weight: " <<  m << " " << c << " " << r << " " << s << " " << conv2_weight[m][c][r][s] << endl;
                }
            }
        }
        
        for (int p = 0; p != 14; p++) {
            for(int c = 0 ; c != 6; c++) {
                for(int q = 0;  q != 14 ; q ++) {
                    write(conv1_activation_quantized[c][p][q], CONV2_MM_BASE, CONV2_SOURCE_ADDR);
                    
                }
            }
            if(p % 2 == 1 && p >= 5){
                for(int q = 0 ; q < 5 ; q++){
                    int P = (p-5)/2;
                    result = read(CONV2_MM_BASE, CONV2_RESULT_ADDR);
                    if(result != conv2_activation_golden[m][P][q]){
                        cout << "ERROR: m" << " " << P << " " << q << " :" <<  result 
                                << "\n   Correct should be: " << conv2_activation_golden[m][P][q] << endl;
                        pass = false;
                        failed_cnt++;
                    }
                    //cout << "get data: " << cnt << " " << result << endl;
                    fc1_input[cnt++] = round(((double)result / conv2_output_scale));             
                }
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
        cout << "       Error count: " << failed_cnt << endl;
    }

}

void Testbench::do_fc1(){
    bool pass = true;
	sc_int<24> result;

    fc1_start_time = sc_time_stamp();

    for (int h = 0; h != FC1_H; h++) { 
        write(fc1_input[h], FC1_MM_BASE, FC1_SOURCE_ADDR);
        for (int m = 0; m != FC1_M; m++) {
            write(fc1_weight[m][h], FC1_MM_BASE, FC1_WEIGHT_ADDR); 
        }                            
    }


    for (int m = 0; m != FC2_H; m++) {
        result = read(FC1_MM_BASE, FC1_RESULT_ADDR);
        if(result != fc1_activation_golden[m]){
            
            cout << "ERROR: " << m << " :" <<  result
                    << "\n   Correct should be: " << fc1_activation_golden[m] << endl;
            pass = false;
        }
        fc1_activation_quantized[m] = round(((double)result / fc1_output_scale));
       
    }

    wait(FC1_EACH_ROUND_TIMES, SC_NS);
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

    //cout << "start fc2" << endl;
    for (int h = 0; h != FC2_H; h++) {
        write(fc1_activation_quantized[h], FC2_MM_BASE, FC2_SOURCE_ADDR);
        for (int m = 0; m != FC2_M; m++) {
            write(fc2_weight[m][h], FC2_MM_BASE, FC2_WEIGHT_ADDR);  
            //cout << "TB sent weight: " << h << " " << m << endl; 
        }                            
    }

    for (int m = 0; m != FC3_H; m++) {
        result = read(FC2_MM_BASE, FC2_RESULT_ADDR);
        //cout << "fc2 get result: " << result << endl;
        if(result != fc2_activation_golden[m]){
            
            cout << "ERROR: " << m << " :" <<  result
                    << "\n   Correct should be: " << fc2_activation_golden[m] << endl;
            pass = false;
        }
        fc2_activation_quantized[m] = round(((double)result / fc2_output_scale));
                
    }

    wait(FC2_EACH_ROUND_TIMES, SC_NS);
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

    for (int h = 0; h != FC3_H; h++) {
        write(fc2_activation_quantized[h], FC3_MM_BASE, FC3_SOURCE_ADDR);  
        //cout << "TB sent input: " << h << endl;
                    
        for (int m = 0; m != FC3_M; m++) {
        write(fc3_weight[m][h], FC3_MM_BASE, FC3_WEIGHT_ADDR);   
            //cout << "TB sent weight: " << h << " " << m << endl; 
        }                            
    }

    for (int m = 0; m != FC3_M; m++) {      
        write(fc3_bias[m], FC3_MM_BASE, FC3_BIAS_ADDR);

        result = read(FC3_MM_BASE, FC3_RESULT_ADDR);
        
        result = round(((double)result / fc3_output_scale));
        if(result != label[m]){
            
            cout << "ERROR: " << m << " :" <<  result
                    << "\n   Correct should be: " << label[m] << endl;
            pass = false;
        }
                
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