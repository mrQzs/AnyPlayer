
#ifndef BASEBUTTON_H
#define BASEBUTTON_H

#include <QWidget>

class BaseButton : public QWidget {
  Q_OBJECT
 public:
  explicit BaseButton(QWidget *parent = nullptr);

 signals:
};

#endif  // BASEBUTTON_H
