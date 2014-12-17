#include "packetzoomdialog.h"
#include "ui_packetzoomdialog.h"
#include "define.h"
#include "utils.h"
#include "mainwindow.h"

PacketZoomDialog::PacketZoomDialog(PacketEditor* packetEditor, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PacketZoomDialog)
{
    ui->setupUi(this);
    m_packetEditor = packetEditor;
    m_packetEditor->ReadPacket();

    //==============
    //update ui ====
    UpdateUI();
    //==============

    //==============
    //event signal
    connect(ui->pushButtonReloadScript, SIGNAL(clicked()), SLOT(OnReloadScript()));
    //===============
}


PacketZoomDialog::~PacketZoomDialog()
{
    delete ui;
}

void PacketZoomDialog::OnReloadScript()
{
    m_packetEditor->ReadPacket();
    UpdateUI();
}

void PacketZoomDialog::UpdateUI()
{
    ui->lineEditeSize->setText(QString::number(m_packetEditor->getSize()));
    ui->lineEditType->setText((m_packetEditor->getPacketType() == PacketEditor::PACKET_CLIENT ? TXT_UI_TABLE_PACKET_CLIENT : TXT_UI_TABLE_PACKET_SERVER));
    ui->lineEditOpcode->setText(QString::number(m_packetEditor->getOpcode()));
    ui->labelUsedScript->setText(TXT_UI_LABEL_SCRIPT + m_packetEditor->getScriptName());
    ui->labelNotReadByte->setText(TXT_UI_LABEL_FREE_BYTE + QString::number(m_packetEditor->getLength()));

    QByteArray byteArray = m_packetEditor->getPacket();
    ui->textBrowserASCII->setText(Utils::ToASCII(byteArray));
    ui->textBrowserHex->setText(Utils::ToHexString(byteArray));

    m_packetEditor->DisplayTree(ui->treeWidgetPacket);
}
