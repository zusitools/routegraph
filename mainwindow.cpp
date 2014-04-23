#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QSettings>
#include <QDateTime>
#include <time.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include "windowserrorhelper.h"
#include "memread/zusimemoryreaderwindows.h"
#endif

#include "model/route.h"
#include "model/trackelement.h"
#include "graphicsitems/viewpoint.h"
#include "signal.h"
#include "graphicsitems/label.h"
#include "memread/zusimemoryreaderexception.h"
#include "model/fahrstrassesegment.h"
#include "graphicsitems/fahrstrassesegmentitem.h"
#include "savecapturedialog.h"
#include "utils.h"

#define CAPTURE_FILE_MAGIC 0x20051

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_route(NULL),
    m_trainManager(NULL),
    m_trainItems(),
    m_registerSets(NULL),
    m_trainToFollow(0)
{
    ui->setupUi(this);

    // Read settings
    QSettings settings;
    ui->actionAntiAliasing->setChecked(settings.value("view/antiAliasing", true).toBool());
    ui->actionShowSignalNames->setChecked(settings.value("view/showSignalNames", true).toBool());
    ui->actionShowStartingPointNames->setChecked(settings.value("view/showStartingPointNames", false).toBool());
    ui->actionShowViewPointNames->setChecked(settings.value("view/showViewPointNames", false).toBool());
    ui->actionShowStationNames->setChecked(settings.value("view/showStationNames", ui->actionShowStationNames->isChecked()).toBool());
    ui->actionShowUnreachableTrackElements->setChecked(settings.value("view/showUnreachableTrackElements", ui->actionShowUnreachableTrackElements->isChecked()).toBool());
    ui->actionShowAutomaticRegisters->setChecked(settings.value("view/showAutomaticRegisters", ui->actionShowAutomaticRegisters->isChecked()).toBool());
    ui->actionShowManualRegisters->setChecked(settings.value("view/showManualRegisters", ui->actionShowManualRegisters->isChecked()).toBool());
    ui->actionTextScaling->setChecked(settings.value("view/textScaling", false).toBool());

    ui->actionShowTrains->setChecked(settings.value("trains/showTrains", ui->actionShowTrains->isChecked()).toBool());
    ui->actionShowTrainName->setChecked(settings.value("trains/showTrainName", ui->actionShowTrainName->isChecked()).toBool());
    ui->actionShowCurrentSpeed->setChecked(settings.value("trains/showCurrentSpeed", ui->actionShowCurrentSpeed->isChecked()).toBool());
    ui->actionShowTargetSignal->setChecked(settings.value("trains/showTargetSignal", ui->actionShowTargetSignal->isChecked()).toBool());

    this->resize(settings.value("mainwindow/size", this->size()).toSize());

    // Hide items that do nothing
    ui->actionReconnectToZusi->setVisible(false);

    // Sim time label, slider and menu are only visible after loading a capture
    ui->simTimeSlider->setVisible(false);
    ui->simTimeLabel->setVisible(false);
    ui->menu_Trains->menuAction()->setVisible(false);

    // RegisterFiles slider is only visible after loading a capture
    ui->registerFilesSlider->setVisible(false);
}

MainWindow::~MainWindow()
{
    // Save settings
    QSettings settings;
    settings.setValue("view/antiAliasing", ui->actionAntiAliasing->isChecked());
    settings.setValue("view/showSignalNames", ui->actionShowSignalNames->isChecked());
    settings.setValue("view/showStartingPointNames", ui->actionShowStartingPointNames->isChecked());
    settings.setValue("view/showViewPointNames", ui->actionShowViewPointNames->isChecked());
    settings.setValue("view/showStationNames", ui->actionShowStationNames->isChecked());
    settings.setValue("view/showUnreachableTrackElements", ui->actionShowUnreachableTrackElements->isChecked());
    settings.setValue("view/showAutomaticRegisters", ui->actionShowAutomaticRegisters->isChecked());
    settings.setValue("view/showManualRegisters", ui->actionShowManualRegisters->isChecked());
    settings.setValue("view/textScaling", ui->actionTextScaling->isChecked());
    settings.setValue("trains/showTrains", ui->actionShowTrains->isChecked());
    settings.setValue("trains/showTrainName", ui->actionShowTrainName->isChecked());
    settings.setValue("trains/showCurrentSpeed", ui->actionShowCurrentSpeed->isChecked());
    settings.setValue("trains/showTargetSignal", ui->actionShowTargetSignal->isChecked());

    settings.setValue("mainwindow/size", this->size());

    delete ui;

    if (m_route) {
        delete m_route;
    }

    if (m_registerSets) {
        qDeleteAll(*m_registerSets);
        delete m_registerSets;
    }
}

