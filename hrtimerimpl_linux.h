#ifndef HRTIMERIMPL_LINUX_H
#define HRTIMERIMPL_LINUX_H

#include "hrtimer.h"

#include <QObject>

#include <ctime>
#include <unistd.h>
#include <sys/timerfd.h>


class HRTimerImpl : public QObject
{
    Q_OBJECT

    explicit HRTimerImpl(Qt::TimerType atype, HRTimer *parent);
    ~HRTimerImpl();

    //! Clock used by the timer.
    clockid_t clockid;

    //! Time configuration of the timer.
    struct itimerspec ts;

    //! File descriptor of the timer.
    int fd;

    //! Single shot flag.
    bool singleShot;

public:
    //! Implementation of HRTimer::isActive().
    bool isActive() const;

    //! Implementation of HRTimer::remainingTime().
    qint64 remainingTime() const;

    //! Implementation of HRTimer::timerType().
    Qt::TimerType timerType() const;

    //! Implementation of HRTimer::start().
    void start();

    //! Implementation of HRTimer::stop().
    void stop();

private slots:
    //! Slot called on timer expiration.
    void onActivation(int fd);

    friend class HRTimer;
};

#endif // HRTIMERIMPL_LINUX_H
