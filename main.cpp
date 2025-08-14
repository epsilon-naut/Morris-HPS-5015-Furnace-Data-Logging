#include <stdio.h>
#include "datalog.h"
#include <string>

using namespace std;

int main(void) {

    // set up necessary parameters
    
    string name = "Analog Discovery 2";
    double out_freq = 9599; // frequency in Hz (20 MHz default)
    double in_freq = 9599;
    int config = 3;
    int chI = 7; // pin 3
    int chO = 6; // pin 4
    int achI = 1;
    int asr = 20000000;
    double offset = 0;
    double amp = 0.5;  // measured amplitude in V (+/- 5 default). Try to limit this range as much as possible as the ADC switches from high to low gain operation past ~ +/- 2.5 V, which means resolution increases from 0.3 mV to 3mV.

    printf("Starting Logging.\n");

    datalog(name, config, out_freq, in_freq, chI, chO, 1, achI, asr, offset, amp, "wtf.csv");

   return 0;
}