void MainWindow::fileOpenTriggered()
{
    // Read base path from registry
    QSettings settings("HKEY_CURRENT_USER\\Software\\Zusi", QSettings::NativeFormat);
    QVariant basePath = settings.value("ZusiDir");
    QString openDialogPath;

    if (!basePath.isNull()) {
        openDialogPath = QDir::fromNativeSeparators(basePath.toString()) + "/Strecken";
    }

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open route"), openDialogPath, QString(tr("Route files (*.str *.STR)")));

    if (fileName.isNull()) {
        return;
    }

    ui->actionReconnectToZusi->setEnabled(false);
    ui->actionOpenCapture->setEnabled(false);
    ui->actionSaveCapture->setEnabled(false);
    ui->actionOpenRegister->setEnabled(false);

    // Remove and delete train items
    if (ui->trackView->scene()) {
        foreach (TrainItem *trainItem, m_trainItems.values()) {
            ui->trackView->scene()->removeItem(trainItem);
        }
    }

    qDeleteAll(m_trainItems.values());
    m_trainItems.clear();

    // Clear route
    if (m_route) {
        ui->trackView->clearRoute();
        delete m_route;
        m_route = NULL;
    }

    try {
        m_route = new Route(fileName);
    } catch (QString error) {
        QMessageBox::critical(this, tr("Error"), error);
        return;
    }

    ui->trackView->setRoute(m_route);
    this->setWindowTitle(QDir::toNativeSeparators(fileName));
    ui->actionReconnectToZusi->setEnabled(true);
    ui->actionOpenCapture->setEnabled(true);
    ui->actionOpenRegister->setEnabled(true);

    showViewPointNamesTriggered(ui->actionShowViewPointNames->isChecked());
    showStartingPointNamesTriggered(ui->actionShowStartingPointNames->isChecked());
    showSignalNamesTriggered(ui->actionShowSignalNames->isChecked());
    showStationNamesTriggered(ui->actionShowStationNames->isChecked());
    showUnreachableTrackElementsTriggered(ui->actionShowUnreachableTrackElements->isChecked());
    showAutomaticRegistersTriggered(ui->actionShowAutomaticRegisters->isChecked());
    showManualRegistersTriggered(ui->actionShowManualRegisters->isChecked());

    textScaleTriggered(ui->actionTextScaling->isChecked());
    antiAliasingTriggered(ui->actionAntiAliasing->isChecked());

    ui->trackView->fitInView(ui->trackView->sceneRect(), Qt::KeepAspectRatio);

    resetTrainManager(false);
    startCapture();
}

void MainWindow::openRegisterTriggered()
{
    // Retrieve file names from open dialog.
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open register occupation file(s)"), "", tr("Register occupation files (*Register.txt)"));

    if (fileNames.isEmpty()) {
        return;
    }

    qSort(fileNames);

    QList<QSet<int>* > *registerSets = new QList<QSet<int>* >();

    foreach(QString fileName, fileNames)
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QTextStream in(&file);
        in.setCodec("ISO 8859-1");

        /* QString title1 = */ in.readLine(); // "Belegte Register"
        /* QString tableHeader = */ in.readLine(); // "Nr. \t Streckenelement-Nr"
        QSet<int> *occupiedRegisters = new QSet<int>;

        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList lineElements = line.split("\t", QString::KeepEmptyParts);
            if (lineElements.isEmpty())
            {
                continue;
            }
            bool ok = true;
            int reg = lineElements.at(0).toInt(&ok);
            if (!ok)
            {
                continue;
            }
            if (!occupiedRegisters->contains(reg))
            {
                occupiedRegisters->insert(reg);
            }
        }
        file.close();

        registerSets->append(occupiedRegisters);
    }

    if (m_registerSets) {
        qDeleteAll(*m_registerSets);
        delete m_registerSets;
    }
    m_registerSets = registerSets;

    ui->registerFilesSlider->setVisible(registerSets->count() > 1);
    ui->registerFilesSlider->setMaximum(registerSets->count() - 1);
    ui->registerFilesSlider->setValue(0);
    registerFilesSliderValueChanged(ui->registerFilesSlider->value());
}

