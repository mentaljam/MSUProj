#include <QFileDialog>

#include <settings.h>
#include <settingswindow.h>
#include <ui_settingswindow.h>


extern msuSettings settingsObj;

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow),
    mLocales(settingsObj.getLocalesList())
{
    ui->setupUi(this);
    bool hasLocaleSet = false;
    QString curLocale(settingsObj.getLocale(&hasLocaleSet));
    foreach (QString localeStr, mLocales)
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

    ui->inputPathEdit->setText(settingsObj.getPath(msuSettings::INPUT_PREFERED));
    ui->inputPathPreferedButton->setChecked(settingsObj.usePreferedInputPath());
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
            settingsObj.setLocale(mLocales[curIndex - 1]);
        else
            settingsObj.unsetLocale();
        settingsObj.setUsePreferedInputPath(ui->inputPathPreferedButton->isChecked());
        settingsObj.setPath(msuSettings::INPUT_PREFERED, ui->inputPathEdit->text());
        break;
    default:
        break;
    }
}

void SettingsWindow::on_inputPathPreferedButton_toggled(bool checked)
{
    ui->inputPathEdit->setEnabled(checked);
    ui->inputPathButton->setEnabled(checked);
}

void SettingsWindow::on_inputPathButton_clicked()
{
    QFileDialog inputPathDialog(this, tr("Select prefered input folder"), ui->inputPathEdit->text());
    inputPathDialog.setFileMode(QFileDialog::DirectoryOnly);
    if(inputPathDialog.exec())
        ui->inputPathEdit->setText(inputPathDialog.selectedUrls()[0].path());
}
