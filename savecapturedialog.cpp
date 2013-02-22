#include "savecapturedialog.h"
#include "ui_savecapturedialog.h"

#include "utils.h"

SaveCaptureDialog::SaveCaptureDialog(QWidget *parent, time_t minSimTime, time_t maxSimTime) :
    QDialog(parent),
    ui(new Ui::SaveCaptureDialog)
{
    ui->setupUi(this);

    ui->simTimeSlider->setMinimum(minSimTime);
    ui->simTimeSlider->setLowerPosition(minSimTime);

    ui->simTimeSlider->setMaximum(maxSimTime);
    ui->simTimeSlider->setUpperPosition(maxSimTime);
}

SaveCaptureDialog::~SaveCaptureDialog()
{
    delete ui;
}

time_t SaveCaptureDialog::lowerValue() const
{
    return ui->simTimeSlider->lowerValue();
}

time_t SaveCaptureDialog::upperValue() const
{
    return ui->simTimeSlider->upperValue();
}

void SaveCaptureDialog::simTimeSliderLowerValueChanged(int value)
{
    ui->startTimeLabel->setText(timeToDateTime(value).toString("hh:mm:ss"));
}

void SaveCaptureDialog::simTimeSliderUpperValueChanged(int value)
{
    ui->endTimeLabel->setText(timeToDateTime(value).toString("hh:mm:ss"));
}
