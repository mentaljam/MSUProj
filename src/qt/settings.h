#ifndef SETTINGS_H
#define SETTINGS_H


#include <QSettings>

class MSUSettings
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

    enum WIDGETS
    {
        MAINWINDOW,
        HELPWINDOW
    };

public:

    MSUSettings();

    void clearSettings();

    QStringList getLocalesList() const;
    QString getLocale(bool *ok = 0) const;
    QString getResourcesPath(const RES_PATHS type) const;
    void setLocale(const QString &locale);
    void unsetLocale();

    QString getPath(const PATHS_TYPES type) const;
    void setPath(const PATHS_TYPES type, const QString &value);

    bool usePreferedInputPath() const;
    void setUsePreferedInputPath(const bool value);

    QByteArray getGeometry(WIDGETS widget) const;
    void setGeometry(WIDGETS widget, const QByteArray &value);

    QByteArray getState(WIDGETS widget) const;
    void setState(WIDGETS widget, const QByteArray &value);

private:

    QSettings   mSettings;
    QString     mResourcesPath;
    QStringList mPathsKeys;

};

#endif // SETTINGS_H
