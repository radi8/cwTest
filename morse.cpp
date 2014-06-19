/* File:   Morse.cpp
* Author: Graeme Jury, ZL2APV
*
* Created on 16 August 2012, 20:00
* Copyright (C)
* 2012 - Graeme Jury, ZL2APV
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/

#include "morse.h"
#include "ui_morse.h"
#include "generator.h"
#include "cwtest.h"

#include <QDebug>
#include <cctype>
#include <QThread>
#include <QSound>
#include <math.h>
#include <qendian.h>

Morse::Morse(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Morse)
{
  setGeometry(500,150,600,471);
  ui->setupUi(this);
  cwMode = false;
  toneFreq = 600;
  durationMsec = 5;
  xferBufSize = 32768; //Make it big enough for QAudioOutput.PeriodSize + \0
//  wpm = ui->spinBox_wpm->value();
  QThread *cwThread = new QThread;

  sendElements *sendEl = new sendElements;
  connect(this,SIGNAL(doWork(QString, unsigned long)),sendEl,SLOT(doElements(QString, unsigned long)));
  sendEl->moveToThread(cwThread);
  cwThread->start();
  m_pullTimer = new QTimer(this);
  initializeAudio();
  cwToneOff();
  keyState = 0;

  connect(sendEl, SIGNAL(cwToneOn()),this, SLOT(cwToneOn()));
  connect(sendEl, SIGNAL(cwToneOff()),this, SLOT(cwToneOff()));
  QSettings settings("ZL2APV", "QtRadio");
  readSettings(&settings);
}

Morse::~Morse()
{
    QSettings settings("ZL2APV", "QtRadio");
    writeSettings(&settings);

    delete ui;
}

void Morse::initializeAudio()
{
  xferBuf.resize(xferBufSize);

  m_format.setSampleRate(DataFrequencyHz);
  m_format.setChannelCount(1);
  m_format.setSampleSize(16);
  m_format.setCodec("audio/pcm");
  m_format.setByteOrder(QAudioFormat::LittleEndian);
  m_format.setSampleType(QAudioFormat::SignedInt);

  QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(m_format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        m_format = info.nearestFormat(m_format);
    }
    m_generator = new Generator(m_format, durationMsec, toneFreq, this);

    m_audioOutput = new QAudioOutput(m_device.defaultOutputDevice(), m_format, this);
    m_audioOutput->setVolume(0.75);
    m_audioOutput->setNotifyInterval(1);

    connect(m_pullTimer, SIGNAL(timeout()), SLOT(pullTimerExpired()));
//    connect(m_audioOutput, SIGNAL(notify()), SLOT(pullTimerExpired())); //todo See if I need this
//    I may need this if I want to use the QAudioOutput instead of a Qtimer for pull mode operation

//    connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));

    m_generator->start();
    m_audioOutput->setBufferSize(440*3);
    m_output = m_audioOutput->start();
    qDebug()<<"m_audioOutput->BufferSize = "<<m_audioOutput->bufferSize()
            <<", m_audioOutput->notifyInterval = "<<m_audioOutput->notifyInterval()
            <<", m_audioOutput->periodSize = "<<m_audioOutput->periodSize();
    m_pullTimer->start(1);
    m_generator->setToneFreq(1000);
}

void Morse::readSettings(QSettings *settings)
{
  settings->beginGroup("cw");
    setGeometry(settings->value("cwPosition",this->geometry()).toRect());
    ui->plainTextEdit_1->setPlainText(settings->value("cwString1","CQ CQ CQ DE ZL2APV ZL2APV ZL2APV K").toString());
    ui->plainTextEdit_2->setPlainText(settings->value("cwString2","CQ TEST DE ZL2APV ZL2APV K").toString());
    ui->plainTextEdit_3->setPlainText(settings->value("cwString3","TU QRZ DE ZL2APV").toString());
    ui->plainTextEdit_4->setPlainText(settings->value("cwString4","OP GRAEME GRAEME").toString());
    ui->plainTextEdit_5->setPlainText(settings->value("cwString5","QTH NEW PLYMOUTH  NEW PLYMOUTH").toString());
  settings->endGroup();
}

void Morse::writeSettings(QSettings *settings)
{
  settings->beginGroup("cw");
    settings->setValue("cwString1", ui->plainTextEdit_1->toPlainText());
    settings->setValue("cwString2", ui->plainTextEdit_2->toPlainText());
    settings->setValue("cwString3", ui->plainTextEdit_3->toPlainText());
    settings->setValue("cwString4", ui->plainTextEdit_4->toPlainText());
    settings->setValue("cwString5", ui->plainTextEdit_5->toPlainText());
    settings->setValue("cwPosition", this->geometry());
    settings->endGroup();
}



void Morse::keyPressEvent(QKeyEvent *event)
{
  int keyOffset;

  keyOffset = Qt::Key_F1 - 1;
//  qDebug()<<"Got here and keyOffset - "<<keyOffset;
//  if (cwMode) {
//  if ((UI::mode.getMode()==MODE_CWL)||(UI::mode.getMode()==MODE_CWU)) {
  switch (event->key()) {
    case Qt::Key_F2:
    case Qt::Key_F3:
    case Qt::Key_F4:
    case Qt::Key_F5:
    case Qt::Key_F6:
      event->accept();
      sendBuffer(event->key() - keyOffset);
      qDebug() << __FUNCTION__ << "The key pressed was F" << event->key()-keyOffset;
      break;
    }
  //  }
}

void Morse::cwToneOn()
{
  keyState = 2;
  m_audioOutput->setVolume(.5);
}

void Morse::cwToneOff()
{
    keyState = 3;
//    m_audioOutput->setVolume(0);
//    qDebug()<<"Size of buffer = "<< m_audioOutput->bufferSize();
}

int Morse::sendBuffer(int editBox)
{
  QString buff;
  unsigned long elTime;//  if (!m_generator->keyState) {
  //      m_audioOutput->setVolume(0);
  //      xferBuf.fill(0);
  //      qDebug()<<"xferBuf filled with 0";
  //    }

  elTime = 1200000/wpm;// time in nanoseconds todo temp value for cwTime remove when setup
  switch (editBox) {
    case 1:
      buff = ui->plainTextEdit->toPlainText();
      break;
    case 2:
      buff = ui->plainTextEdit_1->toPlainText();
      break;
    case 3:
      buff = ui->plainTextEdit_2->toPlainText();
      break;
    case 4:
      buff = ui->plainTextEdit_3->toPlainText();
    break;
    case 5:
      buff = ui->plainTextEdit_4->toPlainText();
    break;
    case 6:
      buff = ui->plainTextEdit_5->toPlainText();
    break;
    default:
      buff.clear();
  }
  if (buff.isEmpty()) {
    return 1;
  } else {
      emit doWork(buff, elTime);
      return 0;
  }
}

/* pullTimerExpired() is a slot accessed every few milliseconds by a QTimer firing. At each
 * call it checks the audioOutput to see if there is enough space in its internal buffer to
 * transfer another multiple of m_audioOutput->periodSize() bytes from xferBuf.
*/

