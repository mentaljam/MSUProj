#include "settings.h"

#include <QTranslator>
#include <QLocale>
#include <QDir>
#include <QRegularExpression>


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
        if (QDir(resourcesPath + "translations").exists() || QDir(resourcesPath + "help").exists())
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
        QStringList qmFiles = QDir(mResourcesPath + "translations").entryList(QStringList("msuproj-qt_*.qm"), QDir::Files, QDir::Name);
        QStringList locales;
        foreach (QString qmFile, qmFiles)
        {
            qmFile.remove(QRegularExpression("(msuproj-qt_|\\.qm)"));
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
    {
        *ok = mSettings.contains("locale");
    }
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
        return mResourcesPath + "translations/";
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

bool MSUSettings::getBool(MSUSettings::BOOL_OPTIONS type) const
{
    switch (type)
    {
    case BOOL_ADD_LOGO:
        return mSettings.value(option(type), true).toBool();
    default:
        return mSettings.value(option(type)).toBool();
    }
}

void MSUSettings::setBool(MSUSettings::BOOL_OPTIONS type, const bool &value)
{
    mSettings.setValue(option(type), value);
}

#ifdef WITH_UPDATES_ACTION
int MSUSettings::getInt(MSUSettings::INT_OPTIONS type) const
{
    switch (type)
    {
    case INT_TIMEOUT_UPDATES:
        return mSettings.value(option(type), 15000).toInt();
    default:
        return mSettings.value(option(type)).toInt();
    }
}

void MSUSettings::setInt(MSUSettings::INT_OPTIONS type, const int &value)
{
    mSettings.setValue(option(type), value);
}
#endif // WITH_UPDATES_ACTION

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

QString MSUSettings::option(MSUSettings::BOOL_OPTIONS type) const
{
    switch (type)
    {
    case BOOL_USE_PREFERED_INPUT:
        return "UsePreferedPath";
    case BOOL_ADD_LOGO:
        return "AddLogo";
#ifdef WITH_UPDATES_ACTION
    case BOOL_UPDATES_ON_START:
        return "CheckUpdatesOnStart";
#endif // WITH_UPDATES_ACTION
    default:
        return 0;
    }
}

#ifdef WITH_UPDATES_ACTION
QString MSUSettings::option(MSUSettings::INT_OPTIONS type) const
{
    switch (type)
    {
    case INT_TIMEOUT_UPDATES:
        return "TimeoutForUpdates";
    default:
        return 0;
    }
}
#endif // WITH_UPDATES_ACTION
