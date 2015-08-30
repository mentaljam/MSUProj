#include <QApplication>
#include <QTranslator>

#include <settings.h>
#include <mainwindow.h>

MSUSettings settingsObj;

int main(int argc, char *argv[])
{
    GDALAllRegister();

    QApplication msuProjQt(argc, argv);
    msuProjQt.setApplicationName("MSUProj-Qt");
    msuProjQt.setOrganizationName("NTsOMZ");
    msuProjQt.setOrganizationDomain("www.ntsomz.ru");

    QTranslator appTranslation;
    if (appTranslation.load("msuproj-qt_" + settingsObj.getLocale(), settingsObj.getPath(MSUSettings::PATH_SHARE_I18N)))
        msuProjQt.installTranslator(&appTranslation);

    MainWindow msuProjMainWindow;
    msuProjMainWindow.show();

    return msuProjQt.exec();
}
