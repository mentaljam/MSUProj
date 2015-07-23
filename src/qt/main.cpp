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
    QStringList qmPaths;
    qmPaths << "i18n/msuproj-qt_"
            << "/usr/share/msuproj/i18n/msuproj-qt_"
            << "/usr/local/share/msuproj/i18n/msuproj-qt_";
    foreach (QString qmPath, qmPaths)
        if (appTranslation.load(qmPath + QLocale::system().name()))
        {
            MSUProjQt.installTranslator(&appTranslation);
            break;
        }

    MainWindow MSUProjMainWindow;
    MSUProjMainWindow.show();

    return MSUProjQt.exec();
}
