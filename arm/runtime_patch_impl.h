// Copyright 2014 Louix Gu
// Author: gzc9047@gmail.com (Louix Gu)

// CppFreeMock: a tool for mock global function, member function, class static function.
//
// Implement runtime patch in x86 architecture.

#ifndef CPP_FREE_MOCK_X86_RUNTIME_PATCH_IMPL_H_
#define CPP_FREE_MOCK_X86_RUNTIME_PATCH_IMPL_H_

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>
#include <sys/mman.h>
#include "Hooker.h" //Used to replace inline function with mock function

namespace CppFreeMock {

namespace RuntimePatcherImpl {
    static std::size_t AlignAddress(const std::size_t address, const std::size_t page_size) {
        return address & (~(page_size - 1));
    }

    static void BackupBinary(const char* const function, std::vector<char>& binary_backup, const std::size_t size) {
        binary_backup = std::vector<char>(function, function + size);
    }

    static bool IsDistanceOverflow(const std::size_t distance) {
        return distance > INT32_MAX && distance < ((long long)INT32_MIN);
    }

    static std::size_t CalculateDistance(const void* const address, const void* const destination) {
        std::size_t distance = reinterpret_cast<std::size_t>(destination)
            - reinterpret_cast<std::size_t>(address)
            - 4; // For jmp instruction;
        return distance;
    }

    static void PatchFunction32bitDistance(char* const function, const std::size_t distance) {
        // Calculate the relative offset from the current instruction to the target address
        std::ptrdiff_t offset = reinterpret_cast<std::ptrdiff_t>(function) - distance;

        // Convert the offset to a 32-bit signed integer (AArch64 instructions use signed offsets)
        int32_t offset32 = static_cast<int32_t>(offset);

        // Store the branch instruction (B) and the offset in the function
        // The branch instruction format in AArch64 is: B <offset>
        // The offset is a signed 26-bit value, shifted right by 2 bits (to ensure it's word-aligned)
        uint32_t branch_instruction = 0x14000000 | ((offset32 >> 2) & 0x03FFFFFF);

        // Copy the 32-bit instruction to the function
        std::memcpy(function, &branch_instruction, sizeof(branch_instruction));
    }

    static void PatchFunction(void* function, void* replacement) {
        using namespace hooker;
        std::unique_ptr<HookerFactory> factory = HookerFactory::getInstance();
        const Hooker& hooker = factory->getHooker();
        hooker.hook(function, replacement, NULL, false);
    }

    static int SetJump(void* address, void* destination, std::vector<char>& binary_backup) {
        // char* const function = reinterpret_cast<char*>(address);
        //std::size_t distance = CalculateDistance(address, destination);
        
        //BackupBinary(function, binary_backup, 5); // short jmp.
        PatchFunction(address, destination);

        return 0;
    }

    static void RevertJump(void* address, const std::vector<char>& binary_backup) {
        //std::copy(binary_backup.begin(), binary_backup.end(), reinterpret_cast<char*>(address));
        /*using namespace hooker;
        std::unique_ptr<HookerFactory> factory = HookerFactory::getInstance();
        const Hooker& hooker = factory->getHooker();
        hooker.unhook(address, binary_b);*/
    }
}

} // namespace CppFreeMock

#endif // CPP_FREE_MOCK_X86_RUNTIME_PATCH_IMPL_H_
