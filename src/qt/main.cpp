#include <QApplication>
#include <QTranslator>
#ifndef Q_OS_WIN
#   include <QLibraryInfo>
#endif

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

    QString i18Path(settingsObj.getPath(MSUSettings::PATH_SHARE_I18N));

    QTranslator appTranslation;
    if (appTranslation.load("msuproj-qt_" + settingsObj.getLocale(), i18Path))
        msuProjQt.installTranslator(&appTranslation);

    QTranslator qtTranslation;
#ifdef Q_OS_WIN
    if (qtTranslation.load("qtbase_" + settingsObj.getLocale(), i18Path))
#else
    if (qtTranslation.load("qtbase_" + settingsObj.getLocale(),
                           QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
#endif // Q_OS_WIN
        msuProjQt.installTranslator(&qtTranslation);

    MainWindow msuProjMainWindow;
    msuProjMainWindow.show();

    return msuProjQt.exec();
}
