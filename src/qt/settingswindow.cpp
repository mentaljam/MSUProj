#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "settings.h"

extern settings settingsObj;

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow),
    locales(settingsObj.getLocalesList())
{
    ui->setupUi(this);
    bool hasLocaleSet = false;
    QString curLocale(settingsObj.getLocale(&hasLocaleSet));
    foreach (QString localeStr, locales)
    {
        QLocale locale(localeStr);
        QString boxItem(locale.nativeLanguageName());
        boxItem = boxItem.left(1).toUpper() + boxItem.mid(1);
        QString country(locale.nativeCountryName());
        if (!country.isEmpty())
            boxItem += QString(" (%1)").arg(country);
        ui->langBox->addItem(boxItem);
        if (hasLocaleSet && localeStr == curLocale)
            ui->langBox->setCurrentIndex(ui->langBox->count() - 1);
    }
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::on_buttonBox_clicked(QAbstractButton *button)
{
    int curIndex = ui->langBox->currentIndex();
    switch (ui->buttonBox->standardButton(button))
    {
    case QDialogButtonBox::Ok:
        if (curIndex > 0)
            settingsObj.setLocale(locales[curIndex - 1]);
        else
            settingsObj.unsetLocale();
        break;
    default:
        break;
    }
}
