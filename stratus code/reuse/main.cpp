#include <iostream>
#include <string>
#include <systemc>
#include <sys/time.h>
#include "System.h"
#include "Compile_def.h"

#include "esc.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;


System* sys = NULL;

extern void esc_elaborate() {
    sys = new System("sys");
}

extern void  esc_cleanup() {
    delete sys;
}

int sc_main(int argc, char *argv[]) {

    esc_initialize(argc, argv);
    esc_elaborate();

    sc_start();

	esc_cleanup();

    std::cout<< "Simulated time == " << sc_core::sc_time_stamp() << std::endl;
    return (0);
}