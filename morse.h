#ifndef MORSE_H
#define MORSE_H

#include <QDialog>
#include <QSettings>
#include <QAudioFormat>
//#include <QBasicTimer>
//#include <QEvent>

#include "generator.h"

enum {_ditsp, _dahsp, _elesp, _ltrsp, _wrdsp};

namespace Ui {
  class Morse;
}

class Send_Elements : public QObject
{
  Q_OBJECT
public:
  explicit Send_Elements(QObject *parent = 0);
  int test;

public slots:
  void doElements(QString, unsigned long);

private:
  struct charFrame {
    unsigned char elementCount;
    unsigned char letterCode;
  };
  // Private Functions
    charFrame ascii2cw(char letter);
    void sendCW(int el_type, unsigned long elTime);   

signals:
    void cwToneOn();
    void cwToneOff();
};

class Morse : public QDialog
{
  Q_OBJECT

public:
  explicit Morse(QWidget *parent = 0);
  ~Morse();
// Public Functions
  void readSettings(QSettings* settings);
  void writeSettings(QSettings* settings);
  void keyPressEvent(QKeyEvent *event);
// Public Variables
  bool cwMode;
  int wpm;
  int toneFreq; //frequency of cw tone

private slots:
  void cwToneOn();
  void cwToneOff();

private:
  Ui::Morse *ui;

// Private Functions
  int sendBuffer(int editBox);

  QAudioFormat     m_format;
  QAudioDeviceInfo m_device;
  Generator*       m_generator;
  QAudioOutput*    m_audioOutput;
  QIODevice*       m_output; // not owned

signals:
  void doWork(QString, unsigned long);
};

#endif // MORSE_H
