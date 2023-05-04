
#include "WriteLog.h"

#include <QAtomicInt>
#include <QDateTime>
#include <QDir>
#include <QTextStream>
#include <iomanip>
#include <iostream>

#include "GlobalVar.h"
#include "UpdateFile.h"

QString g_currentTime;
QString g_fileName;
boost::lockfree::spsc_queue<QString> logQueue{1024};
size_t fileMaxSize{1024 * 10 * 10};

WriteLog &WriteLog::getInstance() {
  static WriteLog instance;
  return instance;
}

void WriteLog::setStopFlag() { m_stopFlog.storeRelaxed(1); }

void WriteLog::updateFile() {
  g_currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd");
  g_fileName = QDir::currentPath() + "/Log/log-" + g_currentTime;
}

void WriteLog::run() {
  while (!m_stopFlog.loadAcquire()) {
    QString message;
    if (logQueue.pop(message)) {
      try {
        writeFile(message);
      } catch (const std::runtime_error &e) {
        std::cerr << "Error writing log: " << e.what() << std::endl;
      }
    } else
      QThread::usleep(10);
  }

  m_updateFile.setStopFlag();
  m_updateFile.wait();

  QString message;
  while (logQueue.pop(message)) {
    try {
      writeFile(message);
    } catch (const std::runtime_error &e) {
      std::cerr << "Error writing log: " << e.what() << std::endl;
    }
  }
}

void WriteLog::writeFile(const QString &message) {
  QTextStream stream;
  stream.setEncoding(QStringConverter::Utf8);
  QString fileName = g_fileName + ".txt";
  QFile file(fileName);

  if (file.exists()) {
    size_t fsize = file.size();
    if (fsize >= fileMaxSize) {
      file.close();
      fileName = getFile();
      file.setFileName(fileName);
    }
  }

  if (!file.open(QFileDevice::Append)) {
    file.close();
    return;
  }

  stream.setDevice(&file);
  stream << message;
  file.close();
}

QString WriteLog::getFile() {
  int i = 1;
  QString fileName = g_fileName + QString("_%1.txt").arg(i++);
  QFile file(fileName);
  while (file.exists()) {
    file.close();
    fileName = g_fileName + QString("_%1.txt").arg(i++);
    file.setFileName(fileName);
  }
  file.close();
  return fileName;
}

WriteLog::WriteLog(QObject *parent)
    : QThread{parent}, m_updateFile{UpdateFile::getInstance()} {
  g_currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd");
  g_fileName = QDir::currentPath() + "/Log/log-" + g_currentTime;
  m_updateFile.start();
  connect(&m_updateFile, SIGNAL(timeout()), this, SLOT(updateFile()));
}

WriteLog::~WriteLog() {}
