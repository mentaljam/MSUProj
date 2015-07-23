#include <msuproj_version.h>
#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <msuproj.h>
#include <QFileDialog>
#include <QResizeEvent>
#include <QMessageBox>
#include <QTextBrowser>

extern MSUMR::MSUProj msuProjObj;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    graphicsScene(new QGraphicsScene(this)),
    fPreffix("")
{
    ui->setupUi(this);
    ui->statusbar->showMessage(tr("Select input files."));
    ui->imageView->setScene(graphicsScene);
    connect(ui->modeLatLonButton, &QRadioButton::clicked, this, &MainWindow::changeOutName);
    connect(ui->modeUTMButton, &QRadioButton::clicked, this, &MainWindow::changeOutName);
    connect(ui->autoOutNameBox, &QCheckBox::toggled, this, &MainWindow::autoOutName);
    connect(ui->statusbar, &QStatusBar::messageChanged, this, &MainWindow::showStdStatus);
    connect(ui->previewBox, &QCheckBox::clicked, this, &MainWindow::setPreview);
    connect(ui->imagePathEdit, &QLineEdit::editingFinished, this, &MainWindow::changeStartButtonState);
    connect(ui->gcpPathEdit, &QLineEdit::editingFinished, this, &MainWindow::changeStartButtonState);
    connect(ui->outPathEdit, &QLineEdit::editingFinished, this, &MainWindow::changeStartButtonState);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showStdStatus(const QString message)
{
    if (message.isEmpty())
        ui->statusbar->showMessage(tr("Select input files."));
}

void MainWindow::loadGCPs(const QString &file)
{
    ui->gcpPathEdit->setText(file);
    msuProjObj.readGCP(file.toStdString());
    ui->gcpRowsLabel->setText(tr("Input GCPs Rows %1").arg(msuProjObj.getGcpXSize()));
    ui->gcpLinesLabel->setText(tr("Input GCPs Lines %1").arg(msuProjObj.getGcpYSize()));
    ui->gcpRowStepLabel->setText(tr("Input GCPs Row Step %1").arg(msuProjObj.getGcpXStep()));
    ui->gcpLineStepLabel->setText(tr("Input GCPs Line Step %1").arg(msuProjObj.getGcpYStep()));
    ui->utmZone->setText(tr("UTM zone %1").arg(msuProjObj.getUTM().c_str()));
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

void MainWindow::autoOutName(bool state)
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
        QString image(ui->imagePathEdit->text());
        if (!image.isEmpty())
        {
            graphicsScene->clear();
            graphicsScene->addPixmap(QPixmap(image));
            ui->imageView->fitInView(graphicsScene->sceneRect(), Qt::KeepAspectRatio);
        }
    }
    else
        graphicsScene->clear();

}

void MainWindow::changeStartButtonState()
{
    if (!ui->imagePathEdit->text().isEmpty() &&
        !ui->gcpPathEdit->text().isEmpty() &&
        !ui->outPathEdit->text().isEmpty())
        ui->startButton->setEnabled(true);
    else
        ui->startButton->setEnabled(false);
}

void MainWindow::on_imagePathButton_clicked()
{
    QFileDialog openImage(this, tr("Select input image"),
                          QFileInfo(ui->imagePathEdit->text()).path(),
                          tr("Meteor-M2 Images (*.jpg *.bmp);;All files (*.*)"));
    openImage.setFileMode(QFileDialog::ExistingFile);
    if (openImage.exec())
    {
        QString file(openImage.selectedFiles()[0]);
        QStringList gcpFiles(file + ".gcp");

        ui->imagePathEdit->setText(file);
        msuProjObj.setSRC(file.toStdString());
        ui->imageRowsLabel->setText(tr("Input Image Rows: %1").arg(msuProjObj.getSrcXSize()));
        ui->imageLinesLabel->setText(tr("Input Image Lines: %1").arg(msuProjObj.getSrcYSize()));
        this->setPreview();
        fPreffix = file.left(file.lastIndexOf('.'));

        gcpFiles.append(fPreffix + ".gcp");
        foreach (QString gcpFile, gcpFiles)
            if (QFile(gcpFile).exists())
            {
                this->loadGCPs(gcpFile);
                break;
            }

        this->changeOutName();
    }
    emit ui->imagePathEdit->editingFinished();
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
        this->loadGCPs(openGCPs.selectedFiles()[0]);
    }
    emit ui->gcpPathEdit->editingFinished();
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
        QString outFilePath(outFile.selectedFiles()[0]);
        if (!outFilePath.contains(QRegularExpression(".*\\.tif")))
            outFilePath += ".tif";
        ui->outPathEdit->setText(outFilePath);
    }
    emit ui->outPathEdit->editingFinished();
}

void MainWindow::on_imagePathEdit_editingFinished()
{
    ui->gcpBox->setDisabled(ui->imagePathEdit->text().isEmpty());
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

void MainWindow::on_startButton_clicked()
{
    if (ui->imagePathEdit->text().isEmpty() ||
        ui->gcpPathEdit->text().isEmpty() ||
        ui->outPathEdit->text().isEmpty())
        QMessageBox::critical(this, tr("Check fields"), tr("All fields must be filled"));
    else
    {
        ui->startButton->setEnabled(false);
        ui->statusbar->showMessage(tr("Transforming image, please wait..."));
        msuProjObj.setDST(ui->outPathEdit->text().toStdString());
        MSUMR::retCode code = msuProjObj.warp(ui->modeUTMButton->isChecked(), ui->modeNDZBox->isChecked());
        if (code == MSUMR::success)
            ui->statusbar->showMessage(tr("Transformation finished successfully."), 7000);
        else
            ui->statusbar->showMessage(tr("An error occured. Please check input data."), 7000);
        this->changeOutName();
        ui->startButton->setEnabled(true);
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About MSUProj-Qt"),
                       QString("<p>%1</p><p>%2</p>"
                               "<p><tr><td><i>%3:</i></td><td></td><td>" VERSION_MSUPROJ " (" VER_DATE_MSUPROJ ")</td></tr>"
                               "<tr><td><i>%4:</i></td><td></td><td>%5 &lt;<a href='mailto:tpr@ntsomz.ru?subject=Feedback for MSUProj-Qt"
                                                                    "&body=Version: " VERSION_MSUPROJ "\nPlatform: " VER_PLATFORM_MSUPROJ "\n"
                                                                    "Architecture: " VER_ARCH_MSUPROJ "\n\n'>tpr@ntsomz.ru</a>&gt;</td></tr>"
                               "<tr><td><i>%6 </i></td><td></td><td><a href='https://github.com/mentaljam/MSUProj'>GitHub</a></td></tr></p>")
                       .arg(tr("MSUProj is a project for georeferencing images from MSU-MR sensing equipment of Russian ERS satellite Meteor-M."),
                            tr("This is a Qt graphical interface for MSUProj."),
                            tr("Version"),
                            tr("Author"), tr("Petr Tsymbarovich"),
                            tr("Progect page on")));
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
    refWindow->resize(650, 100);
    refWindow->setWindowTitle(tr("MSUProj-Qt Reference"));

    QTextBrowser *refBrowser = new QTextBrowser(refWindow);
    refBrowser->setHtml(refText);

    QVBoxLayout *refLayout = new QVBoxLayout(refWindow);
    refLayout->addWidget(refBrowser);
    refWindow->show();
    refWindow->resize(650, refBrowser->document()->size().height() + 50);
}
