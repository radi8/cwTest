#ifndef SENDELEMENTS_H
#define SENDELEMENTS_H

#include <QObject>
//#include <QDialog>
//#include <QSettings>
//#include <QAudioFormat>
#include <QTimer>

//#include "generator.h"
#include "morse.h"

enum {_ditsp, _dahsp, _elesp, _ltrsp, _wrdsp};

class sendElements : public QObject
{
    Q_OBJECT

public:
    explicit sendElements(QObject *parent = 0);
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

#endif // SENDELEMENTS_H
