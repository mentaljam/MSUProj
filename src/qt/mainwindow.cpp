#include <QResizeEvent>
#include <QMessageBox>
#include <QDesktopWidget>
#ifdef WITH_UPDATES_ACTION
#   include <QXmlStreamReader>
#endif // WITH_UPDATES_ACTION
#ifdef Q_OS_WIN32
#   include <QtWinExtras/QWinTaskbarButton>
#endif

#include <settings.h>
#include <settingswindow.h>
#include <helpwindow.h>
#include <mainwindow.h>
#include <ui_mainwindow.h>


extern MSUSettings settingsObj;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mGraphicsScene(new QGraphicsScene(this)),
    mOpenImageDialog(new QFileDialog(this, tr("Select input image"), 0,
                                     tr("Meteor-M2 images (*.jpg *.bmp);;All files (*.*)"))),
    mWarper(new Warper),
    mWarpProgress(new QProgressBar(ui->statusbar)),
#ifdef WITH_UPDATES_ACTION
    mActionCheckUpdates(new QAction(tr("Check for updates"), this)),
#endif // WITH_UPDATES_ACTION
    mFilePreffix(""),
    mCurrentImage(""),
    mProgressMax(mWarper->getProgressMaxPtr()),
    mProgressVal(mWarper->getProgressValPtr())
{
    ui->setupUi(this);
    this->setAcceptDrops(true);

    QByteArray geom(settingsObj.getGeometry(MSUSettings::MAINWINDOW));
    if (geom.size() > 0)
        this->restoreGeometry(geom);

    ui->imageView->setScene(mGraphicsScene);
    mOpenImageDialog->setFileMode(QFileDialog::ExistingFile);
    QString curPath;
    if (settingsObj.getBool(MSUSettings::BOOL_USE_PREFERED_INPUT))
        curPath = settingsObj.getPath(MSUSettings::PATH_INPUT_PREFERED);
    else
        curPath = settingsObj.getPath(MSUSettings::PATH_INPUT_PREVIOUS);
    mOpenImageDialog->setDirectory(curPath);

    mWarpProgress->setVisible(false);
    mWarpProgress->setRange(0, 0);
    ui->statusbar->addPermanentWidget(mWarpProgress);

#ifdef WITH_UPDATES_ACTION
    ui->menuHelp->insertAction(ui->actionAbout, mActionCheckUpdates);
    connect(mActionCheckUpdates, &QAction::triggered, this, &MainWindow::checkUpdates);
    if (settingsObj.getBool(MSUSettings::BOOL_UPDATES_ON_START))
        QTimer::singleShot(500, this, &MainWindow::checkUpdates);
#endif // WITH_UPDATES_ACTION

    connect(ui->statusbar, &QStatusBar::messageChanged, this, &MainWindow::showStdStatus);
    connect(ui->previewBox, &QGroupBox::toggled, this, &MainWindow::setPreview);

    connect(ui->imagePathEdit, &QLineEdit::editingFinished, this, &MainWindow::onImagePathChanged);
    connect(ui->gcpPathEdit, &QLineEdit::editingFinished, this, &MainWindow::onGCPPathChanged);
    connect(ui->outPathEdit, &QLineEdit::editingFinished, this, &MainWindow::changeStartButtonState);

    connect(ui->modeLatLonButton, &QRadioButton::clicked, this, &MainWindow::changeOutName);
    connect(ui->modeUTMButton, &QRadioButton::clicked, this, &MainWindow::changeOutName);
    connect(ui->autoOutNameBox, &QCheckBox::toggled, this, &MainWindow::setOutNameMode);

    connect(mWarper, &Warper::started, this, &MainWindow::onWarpStarted);
    connect(mWarper, &Warper::finished, this, &MainWindow::onWarpFinished);
}

MainWindow::~MainWindow()
{
    settingsObj.setPath(MSUSettings::PATH_INPUT_PREVIOUS, mOpenImageDialog->directory().path());
    settingsObj.setGeometry(MSUSettings::MAINWINDOW, this->saveGeometry());
    QThread *wThread = mWarper->thread();
    if (wThread != qApp->thread())
        wThread->terminate();
    delete mWarper;
    delete ui;
}

