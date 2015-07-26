#ifndef SETTINGS_H
#define SETTINGS_H


#include <QSettings>

class settings
{
public:
    settings();

    QStringList getLocalesList() const;
    QString getLocale(bool *ok = 0) const;
    QString getQmPath() const;
    void setLocale(const QString &locale);
    void unsetLocale();

private:
    QSettings _settings;
    QString   _qmPath;
};

#endif // SETTINGS_H