void Morse::pullTimerExpired()
{
  // Omly do this if m_audioOutput is running
  if (m_audioOutput && m_audioOutput->state() != QAudio::StoppedState) {
          // Calculate how many periodSize chunks of data will fit into audioOutput
          int chunks = m_audioOutput->bytesFree()/m_audioOutput->periodSize();
          while (chunks) {
             // Here we call m_generator to load "periodSize" bytes of tone into xferBuf
             // xferBuf.data() returns char pointer to start of the transfer buffer.
                 const qint64 len = m_generator->read(xferBuf.data(), m_audioOutput->periodSize());

             // Here we check to see if tone is steady, rising, falling or off
             switch (keyState) {
               case 0: // Send silence
                 xferBuf.fill(0);
                 break;
               case 1: //Send continuous tone
                 // We do nothing to the tone in this case
                 break;
               case 2: //Send rising tone
                 short int value;
                 for (int x = 0; x < 440; x+=2) {
                     value = (int(xferBuf[x+1])<<8)&0xFF00;
                     value = (value | (int(xferBuf[x])&0x00FF))*x/442;
//                     qDebug()<<"final value = "<<value<<value*x/442;
                     xferBuf[x]=value&0x00FF;
                     xferBuf[x+1]=(value>>8)&0x00FF;
//                     qDebug()<<"final value of m_buf["<<x<<" = "<<value;
                 }
                 keyState = 1;
                 break;
               case 3: //Send falling to silence
                 for (int x = 0; x < 440; x+=2) {
                     value = (int(xferBuf[440-x+1])<<8)&0xFF00;
                     value = (value | (int(xferBuf[440-x])&0x00FF))*x/442;
//                     qDebug()<<"final value = "<<value<<value*x/442;
                     xferBuf[440-x]=value&0x00FF;
                     xferBuf[440-x+1]=(value>>8)&0x00FF;
//                     qDebug()<<"final value of m_buf["<<x<<" = "<<value;
                 }
                 for (int x = 442; x<= xferBufSize; x++) xferBuf[x] = 0;
                 for (int x =0; x<20; x+=2) {
                     qDebug()<<"The value xferBuf["<<x<<"] = "<<QString::number(xferBuf[x+1], 16)<<QString::number(xferBuf[x], 16);
                   }
                 keyState = 0;
               break;

               default:
                 xferBuf.fill(0);
               }
             if (len)
                 m_output->write(xferBuf.data(), len);
             if (len != m_audioOutput->periodSize())
                 break;
             --chunks;
          }
      }
}
