#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QMessageBox>

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
    void loadGCPs(const QString &file);
    void changeOutName();
    void autoOutName(bool state);
    void on_imagePathButton_clicked();
    void on_gcpPathButton_clicked();
    void on_imagePathEdit_editingFinished();
    void on_startButton_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *graphicsScene;
    QMessageBox *warpingWindow;
    QString fPreffix;
    void warp();

protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // MAINWINDOW_H
