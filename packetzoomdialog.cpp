#include "packetzoomdialog.h"
#include "ui_packetzoomdialog.h"
#include "define.h"
#include "utils.h"

PacketZoomDialog::PacketZoomDialog(PacketEditor* packetEditor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketZoomDialog)
{
    ui->setupUi(this);
    m_packetEditor = packetEditor;

    //==============
    //update ui ====
    ui->lineEditeSize->setText(QString::number(m_packetEditor->getSize()));
    ui->lineEditType->setText((m_packetEditor->getPacketType() == PacketEditor::PACKET_CLIENT ? TXT_UI_TABLE_PACKET_CLIENT : TXT_UI_TABLE_PACKET_SERVER));
    ui->lineEditOpcode->setText(QString::number(m_packetEditor->getOpcode()));
    TableInit();
    //==============
}

PacketZoomDialog::~PacketZoomDialog()
{
    delete ui;
}

void PacketZoomDialog::TableInit()
{
    QByteArray byteArray = m_packetEditor->getPacket();
    ui->textBrowserASCII->setText(Utils::ToASCII(byteArray));
    ui->textBrowserHex->setText(Utils::ToHexString(byteArray));
}
