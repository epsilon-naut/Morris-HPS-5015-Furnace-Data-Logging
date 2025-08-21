#include <stdio.h>
#include <QtWidgets>
#include <QLabel>
#include <QLayout>
#include <string>
#include <QString>
#include <QPushButton>
#include "display.h"

using namespace std;
using namespace Qt;

int display(int argc, char *argv[]) {
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

    QPushButton *start = new QPushButton("Start Logging");

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