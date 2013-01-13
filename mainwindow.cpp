#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>

#include "route.h"
#include "trackelement.h"
#include "viewpoint.h"
#include "signal.h"
#include "label.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    trackView = new TrackView;
    setCentralWidget(trackView);

    m_route = NULL;

    // Read settings
    QSettings settings;
    ui->actionAntiAliasing->setChecked(settings.value("view/antiAliasing", true).toBool());
    ui->actionShowSignalNames->setChecked(settings.value("view/showSignalNames", true).toBool());
    ui->actionShowStartingPointNames->setChecked(settings.value("view/showStartingPointNames", false).toBool());
    ui->actionShowViewPointNames->setChecked(settings.value("view/showViewPointNames", false).toBool());
    ui->actionTextScaling->setChecked(settings.value("view/textScaling", false).toBool());

    this->resize(settings.value("mainwindow/size", this->size()).toSize());
}

MainWindow::~MainWindow()
{
    // Save settings
    QSettings settings;
    settings.setValue("view/antiAliasing", ui->actionAntiAliasing->isChecked());
    settings.setValue("view/showSignalNames", ui->actionShowSignalNames->isChecked());
    settings.setValue("view/showStartingPointNames", ui->actionShowStartingPointNames->isChecked());
    settings.setValue("view/showViewPointNames", ui->actionShowViewPointNames->isChecked());
    settings.setValue("view/textScaling", ui->actionTextScaling->isChecked());

    settings.setValue("mainwindow/size", this->size());

    delete ui;

    if (m_route) {
        delete m_route;
    }
}

void MainWindow::fileOpenTriggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::currentPath(), QString("Streckendateien (*.str *.STR)"));

    if (fileName.isNull()) {
        return;
    }

    if (m_route) {
        delete m_route;
    }

    if (trackView->scene()) {
        delete trackView->scene();
    }

    try {
        m_route = new Route(fileName);
    } catch (QString error) {
        QMessageBox::critical(this, tr("Error"), error);
        return;
    }

    this->setWindowTitle(fileName);

    QGraphicsScene* scene = new QGraphicsScene(this);

    foreach (TrackSegment* te, *(m_route->trackSegments())) {
        scene->addItem(te);
    }

    foreach (QGraphicsPathItem* arrowPath, *(m_route->arrows())) {
        scene->addItem(arrowPath);
    }

    foreach (ViewPoint* vp, *(m_route->viewPoints())) {
        scene->addItem(vp);
    }

    foreach (Signal* sig, *(m_route->signalList())) {
        scene->addItem(sig);
    }

    foreach (StartingPoint* sp, *(m_route->startingPoints())) {
        scene->addItem(sp);
    }

    showViewPointNamesTriggered(ui->actionShowViewPointNames->isChecked());
    showStartingPointNamesTriggered(ui->actionShowStartingPointNames->isChecked());
    showSignalNamesTriggered(ui->actionShowSignalNames->isChecked());

    textScaleTriggered(ui->actionTextScaling->isChecked());
    antiAliasingTriggered(ui->actionAntiAliasing->isChecked());

    trackView->setScene(scene);
    trackView->fitInView(trackView->sceneRect(), Qt::KeepAspectRatio);
}

void triggerTextScale(Label *label, bool on) {
    if (label) {
        label->setFlag(QGraphicsItem::ItemIgnoresTransformations, !on);
        label->font().setPointSize(on ? 30 : 8);
    }
}

void MainWindow::textScaleTriggered(bool on)
{
    if (!m_route) {
        return;
    }

    foreach (ViewPoint* vp, *(m_route->viewPoints())) {
        triggerTextScale(vp->label(), on);
    }

    foreach (Signal* sig, *(m_route->signalList())) {
        triggerTextScale(sig->label(), on);
    }

    foreach (StartingPoint* sp, *(m_route->startingPoints())) {
        triggerTextScale(sp->label(), on);
    }
}

void MainWindow::showViewPointNamesTriggered(bool on)
{
    if (m_route) {
        foreach (ViewPoint* vp, *(m_route->viewPoints())) {
            vp->label()->setVisible(on);
        }
    }
}

void MainWindow::showStartingPointNamesTriggered(bool on)
{
    if (m_route) {
        foreach (StartingPoint* sp, *(m_route->startingPoints())) {
            sp->label()->setVisible(on);
        }
    }
}

void MainWindow::showSignalNamesTriggered(bool on)
{
    if (m_route) {
        foreach (Signal* sig, *(m_route->signalList())) {
            sig->label()->setVisible(on);
        }
    }
}

void MainWindow::antiAliasingTriggered(bool on)
{
    if (on) {
        trackView->setRenderHints(trackView->renderHints() | QPainter::Antialiasing);
    } else {
        trackView->setRenderHints(trackView->renderHints() & ~QPainter::Antialiasing);
    }
}

void MainWindow::zoomInTriggered()
{
    trackView->zoomIn();
}

void MainWindow::zoomOutTriggered()
{
    trackView->zoomOut();
}

void MainWindow::zoomFitTriggered()
{
    trackView->fitInView(trackView->sceneRect(), Qt::KeepAspectRatio);
}
