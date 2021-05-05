#include "Testbench.h"
#include <systemc>

Testbench::Testbench(sc_module_name n): sc_module(n){
    SC_THREAD(conv1_feed_input);
    sensitive << i_clk.pos();
    dont_initialize();

    SC_THREAD(conv1_fetch_result);
    sensitive << i_clk.pos();
    dont_initialize();

    SC_THREAD(conv2_feed_input);
    sensitive << i_clk.pos();
    dont_initialize();

    SC_THREAD(conv2_fetch_result);
    sensitive << i_clk.pos();
    dont_initialize();
    
    SC_THREAD(fc1_feed_input);
    sensitive << i_clk.pos();
    dont_initialize();

    SC_THREAD(fc1_fetch_result);
    sensitive << i_clk.pos();
    dont_initialize();

    SC_THREAD(fc2_feed_input);
    sensitive << i_clk.pos();
    dont_initialize();

    SC_THREAD(fc2_fetch_result);
    sensitive << i_clk.pos();
    dont_initialize();

    SC_THREAD(fc3_feed_input);
    sensitive << i_clk.pos();
    dont_initialize();

    SC_THREAD(fc3_fetch_result);
    sensitive << i_clk.pos();
    dont_initialize();

    conv1_finish = false;
    conv2_finish = false;
    fc1_finish = false;
    fc2_finish = false;
}

