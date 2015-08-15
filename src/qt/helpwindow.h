#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QMainWindow>
#include <QtHelp>


class HelpWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HelpWindow(QWidget *parent = 0);

signals:
    void contentChange(QUrl);

private:
    QHelpEngine *mHelpEngine;

private slots:
    void setPage(const QModelIndex &index);
};

#endif // HELPWINDOW_H
