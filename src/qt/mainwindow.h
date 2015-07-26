#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void showStdStatus(const QString);
    void on_imagePathButton_clicked();
    void onImagePathChanged();
    void on_gcpPathButton_clicked();
    void onGCPPathChanged();
    void on_outPathButton_clicked();
    void changeOutName();
    void setOutNameMode(bool state);
    void setPreview();
    void changeStartButtonState();
    void on_startButton_clicked();
    void on_actionSettings_triggered();
    void on_actionAbout_triggered();
    void on_actionReference_triggered();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *graphicsScene;
    QFileDialog    *openImageDialog;
    QString        fPreffix;

protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
