#include <stdio.h>
#include "datalog.h"
#include <string>

using namespace std;

int main(void) {

    // set up necessary parameters
    
    string name = "Analog Discovery 2";
    double out_freq = 9599; // frequency in Hz (20 MHz default)
    double in_freq = 9599;
    double offset = 0; // offset in V (0 default), we may want to change this depending on the room temperature
    double amp = 0.2; // measured amplitude in V (+/- 5 default). Try to limit this range as much as possible as the ADC switches from high to low gain operation past ~ +/- 2.5 V, which means resolution increases from 0.3 mV to 3mV.
    int config = 3;
    int chI = 7; // pin 3
    int chO = 6; // pin 4

    printf("Starting Logging.\n");

    datalog(name, config, out_freq, in_freq, chI, chO, 1, "wtf.csv");

   return 0;
}