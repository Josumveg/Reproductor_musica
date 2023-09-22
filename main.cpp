#include "mainwindow.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
