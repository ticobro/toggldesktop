#include "feedbackdialog.h"
#include "ui_feedbackdialog.h"

#include <QFileDialog>
#include <QFileInfo>

#include "toggl_api.h"

FeedbackDialog::FeedbackDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FeedbackDialog)
{
    ui->setupUi(this);

    ui->topic->addItem("Bug report");
    ui->topic->addItem("Feature request");
    ui->topic->addItem("Other");
    ui->topic->setCurrentIndex(0);
}

FeedbackDialog::~FeedbackDialog()
{
    delete ui;
}

void FeedbackDialog::on_uploadImageButton_clicked()
{
    path = QFileDialog::getOpenFileName(this, "Select an image to upload");
    QFileInfo info(path);
    ui->selectedImageFilename->setText(info.fileName());
}

void FeedbackDialog::on_sendButton_clicked()
{
    QString content = ui->content->toPlainText();
    if (content.isEmpty())
    {
        ui->content->setFocus();
        return;
    }
    if (!TogglApi::instance->sendFeedback(ui->topic->currentText(),
                                         ui->content->toPlainText(),
                                         path))
    {
        return;
    }
    ui->topic->setCurrentIndex(0);
    ui->content->clear();
    ui->selectedImageFilename->setText("");
    path = "";
    hide();
}