
#include "DecodeThread.h"

#include <ffmpegIn.h>

#include <QDebug>
#include <QImage>
#include <QThread>

DecodeThread::DecodeThread() : m_stopFlag(0) {}

DecodeThread::~DecodeThread() {}

void DecodeThread::startrun(const QString &file) {}

void DecodeThread::stop() { m_stopFlag.storeRelaxed(1); }
