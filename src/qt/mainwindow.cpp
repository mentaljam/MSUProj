#include <QResizeEvent>
#include <QMessageBox>
#include <QDesktopWidget>

#include <msuproj_version.h>
#include <msuproj.h>
#include <settings.h>
#include <settingswindow.h>
#include <helpwindow.h>
#include <mainwindow.h>
#include <ui_mainwindow.h>


extern msumr::MSUProj msuProjObj;
extern MSUSettings settingsObj;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mGraphicsScene(new QGraphicsScene(this)),
    mOpenImageDialog(new QFileDialog(this, tr("Select input image"), 0,
                                     tr("Meteor-M2 Images (*.jpg *.bmp);;All files (*.*)"))),
    mFilePreffix(""),
    mCurrentImage("")
{
    ui->setupUi(this);
    ui->statusbar->showMessage(tr("Select input files."));
    ui->imageView->setScene(mGraphicsScene);
    mOpenImageDialog->setFileMode(QFileDialog::ExistingFile);
    QString curPath;
    if (settingsObj.usePreferedInputPath())
        curPath = settingsObj.getPath(MSUSettings::INPUT_PREFERED);
    else
        curPath = settingsObj.getPath(MSUSettings::INPUT_PREVIOUS);
    mOpenImageDialog->setDirectory(curPath);

    connect(ui->statusbar, &QStatusBar::messageChanged, this, &MainWindow::showStdStatus);
    connect(ui->previewBox, &QGroupBox::toggled, this, &MainWindow::setPreview);

    connect(ui->imagePathEdit, &QLineEdit::editingFinished, this, &MainWindow::onImagePathChanged);
    connect(ui->gcpPathEdit, &QLineEdit::editingFinished, this, &MainWindow::onGCPPathChanged);
    connect(ui->outPathEdit, &QLineEdit::editingFinished, this, &MainWindow::changeStartButtonState);

    connect(ui->modeLatLonButton, &QRadioButton::clicked, this, &MainWindow::changeOutName);
    connect(ui->modeUTMButton, &QRadioButton::clicked, this, &MainWindow::changeOutName);
    connect(ui->autoOutNameBox, &QCheckBox::toggled, this, &MainWindow::setOutNameMode);
}

MainWindow::~MainWindow()
{
    settingsObj.setPath(MSUSettings::INPUT_PREVIOUS, mOpenImageDialog->directory().path());
    delete ui;
}

