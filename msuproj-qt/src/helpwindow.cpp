#include <QDockWidget>
#include <QDebug>

#include "settings.h"
#include "helpwindow.h"

extern MSUSettings settingsObj;

HelpWindow::HelpWindow(QWidget *parent) :
    QMainWindow(parent),
    mActionGoMainPage(new QAction(tr("Home"), this)),
    mActionBackward(new QAction(tr("Backward"), this)),
    mActionForward(new QAction(tr("Forward"), this)),
    mHelpEngine(new QHelpEngine(settingsObj.getPath(MSUSettings::PATH_SHARE_HELP) + "msuproj-qt.qhc", this)),
    mPageWidget(new HelpBrowser(mHelpEngine, this))
{
    QByteArray geom(settingsObj.getGeometry(MSUSettings::HELPWINDOW));
    if (geom.size() > 0)
    {
        this->restoreGeometry(geom);
        this->restoreState(settingsObj.getState(MSUSettings::HELPWINDOW));
    }
    else
        this->resize(900, 600);
    this->setContentsMargins(5, 0, 5, 5);
    this->setCentralWidget(mPageWidget);

    bool errorLoadingHelp = true;
    if (mHelpEngine->setupData())
    {
        locale = settingsObj.getLocale();
        if (!mHelpEngine->filterAttributes().contains(locale) ||
            !QFile(mHelpEngine->documentationFileName("amigos.msuproj-qt." + locale)).exists())
            locale = "en";

        if(QFile(mHelpEngine->documentationFileName("amigos.msuproj-qt." + locale)).exists())
        {
            errorLoadingHelp = false;
            mHelpEngine->setCurrentFilter(locale);

            QToolBar *navigationBar = new QToolBar(tr("Navigation"), this);
            navigationBar->setObjectName("HelpWindowNavBar");
            navigationBar->addActions({mActionGoMainPage, mActionBackward, mActionForward});
            this->addToolBar(navigationBar);

            QDockWidget *contentsDockWidget = new QDockWidget(tr("Contents"), this);
            contentsDockWidget->setObjectName("HelpWindowContentsDock");
            contentsDockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
            this->addDockWidget(Qt::LeftDockWidgetArea, contentsDockWidget);

            QHelpContentWidget *contentWidget = mHelpEngine->contentWidget();
            contentsDockWidget->setWidget(contentWidget);

            connect(contentWidget, &QHelpContentWidget::clicked, this, &HelpWindow::setPage);
            connect(this, &HelpWindow::contentChange, mPageWidget, &HelpBrowser::setSource);

            connect(mPageWidget, &HelpBrowser::backwardAvailable, mActionBackward, &QAction::setEnabled);
            connect(mActionBackward, &QAction::triggered, mPageWidget, &HelpBrowser::backward);

            connect(mPageWidget, &HelpBrowser::forwardAvailable, mActionForward, &QAction::setEnabled);
            connect(mActionForward, &QAction::triggered, mPageWidget, &HelpBrowser::forward);

            connect(mActionGoMainPage, &QAction::triggered, this, &HelpWindow::goMainPage);

            connect(mPageWidget, &HelpBrowser::anchorClicked, this, &HelpWindow::setCurrentContentIndex);
            connect(mActionBackward, &QAction::triggered, this, &HelpWindow::setCurrentContentIndex);
            connect(mActionForward, &QAction::triggered, this, &HelpWindow::setCurrentContentIndex);

            this->goMainPage();
        }
    }

    if (errorLoadingHelp)
        mPageWidget->setHtml(QString("<html><head/><body><p align=\"center\"><span style=\" font-size:16pt; font-weight:600;\">%1</span></p>"
                                     "<p align=\"center\">%2</p></body></html>")
                             .arg(tr("Error loading help files."),
                                  tr("Could not load help files. Have You installed the MSUProj-Qt Help package?")));
}

HelpWindow::~HelpWindow()
{
    settingsObj.setGeometry(MSUSettings::HELPWINDOW, this->saveGeometry());
    settingsObj.setState(MSUSettings::HELPWINDOW, this->saveState());
}

void HelpWindow::setPage(const QModelIndex &index)
{
    emit contentChange(mHelpEngine->contentModel()->contentItemAt(index)->url());
}

void HelpWindow::goMainPage()
{
    mPageWidget->setSource(QString("qthelp://amigos.msuproj-qt.%1/%1/index.html").arg(locale));
    mPageWidget->clearHistory();
}

void HelpWindow::setCurrentContentIndex()
{
    QHelpContentWidget *widget = mHelpEngine->contentWidget();
    widget->setCurrentIndex(widget->indexOf(mPageWidget->source()));
}
