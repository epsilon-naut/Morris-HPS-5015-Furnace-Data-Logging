#ifndef DISPLAY_H
#define DISPLAY_H

#include <QtWidgets>
#include <QLabel>
#include <QLayout>
#include <string>
#include <QString>
#include <QPushButton>
#include <QThread>
#include <QChart>
#include <QChartView>
#include <QLineSeries>

using namespace std;
using namespace Qt;

class Log : public QObject {

    Q_OBJECT

    public:
    string nme;
    int cfg;
    double frq;
    double smp;
    int inp;
    int out;
    int dly;
    int aIn;
    int asp;
    double off;
    double apl;
    string fle;

    Log(string name, int config, double out_freq, double sample_rate, int chI, int chO, int del, int achI, int asr, double offset, double amp, string filename) {
        nme = name;
        cfg = config;
        frq = out_freq;
        smp = sample_rate;
        inp = chI;
        out = chO;
        dly = del;
        aIn = achI;
        asp = asr;
        off = offset;
        apl = amp;
        fle = filename;
        
        time = (char *)malloc(26 * sizeof(char));
        temp = 0;
        press = 0;
        raw = 0;
        measurement = 0;
        count = 0;
    }

    public slots:
    void dtlog();

    signals:
    void temp_updated(int count, int temp);
    void press_updated(int count, int press);

    private: 
    char *time;
    int temp;
    int press;
    double raw;
    double measurement;
    int count;

};

class Label : public QLabel {

    Q_OBJECT
    
    public: 

    Label(Alignment align, string name, string unit) {
        val = 0;
        nm = name;
        un = unit;
        this->setText(QString("%2: %1 %3").arg(val).arg(QString::fromStdString(nm)).arg(QString::fromStdString(un))); 
        this->setAlignment(align);
    }

    public slots:
        void update(int count, int value);

    private:
        int val;
        string nm;
        string un;

};

class disp : public QObject {
    
    Q_OBJECT
    QThread log_thread;

    public:
    
    disp(Log *lg) {
        lg->moveToThread(&log_thread);
        connect(&log_thread, &QThread::finished, lg, &QObject::deleteLater);
        connect(this, &disp::startlog, lg, &Log::dtlog);
        log_thread.start();
    }

    ~disp() {
        log_thread.quit();
        log_thread.wait();
    }

    public slots:
        void startButtonPressed();
    signals:
        void startlog();
};

class Chart : public QChart {

    Q_OBJECT

    public:
    QLineSeries *series;

    Chart(QString name) {
        series = new QLineSeries();
        this->legend()->hide();
        this->addSeries(series);
        this->createDefaultAxes();
        this->setTitle(name);

    }

    public slots:
        void updatevals(int count, int value);

    signals:
        void updated();
};

class Scene : public QObject {

    Q_OBJECT

    public:
    QGraphicsScene *scene;
    QGraphicsView *view;

    Scene(QGraphicsItem *item) {
        scene = new QGraphicsScene();
        scene->addItem(item);
        view = new QGraphicsView(scene);
    }

    public slots:
        void redraw();

    signals:
        void updated();
};

int display(int argc, char *argv[], string name, int config, double out_freq, double sample_rate, int chI, int chO, int del, int achI, int asr, double offset, double amp, string filename);

#endif