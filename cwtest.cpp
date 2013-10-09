#include "cwtest.h"
#include "ui_cwtest.h"
#include "morse.h"

#include <QDebug>
#include <QtCore>
#include <QKeyEvent>
#include <QWidget>

cwTest::cwTest(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::cwTest)
{
  ui->setupUi(this);

  setGeometry(50,150,400,300);
  morse = new Morse(this); //CW settings has UI as parent so it closes with UI close
  morse->wpm = ui->spinBox_wpm->value();
  qDebug() << __FUNCTION__ << "the value of wpm is " << morse->wpm;
}

cwTest::~cwTest()
{
//  QSettings settings("ZL2APV", "QtRadio");

//  morse->writeSettings(&settings);
  delete ui;
}

void cwTest::keyPressEvent(QKeyEvent *event)
{
  if (morse->isVisible()) {
    switch (event->key()) {
      case Qt::Key_F2:
      case Qt::Key_F3:
      case Qt::Key_F4:
      case Qt::Key_F5:
      case Qt::Key_F6:
      qDebug()<<"event->key ="<< event->key();
//        morse->keyPressEvent(event);
      morse->keyPressEvent(event);
    }
  }
}

void cwTest::on_pushButton_clicked()
{

}

void cwTest::on_actionCwModule_triggered()
{
//  QSettings settings("ZL2APV", "QtRadio");

//  qDebug() << __FUNCTION__ << "arrived at on_actionCwModule_triggered()";
  morse->setFixedSize(600, 471);
//  morse->readSettings(&settings);
  morse->show();
}

void cwTest::on_spinBox_wpm_valueChanged(int arg1)
{
    morse->wpm = arg1;
    qDebug() << __FUNCTION__ << "the value of wpm after spin box change is " << morse->wpm;
}

void cwTest::on_spinBox_freq_valueChanged(int arg1)
{
    morse->toneFreq = arg1;
    qDebug() << __FUNCTION__ << "the value of toneFreq after spin box change is " << morse->toneFreq;
}
