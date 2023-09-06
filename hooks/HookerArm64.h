#ifndef C_11_HOOKERARM_64_H
#define C_11_HOOKERARM_64_H


#include "Hooker.h"

namespace hooker {
    class HookerArm64 : public Hooker {
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

#if defined(__aarch64__)
//#include <sys/cachectl.h>

inline void hooker::HookerArm64::doHook(void *func, void *newAddr, void **origFunc) const {
    // Cast the function pointer to the appropriate type
    /*
    uintptr_t *targetFunc = reinterpret_cast<uintptr_t*>(func);

    // Store the original function address if requested
    if (origFunc != nullptr) {
        *origFunc = reinterpret_cast<void*>(*targetFunc);
    }

    // Calculate the new instruction to jump to the newAddr
    uintptr_t jumpInstruction = 0x58000051; // LDR X17, [PC, #8]; BR X17
    uintptr_t jumpAddress = reinterpret_cast<uintptr_t>(newAddr);

    // Write the jump instruction and the jump address
    targetFunc[0] = jumpInstruction;
    targetFunc[1] = jumpAddress;
    */
    uintptr_t *target = reinterpret_cast<uintptr_t*>(func);
    unsigned char code[] = { 0x43, 0x00, 0x00, 0x58, 0x60, 0x00, 0x3F, 0xD6 };
    memcpy(target, code, 8);
    *(void (**)())(target + 8) = newAddr;
   

/*
#ifdef cacheflush
    cacheflush((long)func,(long)func + 8, 0);
#endif */
}
#endif

#endif //C_11_HOOKERARM_64_H