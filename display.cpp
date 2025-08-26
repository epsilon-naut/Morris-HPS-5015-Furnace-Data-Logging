#include <stdio.h>
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
            if(count % 6 == 0) {      
                emit temp_updated(count, temp);
                emit press_updated(count, press);
            }
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

void Chart::updateaxes(int index) {
    ind = index;
    if(ind == 0) {
        if(cnt > 50) {
        x->setRange(0, cnt+10); 
        x->setTickInterval((int)((cnt+10)/10));
        for(int i = 1; i < 50; i++) {
            pastvals[i-1] = pastvals[i];
        }
            pastvals[49] = val;
        } 
        else {
            pastvals[cnt-1] = val;
        }
    }    
    else {
        if(cnt > 50) {
        x->setRange(cnt-40, cnt+10);
        x->setTickAnchor(cnt-40); 
        x->setTickInterval(5);
        for(int i = 1; i < 50; i++) {
            pastvals[i-1] = pastvals[i];
        }
            pastvals[49] = val;
        } 
        else {
            pastvals[cnt-1] = val;
        }
    }
    y->setRange(static_cast<double>(min(*min_element(pastvals, pastvals+49),*max_element(pastvals, pastvals+49)-50)), static_cast<double>(*max_element(pastvals, pastvals+49)+50));
    series->attachAxis(x);
    series->attachAxis(y);
    this->update();
    emit updated();
}

void Chart::updatevals(int count, int value) {
    cnt = count;
    val = value;
    
    series->append(cnt, val);

    updateaxes(ind);
}

void Scene::redraw() {

}

void Save::save() {
    if(this->exec() == QDialog::Accepted) {
        filename = (this->selectedFiles())[0];
    }   
    emit relabel(filename);
}

string return_filename(QString filename) {
    string f = filename.toUtf8().toStdString();
    string end = "";
    int yes = 0;
    for(int i = 0; i < f.length(); i++) {
        if (f.at(i) == '\\') {
            f.at(i) = '/';
        }
        if (i >= f.length()-4) {
            end += f.at(i);
        }
    }
    if(end != ".csv") {
        if(f == "") {

        }
        else if (f.at(f.length()-1) == '/') {

        }
        else {
            f += ".csv";
        }
    }
    return f;
}

void fLabel::update(QString filename) {
    this->setText(QString("File Name: %1").arg(QString::fromStdString(return_filename(filename)))); 
}

void Log::refile(QString filename) {
    fle = return_filename(filename);
}

void viewSelect::changed() {
    emit index(this->currentIndex());
}

int display(int argc, char *argv[], string name, int config, double out_freq, double sample_rate, int chI, int chO, int del, int achI, int asr, double offset, double amp, string filename) {
    QApplication app(argc, argv);
    QWidget window;
    window.resize(1500, 750);
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
    fLabel *flab = new fLabel(AlignLeft);

    Save *save = new Save();

    Log *log = new Log(name, config, out_freq, sample_rate, chI, chO, del, achI, asr, offset, amp, return_filename(save->filename));
    disp *dis = new disp(log);

    QPushButton *start = new QPushButton("Start Logging");

    QPushButton *savebt = new QPushButton("Set Save File");
    
    Chart *chart = new Chart(QString("Temperature"), QString("C"));
    Chart *chart2 = new Chart(QString("Pressure"), QString("bar"));
    
    QChartView *view = new QChartView(chart);
    QChartView *view2 = new QChartView(chart2);

    viewSelect *select = new viewSelect();

    v1->addWidget(temp, 0);
    v1->addWidget(press, 1);
    v1->addWidget(savebt, 2);
    v1->addWidget(start, 3);
    v1->addWidget(flab, 4);
    v1->addWidget(select, 5);
    v1->addSpacing(500);
    

    hori.addWidget(view, 0, 1, 2, 5);
    hori.addWidget(view2, 2, 1, 2, 5);

    QObject::connect(start, &QPushButton::clicked, dis, &disp::startButtonPressed);
    QObject::connect(log, &Log::temp_updated, temp, &Label::update);
    QObject::connect(log, &Log::press_updated, press, &Label::update);
    QObject::connect(log, &Log::temp_updated, chart, &Chart::updatevals);
    QObject::connect(log, &Log::press_updated, chart2, &Chart::updatevals);
    QObject::connect(savebt, &QPushButton::clicked, save, &Save::save);
    QObject::connect(save, &Save::relabel, flab, &fLabel::update);
    QObject::connect(save, &Save::relabel, log, &Log::refile);
    QObject::connect(select, &QComboBox::currentIndexChanged, select, &viewSelect::changed);
    QObject::connect(select, &viewSelect::index, chart, &Chart::updateaxes);
    QObject::connect(select, &viewSelect::index, chart2, &Chart::updateaxes);
    //QObject::connect(chart, &Chart::updated, scene, &Scene::redraw);

    view->show();
    return app.exec();
}