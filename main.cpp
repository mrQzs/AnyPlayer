

#include <QApplication>
#include <QMetaType>

#include "AnyPlay.h"

// 不使用qt自带的QtMultimedia，使用QPainter
// 单例模式使用局部静态变量的方式，线程安全，延迟初始化，但只适用于C++11之后

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  // 注册 QImage* 类型
  qRegisterMetaType<QImage *>("QImage*");

  AnyPlay w;
  w.show();
  return a.exec();
}
