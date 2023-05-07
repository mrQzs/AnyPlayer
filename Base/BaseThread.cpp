
#include "BaseThread.h"

BaseThread::BaseThread(QObject *parent) : QThread{parent}, m_stopFlag{0} {}

void BaseThread::setStopFlag() { m_stopFlag.storeRelaxed(1); }

void BaseThread::setStartFlag() { m_stopFlag.storeRelaxed(0); }
