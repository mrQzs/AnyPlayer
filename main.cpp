

#include <QApplication>
#include <QDir>
#include <QMetaType>

#include "AnyPlay.h"
#include "Logger.hpp"

// 不使用qt自带的QtMultimedia，使用QPainter
// 单例模式使用局部静态变量的方式，线程安全，延迟初始化，但只适用于C++11之后

void initDir() {
  QString LogPath = QDir::currentPath() + "/Log";
  QDir dir(LogPath);
  if (!dir.exists()) dir.mkdir(LogPath);
}

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // 注册 QImage* 类型
  qRegisterMetaType<QImage *>("QImage*");

  initDir();

  AnyPlay w;
  w.show();
  return a.exec();
}
