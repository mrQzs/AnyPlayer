
#include <QApplication>

#include "AnyPlay.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  AnyPlay w;
  w.show();
  return a.exec();
}
