#include "hrtimerimpl_linux.h"

#include <QtGlobal>
#include <QSocketNotifier>

#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <sys/timerfd.h>


HRTimerImpl::HRTimerImpl(Qt::TimerType atype, HRTimer *parent)
    : QObject(parent),
      ts({{0,0},{0,0}}),
      singleShot(false)
{
    // choose the clock according to the specified precision
    switch (atype) {
    case Qt::PreciseTimer:
        this->clockid = CLOCK_REALTIME;
        break;

    case Qt::CoarseTimer:
    case Qt::VeryCoarseTimer:
        this->clockid = CLOCK_REALTIME_COARSE;
        break;

    default:
        this->clockid = CLOCK_REALTIME;
        break;
    }

    // create the timer
    this->fd = timerfd_create(this->clockid, TFD_NONBLOCK);
    Q_ASSERT(this->fd != -1);

    // create monitor for the timer
    QSocketNotifier *notifier = new QSocketNotifier(this->fd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated, this, &HRTimerImpl::onActivation);
}

HRTimerImpl::~HRTimerImpl()
{
    close(this->fd);
}

bool HRTimerImpl::isActive() const
{
    struct itimerspec ts;

    if (timerfd_gettime(this->fd, &ts) == -1)
        throw QString(strerror(errno));

    return (ts.it_value.tv_sec != 0 || ts.it_value.tv_nsec != 0);
}

qint64 HRTimerImpl::remainingTime() const
{
    struct itimerspec ts;

    if (timerfd_gettime(this->fd, &ts) == -1)
        throw QString(strerror(errno));

    return ts.it_value.tv_sec * Q_INT64_C(1000000000) + ts.it_value.tv_nsec;
}

Qt::TimerType HRTimerImpl::timerType() const
{
    struct timespec precision;

    if (clock_getres(this->clockid, &precision) == -1)
        throw QString(strerror(errno));

    if (precision.tv_sec == 0 && precision.tv_nsec <= 1000L)
        return Qt::PreciseTimer;
    else if (precision.tv_sec == 0 && precision.tv_nsec <= 1000000L)
        return Qt::CoarseTimer;
    else
        return Qt::VeryCoarseTimer;
}

void HRTimerImpl::start()
{
    if (timerfd_settime(this->fd, 0, &this->ts, NULL) == -1)
        throw QString(strerror(errno));
}

void HRTimerImpl::stop()
{
    static const struct itimerspec stopTs = { { 0, 0 }, { 0, 0 } };

    if (timerfd_settime(this->fd, 0, &stopTs, NULL) == -1)
        throw QString(strerror(errno));
}

void HRTimerImpl::onActivation(int fd)
{
    Q_UNUSED(fd)

    quint64 cnt;
    ssize_t res;

    do {
        res = read(this->fd, &cnt, sizeof(cnt));
        if (res == sizeof(cnt) && cnt > 0)
            emit qobject_cast<HRTimer*>(this->parent())->timeout(HRTimer::QPrivateSignal());
    } while (res == -1 && errno == EINTR);
}
