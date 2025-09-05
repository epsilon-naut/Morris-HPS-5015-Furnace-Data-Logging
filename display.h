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
#include <QFileDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>

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
    int refresh;
    void *device_data;
    int n;
    double voltages[10000];
    int last_temp;
    string last_time;
    int sec_counter;
    int sec_changed;
    double a;
    double b;

    Log(string name, int config, double out_freq, double sample_rate, int chI, int chO, int del, int achI, int asr, double offset, double amp, string filename, double press_a, double press_b) {
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
        a = press_a;
        b = press_b;
        
        time = (char *)malloc(26 * sizeof(char));
        temp = 0;
        press = 0;
        raw = 0;
        measurement = 0;
        count = 0;
        refresh = 1;
    }

    public slots:
    void init();
    void dtlog();
    void close();
    void refile(QString filename);
    void refresh_change(int rate);
    void change_curve(double coeff, double shift);

    signals:
    void temp_updated(int count, int temp);
    void press_updated(int count, int press);
    void i_want_to_continue();

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

class Chart : public QChart {

    Q_OBJECT

    public:
    QLineSeries *series;
    QValueAxis *x;
    QValueAxis *y;
    int pastvals[1000];
    int cnt;
    int val;
    int ind;
    int ind_y;
    int min_val;
    int max_val;
    double yb;
    double yt;
    double xb;
    double xt;

    Chart(QString name, QString units) {
        // general chart
        this->legend()->hide();
        this->setTitle(name);

        // axis-specific initialization
        ind = 0; ind_y = 0;
        xb = 0; xt = 50; yb = 0; yt = 100;
        x = new QValueAxis(); y = new QValueAxis();
        x->setRange(0, 50); y->setRange(0, 100);
        x->setTickType(QValueAxis::TicksFixed); y->setTickType(QValueAxis::TicksFixed);
        x->setTickCount(11); y->setTickCount(7);
        x->setTitleText("Count"); y->setTitleText(QString("%1 (%2)").arg(name).arg(units));
        

        // series-specific initialization
        cnt = 1;
        val = 0;
        min_val = 1500;
        max_val = -10000;
        series = new QLineSeries();
        series->setPointsVisible();
        
        series->attachAxis(x);
        series->attachAxis(y);
        
        this->addSeries(series);

        //createDefaultAxes();
        this->addAxis(x, Qt::AlignBottom); this->addAxis(y, Qt::AlignLeft);

        for(int i = 0; i < 1000; i++) {
            pastvals[i] = 0;
        }
    }

    public slots:
        void updatevals(int count, int value);
        void updateaxes(int index, int indey);
        void updatecustomy(int ybot, int ytop);
        void updatecustomx(int xbot, int xtop);

    signals:
        void updated(int ind_x, int indy);
        void reset_y(double bot, double top, int aut);
        void reset_x(double bot, double top, int aut);
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

class Save : public QFileDialog {

    Q_OBJECT
    
    public:

    QString filename;

    Save() {
        this->setDirectory("C:/Users/Jeff/Documents/Research/HPS5015/logs");
        this->setFileMode(QFileDialog::AnyFile);
        this->setNameFilter("CSV files (*.csv)");
        this->setViewMode(QFileDialog::Detail);
    }

    public slots:
    void save();

    signals:
    void relabel(QString filename);
};

class fLabel : public QLabel {

    Q_OBJECT
    
    public: 
    QString str;

    fLabel(Alignment align) {
        str = "";
        this->setText(QString("File Name: %1").arg(str)); 
        this->setAlignment(align);
    }

    public slots:
        void update(QString filename);

    private:
        int val;
        string nm;
        string un;

};

class viewSelect : public QComboBox {

    Q_OBJECT

    public:

    viewSelect() {
        this->addItem("All Data");
        this->addItem("Last 50 Values");
        this->addItem("Last 100 Values");
        this->addItem("Last 1000 Values");
        this->addItem("Custom X Axis");
    }

    public slots:
        void changed();

    signals:
        void index(int i, int j);

};

class ySelect : public QComboBox {

    Q_OBJECT

    public:

    ySelect() {
        this->addItem("Auto");
        this->addItem("Custom Y Axis");
    }

    public slots:
        void changed();

    signals:
        void indey(int i, int j);
};

class refreshSelect : public QComboBox {

    Q_OBJECT

    public:

