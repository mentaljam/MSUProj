#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QFileDialog>
#ifdef WITH_UPDATES_ACTION
#   include <QProcess>
#endif // WITH_UPDATES_ACTION

#include <warper.h>

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
    void onWarpStarted();
    void onWarpFinished(msumr::RETURN_CODE code);
#ifdef WITH_UPDATES_ACTION
    void checkUpdates();
    void onCheckUpdatesFinished(int code);
#endif // WITH_UPDATES_ACTION

private:
    Ui::MainWindow *ui;
    QGraphicsScene *mGraphicsScene;
    QFileDialog    *mOpenImageDialog;
    Warper         *mWarper;
#ifdef WITH_UPDATES_ACTION
    QAction        *mActionCheckUpdates;
    QProcess       *mUpdater;
    QTimer         *mUpdatesTimer;
#endif // WITH_UPDATES_ACTION
    QString        mFilePreffix;
    QString        mCurrentImage;

protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
