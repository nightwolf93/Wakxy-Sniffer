#include "packetspoofingdialog.h"
#include "ui_packetspoofingdialog.h"

PacketSpoofingDialog::PacketSpoofingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketSpoofingDialog)
{
    ui->setupUi(this);
}

PacketSpoofingDialog::~PacketSpoofingDialog()
{
    delete ui;
}
