#include "helpbrowser.h"


HelpBrowser::HelpBrowser(QHelpEngine* helpEngine,
                         QWidget* parent) :
    QTextBrowser(parent),
    mHelpEngine(helpEngine)
{
    this->setOpenExternalLinks(true);
}

QVariant HelpBrowser::loadResource(int type, const QUrl &name)
{
    if (name.scheme() == "qthelp")
    {
        return QVariant(mHelpEngine->fileData(name));
    }
    else
        return QTextBrowser::loadResource(type, name);
}