void MainWindow::showStdStatus(const QString message)
{
    if (message.isEmpty())
    {
        if (ui->startButton->isEnabled())
            ui->statusbar->showMessage(tr("Ready to transform."));
        else
            ui->statusbar->showMessage(tr("Select input files."));
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

        ui->imagePathLabel->setText(tr("Input Image File"));
        ui->imageRowsLabel->setText(tr("Input Image Rows"));
        ui->imageLinesLabel->setText(tr("Input Image Lines"));
        ui->gcpPathEdit->clear();
        ui->gcpBox->setEnabled(false);

        if (QFile(file).exists())
        {
            QStringList gcpFiles(file + ".gcp");

            if (msuProjObj.setSrc(file.toStdString()) == msumr::SUCCESS)
            {
                ui->imagePathLabel->setText(tr("Input Image File") + " - " + tr("Loaded"));
                ui->imageRowsLabel->setText(tr("Input Image Rows") + QString(" %1").arg(msuProjObj.getSrcXSize()));
                ui->imageLinesLabel->setText(tr("Input Image Lines") + QString(" %1").arg(msuProjObj.getSrcYSize()));
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
                ui->imagePathLabel->setText(tr("Input Image File") + " - " + tr("Error loading"));
                ui->statusbar->showMessage(tr("Error loading image"), 7000);
            }
        }
        else
        {
            ui->imagePathLabel->setText(tr("Input Image File") + " - " + tr("No such file"));
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
        if (settingsObj.usePreferedInputPath())
            curPath = settingsObj.getPath(MSUSettings::INPUT_PREFERED);
        else
            curPath = settingsObj.getPath(MSUSettings::INPUT_PREVIOUS);
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
        ui->gcpPathLabel->setText(tr("Input GCPs"));
        ui->gcpRowsLabel->setText(tr("Input GCPs Rows"));
        ui->gcpLinesLabel->setText(tr("Input GCPs Lines"));
        ui->gcpRowStepLabel->setText(tr("Input GCPs Row Step"));
        ui->gcpLineStepLabel->setText(tr("Input GCPs Line Step"));
        ui->utmZone->setText(tr("UTM zone"));
    }
    else if (QFile(file).exists())
    {
        if (msuProjObj.readGCP(file.toStdString()) == msumr::SUCCESS)
        {
            ui->gcpPathLabel->setText(tr("Input GCPs") + " - " + tr("Loaded"));
            ui->gcpRowsLabel->setText(tr("Input GCPs Rows") + QString(" %1").arg(msuProjObj.getGCPXSize()));
            ui->gcpLinesLabel->setText(tr("Input GCPs Lines") + QString(" %1").arg(msuProjObj.getGCPYSize()));
            ui->gcpRowStepLabel->setText(tr("Input GCPs Row Step") + QString(" %1").arg(msuProjObj.getGCPXStep()));
            ui->gcpLineStepLabel->setText(tr("Input GCPs Line Step") + QString(" %1").arg(msuProjObj.getGCPYStep()));
            ui->utmZone->setText(tr("UTM zone") + QString(" %1").arg(msuProjObj.getUTM().c_str()));
            this->changeStartButtonState();
            this->changeOutName();
        }
        else
        {
            ui->gcpPathLabel->setText(tr("Input GCPs") + " - " + tr("Error loading"));
            ui->statusbar->showMessage(tr("Error loading GCPs file"), 7000);
        }
    }
    else
    {
        ui->gcpPathLabel->setText(tr("Input GCPs") + " - " + tr("No such file"));
        ui->statusbar->showMessage(tr("GCP file does not exist"), 7000);
    }
    this->changeStartButtonState();
}

void MainWindow::on_outPathButton_clicked()
{
    QString curPath = ui->outPathEdit->text();
    if (curPath.isEmpty())
    {
        if (settingsObj.usePreferedInputPath())
            curPath = settingsObj.getPath(MSUSettings::INPUT_PREFERED);
        else
            curPath = settingsObj.getPath(MSUSettings::INPUT_PREVIOUS);
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
            newName = QString("%1_%2").arg(newName).arg(msuProjObj.getUTM().c_str());
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
    this->showStdStatus(NULL);
}

void MainWindow::on_startButton_clicked()
{
    ui->startButton->setEnabled(false);
    QString file(ui->outPathEdit->text());
    while (file.endsWith('/') || file.endsWith('\\'))
        file = file.left(file.size() - 1);
    if (!file.isEmpty())
    {
        if (!file.contains(QRegularExpression(".*\\.tif")))
            file += ".tif";
        ui->outPathEdit->setText(file);
        msuProjObj.setDst(file.toStdString());
        ui->statusbar->showMessage(tr("Transforming image, please wait..."));
        msumr::RETURN_CODE code = msuProjObj.warp(ui->modeUTMButton->isChecked(), ui->modeNDZBox->isChecked());
        if (code == msumr::SUCCESS)
            ui->statusbar->showMessage(tr("Transformation finished successfully"), 7000);
        else
            ui->statusbar->showMessage(tr("An error occured. Please check input data"), 7000);
        this->changeOutName();
        ui->startButton->setEnabled(true);
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
                       QString("<p>%1</p><p>%2</p>"
                               "<p><tr><td><i>%3:</i></td><td></td><td align='right'>" VERSION_MSUPROJ " (" VER_DATE_MSUPROJ ")</td></tr>"
                               "<tr><td><i>%4:</i></td><td></td><td align='right'>%5 <a href='mailto:tpr@ntsomz.ru?subject=Feedback for MSUProj-Qt"
                                                                    "&body=Version: " VERSION_MSUPROJ "\nPlatform: " VER_PLATFORM_MSUPROJ "\n"
                                                                    "Architecture: " VER_ARCH_MSUPROJ "\n\n'>&lt;tpr@ntsomz.ru&gt;</a></td></tr>"
                               "<tr><td><i>%6 </i></td><td></td><td align='right'><a href='https://github.com/mentaljam/MSUProj'>GitHub</a></td></tr>"
                               "<tr><td><i>%7 </i></td><td></td><td align='right'><a href='https://www.transifex.com/projects/p/msuproj/'>Transifex</a></td></tr></p>")
                       .arg(tr("MSUProj is a project for georeferencing images from MSU-MR sensing equipment of Russian ERS satellite Meteor-M."),
                            tr("This is a Qt graphical interface for MSUProj."),
                            tr("Version"),
                            tr("Author"), tr("Petr Tsymbarovich"),
                            tr("Progect page on"),
                            tr("Translations on")));
}

void MainWindow::on_actionReference_triggered()
{
    ui->actionReference->setDisabled(true);
    HelpWindow *refWindow = new HelpWindow;
    refWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(refWindow, &QDialog::destroyed, ui->actionReference, &QAction::setEnabled);
    refWindow->show();
}

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
