#include <stdio.h>
#include <QtWidgets>
#include <QLabel>
#include <QLayout>
#include <string>
#include <QString>
#include <QPushButton>
#include "display.h"
#include "datalog.h"

using namespace std;
using namespace Qt;

void Log::dtlog() {

    void *device_data;

    // time and tracking related variables
    char *t = (char *)malloc(26*sizeof(char));

    // pressure related variables
    double voltages[10000];
    for(int i = 0; i < 10000; i++) {
        voltages[i] = 0;
    }
    int n = 0;

    // temperature related variables
    int last_temp = 0;

    start_device(nme, cfg, smp, inp, off, apl, &device_data);

    //csv initialize
    string colnames[6] = {"Time", "Count", "Temperature (C)", "Pressure (bar)", "Average Pressure Voltage (mV)", "Analog Pressure Reading (mV)"};
    write_csv_head(fle, colnames, 6);

    for(int i = 60; i < 100000; i--) { // this is done on purpose, changing i changes the preset value
        if (i == 0) {
            printf("Starting Logging. \n");
        }
        if(i < 0) {
            datalog(device_data, frq, smp, inp, out, dly, aIn, asp, off, apl, fle, &time, &temp, &measurement, &count);
            convert_to_pressure(voltages, measurement, &n, &press, &raw);
            if(temp == -1) {
                temp = last_temp;
            }
            else {
                last_temp = temp;
            }
            write_csv(fle, time, temp, press, raw, measurement, count);
        }
    }

    // free time 
    free(t);

    close_device(device_data);
}

void Log::temp_update(int t) {

}

void Log::press_update(int p) {
    
}

int display(int argc, char *argv[], string name, int config, double out_freq, double sample_rate, int chI, int chO, int del, int achI, int asr, double offset, double amp, string filename) {
    QApplication app(argc, argv);
    QWidget window;
    window.resize(1000, 600);
    window.show();
    window.setWindowTitle(QApplication::translate("toplevel", "HPS-5015 Data Logger"));

    //layout declarations
    QGridLayout hori(&window);
    QVBoxLayout *v1 = new QVBoxLayout(&window);

    //add vertical layouts to larger horizontal layout
    hori.addLayout(v1, 0, 0, 3, 1);
    
    //add items to vertical layout
    
    QLabel *temp = new QLabel();
    int t_val = 415;
    temp->setText(QString("Temperature: %1 C").arg(t_val)); 
    temp->setAlignment(AlignLeft);

    QLabel *press = new QLabel();
    int p_val = 285;
    press->setText(QString("Pressure: %1 bar").arg(p_val));
    press->setAlignment(AlignLeft);

    Log *log = new Log(name, config, out_freq, sample_rate, chI, chO, del, achI, asr, offset, amp, filename);

    QPushButton *start = new QPushButton("Start Logging");
    QObject::connect(start, &QPushButton::clicked, log, &Log::dtlog);

    QLabel *graph = new QLabel();
    graph->setText("Graph Goes Here");
    graph->setAlignment(AlignLeft);
    
    v1->addWidget(temp, 0);
    v1->addWidget(press, 1);
    v1->addWidget(start, 2);
    v1->addSpacing(800);
    
    hori.addWidget(graph, 0, 1, 3, 4);

    return app.exec();
}