void MainWindow::openCaptureTriggered()
{
    // Retrieve file name from open dialog
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open capture file"), "", tr("Capture files (*.zusicapture *.ZUSICAPTURE)"));

    if (fileName.isNull()) {
        return;
    }

    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    // Read the serialized data from the file.
    QDataStream in(&file);

    // Read and check the header
    quint32 magic;
    in >> magic;
    if (magic != CAPTURE_FILE_MAGIC) {
        QMessageBox::critical(this, tr("Error"), tr("Wrong file format"));
        return;
    }

    // Read the version
    qint32 version;
    in >> version;

    if (version != 1) {
        QMessageBox::critical(this, tr("Error"), tr("Unsupported capture format version: %1").arg(version));
        return;
    }

    TrainManager *tm = new TrainManager(m_route);

    try {
        tm->loadFromFile(in);
    } catch (QString &error) {
        if (QMessageBox::critical(this, tr("Error"), error, QMessageBox::Abort, QMessageBox::Ignore) == QMessageBox::Ignore) {
            file.reset();
            in.skipRawData(2 * sizeof(qint32)); // skip magic and version again
            tm->loadFromFile(in, true);
        } else {
            return;
        }
    }

    // Display the trains instead of the currently loaded trains.
    resetTrainManager(true);
    this->m_trainManager = tm;
    ui->actionSaveCapture->setEnabled(true);

    // Display UI for handling captures
    ui->simTimeSlider->setVisible(true);
    ui->simTimeLabel->setVisible(true);
    ui->menu_Trains->menuAction()->setVisible(true);

    ui->simTimeSlider->setMinimum(m_trainManager->minSimTime());
    ui->simTimeSlider->setMaximum(m_trainManager->maxSimTime());
    ui->simTimeSlider->setValue(m_trainManager->maxSimTime());
}

void MainWindow::saveCaptureTriggered()
{
    // Retrieve file name from save dialog
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save capture file"), "", tr("Capture files (*.zusicapture *.ZUSICAPTURE)"));

    if (fileName.isNull()) {
        return;
    }

    // Ask for part of capture to save
    SaveCaptureDialog dialog(this, ui->simTimeSlider->minimum(), ui->simTimeSlider->maximum());
    if (dialog.exec() == QDialog::Rejected) {
        return;
    }

    // Serialize the current train data data to the selected file.
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    // Write header with magic number and version
    out << (quint32)CAPTURE_FILE_MAGIC;
    out << (qint32)1;

    // Write train data
    m_trainManager->writeToFile(out, dialog.lowerValue(), dialog.upperValue());
}

void MainWindow::textScaleTriggered(bool on)
{
    foreach (TrainItem *trainItem, m_trainItems.values()) {
        ui->trackView->triggerTextScale(trainItem->label(), on);
    }

    ui->trackView->setTextScaling(on);
}

void MainWindow::showViewPointNamesTriggered(bool on)
{
    ui->trackView->setViewPointNamesVisible(on);
}

void MainWindow::showStartingPointNamesTriggered(bool on)
{
    ui->trackView->setStartingPointNamesVisible(on);
}

void MainWindow::showSignalNamesTriggered(bool on)
{
    ui->trackView->setSignalNamesVisible(on);
}

void MainWindow::showStationNamesTriggered(bool on)
{
    ui->trackView->setStationNamesVisible(on);
}

void MainWindow::showUnreachableTrackElementsTriggered(bool on)
{
    ui->trackView->setUnreachableTrackSegmentsVisible(on);
}

void MainWindow::showAutomaticRegistersTriggered(bool on)
{
    ui->trackView->setAutomaticRegistersVisible(on);
}

void MainWindow::showManualRegistersTriggered(bool on)
{
    ui->trackView->setManualRegistersVisible(on);
}

