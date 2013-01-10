#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

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
    trackView->setRenderHints(QPainter::Antialiasing);
    setCentralWidget(trackView);

    m_route = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
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

    foreach (QGraphicsItem* pe, *(m_route->trackSegments())) {
        scene->addItem(pe);
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

    trackView->setScene(scene);
    trackView->fitInView(trackView->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::textScaleTriggered(bool on)
{
    if (!trackView->scene()) {
        return;
    }

    foreach (QGraphicsItem *item, trackView->scene()->items()) {
        foreach (QGraphicsItem *childItem, item->childItems()) {

            // HACK
            Label *childItemAsLabel = dynamic_cast<Label*>(childItem);

            if (childItemAsLabel) {
                qDebug() << childItemAsLabel->font().pointSize();
                childItem->setFlag(QGraphicsItem::ItemIgnoresTransformations, !on);
                childItemAsLabel->font().setPointSize(on ? 30 : 8);
            }
        }
    }
}
