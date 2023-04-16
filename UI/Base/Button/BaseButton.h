
#ifndef BASEBUTTON_H
#define BASEBUTTON_H

#include <QPushButton>

class BaseButton : public QPushButton {
  Q_OBJECT
 public:
  explicit BaseButton(QPushButton *parent = nullptr);

 signals:
};

#endif  // BASEBUTTON_H
