#ifndef SETTINGS_H
#define SETTINGS_H


#include <QSettings>

class MSUSettings
{

public:

    enum PATHS
    {
        PATH_SHARE_I18N,
        PATH_SHARE_HELP,
        PATH_SHARE,
        PATH_INPUT_PREVIOUS,
        PATH_INPUT_PREFERED,
        PATH_TOTAL
    };

    enum BOOL_OPTIONS
    {
        BOOL_USE_PREFERED_INPUT,
        BOOL_ADD_LOGO
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
    void setLocale(const QString &locale);
    void unsetLocale();

    QString getPath(const PATHS type) const;
    void setPath(const PATHS type, const QString &value);

    bool getBool(BOOL_OPTIONS type) const;
    void setBool(BOOL_OPTIONS type, const bool &value);

    QByteArray getGeometry(WIDGETS type) const;
    void setGeometry(WIDGETS type, const QByteArray &value);

    QByteArray getState(WIDGETS type) const;
    void setState(WIDGETS type, const QByteArray &value);

private:

    QSettings   mSettings;
    QString     mResourcesPath;

private:

    QString option(PATHS type) const;
    QString option(BOOL_OPTIONS type) const;
    QString option(WIDGETS type) const;

};

#endif // SETTINGS_H
