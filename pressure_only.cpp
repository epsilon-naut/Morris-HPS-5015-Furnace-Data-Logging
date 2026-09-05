#include "WF_SDK/WF_SDK.h"
#include <time.h>
#include <stdio.h>
#include <fstream>
#include <string>

using namespace wf;
using namespace std;

// function for writing data to a csv file
void write_csv(string filename, char* time, int temp, int pressure, double raw, double measurement, int count) {
    ofstream output;
    output.open(filename, ios::app);
    for(int i = 0; i < 24; i++) {
        output << time[i];
    }
    output << "," << count << "," << temp << "," << pressure << "," << raw << "," << measurement << "\n";
    output.close();
}

// function for writing individual data values to a csv file
void write_csv_head(string filename, string *colnames, int size) {
    ofstream output;
    output.open(filename);
    for(int i = 0; i < size-1; i++) {
        output << colnames[i] << ",";
    }
    output << colnames[size-1] << "\n";
    output.close();
}

// analog pressure conversion function
void convert_to_pressure(double *voltages, double measurement, int *n, int *pressure, double *raw, double a, double b) {
    // fill the voltages array with measurements
    if(*n < 10000) {
        voltages[*n] = measurement;
        *n += 1;
    }

    // remove the front of the voltages array and add the measurement to the end (FILO)
    else {
        for(int i = 1; i < 10000; i++) {
            voltages[i-1] = voltages[i];
        }
        voltages[9999] = measurement;
    }

    double sum = 0;
    for(int i = 0; i < *n; i++) {
        sum += voltages[i];
    }
    *pressure = round((sum/(*n)*a+b));
    *raw = (sum/(*n));

}

void req_time(struct tm **ptr) {
    time_t t = time(NULL);
    *ptr = localtime(&t);
}

int main() {

    string filename = "test.csv";
    string colnames[6] = {"Time", "Count", "Temperature (C)", "Pressure (bar)", "Average Pressure Voltage (mV)", "Analog Pressure Reading (mV)"};
    write_csv_head("test.csv", colnames, 6);

    wf::Device::Data *device_data;
    device_data = device.open("Analog Discovery 2", 3);
    // get maximum buffer / minimum buffer sizes, else check for errors 
    int max_buf;  
    if(FDwfDigitalInBufferSizeInfo(device_data->handle, &max_buf) == 0) {
        device.check_error(device_data);
    }
    // start the scope
    scope.open(device_data, 20000000, max_buf, 0, 0.5);

    // set analog input triggering
    scope.trigger(device_data, 1, scope.trigger_source.analog, 1, 0);

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
        struct tm *ti;
        req_time(&ti);
        time = asctime(ti);
        time[24] = '\0';
        measurement = scope.measure(device_data, 1) * 1000;
        convert_to_pressure(voltages, measurement, &n, &pressure, &raw, a,  b);
        printf("%f ", raw);
        printf("%d ", pressure);
        printf("%f\n", measurement);
        write_csv("test.csv", time, 0, pressure, raw, measurement, n);
    }
}