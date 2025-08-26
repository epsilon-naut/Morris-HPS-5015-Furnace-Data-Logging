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
#include <QValueAxis>
#include <algorithm>

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
    QValueAxis *x;
    QValueAxis *y;
    int pastvals[100];

    Chart(QString name, QString units) {
        // general chart
        this->legend()->hide();
        this->setTitle(name);

        // axis-specific initialization
        
        x = new QValueAxis(); y = new QValueAxis();
        x->setRange(0, 100); y->setRange(0, 100);
        x->setTickAnchor(0); y->setTickAnchor(0);
        x->setTickInterval(10); y->setTickInterval(10);
        x->setTickType(QValueAxis::TicksDynamic); y->setTickType(QValueAxis::TicksDynamic);
        x->setTitleText("Count"); y->setTitleText(QString("%1 (%2)").arg(name).arg(units));
        

        // series-specific initialization
        series = new QLineSeries();
        series->setPointsVisible();
        
        series->attachAxis(x);
        series->attachAxis(y);
        
        this->addSeries(series);

        //createDefaultAxes();
        this->addAxis(x, Qt::AlignBottom); this->addAxis(y, Qt::AlignLeft);

        for(int i = 0; i < 100; i++) {
            pastvals[i] = 0;
        }
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