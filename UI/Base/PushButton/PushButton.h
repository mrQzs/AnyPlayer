
#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPushButton>

class PushButton : public QPushButton {
  Q_OBJECT
 public:
  explicit PushButton(QPushButton *parent = nullptr);

 signals:
};

#endif  // PUSHBUTTON_H
