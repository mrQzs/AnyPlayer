
#ifndef ANYPLAY_H
#define ANYPLAY_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class AnyPlay;
}
QT_END_NAMESPACE

class AnyPlay : public QMainWindow {
  Q_OBJECT

 public:
  AnyPlay(QWidget *parent = nullptr);
  ~AnyPlay();

 private:
  void hideTitle();

 private:
  Ui::AnyPlay *ui;
};

#endif  // ANYPLAY_H
