#ifndef HRTIMER_H
#define HRTIMER_H

#include "hrtimer_global.h"

#include <QtGlobal>
#include <QObject>
#include <QTimerEvent>

#if __cplusplus >= 201103L
# include <chrono>
#endif

class HRTimerPrivateData;

class HRTIMERSHARED_EXPORT HRTimer : public QObject
{
    Q_OBJECT

    //! This boolean property is \c true if the timer is running; otherwise \c false.
    Q_PROPERTY(bool active READ isActive)

    //! This property holds the timeout interval in nanoseconds.
    Q_PROPERTY(qint64 interval READ interval WRITE setInterval)

    //! This property holds the remaining time in nanoseconds.
    Q_PROPERTY(qint64 remainingTime READ remainingTime)

    //! This property holds whether the timer is a single-shot timer.
    Q_PROPERTY(bool singleShot READ isSingleShot WRITE setSingleShot)

    //! This property controls the accuracy of the timer.
    Q_PROPERTY(Qt::TimerType timerType READ timerType)

public:
    //! Constructs a timer with the given \p parent.
    explicit HRTimer(QObject *parent = Q_NULLPTR);

    //! Destroys the timer.
    virtual ~HRTimer();

    //! Returns \c true if the timer is running (pending); otherwise returns \c false.
    bool isActive() const;

    //! Returns the timeout interval in nanoseconds.
    qint64 interval() const;

    //! Set the timeout interval to \p nsec nanoseconds.
    void setInterval(qint64 nsec);

    //! Returns the remaining time in nanoseconds.
    qint64 remainingTime() const;

    //! Returns \c true if the timer is single shot; otherwise returns \c false.
    bool isSingleShot() const;

    //! Set single shot property of the timer.
    void setSingleShot(bool singleShot);

    //! Return the accuracy of the timer.
    Qt::TimerType timerType() const;

#if __cplusplus >= 201103L
    //! Returns the interval of this timer as a \c std::chrono::nanoseconds object.
    std::chrono::nanoseconds intervalAsDuration() const;

    //! Returns the time remaining in this timer object as a \c std::chrono::nanoseconds object.
    //! If this timer is due or overdue, the returned value is \c std::chrono::nanoseconds::zero().
    //! If the remaining time could not be found or the timer is not active, this function returns a negative duration.
    std::chrono::nanoseconds remainingTimeAsDuration() const;
#endif

public slots:
    //! Starts or restarts the timer with a timeout interval of \p nsec nanoseconds.
    void start(qint64 nsec);

    //! Starts or restarts the timer with the timeout specified in ::interval.
    void start();

#if __cplusplus >= 201103L
    //! Starts or restarts the timer with a timeout of duration \p nsec nanoseconds.
    void start(std::chrono::nanoseconds nsec);
#endif

    //! Stops the timer.
    void stop();

signals:
    //! This signal is emitted when the timer times out.
    void timeout();

private:
    //! Platform specific data.
    HRTimerPrivateData *priv;
};

#endif // HRTIMER_H
