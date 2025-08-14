#include "WF_SDK/WF_SDK.h"
#include "datalog.h"
#include <stdio.h>
#include <string>
#include <time.h>
#include <fstream>
#include <vector>
#include <math.h>

using namespace wf;
using namespace std;

// function to start a delay for a certain amount of milliseconds
void delay(int ms) {
    clock_t start = clock();
    clock_t end = clock();
    while((double)(end - start)/CLOCKS_PER_SEC*1000 < ms) {
        end = clock();
    }
}

// function for writing data to a csv file
void write_csv(string filename, string colname, vector<double> measurements) {
    ofstream output;
    output.open(filename);
    output << colname << "\n";
    for(int i = 0; i < measurements.size(); i++) {
        output << measurements.at(i) << "\n";
    }
    output.close();
}

// function for writing individual data values to a csv file
void write_csv_indiv(string filename, double measurement) {
    ofstream output;
    output.open(filename, ios::app);
    output << measurement << "\n";
    output.close();
}

// function for writing individual data values to a csv file
void write_csv_head(string filename, string colname) {
    ofstream output;
    output.open(filename);
    output << colname << "\n";
    output.close();
}

// function for transforming csv file data into a temperature double array
void read_thermocouple_data(string filename, vector<double> arr) {

}

// function for converting voltage into temperature given thermocouple data 
void convert_to_temperature(double *voltage, vector<double> arr, int *temperature) {

    // abuse linear relationship to get a good guess of temperature
    int temp_g = (int)((*voltage - 0.1248)/0.0409);

    // sweep a range of ~5 C above and below the temperature for a more accurate guess
    int pos_neg = 0;
    for(int i = temp_g - 5; i < temp_g + 6; i++) {
        int diff = arr[i] - *voltage;
        int check = (diff)/(abs(diff));
        if(pos_neg == 0) {
            pos_neg = check;
        } 
        else if (pos_neg != check) {
            if(abs(*voltage - arr[i]) < abs(*voltage - arr[i] + 1)) {
                *temperature = i;
            } 
            else {
                *temperature = i-1;
            }
        }
    }
    
}

uint16_t wrap(uint8_t data) {
    uint8_t copy = data;
    int count = 0;
    while(data > 0) {
        count += (data & 1);
        data >>= 1;
    }
    return copy*4+(count%2)*2+1;
}

uint16_t wrap2(uint8_t data) {
    uint8_t copy = data;
    int count = 0;
    while(data > 0) {
        count += (data & 1);
        data >>= 1;
    }
    return 512+copy*2+(count%2)*256;
}

/*
    uint8_t *arr = (uint8_t *)malloc(int(n*5/4+1)*sizeof(uint8_t));
    for(int i = 0; i < n/4; i++) {
        uint8_t res = 0;
        for(int k = 0; k < 4; k++) {
            uint8_t num = 0;
            uint8_t inter = (res * 1024 + wrap(data[4*i+k])) / pow(2, 2*(k+1));
            res = (res * 1024 + wrap(data[4*i+k])) % int(pow(2, 2*(k+1)));
            int limit = (int)(log(inter)/log(2));
            for(int j = limit; j >= 0; j--) {
                if(pow(2, j) < inter) {
                    inter -= pow(2, j);
                    num += 128 / pow(2, j);
            }
        }
        arr[int(n*5/4)-5*i-k] = num;
        }
        arr[int(n*5/4)-5*i-4] = res;
    }
    uint8_t res = 0;
    uint8_t last_ind = 0;
    for(int k = 0; k < (n % 4); k++) {
        uint8_t num = 0;
        uint8_t inter = (res * 1024 + wrap(data[4*(n/4)+k])) / pow(2, 2*(k+1));
        res = (res * 1024 + wrap(data[4*(n/4)+k])) % int(pow(2, 2*(k+1)));
        int limit = (int)(log(inter)/log(2));
        for(int j = limit; j >= 0; j--) {
            if(pow(2, j) <= inter) {
                inter -= pow(2, j);
                num += 128 / pow(2, j);
            }
        }
        arr[(n%4)-k] = num;
        last_ind = 2*(k+1)-1;
    }
    uint8_t last = 0;
    int limit = (int)(log(res)/log(2));
        for(int j = limit; j >= 0; j--) {
            if(pow(2, j) <= res) {
                res -= pow(2, j);
                last += pow(2, last_ind) / pow(2, j);
            }
        }
    arr[0] = last;
    *nums = arr;
    */

