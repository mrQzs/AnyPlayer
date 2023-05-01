
#include "AnyPlay.h"

#include <ffmpegIn.h>

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QImage>
#include <Qthread>

#include "VideoDecode.h"
#include "ui_AnyPlay.h"

QImage avFrameToQImage(AVFrame *frame);

AnyPlay::AnyPlay(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::AnyPlay),
      m_decode(VideoDecode::getInstance()) {
  ui->setupUi(this);

  hideTitle();

  m_decode.start();

  connect(&m_decode, &VideoDecode::getNewImage, this, &AnyPlay::updateImg);
  connect(ui->OpenBtn, &QPushButton::clicked, this, &AnyPlay::openFile);
}

AnyPlay::~AnyPlay() {
  delete ui;
  m_decode.setStopFlag();
  m_decode.wait();
}

void AnyPlay::updateImg(QImage *img) { ui->widget->setImage(img); }

void AnyPlay::closeEvent(QCloseEvent *e) { QMainWindow::closeEvent(e); }

void AnyPlay::openFile() {
  QString fileName =
      QFileDialog::getOpenFileName(this, tr("Open Video"), QDir::currentPath(),
                                   tr("Video Files (*.mp4 *.mkv *.avi)"));

  OpenVideoFile(fileName);
}

void AnyPlay::hideTitle() {}

void AnyPlay::OpenVideoFile(const QString &file) { m_decode.setSetFile(file); }
