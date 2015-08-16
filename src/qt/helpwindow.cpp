#include <QDockWidget>
#include <QDebug>

#include <settings.h>
#include <helpbrowser.h>

#include <helpwindow.h>

extern msuSettings settingsObj;


HelpWindow::HelpWindow(QWidget *parent) :
    QMainWindow(parent),
    mHelpEngine(new QHelpEngine(settingsObj.getResourcesPath(msuSettings::HELP) + "msuproj-qt.qhc", this))
{
    this->resize(900, 600);

    QDockWidget *pageDockWidget = new QDockWidget(tr("Help section"), this);
    pageDockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->addDockWidget(Qt::RightDockWidgetArea, pageDockWidget);
    HelpBrowser *pageWidget = new HelpBrowser(mHelpEngine, this);
    pageDockWidget->setWidget(pageWidget);

    bool errorLoadingHelp = true;
    if (mHelpEngine->setupData())
    {
        QString locale(settingsObj.getLocale());
        if (!mHelpEngine->filterAttributes().contains(locale) ||
            !QFile(mHelpEngine->documentationFileName("amigos.msuproj-qt." + locale)).exists())
            locale = "en";

        if(QFile(mHelpEngine->documentationFileName("amigos.msuproj-qt." + locale)).exists())
        {
            errorLoadingHelp = false;
            mHelpEngine->setCurrentFilter(locale);

            QDockWidget *contentsDockWidget = new QDockWidget(tr("Contents"), this);
            contentsDockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
            this->addDockWidget(Qt::LeftDockWidgetArea, contentsDockWidget);

            QHelpContentWidget *contentWidget = mHelpEngine->contentWidget();
            contentsDockWidget->setWidget(contentWidget);
            contentWidget->setMaximumWidth(300);

            connect(contentWidget, &QHelpContentWidget::clicked, this, &HelpWindow::setPage);
            connect(this, &HelpWindow::contentChange, pageWidget, &HelpBrowser::setSource);

            pageWidget->setSource(QString("qthelp://amigos.msuproj-qt.%1/%1/index.html").arg(locale));
        }
    }

    if (errorLoadingHelp)
        pageWidget->setHtml(QString("<html><head/><body><p align=\"center\"><span style=\" font-size:16pt; font-weight:600;\">%1</span></p>"
                                    "<p align=\"center\">%2</p></body></html>")
                            .arg(tr("Error loading help files."),
                                 tr("Could not load help files. Have You installed the MSUProj-Qt Help package?")));
}

void HelpWindow::setPage(const QModelIndex &index)
{
    emit contentChange(mHelpEngine->contentModel()->contentItemAt(index)->url());
}
