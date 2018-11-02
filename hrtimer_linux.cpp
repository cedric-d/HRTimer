#include "hrtimer.h"
#include "hrtimerprivatedata_linux.h"

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cerrno>

#include <sys/timerfd.h>


HRTimer::HRTimer(QObject *parent)
    : QObject(parent), priv(new HRTimerPrivateData)
{
    Q_ASSERT(this->priv);

    connect(this->priv, &HRTimerPrivateData::timeout, this, &HRTimer::timeout);
}

HRTimer::~HRTimer()
{
    delete this->priv;
}

bool HRTimer::isActive() const
{
    struct itimerspec ts;

    if (timerfd_gettime(this->priv->fd, &ts) == -1)
        throw QString(strerror(errno));

    return (ts.it_value.tv_sec != 0 || ts.it_value.tv_nsec != 0);
}

qint64 HRTimer::interval() const
{
    return this->priv->ts.it_value.tv_sec * Q_INT64_C(1000000000) + this->priv->ts.it_value.tv_nsec;
}

void HRTimer::setInterval(qint64 nsec)
{
    this->priv->ts.it_value.tv_sec = nsec / 1000000000L;
    this->priv->ts.it_value.tv_nsec = nsec % 1000000000L;

    if (!this->priv->singleShot) {
        this->priv->ts.it_interval = this->priv->ts.it_value;
    }
}

qint64 HRTimer::remainingTime() const
{
    struct itimerspec ts;

    if (timerfd_gettime(this->priv->fd, &ts) == -1)
        throw QString(strerror(errno));

    return ts.it_value.tv_sec * Q_INT64_C(1000000000) + ts.it_value.tv_nsec;
}

bool HRTimer::isSingleShot() const
{
    return this->priv->singleShot;
}

void HRTimer::setSingleShot(bool singleShot)
{
    if (singleShot) {
        this->priv->ts.it_interval.tv_sec = 0;
        this->priv->ts.it_interval.tv_nsec = 0;
    } else {
        this->priv->ts.it_interval = this->priv->ts.it_value;
    }

    this->priv->singleShot = singleShot;
}

Qt::TimerType HRTimer::timerType() const
{
    struct timespec precision;

    if (clock_getres(this->priv->clockid, &precision) == -1)
        throw QString(strerror(errno));

    if (precision.tv_sec != 0)
        return Qt::VeryCoarseTimer;
    else if (precision.tv_nsec <= 1000000L)
        return Qt::PreciseTimer;
    else
        return Qt::CoarseTimer;
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
    this->start();
}
#endif

void HRTimer::start(qint64 nsec)
{
    this->setInterval(nsec);
    this->start();
}

void HRTimer::start()
{
    if (timerfd_settime(this->priv->fd, 0, &this->priv->ts, NULL) == -1)
        throw QString(strerror(errno));
}

void HRTimer::stop()
{
    static const struct itimerspec stopTs = { { 0, 0 }, { 0, 0 } };

    if (timerfd_settime(this->priv->fd, 0, &stopTs, NULL) == -1)
        throw QString(strerror(errno));
}
