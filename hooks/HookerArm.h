//
// Created by lamoliu on 2017/4/1.
//

#ifndef C_11_HOOKERARM_H
#define C_11_HOOKERARM_H


#include "Hooker.h"

namespace hooker {
    class HookerArm : public Hooker {
    public:
        void doHook(void *func, void *newAddr, void **origFunc) const ;
        size_t getHookHeadSize() const {
            return 8;
        }
		size_t getOrigFunctionSize() const {
			return 2 * getHookHeadSize();
		}
    };
}

#ifdef __arm__
#include <sys/cachectl.h>

inline void hooker::HookerArm::doHook(void *func, void *newAddr, void **origFunc) const {
    char *f = (char *)func;
    *(long *)&f[0] = 0xe51ff004; // ldr pc, addr
    *(long *)&f[4] = (long)newAddr;

#ifdef cacheflush
    cacheflush((long)func,(long)func + 8, 0);
#endif
}
#endif


#endif //C_11_HOOKERARM_H
