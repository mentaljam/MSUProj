#include "settings.h"
#include <QTranslator>
#include <QLocale>
#include <QDir>


settings::settings() :
    _settings(QSettings::IniFormat, QSettings::UserScope,
              "NTsOMZ", "MSUProj-Qt"),
    _resPath(""),
    _pathsKeys({
                  "History/InputPath",
                  "InputPath"
              })
{
    QStringList sharePaths;
    sharePaths << "./"
               << "/usr/share/msuproj/"
               << "/usr/local/share/msuproj/";
    foreach (QString sharePath, sharePaths)
        if (QDir(sharePath + "i18n").exists() || QDir(sharePath + "help").exists())
        {
            _resPath = sharePath;
            break;
        }
}

void settings::clearSettings()
{
    _settings.clear();
}

QStringList settings::getLocalesList() const
{
    if (!_resPath.isEmpty())
    {
        QStringList qmFiles = QDir(_resPath + "i18n").entryList(QStringList("msuproj-qt_*.qm"), QDir::Files, QDir::Name);
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

QString settings::getLocale(bool *ok) const
{
    if (ok)
        *ok = _settings.value("locale").toBool();
    return _settings.value("locale", QLocale().system().name()).toString();
}

QString settings::getResPath(const RES_PATHS type) const
{
    switch (type) {
    case I18N:
        return _resPath + "i18n/";
    case HELP:
        return _resPath + "help/";
    default:
        return _resPath;
    }
}

void settings::setLocale(const QString &locale)
{
    _settings.setValue("locale", locale);
}

void settings::unsetLocale()
{
    _settings.remove("locale");
}

QString settings::getPath(const settings::PATHS_TYPES type) const
{
    if (type < PATHS_TYPES_SIZE)
        return _settings.value(_pathsKeys[type]).toString();
    else
        return QString();
}

void settings::setPath(const settings::PATHS_TYPES type, const QString &value)
{
    if (type < PATHS_TYPES_SIZE)
        _settings.setValue(_pathsKeys[type], value);
}

bool settings::usePreferedInputPath() const
{
    return _settings.value("UsePreferedPath").toBool();
}

void settings::setUsePreferedInputPath(const bool value)
{
    _settings.setValue("UsePreferedPath", value);
}
