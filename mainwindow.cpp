#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

#include "route.h"
#include "trackelement.h"
#include "viewpoint.h"

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
    /*QFileDialog fileOpenDialog(this);
    fileOpenDialog.setNameFilter("*.str *.STR");
    fileOpenDialog.setOption(QFileDialog::HideNameFilterDetails);

    if (fileOpenDialog.exec() != QDialog::Accepted) {
        return;
    }*/

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

    m_route = new Route(fileName);
    this->setWindowTitle(fileName);

    QGraphicsScene* scene = new QGraphicsScene(this);
    trackView->setScene(scene);

    foreach (QGraphicsItem* pe, *(m_route->trackSegments())) {
        trackView->scene()->addItem(pe);
    }

    foreach (ViewPoint* vp, *(m_route->viewPoints())) {
        trackView->scene()->addItem(vp);
    }

    trackView->scale(0.1, 0.1);
    foreach (StartingPoint* sp, *(m_route->startingPoints())) {
        trackView->scene()->addItem(sp);
    }
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