void MainWindow::showStdStatus(const QString message)
{
    if (message.isEmpty())
    {
        if (ui->startButton->isEnabled())
            ui->statusbar->showMessage(tr("Ready to transform"));
        else
            ui->statusbar->showMessage(tr("Select input files"));
    }
}

void MainWindow::on_imagePathButton_clicked()
{
    if (mOpenImageDialog->exec())
    {
        ui->imagePathEdit->setText(mOpenImageDialog->selectedFiles()[0]);
        this->onImagePathChanged();
    }
}

void MainWindow::onImagePathChanged()
{
    QString file(ui->imagePathEdit->text());
    if (file != mCurrentImage)
    {
        mCurrentImage = file;

        ui->imagePathLabel->setText(tr("Input image file"));
        ui->imageWidthLabel->setText(tr("Image width"));
        ui->imageHeightLabel->setText(tr("Image height"));
        ui->gcpPathEdit->clear();
        ui->gcpBox->setEnabled(false);

        if (QFile(file).exists())
        {
            QStringList gcpFiles(file + ".gcp");

            if (mWarper->setSrc(file) == msumr::SUCCESS)
            {
                ui->imagePathLabel->setText(tr("Input image file") +
                                            " - <font color=\"green\">" + tr("loaded") + "</font>");
                ui->imageWidthLabel->setText(tr("Image width") + QString(" %1").arg(mWarper->getSrcXSize()));
                ui->imageHeightLabel->setText(tr("Image height") + QString(" %1").arg(mWarper->getSrcYSize()));
                ui->gcpBox->setEnabled(true);
                mFilePreffix = file.left(file.lastIndexOf('.'));
                gcpFiles.append(mFilePreffix + ".gcp");
                foreach (QString gcpFile, gcpFiles)
                    if (QFile(gcpFile).exists())
                    {
                        ui->gcpPathEdit->setText(gcpFile);
                        break;
                    }
                this->changeOutName();
            }
            else
            {
                ui->imagePathLabel->setText(tr("Input image file") +
                                            " - <font color=\"red\">" + tr("error loading") + "</font>");
                ui->statusbar->showMessage(tr("Error loading image"), 7000);
            }
        }
        else
        {
            ui->imagePathLabel->setText(tr("Input image file") +
                                        " - <font color=\"red\">" + tr("no such file") + "</font>");
            ui->statusbar->showMessage(tr("Image file does not exist"), 7000);
        }
        this->setPreview();
        this->changeStartButtonState();
        this->onGCPPathChanged();
    }
}

void MainWindow::on_gcpPathButton_clicked()
{
    QString curPath = ui->gcpPathEdit->text();
    if (curPath.isEmpty())
    {
        if (settingsObj.getBool(MSUSettings::BOOL_USE_PREFERED_INPUT))
            curPath = settingsObj.getPath(MSUSettings::PATH_INPUT_PREFERED);
        else
            curPath = settingsObj.getPath(MSUSettings::PATH_INPUT_PREVIOUS);
    }
    QFileDialog openGCPs(this, tr("Select input GCP file"),
                         curPath,
                         tr("Meteor-M2 GCP file (*.gcp);;All files (*.*)"));
    openGCPs.setFileMode(QFileDialog::ExistingFile);
    if (openGCPs.exec())
    {
        ui->gcpPathEdit->setText(openGCPs.selectedFiles()[0]);
        this->onGCPPathChanged();
    }
}

