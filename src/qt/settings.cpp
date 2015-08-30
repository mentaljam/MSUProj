#include <QTranslator>
#include <QLocale>
#include <QDir>

#include <settings.h>


MSUSettings::MSUSettings() :
    mSettings(QSettings::IniFormat, QSettings::UserScope,
              "NTsOMZ", "MSUProj-Qt"),
    mResourcesPath("")
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

void MSUSettings::setLocale(const QString &locale)
{
    mSettings.setValue("locale", locale);
}

void MSUSettings::unsetLocale()
{
    mSettings.remove("locale");
}

QString MSUSettings::getPath(const MSUSettings::PATHS type) const
{
    switch (type)
    {
    case PATH_SHARE:
        return mResourcesPath;
    case PATH_SHARE_I18N:
        return mResourcesPath + "i18n/";
    case PATH_SHARE_HELP:
        return mResourcesPath + "help/";
    default:
        return mSettings.value(option(type)).toString();
    }
}

void MSUSettings::setPath(const MSUSettings::PATHS type, const QString &value)
{
    if (type > PATH_SHARE)
        mSettings.setValue(option(type), value);
}

bool MSUSettings::getBool(MSUSettings::BOOL_OPTIONS type) const
{
    return mSettings.value(option(type)).toBool();
}

void MSUSettings::setBool(MSUSettings::BOOL_OPTIONS type, const bool &value)
{
    mSettings.setValue(option(type), value);
}

QByteArray MSUSettings::getGeometry(MSUSettings::WIDGETS type) const
{
    return mSettings.value(option(type) + "/geom").toByteArray();
}

void MSUSettings::setGeometry(MSUSettings::WIDGETS type, const QByteArray &value)
{
    mSettings.setValue(option(type) + "/geom", value);
}

QByteArray MSUSettings::getState(MSUSettings::WIDGETS type) const
{
    return mSettings.value(option(type) + "/state").toByteArray();
}

void MSUSettings::setState(MSUSettings::WIDGETS type, const QByteArray &value)
{
    mSettings.setValue(option(type) + "/state", value);
}

QString MSUSettings::option(MSUSettings::PATHS type) const
{
    switch (type)
    {
    case PATH_INPUT_PREVIOUS:
        return "History/InputPath";
    case PATH_INPUT_PREFERED:
        return "InputPath";
    default:
        return 0;
    }
}

QString MSUSettings::option(MSUSettings::BOOL_OPTIONS type) const
{
    switch (type)
    {
    case BOOL_USE_PREFERED_INPUT:
        return "UsePreferedPath";
    default:
        return 0;
    }
}

QString MSUSettings::option(MSUSettings::WIDGETS type) const
{
    switch (type)
    {
    case MAINWINDOW:
        return "MainWindow";
    case HELPWINDOW:
        return "HelpWindow";
    default:
        return 0;
    }
}
