#include <QApplication>
#include "cwtest.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  cwTest w;
  w.show();
  
  return a.exec();
}
