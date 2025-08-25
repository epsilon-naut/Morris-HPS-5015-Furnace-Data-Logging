#include <stdio.h>
#include <QtWidgets>
#include <QLabel>
#include <QLayout>
#include <string>
#include <QString>
#include <QPushButton>
#include <QThread>
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
                emit temp_updated(count, temp);
            }
            emit press_updated(count, press);
            write_csv(fle, time, temp, press, raw, measurement, count);
        }
    }

    // free time 
    free(t);

    close_device(device_data);
}

void disp::startButtonPressed() {
    emit startlog();
}

void Label::update(int count, int value) {
    val = value;
    this->setText(QString("%2: %1 %3").arg(val).arg(QString::fromStdString(nm)).arg(QString::fromStdString(un)));
    this->repaint();
}

void Chart::updatevals(int count, int value) {
    this->removeSeries(series);
    series->append(count, value);
    this->addSeries(series);
    this->createDefaultAxes();
    this->update();
    emit updated();
}

void Scene::redraw() {

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
    
    Label *temp = new Label(AlignLeft, "Temperature", "C"); 
    Label *press = new Label(AlignLeft, "Pressure", "bar"); 

    Log *log = new Log(name, config, out_freq, sample_rate, chI, chO, del, achI, asr, offset, amp, filename);
    disp *dis = new disp(log);

    QPushButton *start = new QPushButton("Start Logging");
    
    Chart *chart = new Chart(QString("Temperature"));
    Chart *chart2 = new Chart(QString("Pressure"));
    
    QChartView *view = new QChartView(chart);
    QChartView *view2 = new QChartView(chart2);
    

    v1->addWidget(temp, 0);
    v1->addWidget(press, 1);
    v1->addWidget(start, 2);
    v1->addSpacing(500);
    

    hori.addWidget(view, 0, 1, 2, 5);
    hori.addWidget(view2, 2, 1, 2, 5);

    QObject::connect(start, &QPushButton::clicked, dis, &disp::startButtonPressed);
    QObject::connect(log, &Log::temp_updated, temp, &Label::update);
    QObject::connect(log, &Log::press_updated, press, &Label::update);
    QObject::connect(log, &Log::temp_updated, chart, &Chart::updatevals);
    QObject::connect(log, &Log::press_updated, chart2, &Chart::updatevals);
    //QObject::connect(chart, &Chart::updated, scene, &Scene::redraw);

    view->show();
    return app.exec();
}