void reverse(uint8_t *data, int n, uint8_t **nums, uint8_t **debug) {
    uint8_t *arr = (uint8_t *)malloc(n*10);

    // index from last to first, wrap the message and store it into individual bits, LSB first
    for(int i = 0; i < n; i++) {
        uint16_t wrapped = wrap(data[i]);
        //printf("%02x\n", wrapped);
        for(int j = 9; j >= 0; j--) {
            if(int(wrapped / pow(2, j)) == 1) {
                arr[10*(n-1-i)+j] = 1;
                wrapped -= int(pow(2, j));
            }
            else{
                arr[10*(n-1-i)+j] = 0;
            }
        }
    }

    //  repackage bits into bytes for output
    uint8_t *res = (uint8_t *)malloc(int(n*5/4)+1);
    uint8_t pos = 0;
    for(int i = 10 * n - 8; i > -1; i -= 8) {
        uint8_t num = 0;
        for(int j = 0; j < 8; j++) {
            //printf("%01x", arr[i+j]);
            num += int(pow(2, 7-j))*arr[i+j];
        }
        //printf(" %d\n", num);
        res[pos] = num;
        pos++;
    }
    uint8_t num = 0;
    for(int i = 10 * n % 8 - 1; i >= 0; i--) {
        //printf("%01x ", arr[i]);
        //printf("%d\n", int(pow(2, i)));
        num += int(pow(2, i))*arr[i];
    }
    if(10 * n % 8 != 0) {
        res[0] = num;
    }

    // store output
    *nums = res;
    *debug = arr;

}

void rewrap(uint8_t *data, int n, uint8_t **nums, uint8_t **debug) {
    uint8_t *arr = (uint8_t *)malloc(n*10);

    // index from last to first, wrap the message and store it into individual bits, LSB first
    for(int i = 0; i < n; i++) {
        uint16_t wrapped = wrap2(data[i]);
        //printf("%02x\n", wrapped);
        for(int j = 0; j < 10; j++) {
            if(int(wrapped / pow(2, 9-j)) == 1) {
                arr[10*(n-i-1)+j] = 1;
                wrapped -= int(pow(2, 9-j));
            }
            else{
                arr[10*(n-i-1)+j] = 0;
            }
        }
    }

    //  repackage bits into bytes for output
    uint8_t *res = (uint8_t *)malloc(int(n*5/4)+1);
    uint8_t *ult = (uint8_t *)malloc(int(n*5/4)+3);
    uint8_t pos = 0;
    for(int i = 10 * n - 8; i > -1; i -= 8) {
        uint8_t num = 0;
        for(int j = 0; j < 8; j++) {
            //printf("%01x", arr[i+j]);
            num += int(pow(2, 7-j))*arr[i+j];
        }
        //printf(" %d\n", num);
        res[pos] = num;
        pos++;
    }
    uint8_t num = 0;
    for(int i = 10 * n % 8 - 1; i >= 0; i--) {
        //printf("%01x ", arr[i]);
        //printf("%d\n", int(pow(2, i)));
        num += int(pow(2, i))*arr[i];
    }
    if(10 * n % 8 != 0) {
        res[pos] = num;
        pos++;
    }
    ult[0] = 0xFF;
    ult[pos+1] = 0xFF;
    for(int i = 1; i < pos+1; i++) {
        ult[i] = res[i-1];
    } 
    // store output
    *nums = ult;
    *debug = arr;
}

