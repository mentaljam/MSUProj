#ifndef SETTINGS_H
#define SETTINGS_H


#include <QSettings>

class settings
{

public:

    enum PATHS_TYPES
    {
        INPUT_PREVIOUS,
        INPUT_PREFERED,
        PATHS_TYPES_SIZE,
    };

    enum RES_PATHS
    {
        RES_ROOT,
        I18N,
        HELP
    };

public:

    settings();

    void clearSettings();

    QStringList getLocalesList() const;
    QString getLocale(bool *ok = 0) const;
    QString getResPath(const RES_PATHS type) const;
    void setLocale(const QString &locale);
    void unsetLocale();

    QString getPath(const PATHS_TYPES type) const;
    void setPath(const PATHS_TYPES type, const QString &value);

    bool usePreferedInputPath() const;
    void setUsePreferedInputPath(const bool value);

private:

    QSettings   _settings;
    QString     _resPath;
    QStringList _pathsKeys;

};

#endif // SETTINGS_H
