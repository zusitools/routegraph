#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "trackview.h"
#include "trainmanager.h"
#include "model/route.h"
#include "graphicsitems/trainitem.h"
#include "memread/zusimemoryreaderexception.h"

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
    void openCaptureTriggered();
    void saveCaptureTriggered();

    void textScaleTriggered(bool on);
    void showViewPointNamesTriggered(bool on);
    void showStartingPointNamesTriggered(bool on);
    void showSignalNamesTriggered(bool on);
    void showStationNamesTriggered(bool on);
    void showUnreachableTrackElementsTriggered(bool on);
    void showAutomaticRegistersTriggered(bool on);
    void showManualRegistersTriggered(bool on);
    void antiAliasingTriggered(bool on);
    void zoomInTriggered();
    void zoomOutTriggered();
    void zoomFitTriggered();

    void trainsUpdated();
    void trainUpdateException(ZusiMemoryReaderException &e);

    void simTimeSliderValueChanged(int value);

    void reconnectToZusiTriggered();
    void showTrainsTriggered(bool on);
    void showTrainNameTriggered(bool on);
    void showCurrentSpeedTriggered(bool on);
    void showTargetSignalTriggered(bool on);

    void trackViewDoubleClicked(QMouseEvent *event);

private:
    Ui::MainWindow *ui;
    Route *m_route;

    TrainManager *m_trainManager;
    QHash<uint32_t, TrainItem *> m_trainItems;

    /**
     * Is called when a key is pressed.
     */
    void keyPressEvent(QKeyEvent *event);

    /**
     * The address of the train to keep in view.
     */
    uint32_t m_trainToFollow;

    /**
     * Selects the train with the specified index in Zusi.
     */
    void selectTrain(int trainIndex);

    /**
     * Creates a new TrainManager and starts the capturing process.
     */
    void startCapture();

    /**
     * Deletes the train manager and resets the UI.
     * @param deleteTrainItems Whether to delete the train items from the scene (set to false
     * if the scene was previously cleared.)
     */
    void resetTrainManager(bool deleteTrainItems);

#ifdef _WIN32
    /**
     * Returns the handle of the Zusi main window
     */
    HWND getZusiWindowHandle();
#endif
};

#endif // MAINWINDOW_H
