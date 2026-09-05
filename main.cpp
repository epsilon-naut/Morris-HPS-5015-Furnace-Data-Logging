#define _GLIBCXX_USE_CXX11_ABI 0
#include <stdio.h>
//#include "display.h"
#include "datalog.h"
#include <string>

using namespace std;

int main(void) {

    // set up necessary parameters
    
    string name = "Analog Discovery 2";
    double out_freq = 9599; // frequency in Hz (20 MHz default)
    double in_freq = 9599;
    int config = 3;
    int chI = 5; // pin 3
    int chO = 4; // pin 4
    int achI = 1;
    int asr = 20000000;
    double offset = 0;
    double amp = 0.5;  // measured amplitude in V (+/- 5 default). Try to limit this range as much as possible as the ADC switches from high to low gain operation past ~ +/- 2.5 V, which means resolution increases from 0.3 mV to 3mV.
    //display(0, {}, name, config, out_freq, out_freq, chI, chO, 1, achI, asr, offset, amp, "C:/Users/Jeff/Documents/Research/HPS5015/code/Debug/wtf.csv");

    void *device_data;
    start_device(name, config, out_freq, in_freq, chI, offset, &device_data);
    string filename = "test.csv";
    string colnames[6] = {"Time", "Count", "Temperature (C)", "Pressure (bar)", "Average Pressure Voltage (mV)", "Analog Pressure Reading (mV)"};
    write_csv_head("test.csv", colnames, 6);
    int temp;
    double measurement;
    char *time;
    int count;
    double a = 6.33;
    double b = -8.82;
    double voltages[10000];
    double raw;
    int pressure;
    int n = 0;
    while(1) {
        datalog(device_data, out_freq, in_freq, chI, chO, 0, achI, asr, offset, amp, filename, &time, &temp, &measurement, &count);
        printf("%f ", measurement);
        convert_to_pressure(voltages, measurement, &n, &pressure, &raw, a,  b);
        printf("%f ", raw);
        printf("%d\n", pressure);
        write_csv("test.csv", time, temp, pressure, raw, measurement, count);
        
    }

   return 0;
}