#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_inputPathPreferedButton_toggled(bool checked);

    void on_inputPathButton_clicked();

private:
    Ui::SettingsWindow *ui;
    QStringList mLocales;
};

#endif // SETTINGSWINDOW_H
