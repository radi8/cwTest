#include "generator.h"

#include <QDebug>
#include <QAudioOutput>
#include <qmath.h>
#include <qendian.h>


Generator::Generator(const QAudioFormat &format,
//                     qint64 durationUs,
                     int cyclesOfTone,
                     int frequency,
                     QObject *parent)
    :   QIODevice(parent)
    ,   m_pos(0)
{
    cyclesOfTone = 3;
//    generateData(format, durationUs, frequency);
    generateData(format, cyclesOfTone, frequency);
}

Generator::~Generator()
{

}

void Generator::start()
{
    open(QIODevice::ReadOnly);
}

void Generator::stop()
{
    m_pos = 0;
    close();
}

void Generator::generateData(const QAudioFormat &format, int cyclesOfTone, int frequency)
{
    const int channelBytes = format.sampleSize() / 8;
    const int sampleBytes = format.channelCount() * channelBytes;
    qint64 durationUs;

    frequency = 600;
    durationUs = (1.0f/frequency)*cyclesOfTone * 1000000;
//    durationUs = (qint64)((1.0f/frequency)*3000000); //Build a buffer containing 3 cycles of tone

    qint64 length = (format.sampleRate() * format.channelCount() * (format.sampleSize() / 8))
                        * durationUs / 1000000;
    length &= 0x7ffffffe; //length must be divisible by 2

    qDebug()<<"Length = "<<length<<", format.sampleRate() = "<<format.sampleRate()
           <<", format.sampleSize() = "<<format.sampleSize()<<", durationUs = "<<durationUs;
    qDebug()<<"frequency = "<<frequency<<"format.channelCount()"<<format.channelCount();

    Q_ASSERT(length % sampleBytes == 0);
    Q_UNUSED(sampleBytes); // suppress warning in release builds

    m_buffer.resize(length);
    unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer.data());
    int sampleIndex = 0;
    while (length) {
        const qreal x = qSin(2 * M_PI * frequency * qreal(sampleIndex % format.sampleRate()) / format.sampleRate());
        for (int i=0; i<format.channelCount(); ++i) {
            if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::UnSignedInt) {
                const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
                *reinterpret_cast<quint8*>(ptr) = value;
            } else if (format.sampleSize() == 8 && format.sampleType() == QAudioFormat::SignedInt) {
                const qint8 value = static_cast<qint8>(x * 127);
                *reinterpret_cast<quint8*>(ptr) = value;
            } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::UnSignedInt) {
                quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    qToLittleEndian<quint16>(value, ptr);
                else
                    qToBigEndian<quint16>(value, ptr);
            } else if (format.sampleSize() == 16 && format.sampleType() == QAudioFormat::SignedInt) {
                qint16 value = static_cast<qint16>(x * 32767);
                if (format.byteOrder() == QAudioFormat::LittleEndian)
                    qToLittleEndian<qint16>(value, ptr);
                else
                    qToBigEndian<qint16>(value, ptr);
            }

            ptr += channelBytes;
            length -= channelBytes;
        }
        ++sampleIndex;
    }
    qDebug()<<"sampleIndex = "<<sampleIndex;
}

qint64 Generator::readData(char *data, qint64 len)
{
    qint64 total = 0;
    while (len - total > 0) {
        const qint64 chunk = qMin((m_buffer.size() - m_pos), len - total);
        memcpy(data + total, m_buffer.constData() + m_pos, chunk);
        m_pos = (m_pos + chunk) % m_buffer.size();
        total += chunk;
    }
    return total;
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 Generator::bytesAvailable() const
{
  return m_buffer.size() + QIODevice::bytesAvailable();
}

void Generator::calcToneDuration(int mSec)
{
  samples = (DataFrequencyHz * mSec) / 1000;
  samples &= 0x7ffffffe;
//  m_pos = m_buffer;
}

void Generator::setToneFreq(int freq)
{
//  if (buffer)
//          delete[] buffer;
//  freq = frequency;

  const int upper_freq = 1200;
  const int full_waves = 3;

  // Arbitrary upper frequency
  if (freq > upper_freq)
          freq = upper_freq;

//  generateData(, full_waves, freq); //todo work out how to send to generateData

  // We create a buffer with some full waves of freq,
  // therefore we need room for this many samples:
//  int buflen = DataFrequencyHz * full_waves / freq; //DataFrequencyHz = sampleRate

//  MYVERBOSE("buf needs to hold %d samples", buflen);

//  m_buffer.resize(buflen);

  // Now fill this buffer with the sine wave
//  QByteArray t = m_buffer;
//  for (int i = 0; i < buflen; i++) {
//          int value = 32767.0 * sin(M_PI * 2 * i * freq / DataFrequencyHz);
//          MYVERBOSE("%4d: %6d, pos %d", i, value, t - buffer);
//          t++ = value;
//  }

//  sendpos = m_buffer;
//  end = m_buffer.size();
//  samples = 0;
}