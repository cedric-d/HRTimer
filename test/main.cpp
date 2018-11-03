#include <QtGlobal>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QObject>

#include <hrtimer.h>


static const long DEFAULT_INTERVAL = 500000L;

static QElapsedTimer elapsedTimer;

static void onTimeout()
{
    qint64 delta = elapsedTimer.nsecsElapsed();
    elapsedTimer.start();

    qInfo("Elapsed: %ld.%.9ld", (long)(delta / 1000000000L), (long)(delta % 1000000000L));
}

int main(int argc, char *argv[])
{
    long long interval = DEFAULT_INTERVAL;

    QCoreApplication a(argc, argv);

    HRTimer *timer = new HRTimer(Qt::PreciseTimer, &a);

    if (a.arguments().length() >= 2)
        interval = a.arguments().value(1).toLongLong();
    timer->setInterval(interval > 0 ? interval : DEFAULT_INTERVAL);

    QObject::connect(timer, &HRTimer::timeout, &onTimeout);

    timer->start();
    elapsedTimer.start();

    return a.exec();
}
