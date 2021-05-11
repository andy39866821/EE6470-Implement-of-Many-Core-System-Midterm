# EE6470 Midterm
###### tags: `EE6470`
## Problems and Solutions
### Multiple module integration
Q: How to define multiple module in tcl file
A: Modify tcl file as below
```
define_sim_config B 
foreach cfg { BASIC DPA } {
	define_sim_config V_${cfg} \
	"Convolution_1 RTL_V ${cfg}" \
	"Convolution_2 RTL_V ${cfg}" \
	"FC_1 RTL_V ${cfg}" \
	"FC_2 RTL_V ${cfg}" \
	"FC_3 RTL_V ${cfg}"
}

```
### Same module different parameters
Q: Can not change parameters in declaration.
A: Create multiple files with different fixed parameters for HLS.
```
Conv_1.cpp
Conv_2.cpp
```

## Implementation details 
### Model
Here is the model description in Python, which has two convoution layers and three fully connection layers.
![](https://i.imgur.com/E2DPcW6.png)

### Reuse method
For convolution layers, I will buffer rows of input, that is, we can reuse row and colum of data when calculating result, and here is the figure for understanding.
![](https://i.imgur.com/aypIe8g.png)
For fully connection layers, I will buffer reulst array to prevent redundent source reading.
![](https://i.imgur.com/BrLGSgi.png)

### TLM overview
Red modules are **Simple bus** that TA provided, and five green modules is for HLS.
![](https://i.imgur.com/znlO9Ca.png)

### Convolution design
First the convolution module will requeset source and weight, after convolution, the result wii prepared for RELU and max-pooling, finally it will feed back result to testbench via simple bus and socket port.
![](https://i.imgur.com/9TGXAxR.png)


### Fully connection design
Same to convolution part, FC part will request data then feed back data after doing RELU.
![](https://i.imgur.com/L9IZry2.png)

### Testbench
Here is five process in testbench, which has connected to each module ports(including conv1, conv2, fc1, fc2, and fc3), these processes will feed and fetch data with the connected modules.
![](https://i.imgur.com/dyo7H9W.png)


## Additional features
### Quantization method for hardware implementation

Since 32-bits floating point multiplication is too expensive for hardware design, I modify the Python training model to 8-bits multiplication and 32-bits accumulation to reduce the aread and power.
![](https://i.imgur.com/cywyiCy.png)
.

## Experimental results
### Input times 
As the table showing, reuse method can save up to 90% input times, which show this method is work.
![](https://i.imgur.com/TuGkht4.png)
### Area

Since reuse method will consume more area to place buffer and controller, this method can not save are.
![](https://i.imgur.com/fLT68Gz.png)

Reuse version HLS area diagram in Stratus
![](https://i.imgur.com/SgbN6Cv.png)


Non-reuse version HLS area diagram in Stratus
![](https://i.imgur.com/vt8QH4O.png)

### Latency
We can found that for the higher IO delay, reuse version can save more times as the figrue I plot below. 
#### Reuse version
![](https://i.imgur.com/uI1DSsK.png)

#### Non-Reuse version
![](https://i.imgur.com/greD1ZN.png)
#### Growing rate
![](https://i.imgur.com/NJ2N4V3.png)
            

## Discussions and conclusions
 By using systemC to design this DNN model, it is really convinient for data path.Since no need to design FSM and timing control, I think it can save up to 80% time compared to RTL coding. Also the TLM modeling is very powerful tool to verify the design.
And I learned again that data reading ordering cna really influence the performance, also I learned lost of about area-latency trade-off.