void req_data(Device::Data *device_data, int chI, int chO, double freq, double sample_rate, unsigned short mnem, unsigned short *data) {
    // create message to send
    // EOT = 0x04 => 0 0000100 1 1 -> 0000010011 -> 0x13 -> 0000010011 -> 00000100 -> 0x04 -> 00100000 -> 0x20
    // ADDR = 0x31 => 0 00110001 1 1 -> 00011000111 -> 0xC7 -> 110001100011 -> 11000110 -> 0xC6 -> 01100011 -> 0x33
    // mnem = 0x56 => 0 1010110 0 1 -> 0101011001 -> 0x159 -> 01110101011001 -> 01110101 -> 0x75 -> 00001111 -> 0x0F
    // ENQ = 0x05 => 0 0000101 0 1 -> 0000010101 -> 0x15 -> 0110010000010101 -> 01100100 -> 0x64
    // Leftovers => 00010101 -> 0x15
    // 0000010011
    // 0000000111
    // 0101011001
    // 0000010101
    // 0000010011
    uint8_t msg[8] = {EOT, ZERO, ZERO, ONE, ONE, P, V, ENQ};  // 0x04, 0x30, 0x30, 0x31, 0x31, 0x50, 0x56, 0x05
    uint8_t msg2[8] = {ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO, ZERO};
    uint8_t idle[1] = {0xFF};
    uint8_t *send;
    uint8_t *garbage;
    uint8_t *debug;
    rewrap(msg, 8, &send, &debug);
    rewrap(msg2, 8, &garbage, &debug);

    uint8_t send2[10] = {0x20, 0x33, 0xc8, 0x20, 0xe3, 0x8c, 0x2b, 0xa8, 0x26, 0xa8}; //probably the old reverse message
    uint8_t *wtf = (uint8_t *)malloc(12);
    
    /*
    for(int i = 0; i < 12; i++) {
        //printf("%02x ", send[i]);
        wtf[i] = 0xFF-send[i];
        //printf("%02x\n", wtf[i]);
    }
    */
    

    // set up pattern generator and custom data type
    FDwfDigitalOutEnableSet(device_data->handle, chO, 1);
    FDwfDigitalOutTypeSet(device_data->handle, chO, DwfDigitalOutTypeCustom);
    FDwfDigitalOutRepeatSet(device_data->handle, 1);
    FDwfDigitalOutRunSet(device_data->handle, 0.02085);
    unsigned int pnMin;
    unsigned int pnMax;
    //FDwfDigitalOutRepeatInfo(device_data->handle, &pnMin, &pnMax);
    //printf("Should repeat no more than %u times and no less than %u times\n", pnMax, pnMin);

    // get a divider value for output frquency
    double int_freq = 0;
    if (FDwfDigitalOutInternalClockInfo(device_data->handle, &int_freq) == 0) {
        device.check_error(device_data);
    }

    // get counter value range
    unsigned int counter_limit = 0;
    if (FDwfDigitalOutCounterInfo(device_data->handle, 0, 0, &counter_limit) == 0) {
        device.check_error(device_data);
    }
    
    // calculate the divider for the given signal frequency
    int divider = int(int_freq / freq);
    FDwfDigitalOutDividerSet(device_data->handle, chO, divider);

    // just idle
    //uint8_t idle = 0xAA; // 01010101 when flipped
    //FDwfDigitalOutDataSet(device_data->handle, chO, &idle, 8);
    FDwfDigitalOutIdleSet(device_data->handle, chO, 0xFF);
    FDwfDigitalOutConfigure(device_data->handle, 1);
    /*
    vector<unsigned short> input2 = logic.record(device_data, chI);   
    for(int i = 0; i < input2.size()/10; i++) {
        for(int j = 0; j < 10; j++) {
            printf("%01x", input2.at(i*10+j));
        }
        printf("\n");
    }
    */
    // receive data and (for now) output it for debugging
    uint16_t askye[1024];
    for(int i = 0; i < 1024; i++) {
        askye[i] = -1;
    }
    
    askye[304] = 0; //current askyes for 0
    askye[608] = 0;
    askye[816] = 0;
    askye[416] = 0;
    askye[832] = 0;

    //current askyes for 1
    askye[433] = 1;
    askye[866] = 1;
    askye[945] = 1;

    //current askyes for 2
    askye[434] = 2;
    askye[868] = 2;
    askye[946] = 2;

    //current askyes for 3
    askye[307] = 3;
    askye[614] = 3;
    askye[819] = 3;

    askye[436] = 4; //current askyes for 4
    askye[872] = 4;
    askye[948] = 4;

    //current askyes for 5
    askye[309] = 5;
    askye[618] = 5;
    askye[821] = 5;

    //current askyes for 6
    askye[310] = 6;
    askye[620] = 6;
    askye[822] = 6;

    //current askyes for 7
    askye[439] = 7;
    askye[878] = 7;
    askye[951] = 7;

    //current askyes for 8
    askye[440] = 8;
    askye[880] = 8;
    askye[952] = 8;

    //current askyes for 9
    askye[313] = 9;
    askye[616] = 9;
    askye[825] = 9;

   // set up the instrument
    FDwfDigitalInConfigure(device_data->handle, 0, 1);

    // send message
    //printf("Sending...?\n");
    //for(int i = 0; i < 12; i++) {
        //printf("%02x\n", send[i]);
    //}
    
    uint8_t rec = 0;
    uint16_t temp = 0;

    FDwfDigitalOutDataSet(device_data->handle, chO, send, 96);
    FDwfDigitalOutConfigure(device_data->handle, 1);
    vector<unsigned short> input = logic.record(device_data, chI);

    for(int i = 0; i < input.size()/10; i++) {
        uint16_t test_num = 0;
        for(int j = 0; j < 10; j++) {
            test_num += input.at(i*10+j)*int(pow(2, j));
            //printf("%01x", input.at(i*10+j));
        }
        //printf("\n");
        //printf("test_num: %u\n", test_num);
        if(i == 4 && (test_num == 416 || test_num == 832)) {
            rec = 1;
        }
        if(rec == 1) {
            //printf("Current askye: %u\n", askye[test_num]);
            if (i == 8) {
                printf("Current temperature: %u C\n", temp);
                rec = 0;
                temp = 0;
            }
            else if (askye[test_num] == 65535){
                rec = 0;
                temp = 0;
            }
            else {
                temp += int(pow(10, (7-i)))*askye[test_num];
            }
        }
        
    }

}

