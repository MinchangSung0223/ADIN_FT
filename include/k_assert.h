#ifndef _KASSERT_H
#define _KASSERT_H

#ifdef __linux__

#include <linux/bug.h>

// Kernel style assert without crashing
#define K_ASSERT(cond) WARN_ON(!(cond))

#else

#define K_ASSERT(cond)

#endif /* __linux__ */

#endif // _KASSERT_H
