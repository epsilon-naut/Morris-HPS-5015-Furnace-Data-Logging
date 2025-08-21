#ifndef DISPLAY_H
#define DISPLAY_H

#include <QtWidgets>
#include <QLabel>
#include <QLayout>
#include <string>
#include <QString>
#include <QPushButton>

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
    void temp_update(int t);
    void press_update(int p);

    signals:
    void temp_updated(int temp);
    void press_updated(int press);

    private: 
    char *time;
    int temp;
    int press;
    double raw;
    double measurement;
    int count;

};

int display(int argc, char *argv[], string name, int config, double out_freq, double sample_rate, int chI, int chO, int del, int achI, int asr, double offset, double amp, string filename);

#endif