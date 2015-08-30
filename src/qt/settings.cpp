#include <QTranslator>
#include <QLocale>
#include <QDir>

#include <settings.h>


MSUSettings::MSUSettings() :
    mSettings(QSettings::IniFormat, QSettings::UserScope,
              "NTsOMZ", "MSUProj-Qt"),
    mResourcesPath(""),
    mPathsKeys({
                  "History/InputPath",
                  "InputPath"
              })
{
    QStringList resourcesPaths;
    resourcesPaths << "./"
               << "/usr/share/msuproj/"
               << "/usr/local/share/msuproj/";
    foreach (QString resourcesPath, resourcesPaths)
        if (QDir(resourcesPath + "i18n").exists() || QDir(resourcesPath + "help").exists())
        {
            mResourcesPath = resourcesPath;
            break;
        }
}

void MSUSettings::clearSettings()
{
    mSettings.clear();
}

QStringList MSUSettings::getLocalesList() const
{
    if (!mResourcesPath.isEmpty())
    {
        QStringList qmFiles = QDir(mResourcesPath + "i18n").entryList(QStringList("msuproj-qt_*.qm"), QDir::Files, QDir::Name);
        QStringList locales;
        foreach (QString qmFile, qmFiles)
        {
            qmFile = qmFile.right(qmFile.indexOf('_') - 2);
            qmFile.remove(".qm");
            locales.append(qmFile);
        }
        return locales;
    }
    else
        return QStringList();
}

QString MSUSettings::getLocale(bool *ok) const
{
    if (ok)
        *ok = mSettings.value("locale").toBool();
    return mSettings.value("locale", QLocale().system().name()).toString();
}

QString MSUSettings::getResourcesPath(const RES_PATHS type) const
{
    switch (type) {
    case I18N:
        return mResourcesPath + "i18n/";
    case HELP:
        return mResourcesPath + "help/";
    default:
        return mResourcesPath;
    }
}

void MSUSettings::setLocale(const QString &locale)
{
    mSettings.setValue("locale", locale);
}

void MSUSettings::unsetLocale()
{
    mSettings.remove("locale");
}

QString MSUSettings::getPath(const PATHS_TYPES type) const
{
    if (type < PATHS_TYPES_SIZE)
        return mSettings.value(mPathsKeys[type]).toString();
    else
        return QString();
}

void MSUSettings::setPath(const PATHS_TYPES type, const QString &value)
{
    if (type < PATHS_TYPES_SIZE)
        mSettings.setValue(mPathsKeys[type], value);
}

bool MSUSettings::usePreferedInputPath() const
{
    return mSettings.value("UsePreferedPath").toBool();
}

void MSUSettings::setUsePreferedInputPath(const bool value)
{
    mSettings.setValue("UsePreferedPath", value);
}

QByteArray MSUSettings::getGeometry(MSUSettings::WIDGETS widget) const
{
    switch (widget)
    {
    case MAINWINDOW:
        return mSettings.value("MainWindow/geom").toByteArray();
    case HELPWINDOW:
        return mSettings.value("HelpWindow/geom").toByteArray();
    default:
        return 0;
    }
}

void MSUSettings::setGeometry(MSUSettings::WIDGETS widget, const QByteArray &value)
{
    switch (widget)
    {
    case MAINWINDOW:
        mSettings.setValue("MainWindow/geom", value);
        break;
    case HELPWINDOW:
        mSettings.setValue("HelpWindow/geom", value);
        break;
    default:
        break;
    }
}

QByteArray MSUSettings::getState(MSUSettings::WIDGETS widget) const
{
    switch (widget)
    {
    case MAINWINDOW:
        return mSettings.value("MainWindow/state").toByteArray();
    case HELPWINDOW:
        return mSettings.value("HelpWindow/state").toByteArray();
    default:
        return 0;
    }
}

void MSUSettings::setState(MSUSettings::WIDGETS widget, const QByteArray &value)
{
    switch (widget)
    {
    case MAINWINDOW:
        mSettings.setValue("MainWindow/state", value);
        break;
    case HELPWINDOW:
        mSettings.setValue("HelpWindow/state", value);
        break;
    default:
        break;
    }
}
