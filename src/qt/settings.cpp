#include <QTranslator>
#include <QLocale>
#include <QDir>

#include <settings.h>


msuSettings::msuSettings() :
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

void msuSettings::clearSettings()
{
    mSettings.clear();
}

QStringList msuSettings::getLocalesList() const
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

QString msuSettings::getLocale(bool *ok) const
{
    if (ok)
        *ok = mSettings.value("locale").toBool();
    return mSettings.value("locale", QLocale().system().name()).toString();
}

QString msuSettings::getResourcesPath(const RES_PATHS type) const
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

void msuSettings::setLocale(const QString &locale)
{
    mSettings.setValue("locale", locale);
}

void msuSettings::unsetLocale()
{
    mSettings.remove("locale");
}

QString msuSettings::getPath(const PATHS_TYPES type) const
{
    if (type < PATHS_TYPES_SIZE)
        return mSettings.value(mPathsKeys[type]).toString();
    else
        return QString();
}

void msuSettings::setPath(const PATHS_TYPES type, const QString &value)
{
    if (type < PATHS_TYPES_SIZE)
        mSettings.setValue(mPathsKeys[type], value);
}

bool msuSettings::usePreferedInputPath() const
{
    return mSettings.value("UsePreferedPath").toBool();
}

void msuSettings::setUsePreferedInputPath(const bool value)
{
    mSettings.setValue("UsePreferedPath", value);
}