Testbench::~Testbench(){

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

void Testbench::conv1_feed_input(){
    int data_sent_count = 0;
    int weight_sent_count = 0;
	o_conv1_input.reset();
	o_conv1_weight.reset();

	o_rst.write(false);
	wait(5);
	o_rst.write(true);
	wait(1);

    conv1_start_time = sc_time_stamp();
    for (int m = 0; m != 6; m++) {
        for (int c = 0; c != 3; c++) {
            for (int r = 0; r != 5; r++) {
                for(int s = 0; s != 5 ; s++) {
                    o_conv1_weight.put(conv1_weight[m][c][r][s]);
                    weight_sent_count++;
                }
            }
        }
            
        for (int p = 0; p != 32; p++) {
            for(int c = 0 ; c != 3; c++) {
                for(int q = 0;  q != 32 ; q ++) {
                    o_conv1_input.put(input_quantized[c][p][q]);
                    data_sent_count++;
                    //cout << "Feed data: " << m << " " << c << " " << r << " " << s << endl;
                }
                conv1_each_row_start_time = sc_time_stamp();
            }
        }
    }
    
    cout << "   Conv1 sent data: " << data_sent_count << " times" << endl;
    cout << "   Conv1 sent weight: " << weight_sent_count << " times" << endl;
    

}
void Testbench::conv1_fetch_result(){
    bool pass = true;
    bool has_show_each_row_time = false;
	sc_int<24> result;

	i_conv1_activation.reset();

	wait(5);

    for (int m = 0; m != 6; m++) {
        for (int p = 0; p != 14; p++) {
            for(int q = 0; q != 14 ; q++) {
                result = i_conv1_activation.get();
                
                if(result != conv1_activation_golden[m][p][q]){
                    cout << "ERROR: m" << " " << p << " " << q << " :" <<  result 
                            << "\n   Correct should be: " << conv1_activation_golden[m][p][q] << endl;
                    pass = false;
                }
                conv1_activation_quantized[m][p][q] = round(((double)result / conv1_output_scale));             
            }
            if(has_show_each_row_time == false){
                has_show_each_row_time = true;
                cout << "   Conv1 each row simulated time: " << sc_time_stamp() - conv1_each_row_start_time << endl;
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
    conv1_finish = true;
	
}

void Testbench::conv2_feed_input(){
    
    int data_sent_count = 0;
    int weight_sent_count = 0;
	o_conv2_input.reset();
	o_conv2_weight.reset();

    while(conv1_finish == false) {
        wait();
    }

    conv2_start_time = sc_time_stamp();
    
    for (int m = 0; m != 16; m++) {
        for (int c = 0; c != 6; c++) {
            for (int r = 0; r != 5; r++) {
                for(int s = 0; s != 5 ; s++) {
                    o_conv2_weight.put(conv2_weight[m][c][r][s]);
                    weight_sent_count++;
                }
            }
        }
            
        for (int p = 0; p != 14; p++) {
            for(int c = 0 ; c != 6; c++) {
                for(int q = 0;  q != 14 ; q ++) {
                    o_conv2_input.put(conv1_activation_quantized[c][p][q]);
                    data_sent_count++;
                    //cout << "Feed data: " << m << " " << c << " " << r << " " << s << endl;      
                }
                conv2_each_row_start_time = sc_time_stamp();
            }
        }
    }

    cout << "   Conv2 sent data: " << data_sent_count << " times" << endl;
    cout << "   Conv2 sent weight: " << weight_sent_count << " times" << endl;
}

void Testbench::conv2_fetch_result(){
    bool pass = true;
    bool has_show_each_row_time = false;
    int cnt = 0;
	sc_uint<24> result;

	i_conv2_activation.reset();

    while(conv1_finish == false) {
        wait();
    }
	wait(5);
    for (int m = 0; m != 16; m++) {
        for (int p = 0; p != 5; p++) {
            for(int q = 0; q != 5 ; q++) {
                result = i_conv2_activation.get();
                if(result != conv2_activation_golden[m][p][q]){
                    cout << "ERROR: m" << " " << p << " " << q << " :" <<  result 
                            << "\n   Correct should be: " << conv2_activation_golden[m][p][q] << endl;
                    pass = false;
                }
                fc1_input[cnt++] = round(((double)result / conv2_output_scale));             
            }
            if(has_show_each_row_time == false){
                has_show_each_row_time = true;
                cout << "   Conv2 each row simulated time: " << sc_time_stamp() - conv2_each_row_start_time << endl;
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
    conv2_finish = true;
}


void Testbench::fc1_feed_input(){
    
    int data_sent_count = 0;
    int weight_sent_count = 0;
	o_fc1_input.reset();
	o_fc1_weight.reset();

    while(conv2_finish == false) {
        wait();
    }

    fc1_start_time = sc_time_stamp();

    for (int h = 0; h != FC1_H; h++) {
        o_fc1_input.put(fc1_input[h]);
        data_sent_count++;
        for (int m = 0; m != FC1_M; m++) {
            o_fc1_weight.put(fc1_weight[m][h]);
            weight_sent_count++;
        }                            
    }
    
    cout << "   FC1 sent data: " << data_sent_count << " times" << endl;
    cout << "   FC1 sent weight: " << weight_sent_count << " times" << endl;
}

void Testbench::fc1_fetch_result(){
    bool pass = true;
	sc_int<24> result;

	i_fc1_activation.reset();

    while(conv2_finish == false) {
        wait();
    }
	wait(5);

    for (int m = 0; m != FC2_H; m++) {
        fc1_activation[m] = i_fc1_activation.get();
        // cout << "Get data: " << result << endl;
        if(fc1_activation[m] != fc1_activation_golden[m]){
            
            cout << "ERROR: " << m << " :" <<  fc1_activation[m]
                    << "\n   Correct should be: " << fc1_activation_golden[m] << endl;
            pass = false;
        }
        fc1_activation_quantized[m] = round(((double)fc1_activation[m] / fc1_output_scale));
                
    }

    cout << "     FC1 activation simulated time: " << sc_time_stamp() - fc1_start_time << endl;
    if(pass == true){
        cout << "=====> FC1 activation PASS" << endl;
    }
    else{
        cout << "=====> FC1 activation FAILED" << endl;
    }
    fc1_finish = true;

}


void Testbench::fc2_feed_input(){
    int data_sent_count = 0;
    int weight_sent_count = 0;
	o_fc2_input.reset();
	o_fc2_weight.reset();

    while(fc1_finish == false) {
        wait();
    }

    fc2_start_time = sc_time_stamp();

    for (int h = 0; h != FC2_H; h++) {
        o_fc2_input.put(fc1_activation_quantized[h]);
        data_sent_count++;
        for (int m = 0; m != FC2_M; m++) {
            o_fc2_weight.put(fc2_weight[m][h]);
            weight_sent_count++;
        }                            
    }
    
    cout << "   FC2 sent data: " << data_sent_count << " times" << endl;
    cout << "   FC2 sent weight: " << weight_sent_count << " times" << endl;
}

void Testbench::fc2_fetch_result(){
    bool pass = true;
	sc_int<24> result;

	i_fc2_activation.reset();

    while(fc1_finish == false) {
        wait();
    }
	wait(5);

    for (int m = 0; m != FC3_H; m++) {
        fc2_activation[m] = i_fc2_activation.get();
        // cout << "Get data: " << result << endl;
        if(fc2_activation[m] != fc2_activation_golden[m]){
            
            cout << "ERROR: " << m << " :" <<  fc2_activation[m]
                    << "\n   Correct should be: " << fc2_activation_golden[m] << endl;
            pass = false;
        }
        fc2_activation_quantized[m] = round(((double)fc2_activation[m] / fc2_output_scale));
                
    }

    cout << "     FC2 activation simulated time: " << sc_time_stamp() - fc2_start_time << endl;
    if(pass == true){
        cout << "=====> FC2 activation PASS" << endl;
    }
    else{
        cout << "=====> FC2 activation FAILED" << endl;
    }
    fc2_finish = true;

}

void Testbench::fc3_feed_input(){
    int data_sent_count = 0;
    int weight_sent_count = 0;
    int bias_sent_count = 0;
	o_fc3_input.reset();
	o_fc3_weight.reset();
	o_fc3_bias.reset();

    while(fc2_finish == false) {
        wait();
    }

    fc3_start_time = sc_time_stamp();

    for (int h = 0; h != FC3_H; h++) {
        o_fc3_input.put(fc2_activation_quantized[h]);
        data_sent_count++;
        for (int m = 0; m != FC3_M; m++) {
            o_fc3_weight.put(fc3_weight[m][h]);
            weight_sent_count++;
        }                            

    }

    for (int m = 0; m != FC3_M; m++) {
        o_fc3_bias.put(fc3_bias[m]);
        bias_sent_count++;
    }                    
    
    
    cout << "   FC3 sent data: " << data_sent_count << " times" << endl;
    cout << "   FC3 sent weight: " << weight_sent_count << " times" << endl;
    cout << "   FC3 sent bias: " << bias_sent_count << " times" << endl;
}

void Testbench::fc3_fetch_result(){
    bool pass = true;
	sc_int<24> result;

	i_fc3_activation.reset();

    while(fc2_finish == false) {
        wait();
    }
	wait(5);

    for (int m = 0; m != FC3_M; m++) {
        fc3_activation[m] = i_fc3_activation.get();
        fc3_activation[m] = round(((double)fc3_activation[m] / fc3_output_scale));
        if(fc3_activation[m] != label[m]){
            
            cout << "ERROR: " << m << " :" <<  fc3_activation[m]
                    << "\n   Correct should be: " << label[m] << endl;
            pass = false;
        }
    }

    cout << "   FC3 activation simulated time: " << sc_time_stamp() - fc3_start_time << endl;
    if(pass == true){
        cout << "=====> FC3 activation PASS" << endl;
    }
    else{
        cout << "=====> FC3 activation FAILED" << endl;
    }
    sc_stop();
}