void MainWindow::onGCPPathChanged()
{
    QString file(ui->gcpPathEdit->text());
    if (file.isEmpty())
    {
        ui->gcpPathLabel->setText(tr("Input GCPs"));
        ui->gcpWidthLabel->setText(tr("GCPs grid width"));
        ui->gcpHeightLabel->setText(tr("GCPs grid height"));
        ui->gcpColumnStepLabel->setText(tr("Step of GCPs grid columns"));
        ui->gcpLineStepLabel->setText(tr("Step of GCPs grid rows"));
        ui->utmZone->setText(tr("UTM zone"));
    }
    else if (QFile(file).exists())
    {
        if (mWarper->readGCP(file) == msumr::SUCCESS)
        {
            ui->gcpPathLabel->setText(tr("Input GCPs") +
                                      " - <font color=\"green\">" + tr("loaded") + "</font>");
            ui->gcpWidthLabel->setText(tr("GCPs grid width") + QString(" %1").arg(mWarper->getGCPXSize()));
            ui->gcpHeightLabel->setText(tr("GCPs grid height") + QString(" %1").arg(mWarper->getGCPYSize()));
            ui->gcpColumnStepLabel->setText(tr("Step of GCPs grid columns") + QString(" %1").arg(mWarper->getGCPXStep()));
            ui->gcpLineStepLabel->setText(tr("Step of GCPs grid rows") + QString(" %1").arg(mWarper->getGCPYStep()));
            ui->utmZone->setText(tr("UTM zone") + QString(" %1").arg(mWarper->getUTM()));
            this->changeStartButtonState();
            this->changeOutName();
        }
        else
        {
            ui->gcpPathLabel->setText(tr("Input GCPs") +
                                      " - <font color=\"red\">" + tr("Error loading") + "</font>");
            ui->statusbar->showMessage(tr("Error loading GCP file"), 7000);
        }
    }
    else
    {
        ui->gcpPathLabel->setText(tr("Input GCPs") +
                                  " - <font color=\"red\">" + tr("No such file") + "</font>");
        ui->statusbar->showMessage(tr("GCP file does not exist"), 7000);
    }
    this->changeStartButtonState();
}

void MainWindow::on_outPathButton_clicked()
{
    QString curPath = ui->outPathEdit->text();
    if (curPath.isEmpty())
    {
        if (settingsObj.getBool(MSUSettings::BOOL_USE_PREFERED_INPUT))
            curPath = settingsObj.getPath(MSUSettings::PATH_INPUT_PREFERED);
        else
            curPath = settingsObj.getPath(MSUSettings::PATH_INPUT_PREVIOUS);
    }
    QFileDialog outFile(this, tr("Specify output file"),
                        QFileInfo(curPath).path(),
                        tr("GeoTiff images (*.tif)"));
    outFile.setFileMode(QFileDialog::AnyFile);
    if (outFile.exec())
    {
        QString file(outFile.selectedFiles()[0]);
        if (!file.contains(QRegularExpression(".*\\.tif")))
            file += ".tif";
        ui->outPathEdit->setText(file);
        this->changeStartButtonState();
    }
}

void MainWindow::changeOutName()
{
    if (ui->autoOutNameBox->isChecked() && !mFilePreffix.isEmpty())
    {
        QString newName(mFilePreffix);
        if (ui->modeLatLonButton->isChecked())
            newName += "_proj";
        else
            newName = QString("%1_%2").arg(newName).arg(mWarper->getUTM());
        if (QFile(newName + ".tif").exists())
        {
            unsigned int postfix = 0;
            while (QFile(QString("%1_%2.tif").arg(newName).arg(++postfix)).exists());
            newName = QString("%1_%2.tif").arg(newName).arg(postfix);
        }
        else
            newName += ".tif";
        ui->outPathEdit->setText(newName);
    }
}

void MainWindow::setOutNameMode(bool state)
{
    ui->outPathEdit->setEnabled(!state);
    ui->outPathButton->setEnabled(!state);
    if (state)
        this->changeOutName();
}

void MainWindow::setPreview()
{
    mGraphicsScene->clear();
    if (ui->previewBox->isChecked())
    {
        ui->imageView->setBackgroundBrush(this->palette().color(QPalette::Light));
        QString image(ui->imagePathEdit->text());
        if (QFileInfo(image).isFile())
        {
            mGraphicsScene->addPixmap(QPixmap(image));
            ui->imageView->fitInView(mGraphicsScene->sceneRect(), Qt::KeepAspectRatio);
        }
    }
    else
    {
        ui->imageView->setBackgroundBrush(this->palette().color(QPalette::Midlight));
    }
}

void MainWindow::changeStartButtonState()
{
    if (QFile(ui->imagePathEdit->text()).exists() &&
        QFile(ui->gcpPathEdit->text()).exists() &&
        !ui->outPathEdit->text().isEmpty())
        ui->startButton->setEnabled(true);
    else
        ui->startButton->setEnabled(false);
    this->showStdStatus(0);
}

