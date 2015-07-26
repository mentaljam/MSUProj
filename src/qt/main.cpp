#include "mainwindow.h"
#include "settings.h"
#include <msuproj.h>
#include <QApplication>
#include <QTranslator>

MSUMR::MSUProj msuProjObj;
settings settingsObj;

int main(int argc, char *argv[])
{
    GDALAllRegister();

    QApplication MSUProjQt(argc, argv);
    MSUProjQt.setApplicationName("MSUProj-Qt");
    MSUProjQt.setOrganizationName("NTsOMZ");
    MSUProjQt.setOrganizationDomain("www.ntsomz.ru");

    QTranslator appTranslation;
    if (appTranslation.load("msuproj-qt_" + settingsObj.getLocale(), settingsObj.getQmPath()))
        MSUProjQt.installTranslator(&appTranslation);

    MainWindow MSUProjMainWindow;
    MSUProjMainWindow.show();

    return MSUProjQt.exec();
}
