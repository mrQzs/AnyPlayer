
#include "AnyPlay.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QImage>
#include <Qthread>

#include "Decode.h"
#include "GlobalVar.h"
#include "ui_AnyPlay.h"

AnyPlay::AnyPlay(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::AnyPlay), m_decode(new Decode(this)) {
  ui->setupUi(this);

  hideTitle();

  m_decode->start();

  connect(ui->OpenBtn, &QPushButton::clicked, this, &AnyPlay::openFile);
}

AnyPlay::~AnyPlay() {
  delete ui;
  m_decode->setStopFlag();
  m_decode->wait();
}

void AnyPlay::closeEvent(QCloseEvent *e) { QMainWindow::closeEvent(e); }

void AnyPlay::openFile() {
  QString fileName =
      QFileDialog::getOpenFileName(this, tr("Open Video"), QDir::currentPath(),
                                   tr("Video Files (*.mp4 *.mkv *.avi)"));

  OpenVideoFile(fileName);
}

void AnyPlay::hideTitle() {}

void AnyPlay::OpenVideoFile(const QString &file) { m_decode->setSetFile(file); }
