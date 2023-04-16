
#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>

class LineEdit : public QLineEdit {
  Q_OBJECT
 public:
  explicit LineEdit(QLineEdit *parent = nullptr);

 signals:
};

#endif  // LINEEDIT_H