void MainWindow::antiAliasingTriggered(bool on)
{
    if (on) {
        ui->trackView->setRenderHints(ui->trackView->renderHints() | QPainter::Antialiasing);
    } else {
        ui->trackView->setRenderHints(ui->trackView->renderHints() & ~QPainter::Antialiasing);
    }
}

void MainWindow::zoomInTriggered()
{
    ui->trackView->zoomIn();
}

void MainWindow::zoomOutTriggered()
{
    ui->trackView->zoomOut();
}

void MainWindow::zoomFitTriggered()
{
    ui->trackView->fitInView(ui->trackView->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::trainsUpdated()
{
    bool isLive = ui->simTimeSlider->value() == ui->simTimeSlider->maximum();
    int oldMaximum = ui->simTimeSlider->maximum();

    ui->simTimeSlider->setMaximum(m_trainManager->maxSimTime());
    ui->simTimeSlider->setMinimum(m_trainManager->minSimTime());

    if (isLive) {
        if (m_trainManager->maxSimTime() == oldMaximum) {
            simTimeSliderValueChanged(ui->simTimeSlider->value());
        } else {
            ui->simTimeSlider->setValue(ui->simTimeSlider->maximum());
        }
    }
}

void MainWindow::trainUpdateException(ZusiMemoryReaderException &e)
{
    QMessageBox::critical(this, tr("Error"), tr("Error while executing the following action: \"%1\"\nMessage: \"%2\"").arg(e.action(), e.message()));
}

void MainWindow::registerFilesSliderValueChanged(int value)
{
    if ((m_registerSets->length() > value) && (value >= 0) && m_route)
        m_route->setOccupiedRegisters(m_registerSets->at(value));

}

void MainWindow::simTimeSliderValueChanged(int value)
{
    if (!m_trainManager) {
        return;
    }

    if (value != 0) {
        ui->simTimeLabel->setText(timeToDateTime(value).toString("hh:mm:ss"));
    }

    QHash<uint32_t, Train*> currentTrains = m_trainManager->trainsAt(value);

    // FIXME: Invent a more efficient way to do this.
    foreach (FahrstrasseSegment *seg, *m_route->fahrstrasseSegments()) {
        seg->setIsOccupied(false);
    }

    foreach (uint32_t trainAddr, currentTrains.keys()) {
        // If a TrainItem for this train does not exist yet, create one and insert it into the scene
        if (!m_trainItems.contains(trainAddr)) {
            StaticTrainInfo *staticTrainInfo = m_trainManager->staticTrainInfo(trainAddr);
            TrainItem *trainItem = new TrainItem(staticTrainInfo,
                                                 ui->actionShowTrainName->isChecked(),
                                                 ui->actionShowCurrentSpeed->isChecked(),
                                                 ui->actionShowTargetSignal->isChecked());
            TrackView::triggerTextScale(trainItem->label(), ui->actionTextScaling->isChecked());

            m_trainItems.insert(trainAddr, trainItem);
            ui->trackView->scene()->addItem(trainItem);
        }

        // Update the TrainItem and Fahrstrasse segments for that particular train
        if (ui->actionShowTrains->isChecked()) {
            Train *train = currentTrains.value(trainAddr);

            // Update train item only when train changed
            if (train != m_trainItems.value(trainAddr)->train()) {
                m_trainItems.value(trainAddr)->setTrain(train);
            }

            if (train != NULL && train->onTrack) {
                // Update Fahrstrasse segments
                // The last segment may be only partly occupied.
                train->occupiedFahrstrasseSegments->last()->setIsOccupied(true, m_route->trackElement(train->fahrstrasseEndElemNumber));
                for (unsigned int i = 1; i < train->occupiedFahrstrasseSegments->count(); i++) {
                    train->occupiedFahrstrasseSegments->at(i)->setIsOccupied(true);
                }

                // Center on train to follow
                if (trainAddr == m_trainToFollow) {
                    ui->trackView->centerOn(m_trainItems.value(trainAddr));
                }
            }
        }
    }
}

void MainWindow::reconnectToZusiTriggered()
{
    resetTrainManager(true);
    startCapture();
}

void MainWindow::resetTrainManager(bool deleteTrainItems)
{
    // Do not receive any more signals from train manager.
    if (m_trainManager) {
        disconnect(m_trainManager);
    }

    // If a new file was opened, deletion of the TrainItems was handled before clearing ui->trackView->scene
    if (deleteTrainItems) {
        if (ui->trackView->scene()) {
            foreach (TrainItem *trainItem, m_trainItems.values()) {
                ui->trackView->scene()->removeItem(trainItem);
            }
        }

        // Delete train items
        qDeleteAll(m_trainItems.values());
        m_trainItems.clear();
    }

    // Do not delete train manager until now because the trains' usage count might not be zero before
    // deleting the TrainItems.
    if (m_trainManager) {
        delete m_trainManager;
        m_trainManager = NULL;
    }

    ui->simTimeSlider->setMinimum(0);
    ui->simTimeSlider->setMaximum(0);
    ui->simTimeLabel->setText(tr("(Simulation time)"));
}

void MainWindow::startCapture()
{
    try {
        m_trainManager = new TrainManager(m_route);

        connect(m_trainManager, SIGNAL(trainsUpdated()), this, SLOT(trainsUpdated()));
        connect(m_trainManager, SIGNAL(trainUpdateException(ZusiMemoryReaderException&)), this, SLOT(trainUpdateException(ZusiMemoryReaderException&)));

        m_trainManager->startCapture();
        ui->actionSaveCapture->setEnabled(true);
    } catch (ZusiMemoryReaderException &e) {
        QMessageBox::critical(this, tr("Error"), tr("Error while executing the following action: \"%1\"\nMessage: \"%2\"").arg(e.action(), e.message()));
    }
}

void MainWindow::showTrainsTriggered(bool on)
{
    foreach (TrainItem *trainItem, m_trainItems.values()) {
        trainItem->setVisible(on);
    }

    // Display current train info and Fahrstrasse segment items
    simTimeSliderValueChanged(ui->simTimeSlider->value());
}

void MainWindow::showTrainNameTriggered(bool on)
{
    foreach (TrainItem *trainItem, m_trainItems.values()) {
        trainItem->setShowTrainName(on);
    }
}

void MainWindow::showCurrentSpeedTriggered(bool on)
{
    foreach (TrainItem *trainItem, m_trainItems.values()) {
        trainItem->setShowCurrentSpeed(on);
    }
}

void MainWindow::showTargetSignalTriggered(bool on)
{
    foreach (TrainItem *trainItem, m_trainItems.values()) {
        trainItem->setShowTargetSignal(on);
    }
}

void MainWindow::trackViewDoubleClicked(QMouseEvent *event)
{
    QGraphicsItem *item = ui->trackView->itemAt(event->pos());
    TrainItem *itemAsTrainItem = dynamic_cast<TrainItem*>(item);

    if (itemAsTrainItem == NULL) {
        m_trainToFollow = 0;
    } else {
        uint32_t trainAddr = itemAsTrainItem->staticTrainInfo()->address;

        if (event->modifiers().testFlag(Qt::ControlModifier)) {
            // Follow the train in the 2D view
            m_trainToFollow = trainAddr;
            ui->trackView->centerOn(itemAsTrainItem);
        } else {
            // Switch to the train in Zusi
            int trainIndex = m_trainManager->indexOfTrain(trainAddr);

            if (trainIndex != -1) {
                selectTrain(trainIndex);
            }
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
#if false //#ifdef _WIN32
    // Send F2, F9, F11 event to the Zusi window
    // does not yet work on Linux
    if (event->key() == Qt::Key_F2 || event->key() == Qt::Key_F9 || event->key() == Qt::Key_F11) {
        HWND zusiWindowHandle = getZusiWindowHandle();

        if (zusiWindowHandle == NULL) {
            return;
        }

        WORD key;
        switch (event->key()) {
            case Qt::Key_F2: key = VK_F2; break;
            case Qt::Key_F9: key = VK_F9; break;
            case Qt::Key_F11: key = VK_F11; break;
        }

        PostMessage(zusiWindowHandle, WM_KEYDOWN, key, 0);
        PostMessage(zusiWindowHandle, WM_KEYUP, key, 0);
    }
#endif

    QMainWindow::keyPressEvent(event);
}

#if false //#ifdef _WIN32
void mySendInput(INPUT &ip, WORD key, bool ctrl) {
    if (ctrl) {
        // Press the "Ctrl" key
        ip.ki.wVk = VK_CONTROL;
        ip.ki.dwFlags = 0; // 0 for key press
        SendInput(1, &ip, sizeof(INPUT));
    }

    // Press the key
    ip.ki.wVk = key;
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    // Release the key
    ip.ki.wVk = key;
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));

    if (ctrl) {
        // Release the "Ctrl" key
        ip.ki.wVk = VK_CONTROL;
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));
    }
}

