#ifndef PACKETSPOOFINGDIALOG_H
#define PACKETSPOOFINGDIALOG_H

#include <QDialog>

namespace Ui {
class PacketSpoofingDialog;
}

class PacketSpoofingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PacketSpoofingDialog(QWidget *parent = 0);
    ~PacketSpoofingDialog();

private:
    Ui::PacketSpoofingDialog *ui;
};

#endif // PACKETSPOOFINGDIALOG_H
