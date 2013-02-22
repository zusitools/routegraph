#ifndef SAVECAPTUREDIALOG_H
#define SAVECAPTUREDIALOG_H

#include <QDialog>

namespace Ui {
class SaveCaptureDialog;
}

class SaveCaptureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveCaptureDialog(QWidget *parent = 0, time_t minSimTime = 0, time_t maxSimTime = 0);
    ~SaveCaptureDialog();

    /**
     * Returns the lower value of the slider.
     */
    time_t lowerValue() const;

    /**
     * Returns the upper value of the slider.
     */
    time_t upperValue() const;

public slots:
    void simTimeSliderLowerValueChanged(int value);
    void simTimeSliderUpperValueChanged(int value);

private:
    Ui::SaveCaptureDialog *ui;
};

#endif // SAVECAPTUREDIALOG_H
