#include <stdio.h>
#include "display.h"
#include "datalog.h"

using namespace std;
using namespace Qt;

void Log::init() {
    start_device(nme, cfg, smp, inp, off, apl, &device_data);

    // time related variables
    last_time = "";
    sec_counter = 0;
    sec_changed = 0;

    // pressure related variables
    for(int i = 0; i < 10000; i++) {
        voltages[i] = 0;
    }
    n = 0;

    // temperature related variables
    last_temp = 0;

    //csv initialize
    string colnames[6] = {"Time", "Count", "Temperature (C)", "Pressure (bar)", "Average Pressure Voltage (mV)", "Analog Pressure Reading (mV)"};
    write_csv_head(fle, colnames, 6);

    emit i_want_to_continue();
}

void Log::dtlog() {

    datalog(device_data, frq, smp, inp, out, dly, aIn, asp, off, apl, fle, &time, &temp, &measurement, &count);
    convert_to_pressure(voltages, measurement, &n, &press, &raw);
    if(temp == -1) {
        temp = last_temp;
    }
    else {
        last_temp = temp;
    }
    if(string(time, 26) != last_time) {
        last_time = string(time, 26);
        sec_counter++;
    }
    if(refresh == 1) {
        emit temp_updated(count, temp);
        emit press_updated(count, press);
    }
    if((refresh == 6) && (sec_counter-sec_changed)) {
        emit temp_updated(count, temp);
        emit press_updated(count, press);
    }
    if((refresh == 360) && (sec_counter-sec_changed) && (sec_changed % 60 == 0)) {
        emit temp_updated(count, temp);
        emit press_updated(count, press);
    }
    if(sec_counter != sec_changed) {
        sec_changed++;
    }
    write_csv(fle, time, temp, press, raw, measurement, count);

    emit i_want_to_continue();
}

void Log::close() {
    close_device(device_data);
}

void disp::startButtonPressed() {
    log_thread.start();
    emit start_log();
}

void disp::continue_granted() {
    emit continue_log();
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
            for(int i = 951; i < 1000; i++) {
                pastvals[i-1] = pastvals[i];
            }
            pastvals[999] = val;
        } 
        else {
            pastvals[950+cnt-1] = val;
        }
        y->setRange(static_cast<double>(min_val)-30, static_cast<double>(max_val)+30);
    }    
    else if (ind == 1) {
        if(cnt > 50) {
            x->setRange(cnt-50, cnt+10);
            for(int i = 951; i < 1000; i++) {
                pastvals[i-1] = pastvals[i];
            }
            pastvals[999] = val;
        } 
        else {
            pastvals[950+cnt-1] = val;
        }
        y->setRange(static_cast<double>(min(*min_element(pastvals+950, pastvals+999),*max_element(pastvals+950, pastvals+999)-30)), static_cast<double>(*max_element(pastvals+950, pastvals+999)+30));
    }
    else if (ind == 2) {
        if(cnt > 100) {
            x->setRange(cnt-90, cnt+10);
            for(int i = 901; i < 1000; i++) {
                pastvals[i-1] = pastvals[i];
            }
            pastvals[999] = val;
        } 
        else {
            x->setRange(0, 100);
            pastvals[900+cnt-1] = val;
        }
        y->setRange(static_cast<double>(min(*min_element(pastvals+900, pastvals+999),*max_element(pastvals+900, pastvals+999)-30)), static_cast<double>(*max_element(pastvals+900, pastvals+999)+30));
    }
    else if (ind == 3) {
        if(cnt > 1000) {
            x->setRange(cnt-990, cnt+10);
            for(int i = 1; i < 1000; i++) {
                pastvals[i-1] = pastvals[i];
            }
            pastvals[999] = val;
        } 
        else {
            x->setRange(0, 1000);
            pastvals[cnt-1] = val;
        }
        y->setRange(static_cast<double>(min(*min_element(pastvals, pastvals+999),*max_element(pastvals, pastvals+999)-30)), static_cast<double>(*max_element(pastvals, pastvals+999)+30));
    }
    
    series->attachAxis(x);
    series->attachAxis(y);
    this->update();
    emit updated();
}

void Chart::updatevals(int count, int value) {
    cnt = count;
    val = value;
    if(val > max_val) {
        max_val = val;
    }
    if(val < min_val) {
        min_val = val;
    }
    
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

void Log::refresh_change(int rate) {
    refresh = rate;
}

void refreshSelect::changed() {
    if(this->currentIndex() == 0) {
        emit refresh_rate(1);
    }
    else if(this->currentIndex() == 1) {
        emit refresh_rate(6);
    }
    else {
        emit refresh_rate(360);
    }
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
    QHBoxLayout *refresh = new QHBoxLayout(&window);

    //add vertical layouts to larger horizontal layout
    hori.addLayout(v1, 0, 0, 3, 1);
    
    //add items to vertical layout
    
    Label *temp = new Label(AlignLeft, "Temperature", "C"); 
    Label *press = new Label(AlignLeft, "Pressure", "bar"); 
    fLabel *flab = new fLabel(AlignLeft);

    Save *save = new Save();

    Log *log = new Log(name, config, out_freq, sample_rate, chI, chO, del, achI, asr, offset, amp, return_filename(save->filename));
    disp *dis = new disp(log);
    dis->setParent(&window);

    QPushButton *start = new QPushButton("Start Logging");

    QPushButton *savebt = new QPushButton("Set Save File");
    
    Chart *chart = new Chart(QString("Temperature"), QString("C"));
    Chart *chart2 = new Chart(QString("Pressure"), QString("bar"));
    
    QChartView *view = new QChartView(chart);
    QChartView *view2 = new QChartView(chart2);

    viewSelect *select = new viewSelect();

    QLabel *reflabel = new QLabel("Refresh rate: "); 
    refreshSelect *ref = new refreshSelect();

    v1->addWidget(temp, 0);
    v1->addWidget(press, 1);
    v1->addWidget(savebt, 2);
    v1->addLayout(refresh, 3);
    v1->addWidget(start, 4);
    v1->addWidget(flab, 5);
    v1->addWidget(select, 6);
    v1->addSpacing(400);
    
    hori.addWidget(view, 0, 1, 2, 5);
    hori.addWidget(view2, 2, 1, 2, 5);

    refresh->addWidget(reflabel, 0);
    refresh->addWidget(ref, 1);

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
    QObject::connect(ref, &QComboBox::currentIndexChanged, ref, &refreshSelect::changed);
    QObject::connect(ref, &refreshSelect::refresh_rate, log, &Log::refresh_change);
    //QObject::connect(chart, &Chart::updated, scene, &Scene::redraw);

    view->show();
    return app.exec();
}