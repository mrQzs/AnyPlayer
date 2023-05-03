

#include <QApplication>
#include <QDir>
#include <QMetaType>

#include "AnyPlay.h"

void initDir() {
  QString LogPath = QDir::currentPath() + "/Log";
  QDir dir(LogPath);
  if (!dir.exists()) dir.mkdir(LogPath);
}

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  initDir();

  AnyPlay w;
  w.show();
  return a.exec();
}
