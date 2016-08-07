#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QMainWindow>
#include <QtHelp>

#include "helpbrowser.h"


class HelpWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HelpWindow(QWidget *parent = 0);
    ~HelpWindow();

signals:
    void contentChange(QUrl);

private:
    QAction *mActionGoMainPage;
    QAction *mActionBackward;
    QAction *mActionForward;
    QHelpEngine *mHelpEngine;
    HelpBrowser *mPageWidget;
    QString locale;

private slots:
    void setPage(const QModelIndex &index);
    void goMainPage();
    void setCurrentContentIndex();
};

#endif // HELPWINDOW_H