    refreshSelect() {
        this->addItem("All");
        this->addItem("Per Second");
        this->addItem("Per Minute");
    }

    public slots:
        void changed();

    signals:
        void refresh_rate(int rate);

};

class botEdit : public QLineEdit {

    Q_OBJECT

    public:
    int modifiable;

    botEdit(QString text) {
        this->setText(text);
        modifiable = 0;
    }

    public slots:
    void updated();
    void reset(double bot, double top, int aut);

    signals:
    void update(int val, int ex);
};

class topEdit : public QLineEdit {

    Q_OBJECT

    public:
    int modifiable;

    topEdit(QString text) {
        this->setText(text);
        modifiable = 0;
    }

    public slots:
    void updated();
    void reset(double bot, double top, int aut);

    signals:
    void update(int ex, int val);
};

class chartCheck : public QObject {

    Q_OBJECT

    public:

    Chart *c1;
    Chart *c2;
    QCheckBox *tc;
    QCheckBox *pc;
    viewSelect *xsel;
    ySelect *ysel;
    botEdit *yb;
    topEdit *yt;
    botEdit *xb;
    topEdit *xt;

    chartCheck(botEdit *ybedit, topEdit *ytedit, botEdit *xbedit, topEdit *xtedit, viewSelect *xselect, ySelect *yselect, Chart *chart, Chart *chart2) {
        xsel = xselect;
        ysel = yselect;
        yb = ybedit;
        yt = ytedit;
        xb = xbedit;
        xt = xtedit;
        c1 = chart;
        c2 = chart2;
        tc = new QCheckBox("Temperature");
        pc = new QCheckBox("Pressure");
        state = 0;
        connect(tc, &QCheckBox::checkStateChanged, this, &chartCheck::checked);
        connect(pc, &QCheckBox::checkStateChanged, this, &chartCheck::checked);
    }

    public slots:
    void checked();

    signals:
    void update(int ind, int ind_y);

    private:
    int state;
};

class indyLabel : public QLabel {

    Q_OBJECT

    public:

    int indy;

    indyLabel() {
        indy = 5;
        setText(QString("Ind_y: %1").arg(indy));
    }

    public slots:
        void update(int ex, int ind_y);

};

class pressChange : public QHBoxLayout {

    Q_OBJECT

    public:
    QLabel *colabel;
    QLineEdit *coedit;
    QLabel *shlabel;
    QLineEdit *shedit;
    QHBoxLayout *co;
    QHBoxLayout *sh;
    double coeff;
    double shift;
    string fle;

    pressChange(double a, double b, string filename) {
        coeff = a;
        shift = b;
        fle = filename;
        colabel = new QLabel("Pressure Coefficient: ");
        coedit = new QLineEdit(QString("%1").arg(coeff));        
        shlabel = new QLabel("Shift: ");
        shedit = new QLineEdit(QString("%1").arg(shift));   
        co = new QHBoxLayout();
        sh = new QHBoxLayout();
        co->addWidget(colabel, 0);
        co->addWidget(coedit, 1);
        sh->addWidget(shlabel, 0);
        sh->addWidget(shedit, 1);
        this->addLayout(co, 0);
        this->addLayout(sh, 1);
        connect(coedit, &QLineEdit::returnPressed, this, &pressChange::edit_coeff);
        connect(shedit, &QLineEdit::returnPressed, this, &pressChange::edit_shift);
    }

    public slots:
    void edit_coeff();
    void edit_shift();
    
    signals:
    void edited(double coe, double shi);
};

class disp : public QObject {
    
    Q_OBJECT
    QThread log_thread;

    public:
    
    disp(Log *lg) {
        lg->moveToThread(&log_thread);
        connect(&log_thread, &QThread::finished, lg, &QObject::deleteLater);
        connect(this, &disp::start_log, lg, &Log::init);
        connect(lg, &Log::i_want_to_continue, this, &disp::continue_granted);
        connect(this, &disp::continue_log, lg, &Log::dtlog);
        connect(&log_thread, &QThread::finished, lg, &Log::close);
        
    }

    ~disp() {
        log_thread.quit();
        log_thread.wait();
    }

    public slots:
        void startButtonPressed();
        void continue_granted();

    signals:
        void start_log();
        void continue_log();
};

int display(int argc, char *argv[], string name, int config, double out_freq, double sample_rate, int chI, int chO, int del, int achI, int asr, double offset, double amp, string filename);

#endif