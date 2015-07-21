#include "mainwindow.h"
#include <msuproj.h>
#include <QApplication>
#include <QTranslator>

MSUMR::MSUProj msuProjObj;

int main(int argc, char *argv[])
{
    GDALAllRegister();

    QApplication MSUProjQt(argc, argv);
    MSUProjQt.setApplicationName("MSUProj-Qt");
    MSUProjQt.setOrganizationName("NTsOMZ");
    MSUProjQt.setOrganizationDomain("www.ntsomz.ru");

    QTranslator appTranslation;
        if (appTranslation.load(":/translations/MSUProjQt_" + QLocale::system().name()))
            MSUProjQt.installTranslator(&appTranslation);

    MainWindow MSUProjMainWindow;
    MSUProjMainWindow.show();

    return MSUProjQt.exec();
}
