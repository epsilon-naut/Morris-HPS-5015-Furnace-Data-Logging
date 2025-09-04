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
        emit temp_updated(sec_counter, temp);
        emit press_updated(sec_counter, press);
    }
    if((refresh == 360) && (sec_counter-sec_changed) && (sec_changed % 60 == 0)) {
        emit temp_updated(static_cast<int>(sec_changed/60), temp);
        emit press_updated(static_cast<int>(sec_changed/60), press);
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

void Chart::updateaxes(int index, int indey) {
    if(index > -1) {
        ind = index;
    }
    if(indey > -1) {
        ind_y = indey;
    }
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
        if(ind_y < 1) {
            yb = static_cast<double>(min_val)-30;
            yt = static_cast<double>(max_val)+30;
        }
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
            x->setRange(0, 50);
            pastvals[950+cnt-1] = val;
        }
        if(ind_y < 1) {
            yb = static_cast<double>(min(*min_element(pastvals+950, pastvals+999),*max_element(pastvals+950, pastvals+999)-30));
            yt = static_cast<double>(*max_element(pastvals+950, pastvals+999)+30);
        }
    }
    else if (ind == 2) {
        if(cnt > 100) {
            x->setRange(cnt-100, cnt+10);
            for(int i = 901; i < 1000; i++) {
                pastvals[i-1] = pastvals[i];
            }
            pastvals[999] = val;
        } 
        else {
            x->setRange(0, 100);
            pastvals[900+cnt-1] = val;
        }
        if(ind_y < 1) {
            yb = static_cast<double>(min(*min_element(pastvals+900, pastvals+999),*max_element(pastvals+900, pastvals+999)-30));
            yt = static_cast<double>(*max_element(pastvals+900, pastvals+999)+30);
        }
    }
    else if (ind == 3) {
        if(cnt > 1000) {
            x->setRange(cnt-1000, cnt+10);
            for(int i = 1; i < 1000; i++) {
                pastvals[i-1] = pastvals[i];
            }
            pastvals[999] = val;
        } 
        else {
            x->setRange(0, 1000);
            pastvals[cnt-1] = val;
        }
        if(ind_y < 1) {
            yb = static_cast<double>(min(*min_element(pastvals, pastvals+999),*max_element(pastvals, pastvals+999)-30));
            yt = static_cast<double>(*max_element(pastvals, pastvals+999)+30);
        }
    }
    else {
        if(cnt > 1000) {
            for(int i = 1; i < 1000; i++) {
                pastvals[i-1] = pastvals[i];
            }
            pastvals[999] = val;
        }
        else {
            pastvals[cnt-1] = val;
        }
        x->setRange(xb, xt);
    }
    y->setRange(yb, yt);
    
    series->attachAxis(x);
    series->attachAxis(y);
    this->update();
    emit updated(ind, ind_y);
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

    updateaxes(-1, -1);
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
    emit index(this->currentIndex(), -1);
}