// function for logging thermocouple voltage data (timestamp log not implemented yet but will be once this code actually works)
void datalog(string name, int config, double out_freq, double sample_rate, int chI, int chO, int del, string filename) {

    // start the Analog Discovery 2
    // To check the list of configurations, check Device Manager in Waveforms Software - for best digital I/O choose config = 4 or 6
    // To be frank I'm not sure what the 16 x 16384 means just yet - could be tied to the resolution of the signal? That being said for digital signals that's pointless as you're just searching for a high and low input no
    Device::Data *device_data;
    device_data = device.open(name, config); 

    // initialize power supply
    FDwfAnalogIOChannelNodeSet(device_data->handle, 0, 1, 0);
    FDwfAnalogIOChannelNodeSet(device_data->handle, 0, 0, 0);
    delay(5000);
    FDwfAnalogIOReset(device_data->handle);
    FDwfAnalogIOConfigure(device_data->handle);

    // generate 5 volts
    FDwfAnalogIOChannelNodeSet(device_data->handle, 0, 0, 1); // enables positive power supply
    FDwfAnalogIOChannelNodeSet(device_data->handle, 0, 1, 3.3); // set 3.3 V on the positive power supply
    FDwfAnalogIOChannelNodeSet(device_data->handle, 1, 0, 1); // enables negative power supply
    FDwfAnalogIOChannelNodeSet(device_data->handle, 1, 1, 0); // set 0 V on negative power supply
    FDwfAnalogIOEnableSet(device_data->handle, 1); //run the damn thing
 
    // get maximum buffer / minimum buffer sizes, else check for errors 
    int max_buf;  
    if(FDwfDigitalInBufferSizeInfo(device_data->handle, &max_buf) == 0) {
        device.check_error(device_data);
    }

    // start digital logic analyzer and pattern generator
    logic.open(device_data, sample_rate, 120);
    logic.trigger(device_data, 1, chI, 0, 0, 0, 0, 20, 0);
    
    // query temperature controller for data
    write_csv_head(filename, "Thermocouple Voltage (mV)");
    unsigned short data[1];
    while(1) {
        req_data(device_data, chI, chO, out_freq, sample_rate, 0x56, data);
        //printf("Delay\n");
        delay(del);
    }

    // disable analog input
    FDwfAnalogIOChannelNodeSet(device_data->handle, 0, 0, 0);

    // close logic
    logic.close(device_data);
    
    // close the device
    device.close(device_data);
}