void MainWindow::on_startButton_clicked()
{
    QString file(ui->outPathEdit->text());
    while (file.endsWith('/') || file.endsWith('\\'))
        file = file.left(file.size() - 1);
    if (!file.isEmpty())
    {
        if (!file.contains(QRegularExpression(".*\\.tif")))
            file += ".tif";
        ui->outPathEdit->setText(file);
        mWarper->setDst(file);
        mWarper->setUseUTM(ui->modeUTMButton->isChecked());
        mWarper->setZerosAsND(ui->modeNDZBox->isChecked());
        mWarper->setAddLogo(settingsObj.getBool(MSUSettings::BOOL_ADD_LOGO));
        QThread *thread = new QThread;
        mWarper->moveToThread(thread);
        connect(thread, &QThread::started, mWarper, &Warper::start);
        connect(mWarper, &Warper::finished, thread, &QThread::exit);
        thread->start();
    }
    else
    {
        ui->statusbar->showMessage(tr("Output file name is empty"), 7000);
        ui->outPathEdit->clear();
    }
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsWindow settingsWindow(this);
    settingsWindow.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About MSUProj-Qt"),
                       QString("<p>%1</p><p>%2</p><p>%3</p>"
                               "<p><tr><td align='right'>%4:&nbsp;&nbsp;</td><td>" VERSION_MSUPROJ_STRING " (" VERSION_MSUPROJ_DATE ")</td></tr>"
                               "<tr><td align='right'>%5:</td>&nbsp;&nbsp;<td><a href='mailto:tpr@ntsomz.ru?subject=Feedback for MSUProj-Qt"
                                                                    "&body=Version: " VERSION_MSUPROJ_STRING "\nPlatform: " VERSION_MSUPROJ_OS "\n"
                                                                    "Architecture: " VERSION_MSUPROJ_ARCH "\n\n'>tpr@ntsomz.ru</a></td></tr>"
                               "<tr><td align='right'>%6:</td>&nbsp;&nbsp;<td><a href='https://github.com/mentaljam/MSUProj'>GitHub</a></td></tr>"
                               "<tr><td align='right'>%7:</td>&nbsp;&nbsp;<td><a href='https://www.transifex.com/projects/p/msuproj/'>Transifex</a></td></tr></p>")
                       .arg(tr("MSUProj is a project for georeferencing images from MSU-MR sensing equipment of Russian ERS satellite Meteor-M."))
                       .arg(tr("This is a Qt graphical interface for MSUProj."))
                       .arg(tr("MSUProj is provided under <a href='http://opensource.org/licenses/Zlib'>The Zlib License</a>."))
                       .arg(tr("Version"))
                       .arg(tr("Author"))
                       .arg(tr("Project page"))
                       .arg(tr("Translations")));
}

void MainWindow::on_actionReference_triggered()
{
    ui->actionReference->setEnabled(false);
    HelpWindow *refWindow = new HelpWindow;
    refWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(refWindow, &QDialog::destroyed, ui->actionReference, &QAction::setEnabled);
    refWindow->show();
}

void MainWindow::onWarpStarted()
{
    QTimer::singleShot(250, this, &MainWindow::setProgressMax);
    mWarpProgress->setVisible(true);
#ifdef Q_OS_WIN32
    mWinProgress->setVisible(true);
#endif // Q_OS_WIN32
    ui->centralwidget->setEnabled(false);
    ui->menubar->setEnabled(false);
    ui->statusbar->showMessage(tr("Transforming image, please wait..."));
}

void MainWindow::onWarpFinished(msumr::RETURN_CODE code)
{
    qApp->alert(this);
    mWarpProgress->setVisible(false);
#ifdef Q_OS_WIN32
        mWinProgress->setVisible(false);
#endif // Q_OS_WIN32
    mWarpProgress->setMaximum(0);
    mWarpProgress->setValue(0);
    if (code == msumr::SUCCESS)
        ui->statusbar->showMessage(tr("Transformation finished successfully"), 7000);
    else
        ui->statusbar->showMessage(tr("An error occured. Please check input data"), 7000);
    this->changeOutName();
    ui->centralwidget->setEnabled(true);
    ui->menubar->setEnabled(true);
}

void MainWindow::setProgressMax()
{
    if (*mProgressMax > 0)
    {
        mWarpProgress->setMaximum(*mProgressMax - 1);
#ifdef Q_OS_WIN32
        mWinProgress->setMaximum(*mProgressMax - 1);
#endif // Q_OS_WIN32
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWindow::setProgressVal);
        connect(mWarper, &Warper::finished, timer, &QTimer::deleteLater);
        timer->start(250);
    }
    else
        QTimer::singleShot(500, this, &MainWindow::setProgressMax);
}

