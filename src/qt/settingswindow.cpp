#include <QFileDialog>

#include <settings.h>
#include <settingswindow.h>
#include <ui_settingswindow.h>


extern MSUSettings settingsObj;

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
        if (hasLocaleSet)
            if (localeStr == curLocale)
                ui->langBox->setCurrentIndex(ui->langBox->count() - 1);
            else
                ui->langBox->setCurrentIndex(1);
    }

    ui->inputPathEdit->setText(settingsObj.getPath(MSUSettings::PATH_INPUT_PREFERED));
    ui->inputPathPreferedButton->setChecked(settingsObj.getBool(MSUSettings::BOOL_USE_PREFERED_INPUT));
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
        switch (curIndex)
        {
        case 0:
            settingsObj.unsetLocale();
            break;
        case 1:
            settingsObj.setLocale("default");
            break;
        default:
            settingsObj.setLocale(mLocales[curIndex - 2]);
            break;
        }
        settingsObj.setBool(MSUSettings::BOOL_USE_PREFERED_INPUT, ui->inputPathPreferedButton->isChecked());
        settingsObj.setPath(MSUSettings::PATH_INPUT_PREFERED, ui->inputPathEdit->text());
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
