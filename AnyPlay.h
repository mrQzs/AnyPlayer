
#ifndef ANYPLAY_H
#define ANYPLAY_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class AnyPlay;
}
QT_END_NAMESPACE

class VideoDecode;
class QThread;
class DecodeThread;

class AnyPlay : public QMainWindow {
  Q_OBJECT

 public:
  AnyPlay(QWidget *parent = nullptr);
  ~AnyPlay();

 public slots:
  void updateImg(QImage *img);

 protected:
  void closeEvent(QCloseEvent *e) override;

 private slots:
  void openFile();

 signals:
  void videoDecode(const QString &);
  void stopVideoDecode();

 private:
  void hideTitle();
  void OpenVideoFile(const QString &file);

 private:
  Ui::AnyPlay *ui;
  // DecodeThread *m_decodeThread;
  // QThread *m_ddthread;
  VideoDecode *m_decode;
};

#endif  // ANYPLAY_H
