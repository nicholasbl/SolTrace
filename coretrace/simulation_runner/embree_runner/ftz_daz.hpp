#ifndef SOLTRACE_FTZ_DAZ_HPP
#define SOLTRACE_FTZ_DAZ_HPP

// Set Flush-to-Zero (FTZ) and Denormals-are-Zero (DAZ) floating-point flags
// for the calling thread. These are thread-local CPU register settings that
// avoid slow denormal handling in the FPU, as recommended by the Embree docs.
// On ARM, DAZ is implicit when FZ is set (no separate bit).

#if defined(__SSE__) || defined(_M_X64) || defined(_M_IX86)
#  include <xmmintrin.h>
#  include <pmmintrin.h>
#  define SOLTRACE_SET_FTZ_DAZ() \
     _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON); \
     _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON)
#elif defined(__aarch64__) || defined(__arm64__) || defined(_M_ARM64)
#  if defined(_MSC_VER)
#    include <intrin.h>
#    define SOLTRACE_SET_FTZ_DAZ() do { \
       uint64_t _fpcr = _ReadStatusReg(ARM64_FPCR); \
       _fpcr |= (1ULL << 24); \
       _WriteStatusReg(ARM64_FPCR, _fpcr); \
     } while(0)
#  else
     /* GCC / Clang on ARM64 */
#    define SOLTRACE_SET_FTZ_DAZ() do { \
       uint64_t _fpcr; \
       __asm__ __volatile__("mrs %0, fpcr" : "=r"(_fpcr)); \
       _fpcr |= (1ULL << 24); \
       __asm__ __volatile__("msr fpcr, %0" : : "r"(_fpcr)); \
     } while(0)
#  endif
#else
#  define SOLTRACE_SET_FTZ_DAZ() /* unsupported architecture, no-op */
#endif

#endif // SOLTRACE_FTZ_DAZ_HPP
