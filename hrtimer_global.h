#ifndef HRTIMER_GLOBAL_H
#define HRTIMER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(HRTIMER_LIBRARY)
#  define HRTIMERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define HRTIMERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // HRTIMER_GLOBAL_H
