#ifndef CWTEST_H
#define CWTEST_H

#include <QMainWindow>
#include <QDialog>

#include "morse.h"

namespace Ui {
  class cwTest;
}

class cwTest : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit cwTest(QWidget *parent = 0);
  ~cwTest();

private:
  Ui::cwTest *ui;
  Morse* morse;
  void keyPressEvent(QKeyEvent *event);

private slots:
    void on_pushButton_clicked();
    void on_actionCwModule_triggered();
    void on_spinBox_wpm_valueChanged(int arg1);
    void on_spinBox_freq_valueChanged(int arg1);
};

#endif // CWTEST_H
