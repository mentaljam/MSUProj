#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>

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
    void loadGCPs(const QString &file);
    void changeOutName();
    void autoOutName(bool state);
    void setPreview();
    void changeStartButtonState();
    void on_imagePathButton_clicked();
    void on_gcpPathButton_clicked();
    void on_outPathButton_clicked();
    void on_imagePathEdit_editingFinished();
    void on_startButton_clicked();
    void on_actionAbout_triggered();
    void on_actionReference_triggered();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *graphicsScene;
    QString fPreffix;

protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