HWND MainWindow::getZusiWindowHandle() {
    HWND zusiWindowHandle = FindWindow(L"TFormZusiD3DApplication", NULL);

    if (zusiWindowHandle == NULL) {
        if (GetLastError() != S_OK) {
            QMessageBox::critical(this, tr("Error"), tr("Error retrieving Zusi window handle: %1").arg(WindowsErrorHelper::errorToString(GetLastError())));
        }

        QMessageBox::critical(this, tr("Error"), tr("Could not find Zusi main window."));
        return NULL;
    }

    return zusiWindowHandle;
}

void MainWindow::selectTrain(int trainIndex)
{
    SetLastError(S_OK);
    HWND zusiWindowHandle = getZusiWindowHandle();

    // Make the Zusi window the foreground window
    SetForegroundWindow(zusiWindowHandle);
    int numTries = 0;
    while (GetForegroundWindow() != zusiWindowHandle && numTries++ < 500) {
        printf("setting foreground win\n");
        SetForegroundWindow(zusiWindowHandle);
        Sleep(10);
    }

    if (numTries >= 500) {
        QMessageBox::critical(this, tr("Error"), tr("Could not bring Zusi window to foreground."));
        return;
    }

    // Set keyboard focus to the Zusi window.
    SetFocus(zusiWindowHandle);

    // Attach to the Zusi window to be able to call GetFocus()
    // to check whether the window has the keyboard focus.
    DWORD remoteThreadId = GetWindowThreadProcessId(zusiWindowHandle, NULL);
    DWORD currentThreadId = GetCurrentThreadId();
    AttachThreadInput(remoteThreadId, currentThreadId, TRUE);

    numTries = 0;
    while (GetFocus() != zusiWindowHandle && numTries++ < 500) {
        printf("setting focus\n");
        SetFocus(zusiWindowHandle);
        Sleep(10);
    }

    AttachThreadInput(remoteThreadId, currentThreadId, FALSE);

    if (numTries >= 500) {
        QMessageBox::critical(this, tr("Error"), tr("Could not set focus to Zusi window."));
        return;
    }

    // Set up a generic keyboard event.
    // see also http://batchloaf.wordpress.com/2012/10/18/simulating-a-ctrl-v-keystroke-in-win32-c-or-c-using-sendinput/
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    // Send Ctrl+A
    mySendInput(ip, 'A', true);

    // Wait for TFormAuswahlMeinZug to appear
    HWND trainSelectionWindowHandle;
    numTries = 0;
    while ((trainSelectionWindowHandle = FindWindow(L"TFormAuswahlMeinZug", NULL)) == NULL && numTries++ < 500) {
        Sleep(10);
    }

    if (trainSelectionWindowHandle == NULL) {
        AttachThreadInput(remoteThreadId, currentThreadId, FALSE);
        QMessageBox::critical(this, tr("Error"), tr("Could not find Zusi train selection window."));
        return;
    }

    // Do not try to SetFocus(), for some reason that will break things.
    // Instead wait a bit for the train selection window to appear.
    Sleep(100);

    // Send the "Up" key three times to be sure that the focus is on the StringGrid
    for (int i = 0; i < 3; i++) {
        mySendInput(ip, VK_UP, false);
    }

    // Send Ctrl+Home to jump to the beginning of the train list
    mySendInput(ip, VK_HOME, true);

    // Send the Down key x times to select the right train
    for (int i = 0; i < trainIndex; i++) {
        mySendInput(ip, VK_DOWN, false);
    }

    // Send Enter to select the train
    mySendInput(ip, VK_RETURN, false);
}
#else
void MainWindow::selectTrain(int trainIndex)
{
    Q_UNUSED(trainIndex);
    // Does not (yet) work under Linux/Wine
}
#endif
