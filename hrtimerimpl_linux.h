#ifndef HRTIMERPRIVATEDATA_LINUX_H
#define HRTIMERPRIVATEDATA_LINUX_H

#include <QObject>

#include <ctime>
#include <unistd.h>
#include <sys/timerfd.h>


class HRTimerPrivateData : public QObject
{
    Q_OBJECT

    friend class HRTimer;

    explicit HRTimerPrivateData(QObject *parent = Q_NULLPTR);
    ~HRTimerPrivateData();

    //! Clock used by the timer.
    clockid_t clockid;

    //! Time configuration of the timer.
    struct itimerspec ts;

    //! File descriptor of the timer.
    int fd;

    //! Single shot flag.
    bool singleShot;

signals:
    //! Signal emitted on timeout.
    void timeout(QPrivateSignal);

private slots:
    void onActivation(int s);
};

#endif // HRTIMERPRIVATEDATA_LINUX_H
