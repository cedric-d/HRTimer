#include "hrtimerprivatedata_linux.h"

#include <QSocketNotifier>

#include <cerrno>
#include <unistd.h>


HRTimerPrivateData::HRTimerPrivateData(QObject *parent)
    : QObject(parent),
      clockid(CLOCK_REALTIME),
      ts({{0,0},{0,0}}),
      singleShot(false)
{
    // create the timer
    this->fd = timerfd_create(this->clockid, TFD_NONBLOCK);
    Q_ASSERT(this->fd != -1);

    // create monitor for the timer
    QSocketNotifier *notifier = new QSocketNotifier(this->fd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated, this, &HRTimerPrivateData::onActivation);
}

HRTimerPrivateData::~HRTimerPrivateData()
{
    close(this->fd);
}

void HRTimerPrivateData::onActivation(int fd)
{
    Q_UNUSED(fd)

    quint64 cnt;
    ssize_t res;

    do {
        res = read(this->fd, &cnt, sizeof(cnt));
        if (res == sizeof(cnt) && cnt > 0)
            emit timeout(QPrivateSignal());
    } while (res == -1 && errno == EINTR);
}
