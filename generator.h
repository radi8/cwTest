#ifndef GENERATOR_H
#define GENERATOR_H

#include <QIODevice>
#include <QAudioOutput>

const int DurationSeconds = 1;
const int DataFrequencyHz = 44100;
//const int BufferSize      = 32768;
const int ToneFrequencyHz = 600;

class Generator : public QIODevice
{
    Q_OBJECT
public:
    Generator(const QAudioFormat &format, int cyclesOfTone, int frequency, QObject *parent);//qint64 durationUs
    ~Generator();

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
    qint64 bytesAvailable() const;
    void  calcToneDuration(int mSec);
    void  setToneFreq(int freq);

private:
    void generateData(const QAudioFormat &format, int cyclesOfTone, int frequency); //
    void generateData1(const QAudioFormat &format, int cyclesOfTone, int frequency);
private:
    QByteArray  m_buffer; // Same as buffer (I think) todo
    qint64      m_pos;    // Current pos into the circular buffer (sendpos) todo
    int         freq;
    int         *end;     //!< \brief Last position in \ref buffer, for faster comparison
    int         samples;  //!< \brief Samples to play for desired sound duration
};


#endif // GENERATOR_H
