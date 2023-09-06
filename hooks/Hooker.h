//
// Created by lamoliu on 2017/4/1.
//

#ifndef C_11_HOOKER_H
#define C_11_HOOKER_H

#include <cstdint>
#include <cstddef>
#include <unordered_map>

#include <stdexcept>
#include <errno.h>
#include <string>
#include <cstring>

namespace hooker {
	namespace error {
		class HookerError : public std::logic_error {
		public:
			HookerError(std::string msg) : std::logic_error(errno != 0 ? std::string(msg) + ": " + strerror(errno) : msg) {}
		};
	}
}

namespace hooker {
#define PAGE_START(x,pagesize)	((x) &~ ((pagesize) - 1))
#define CODE_WRITE				PROT_READ | PROT_WRITE | PROT_EXEC
#define CODE_READ_ONLY			PROT_READ | PROT_EXEC

    class Hooker {
    public:
        void changeCodeAttrs(void *func, int attr) const;
        void hook(void *func, void *newAddr, void **origFunc, bool saveOrig = true) const;
        void unhook(void *func, void *oldFunc = nullptr) const;
        virtual size_t getHookHeadSize() const = 0;

		// when construct the origin function, it's best to know
		// how many bytes should we allocate, so when we just use
		// the origin function as normal.
		virtual size_t getOrigFunctionSize() const {
			return getHookHeadSize();
		}

		virtual ~Hooker() = 0;

    protected:
        virtual void doHook(void *func, void *newAddr, void **origFunc) const = 0;
        virtual void doUnHook(void *func, void *oldFunc = nullptr) const;
        mutable std::unordered_map<std::size_t,std::size_t> gHookedMap;
    private:
        void saveOriginFuncBytes(void *func) const;
    };
}

#include <unistd.h>
#include <sys/mman.h>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <cstring>
#include <assert.h>

inline hooker::Hooker::~Hooker() {}

inline void hooker::Hooker::changeCodeAttrs(void *func, int attr) const {
    int pagesize = getpagesize();
    std::size_t start = PAGE_START((std::size_t)func,pagesize);

     if (mprotect((void *)start, (size_t)pagesize, attr) < 0) {
        throw hooker::error::HookerError("mprotect error");
    }
}

inline void hooker::Hooker::hook(void *func, void *newAddr, void **origFunc, bool saveOrig) const {
    changeCodeAttrs(func,CODE_WRITE);

	if (saveOrig)
		saveOriginFuncBytes(func);

    doHook(func,newAddr,origFunc);

    changeCodeAttrs(func,CODE_READ_ONLY);
}

inline void hooker::Hooker::saveOriginFuncBytes(void *func) const {
    const size_t hookHeadSize = getHookHeadSize();
	const size_t originFunctionSize = getOrigFunctionSize();
	assert(originFunctionSize >= hookHeadSize);

    void *save_bytes = malloc(hookHeadSize);
    if (save_bytes == nullptr)
        throw hooker::error::HookerError("malloc error");

	// save the origin bytes here.
    memcpy(save_bytes,func,hookHeadSize);
    gHookedMap[(std::size_t)func] = (std::size_t)(save_bytes);

	// and now, we want to insert a jump to the next instruction
	// after the hooked start.
	if (originFunctionSize > hookHeadSize) {
		char *f = (char *)func;
		char *n = (char *)save_bytes;
		hook((void *)&n[hookHeadSize],(void *)&f[hookHeadSize],nullptr, false);
	}

}

inline void hooker::Hooker::doUnHook(void *func, void *oldfunc) const {
	if (oldfunc != nullptr) {
		changeCodeAttrs(oldfunc, CODE_WRITE);
		free(oldfunc);
	}
    std::size_t addr = gHookedMap[(std::size_t)func];
    if (addr == 0)
        throw hooker::error::HookerError("it must be hooked before");
    memcpy(func,(void *)addr,getHookHeadSize());
    free((void *)addr);
	gHookedMap.erase((std::size_t)func);
}

inline void hooker::Hooker::unhook(void *func, void *oldfunc) const {
    changeCodeAttrs(func,CODE_WRITE);
    doUnHook(func, oldfunc);
    changeCodeAttrs(func,CODE_READ_ONLY);
}

#include <mutex>
#include <memory>
#include "singleton.h"

namespace hooker {
    class HookerFactory {
    public:
        static std::unique_ptr<HookerFactory> getInstance();
		const Hooker& getHooker() const;
		Hooker* createHooker();
		~HookerFactory() {
			delete hooker;
		}
    private:
		Hooker* hooker;
        HookerFactory(){
			hooker = createHooker();
		}
		friend class utils::design_pattern::NewPolicy<HookerFactory>;
		friend class utils::design_pattern::Singleton<HookerFactory>;
    };
}

#include <utility>
#include <memory>
#include <mutex>
#include "config.h"

inline std::unique_ptr<hooker::HookerFactory> hooker::HookerFactory::getInstance() {
	using namespace utils::design_pattern;
	HookerFactory* result =  Singleton<HookerFactory,MultiThreadPolicy>::getInstance();
	return std::unique_ptr<HookerFactory>(result);
}

inline hooker::Hooker* hooker::HookerFactory::createHooker() {
#ifdef __x86_64__
    return new hooker::HookerX64;
#elif defined(__arm__)
    return new hooker::HookerArm;
#elif defined(__aarch64__)
	return new hooker::HookerArm64;	
#else
	throw hooker::error::HookerError("unsupported hook architecture");
#endif
}

inline const hooker::Hooker& hooker::HookerFactory::getHooker() const {
	return *hooker;
}

#endif //C_11_HOOKER_H
