#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "trackview.h"
#include "route.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void fileOpenTriggered();
    void textScaleTriggered(bool on);

private:
    Ui::MainWindow *ui;
    TrackView *trackView;
    Route *m_route;
};

#endif // MAINWINDOW_H
