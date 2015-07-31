#include <msuproj_version.h>
#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <msuproj.h>
//#include <settings.h>
#include <settingswindow.h>
#include <QResizeEvent>
#include <QMessageBox>
#include <QTextBrowser>
#include <QDesktopWidget>

extern MSUMR::MSUProj msuProjObj;
//extern settings settingsObj;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    graphicsScene(new QGraphicsScene(this)),
    openImageDialog(new QFileDialog(this, tr("Select input image"), 0,
                                    tr("Meteor-M2 Images (*.jpg *.bmp);;All files (*.*)"))),
    fPreffix("")
{
    ui->setupUi(this);
    ui->statusbar->showMessage(tr("Select input files."));
    ui->imageView->setScene(graphicsScene);
    openImageDialog->setFileMode(QFileDialog::ExistingFile);

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
    if (openImageDialog->exec())
    {
        ui->imagePathEdit->setText(openImageDialog->selectedFiles()[0]);
        this->onImagePathChanged();
    }
}

void MainWindow::onImagePathChanged()
{
    QString file(ui->imagePathEdit->text());
    if (file.isEmpty())
    {
        ui->imagePathLabel->setText(tr("Input Image File"));
        ui->imageRowsLabel->setText(tr("Input Image Rows"));
        ui->imageLinesLabel->setText(tr("Input Image Lines"));
        ui->gcpBox->setEnabled(false);
        ui->gcpPathEdit->clear();
        this->onGCPPathChanged();
    }
    else if (QFile(file).exists())
    {
        QStringList gcpFiles(file + ".gcp");

        if (msuProjObj.setSRC(file.toStdString()) == MSUMR::success)
        {
            ui->imagePathLabel->setText(tr("Input Image File") + " - " + tr("Loaded"));
            ui->imageRowsLabel->setText(tr("Input Image Rows") + QString(" %1").arg(msuProjObj.getSrcXSize()));
            ui->imageLinesLabel->setText(tr("Input Image Lines") + QString(" %1").arg(msuProjObj.getSrcYSize()));
            fPreffix = file.left(file.lastIndexOf('.'));
            gcpFiles.append(fPreffix + ".gcp");
            foreach (QString gcpFile, gcpFiles)
                if (QFile(gcpFile).exists())
                {
                    ui->gcpPathEdit->setText(gcpFile);
                    this->onGCPPathChanged();
                    break;
                }
            ui->gcpBox->setEnabled(true);
            this->changeOutName();
            this->setPreview();
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
    this->changeStartButtonState();
}

void MainWindow::on_gcpPathButton_clicked()
{
    QString curPath = ui->gcpPathEdit->text();
    if (curPath.isEmpty())
        curPath = ui->imagePathEdit->text();
    QFileDialog openGCPs(this, tr("Select input GCP file"),
                         QFileInfo(curPath).path(),
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
        if (msuProjObj.readGCP(file.toStdString()) == MSUMR::success)
        {
            ui->gcpPathLabel->setText(tr("Input GCPs") + " - " + tr("Loaded"));
            ui->gcpRowsLabel->setText(tr("Input GCPs Rows") + QString(" %1").arg(msuProjObj.getGcpXSize()));
            ui->gcpLinesLabel->setText(tr("Input GCPs Lines") + QString(" %1").arg(msuProjObj.getGcpYSize()));
            ui->gcpRowStepLabel->setText(tr("Input GCPs Row Step") + QString(" %1").arg(msuProjObj.getGcpXStep()));
            ui->gcpLineStepLabel->setText(tr("Input GCPs Line Step") + QString(" %1").arg(msuProjObj.getGcpYStep()));
            ui->utmZone->setText(tr("UTM zone") + QString(" %1").arg(msuProjObj.getUTM().c_str()));
            this->changeStartButtonState();
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
        curPath = ui->imagePathEdit->text();
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
    if (ui->autoOutNameBox->isChecked() && !fPreffix.isEmpty())
    {
        QString newName(fPreffix);
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
    if (ui->previewBox->isChecked())
    {
        ui->imageView->setBackgroundBrush(this->palette().color(QPalette::Light));
        QString image(ui->imagePathEdit->text());
        if (QFileInfo(image).isFile())
        {
            graphicsScene->clear();
            graphicsScene->addPixmap(QPixmap(image));
            ui->imageView->fitInView(graphicsScene->sceneRect(), Qt::KeepAspectRatio);
        }
    }
    else
    {
        ui->imageView->setBackgroundBrush(this->palette().color(QPalette::Midlight));
        graphicsScene->clear();
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
        msuProjObj.setDST(file.toStdString());
        ui->statusbar->showMessage(tr("Transforming image, please wait..."));
        MSUMR::retCode code = msuProjObj.warp(ui->modeUTMButton->isChecked(), ui->modeNDZBox->isChecked());
        if (code == MSUMR::success)
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
                               "<p><tr><td><i>%3:</i></td><td></td><td>" VERSION_MSUPROJ " (" VER_DATE_MSUPROJ ")</td></tr>"
                               "<tr><td><i>%4:</i></td><td></td><td>%5 &lt;<a href='mailto:tpr@ntsomz.ru?subject=Feedback for MSUProj-Qt"
                                                                    "&body=Version: " VERSION_MSUPROJ "\nPlatform: " VER_PLATFORM_MSUPROJ "\n"
                                                                    "Architecture: " VER_ARCH_MSUPROJ "\n\n'>tpr@ntsomz.ru</a>&gt;</td></tr>"
                               "<tr><td><i>%6 </i></td><td></td><td><a href='https://github.com/mentaljam/MSUProj'>GitHub</a></td></tr>"
                               "<tr><td><i>%7 </i></td><td></td><td><a href='https://www.transifex.com/projects/p/msuproj/'>Transifex</a></td></tr></p>")
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

    QString refText;
    refText +=  QString("<table  cellpadding=4><tr><td colspan=2><h3>%1</h3></td></tr>"
                        "<tr><td><b>%2</b>:</td><td>%3</td></tr>"
                        "<tr><td><b>%4</b>:</td><td>%5</td></tr>"
                        "<tr><td><b>%6</b>:</td><td>%7</td></tr>")
                .arg(tr("Input files"),
                     tr("Input file"), tr("A JPG or BMP MSU-MR image produced with <i>LRPToffLineDecoder</i>."),
                     tr("Show preview"), tr("Toggle this checkbox to show or hide the preview for an input image. "
                                            "For large images a preview can consume much RAM."),
                     tr("GCPs file"), tr("A text file with ground control points produced with <i>LRPToffLineDecoder</i>. "
                                         "Application will automatically try to find it based on source image name - "
                                         "You need to specify GCP file manually if it has different name."));

    refText +=  QString("<tr><td colspan=2><h3>%1</h3></td></tr>"
                        "<tr><td><b>%2</b>:</td><td>%3</td></tr>"
                        "<tr><td><b>%4</b>:</td><td>%5</td></tr>"
                        "<tr><td><b>%6</b>:</td><td>%7</td></tr></table>")
                .arg(tr("Operation options"),
                     tr("Operation mode"), tr("A source image is projected into WGS84. Select <i>\"Lat/Lon mode\"</i> "
                                              "to use geographic coordinates (degrees) or <i>\"UTM mode\"</i> to use UTM coordinates (meters). "
                                              "The UTM zone number is selected basing on the longitude of the center of the image."),
                     tr("Zeros as NoData"), tr("Check this box to set zero (black) pixels as NoData. These pixels won't be displayed in some "
                                               "GIS applications such as QGIS."),
                     tr("Output file"), tr("An output GeoTiff image (compression - JPEG, quality - 100). By default application saves the result image "
                                           "near original one with the same name and postfix based on operation mode: \"_proj\" for Lat/Lon mode "
                                           "and zone number for UTM mode (for example \"_N38\"). Uncheck the <i>\"Automatic Output name\"</i> "
                                           "box to specify a custom name."));

    QDialog *refWindow = new QDialog;
    connect(refWindow, &QDialog::destroyed, ui->actionReference, &QAction::setEnabled);
    refWindow->setAttribute(Qt::WA_DeleteOnClose);
    refWindow->resize(800, 100);
    refWindow->setWindowTitle(tr("MSUProj-Qt Reference"));

    QTextBrowser *refBrowser = new QTextBrowser(refWindow);
    refBrowser->setHtml(refText);

    QVBoxLayout *refLayout = new QVBoxLayout(refWindow);
    refLayout->addWidget(refBrowser);
    refWindow->show();
    int height = refBrowser->document()->size().height() + 50;
    if (height > 600)
        height = 600;
    refWindow->resize(800, height);
    QSize point((QApplication::desktop()->screen()->rect().size() - refWindow->size()) / 2);
    refWindow->move(point.width(), point.height());
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    event->accept();
    ui->imageView->fitInView(graphicsScene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QApplication::closeAllWindows();
    event->accept();
}