void ySelect::changed() {
    emit indey(-1, this->currentIndex());
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

void Chart::updatecustomy(int ybot, int ytop) {
    if((ybot > 0) && (ybot < yt)) {
        yb = static_cast<double>(ybot);
    }
    if((ytop > yb)) {
        yt = static_cast<double>(ytop);
    }
    updateaxes(-1, -1);
}

void Chart::updatecustomx(int xbot, int xtop) {
    xb = static_cast<double>(xbot);
    xt = static_cast<double>(xtop);
    updateaxes(-1, -1);
}

void botEdit::updated(QString raw) {
    bool ok;
    int val = raw.toInt(&ok);
    if(ok) {
        emit update(val, -1);
    }
}

void topEdit::updated(QString raw) {
    bool ok;
    int val = raw.toInt(&ok);
    if(ok) {
        emit update(-1, val);
    }
}

void chartCheck::checked() {
    state = 0;
    
    if(tc->checkState() == Qt::Checked) {
        state += 1;
    }
    if(pc->checkState() == Qt::Checked) {
        state += 2;
    }
    if(state == 1) {
        this->connect(xsel, &viewSelect::index, c1, &Chart::updateaxes);
        this->connect(ysel, &ySelect::indey, c1, &Chart::updateaxes); 
        this->connect(yb, &botEdit::update, c1, &Chart::updatecustomy);
        this->connect(yt, &topEdit::update, c1, &Chart::updatecustomy); 
        this->disconnect(xsel, &viewSelect::index, c2, &Chart::updateaxes);
        this->disconnect(ysel, &ySelect::indey, c2, &Chart::updateaxes);
        this->disconnect(yb, &botEdit::update, c2, &Chart::updatecustomy);
        this->disconnect(yt, &topEdit::update, c2, &Chart::updatecustomy);
    }
    else if(state == 2) {
        this->connect(xsel, &viewSelect::index, c2, &Chart::updateaxes);
        this->connect(ysel, &ySelect::indey, c2, &Chart::updateaxes);
        this->connect(yb, &botEdit::update, c2, &Chart::updatecustomy);
        this->connect(yt, &topEdit::update, c2, &Chart::updatecustomy);
        this->disconnect(xsel, &viewSelect::index, c1, &Chart::updateaxes);
        this->disconnect(ysel, &ySelect::indey, c1, &Chart::updateaxes);
        this->disconnect(yb, &botEdit::update, c1, &Chart::updatecustomy);
        this->disconnect(yt, &topEdit::update, c1, &Chart::updatecustomy); 
    }
    else if(state == 3){
        this->connect(xsel, &viewSelect::index, c1, &Chart::updateaxes);
        this->connect(ysel, &ySelect::indey, c1, &Chart::updateaxes);
        this->connect(yb, &botEdit::update, c1, &Chart::updatecustomy);
        this->connect(yt, &topEdit::update, c1, &Chart::updatecustomy);
        this->connect(xsel, &viewSelect::index, c2, &Chart::updateaxes);
        this->connect(ysel, &ySelect::indey, c2, &Chart::updateaxes);
        this->connect(yb, &botEdit::update, c2, &Chart::updatecustomy);
        this->connect(yt, &topEdit::update, c2, &Chart::updatecustomy);
    }
    else {
        this->disconnect(xsel, &viewSelect::index, c1, &Chart::updateaxes);
        this->disconnect(ysel, &ySelect::indey, c1, &Chart::updateaxes);
        this->disconnect(xsel, &viewSelect::index, c2, &Chart::updateaxes);
        this->disconnect(ysel, &ySelect::indey, c2, &Chart::updateaxes);
        this->disconnect(yb, &botEdit::update, c1, &Chart::updatecustomy);
        this->disconnect(yt, &topEdit::update, c1, &Chart::updatecustomy);
        this->disconnect(yb, &botEdit::update, c2, &Chart::updatecustomy);
        this->disconnect(yt, &topEdit::update, c2, &Chart::updatecustomy);
    }
}

void indyLabel::update(int ex, int ind_y) {
    indy = ind_y;
    setText(QString("Ind_y: %1").arg(indy));
}

int display(int argc, char *argv[], string name, int config, double out_freq, double sample_rate, int chI, int chO, int del, int achI, int asr, double offset, double amp, string filename) {
    QApplication app(argc, argv);
    QWidget window;
    window.resize(1500, 750);
    window.show();
    window.setWindowTitle(QApplication::translate("toplevel", "HPS-5015 Data Logger"));

    //layout declarations
    QGridLayout hori(&window);
    QVBoxLayout *v1 = new QVBoxLayout();
    QHBoxLayout *xlayout = new QHBoxLayout();
    QHBoxLayout *ylayout = new QHBoxLayout();
    QHBoxLayout *refresh = new QHBoxLayout();
    QHBoxLayout *yedit = new QHBoxLayout();
    QHBoxLayout *ybot = new QHBoxLayout();
    QHBoxLayout *ytop = new QHBoxLayout();
    QHBoxLayout *check = new QHBoxLayout();

    //add vertical layouts to larger horizontal layout
    hori.addLayout(v1, 0, 0, 3, 1);
    
    //add items to vertical layout
    
    Label *temp = new Label(AlignLeft, "Temperature", "C"); 
    Label *press = new Label(AlignLeft, "Pressure", "bar"); 
    fLabel *flab = new fLabel(AlignLeft);

    Save *save = new Save();

    Log *log = new Log(name, config, out_freq, sample_rate, chI, chO, del, achI, asr, offset, amp, return_filename(save->filename));

    QPushButton *start = new QPushButton("Start Logging");

    QPushButton *savebt = new QPushButton("Set Save File");
    
    Chart *chart = new Chart(QString("Temperature"), QString("C"));
    Chart *chart2 = new Chart(QString("Pressure"), QString("bar"));
    
    QChartView *view = new QChartView(chart);
    QChartView *view2 = new QChartView(chart2);

    QLabel *xlabel = new QLabel("X-axis settings: ");
    viewSelect *xsel = new viewSelect();

    QLabel *ylabel = new QLabel("Y-axis settings: ");
    ySelect *ysel = new ySelect();

    QLabel *reflabel = new QLabel("Refresh Rate: "); 
    refreshSelect *ref = new refreshSelect();

    QLabel *yblabel = new QLabel("Y-axis bottom: ");
    botEdit *ybedit = new botEdit("Auto");
    QLabel *ytlabel = new QLabel("Top: ");
    topEdit *ytedit = new topEdit("Auto");

    QLabel *xblabel = new QLabel("X-axis bottom: ");
    botEdit *xbedit = new botEdit("Auto");
    QLabel *xtlabel = new QLabel("Top: ");
    topEdit *xtedit = new topEdit("Auto");

    QLabel *checklab = new QLabel("Apply To:");
    chartCheck *ccheck = new chartCheck(ybedit, ytedit, xbedit, xtedit, xsel, ysel, chart, chart2);

    indyLabel *funny = new indyLabel();

    disp *dis = new disp(log);
    dis->setParent(&window);

    v1->addWidget(temp, 0);
    v1->addWidget(press, 1);
    v1->addWidget(savebt, 2);
    v1->addLayout(refresh, 3);
    v1->addWidget(start, 4);
    v1->addWidget(flab, 5);
    v1->addLayout(xlayout, 6);
    v1->addLayout(ylayout, 7);
    v1->addLayout(check, 8);
    v1->addLayout(yedit, 9);
    v1->addSpacing(400);
    
    hori.addWidget(view, 0, 1, 2, 5);
    hori.addWidget(view2, 2, 1, 2, 5);

    xlayout->addWidget(xlabel, 0);
    xlayout->addWidget(xsel, 1);

    ylayout->addWidget(ylabel, 0);
    ylayout->addWidget(ysel, 1);

    refresh->addWidget(reflabel, 0);
    refresh->addWidget(ref, 1);

    check->addWidget(checklab, 0);
    check->addWidget(ccheck->tc, 1);
    check->addWidget(ccheck->pc, 2);

    yedit->addLayout(ybot, 0);
    yedit->addLayout(ytop, 1);
    
    ybot->addWidget(yblabel, 0);
    ybot->addWidget(ybedit, 1);
    
    ytop->addWidget(ytlabel, 0);
    ytop->addWidget(ytedit, 1);

    QObject::connect(start, &QPushButton::clicked, dis, &disp::startButtonPressed);
    QObject::connect(log, &Log::temp_updated, temp, &Label::update);
    QObject::connect(log, &Log::press_updated, press, &Label::update);
    QObject::connect(log, &Log::temp_updated, chart, &Chart::updatevals);
    QObject::connect(log, &Log::press_updated, chart2, &Chart::updatevals);
    QObject::connect(savebt, &QPushButton::clicked, save, &Save::save);
    QObject::connect(save, &Save::relabel, flab, &fLabel::update);
    QObject::connect(save, &Save::relabel, log, &Log::refile);
    QObject::connect(xsel, &QComboBox::currentIndexChanged, xsel, &viewSelect::changed);
    QObject::connect(ysel, &QComboBox::currentIndexChanged, ysel, &ySelect::changed);
    //QObject::connect(chart, &Chart::updated, funny, &indyLabel::update);
    QObject::connect(ybedit, &QLineEdit::textChanged, ybedit, &botEdit::updated);
    QObject::connect(ytedit, &QLineEdit::textChanged, ytedit, &topEdit::updated);
    QObject::connect(ref, &QComboBox::currentIndexChanged, ref, &refreshSelect::changed);
    QObject::connect(ref, &refreshSelect::refresh_rate, log, &Log::refresh_change);
    //QObject::connect(ybedit, &botEdit::update, chart, &Chart::updatecustomy);
    //QObject::connect(ytedit, &topEdit::update, chart, &Chart::updatecustomy); 
    //QObject::connect(chart, &Chart::updated, scene, &Scene::redraw);

    view->show();
    return app.exec();
}