//
// Created by lamoliu on 2017/4/1.
//

#ifndef C_11_HOOKERX64_H
#define C_11_HOOKERX64_H

#include "Hooker.h"

namespace hooker {
class HookerX64 : public Hooker {
public:
    void doHook(void *func, void *newAddr, void **origFunc) const;
    size_t getHookHeadSize() const {
        return 14;
    }
	~HookerX64() {}
};
}

#include <cstdlib>
#include <string>
#include <sys/mman.h>
#include "Hooker.h"
#include "memory.h"
#include <iostream>

#ifdef __x86_64__
inline void hooker::HookerX64::doHook(void *func,void *newAddr,void **origFunc) const {

    char *f = (char *)func;
	if (origFunc) {
		// find the return instruction retq: c3
		int index = 0;
		while (true) {
			if (static_cast<uint8_t>(f[index++]) == 0xc3 || index >= 1024) {
				break;
			}
		}

		void *old = malloc(index);
		if (old != nullptr) {
			memcpy(old, func, index);
			changeCodeAttrs(old, CODE_READ_ONLY);
			*origFunc = old;
		}
	}

    *(uint16_t *)&f[0] = 0x25ff;
    *(int *)&f[2] = 0x00000000;
    *(std::size_t *)&f[6] = (std::size_t)newAddr;
}
#endif //__x86_64__

#endif //C_11_HOOKERX64_H
