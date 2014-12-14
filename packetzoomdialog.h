#ifndef PACKETZOOMDIALOG_H
#define PACKETZOOMDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include "packeteditor.h"

namespace Ui {
class PacketZoomDialog;
}

class PacketZoomDialog : public QDialog
{
    Q_OBJECT

public:
    PacketZoomDialog(PacketEditor* packetEditor, QWidget *parent = 0);
    ~PacketZoomDialog();

public slots:
    void OnReloadScript();

private:
    void UpdateUI();


    Ui::PacketZoomDialog *ui;
    PacketEditor* m_packetEditor;
};

#endif // PACKETZOOMDIALOG_H
