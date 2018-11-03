#include "hrtimer.h"
#include "hrtimerimpl_linux.h"


HRTimer::HRTimer(Qt::TimerType atype, QObject *parent)
    : QObject(parent), impl(new HRTimerImpl(atype, this))
{
    Q_ASSERT(this->impl);
}

HRTimer::~HRTimer()
{
    delete this->impl;
}

bool HRTimer::isActive() const
{
    return this->impl->isActive();
}

qint64 HRTimer::interval() const
{
    return this->impl->ts.it_value.tv_sec * Q_INT64_C(1000000000) + this->impl->ts.it_value.tv_nsec;
}

void HRTimer::setInterval(qint64 nsec)
{
    this->impl->ts.it_value.tv_sec = nsec / 1000000000L;
    this->impl->ts.it_value.tv_nsec = nsec % 1000000000L;

    if (!this->impl->singleShot) {
        this->impl->ts.it_interval = this->impl->ts.it_value;
    }
}

qint64 HRTimer::remainingTime() const
{
    return this->impl->remainingTime();
}

bool HRTimer::isSingleShot() const
{
    return this->impl->singleShot;
}

void HRTimer::setSingleShot(bool singleShot)
{
    if (singleShot) {
        this->impl->ts.it_interval.tv_sec = 0;
        this->impl->ts.it_interval.tv_nsec = 0;
    } else {
        this->impl->ts.it_interval = this->impl->ts.it_value;
    }

    this->impl->singleShot = singleShot;
}

Qt::TimerType HRTimer::timerType() const
{
    return this->impl->timerType();
}

#if __cplusplus >= 201103L
std::chrono::nanoseconds HRTimer::intervalAsDuration() const
{
    return std::chrono::nanoseconds(this->interval());
}

std::chrono::nanoseconds HRTimer::remainingTimeAsDuration() const
{
    return std::chrono::nanoseconds(this->remainingTime());
}

void HRTimer::start(std::chrono::nanoseconds nsec)
{
    this->setInterval(nsec.count());
    this->impl->start();
}
#endif

void HRTimer::start(qint64 nsec)
{
    this->setInterval(nsec);
    this->impl->start();
}

void HRTimer::start()
{
    this->impl->start();
}

void HRTimer::stop()
{
    this->impl->stop();
}
