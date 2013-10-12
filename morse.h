#ifndef MORSE_H
#define MORSE_H

#include <QDialog>
#include <QSettings>
#include <QAudioFormat>
#include <QTimer>
#include <QObject>

#include "generator.h"
#include "sendelements.h"

//enum {_ditsp, _dahsp, _elesp, _ltrsp, _wrdsp};

namespace Ui {
  class Morse;
}

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
  void pullTimerExpired();

private:
  Ui::Morse *ui;

  int keyState;
  int durationMsec;
  QTimer*          m_pullTimer;
  QAudioDeviceInfo m_device;
  Generator*       m_generator;
  QAudioOutput*    m_audioOutput;
  QIODevice*       m_output; // not owned
  QAudioFormat     m_format;
  QByteArray       xferBuf; //This buffer is used to transfer tone to audio stream
  int              xferBufSize; //Needs to be bigger than periodSize + \0 terminator
// Private Functions
  void initializeAudio();
  int sendBuffer(int editBox);
signals:
  void doWork(QString, unsigned long);
};

#endif // MORSE_H
