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

    enum WIDGETS
    {
        MAINWINDOW,
        HELPWINDOW
    };

    enum BOOL_OPTIONS
    {
        BOOL_USE_PREFERED_INPUT,
        BOOL_ADD_LOGO,
        BOOL_UPDATES_ON_START
    };

    enum INT_OPTIONS
    {
        INT_TIMEOUT_UPDATES
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

    QByteArray getGeometry(WIDGETS type) const;
    void setGeometry(WIDGETS type, const QByteArray &value);

    QByteArray getState(WIDGETS type) const;
    void setState(WIDGETS type, const QByteArray &value);

    bool getBool(BOOL_OPTIONS type) const;
    void setBool(BOOL_OPTIONS type, const bool &value);

#ifdef WITH_UPDATES_ACTION
    int getInt(INT_OPTIONS type) const;
    void setInt(INT_OPTIONS type, const int &value);
#endif // WITH_UPDATES_ACTION

private:

    QSettings   mSettings;
    QString     mResourcesPath;

private:

    QString option(PATHS type) const;
    QString option(WIDGETS type) const;
    QString option(BOOL_OPTIONS type) const;
#ifdef WITH_UPDATES_ACTION
    QString option(INT_OPTIONS type) const;
#endif // WITH_UPDATES_ACTION

};

#endif // SETTINGS_H