void MainWindow::setProgressVal()
{
    mWarpProgress->setValue(*mProgressVal);
#ifdef Q_OS_WIN32
    mWinProgress->setValue(*mProgressVal);
#endif // Q_OS_WIN32
}

#ifdef WITH_UPDATES_ACTION
void MainWindow::checkUpdates()
{

    QString tool = QDir::currentPath() + "/maintenancetool.exe";
    if (QFile(tool).exists())
    {
        mActionCheckUpdates->setEnabled(false);
        ui->statusbar->showMessage(tr("Searching for updates..."));
        mUpdater = new QProcess(this);
        connect(mUpdater, static_cast<void (QProcess::*)(int)>(&QProcess::finished),
                this, &MainWindow::onCheckUpdatesFinished);
        mUpdatesTimer = new QTimer(this);
        connect(mUpdatesTimer, &QTimer::timeout, mUpdater, &QProcess::kill);
        mUpdater->start(tool, QStringList("--checkupdates"));
        mUpdatesTimer->start(settingsObj.getInt(MSUSettings::INT_TIMEOUT_UPDATES));
    }
    else
    {
        qDebug() << "Error searching for updates: cannot find" << tool;
        QMessageBox::critical(this, tr("Error searching for updates"),
                              tr("Cannot find update tool \"maintenancetool.exe\"."));
    }
}

void MainWindow::onCheckUpdatesFinished(int code)
{
    mUpdatesTimer->stop();
    mUpdatesTimer->deleteLater();
    qApp->alert(this);
    qDebug() << "maintenancetool returned code" << code;
    if (code == 0)
    {
        QString xmlData(mUpdater->readAll());
        qDebug() << "Parsing answer:";
        qDebug() << xmlData;
        QXmlStreamReader xml(xmlData);
        QString version;
        while (!xml.atEnd() && !xml.hasError())
        {
            xml.readNext();
            QXmlStreamAttributes attributes = xml.attributes();
            if (attributes.value("name") == "MSUProj-Qt")
            {
                version = attributes.value("version").toString();
                break;
            }
        }

        QString title(tr("Updates are available"));
        QString messagePrefix;
        if (!version.isEmpty())
            messagePrefix = tr("MSUProj-Qt version %1 is available.").arg(version);
        else
            messagePrefix = tr("MSUProj components updates are available.");
        ui->statusbar->showMessage(title);
        if (QMessageBox::question(this, title, QString("<p>%1</p><p>%2</p>")
                                  .arg(messagePrefix)
                                  .arg(tr("Start the maintenance tool now?")))
                ==
            QMessageBox::Yes)
        {
            qDebug() << "Starting maintenance tool";
            QProcess::startDetached(QDir::currentPath() + "/maintenancetool.exe",
                                    QStringList("--updater"));
            qApp->quit();
        }
        else
            this->showStdStatus(0);
    }
    else if (code == 1)
    {
        qDebug() << "No updates are available";
        ui->statusbar->showMessage(tr("No updates are available"), 7000);
    }
    else
    {
        qDebug() << "Checking updates timeout";
        ui->statusbar->showMessage(tr("Error searching for updates, check your internet connection"),
                                   7000);
    }
    mUpdater->deleteLater();
    mActionCheckUpdates->setEnabled(true);
}
#endif // WITH_UPDATES_ACTION

void MainWindow::resizeEvent(QResizeEvent *event)
{
    event->accept();
    ui->imageView->fitInView(mGraphicsScene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QApplication::closeAllWindows();
    event->accept();
}

void MainWindow::showEvent(QShowEvent *event)
{
#ifdef Q_OS_WIN32
    QWinTaskbarButton *taskButton = new QWinTaskbarButton(this);
    taskButton->setWindow(this->windowHandle());
    mWinProgress = taskButton->progress();
    mWinProgress->setVisible(false);
#endif // Q_OS_WIN32
    this->showStdStatus(0);
    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls())
    {
        QUrl url(mimeData->urls()[0]);
        QMimeType type(QMimeDatabase().mimeTypeForUrl(url));
        if (type.name() == "image/jpeg" || type.name() == "image/bmp")
            event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    ui->imagePathEdit->setText(event->mimeData()->urls()[0].toLocalFile());
    emit ui->imagePathEdit->editingFinished();
}
