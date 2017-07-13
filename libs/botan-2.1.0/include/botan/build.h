#ifndef BOTAN_BUILD_CONFIG_H__
#define BOTAN_BUILD_CONFIG_H__

/*
* This file was automatically generated running
* 'configure.py --minimized-build --single-amalgamation-file --amalgamation'
*
* Target
*  - Compiler: g++ -fstack-protector -m64 -pthread -std=c++11 -D_REENTRANT -O3 -momit-leaf-frame-pointer
*  - Arch: x86_64/x86_64
*  - OS: linux
*/

#define BOTAN_VERSION_MAJOR 2
#define BOTAN_VERSION_MINOR 2
#define BOTAN_VERSION_PATCH 0
#define BOTAN_VERSION_DATESTAMP 0

#define BOTAN_VERSION_RELEASE_TYPE "unreleased"

#define BOTAN_VERSION_VC_REVISION "git:38fe6d3ab3e8a4be2becd5fd0b8c7bb4a8f1e192"

#define BOTAN_DISTRIBUTION_INFO "unspecified"

/* How many bits per limb in a BigInt */
#define BOTAN_MP_WORD_BITS 64


#define BOTAN_INSTALL_PREFIX R"(/usr/local)"
#define BOTAN_INSTALL_HEADER_DIR "include/botan-2"
#define BOTAN_INSTALL_LIB_DIR "lib"
#define BOTAN_LIB_LINK "-lrt"

#ifndef BOTAN_DLL
  #define BOTAN_DLL __attribute__((visibility("default")))
#endif

/* Target identification and feature test macros */
#define BOTAN_TARGET_OS_IS_LINUX
#define BOTAN_TARGET_OS_TYPE_IS_UNIX
#define BOTAN_TARGET_OS_HAS_CLOCK_GETTIME
#define BOTAN_TARGET_OS_HAS_DLOPEN
#define BOTAN_TARGET_OS_HAS_FILESYSTEM
#define BOTAN_TARGET_OS_HAS_GETAUXVAL
#define BOTAN_TARGET_OS_HAS_GETTIMEOFDAY
#define BOTAN_TARGET_OS_HAS_GMTIME_R
#define BOTAN_TARGET_OS_HAS_POSIX_MLOCK
#define BOTAN_TARGET_OS_HAS_READDIR
#define BOTAN_TARGET_OS_HAS_SOCKETS
#define BOTAN_TARGET_OS_HAS_THREADS
#define BOTAN_TARGET_OS_HAS_TIMEGM

#define BOTAN_TARGET_ARCH_IS_X86_64
#define BOTAN_TARGET_SUPPORTS_AESNI
#define BOTAN_TARGET_SUPPORTS_AVX2
#define BOTAN_TARGET_SUPPORTS_BMI2
#define BOTAN_TARGET_SUPPORTS_RDRAND
#define BOTAN_TARGET_SUPPORTS_RDSEED
#define BOTAN_TARGET_SUPPORTS_SHA
#define BOTAN_TARGET_SUPPORTS_SSE2
#define BOTAN_TARGET_SUPPORTS_SSE41
#define BOTAN_TARGET_SUPPORTS_SSE42
#define BOTAN_TARGET_SUPPORTS_SSSE3
#define BOTAN_TARGET_CPU_IS_LITTLE_ENDIAN
#define BOTAN_TARGET_CPU_IS_X86_FAMILY
#define BOTAN_TARGET_CPU_NATIVE_WORD_SIZE 64
#define BOTAN_TARGET_CPU_HAS_NATIVE_64BIT
#define BOTAN_TARGET_UNALIGNED_MEMORY_ACCESS_OK 1

#define BOTAN_BUILD_COMPILER_IS_GCC

/*
* Module availability definitions
*/
#define BOTAN_HAS_BLOCK_CIPHER 20131128
#define BOTAN_HAS_ENTROPY_SOURCE 20151120
#define BOTAN_HAS_HEX_CODEC 20131128
#define BOTAN_HAS_MAC 20150626
#define BOTAN_HAS_MODES 20150626
#define BOTAN_HAS_STREAM_CIPHER 20131128
#define BOTAN_HAS_UTIL_FUNCTIONS 20161127

/*
* Local/misc configuration options (if any) follow
*/



/*
* Things you can edit (but probably shouldn't)
*/

/* How much to allocate for a buffer of no particular size */
#define BOTAN_DEFAULT_BUFFER_SIZE 1024

/* Minimum and maximum sizes to allocate out of the mlock pool (bytes)
   Default min is 16 as smaller values are easily bruteforceable and thus
   likely not cryptographic keys.
*/
#define BOTAN_MLOCK_ALLOCATOR_MIN_ALLOCATION 16
#define BOTAN_MLOCK_ALLOCATOR_MAX_ALLOCATION 128

/*
* Total maximum amount of RAM (in KiB) we will lock into memory, even
* if the OS would let us lock more
*/
#define BOTAN_MLOCK_ALLOCATOR_MAX_LOCKED_KB 512

/*
* If enabled uses memset via volatile function pointer to zero memory,
* otherwise does a byte at a time write via a volatile pointer.
*/
#define BOTAN_USE_VOLATILE_MEMSET_FOR_ZERO 1

/*
* If enabled the ECC implementation will use scalar blinding with order.bits()/2
* bit long masks.
*/
#define BOTAN_POINTGFP_USE_SCALAR_BLINDING 1

/*
* Set number of bits used to generate mask for blinding the
* representation of an ECC point. Set to zero to disable this
* side-channel countermeasure.
*/
#define BOTAN_POINTGFP_RANDOMIZE_BLINDING_BITS 80

/*
* Normally blinding is performed by choosing a random starting point (plus
* its inverse, of a form appropriate to the algorithm being blinded), and
* then choosing new blinding operands by successive squaring of both
* values. This is much faster than computing a new starting point but
* introduces some possible corelation
*
* To avoid possible leakage problems in long-running processes, the blinder
* periodically reinitializes the sequence. This value specifies how often
* a new sequence should be started.
*/
#define BOTAN_BLINDING_REINIT_INTERVAL 32

/*
* Userspace RNGs like HMAC_DRBG will reseed after a specified number
* of outputs are generated. Set to zero to disable automatic reseeding.
*/
#define BOTAN_RNG_DEFAULT_RESEED_INTERVAL 1024
#define BOTAN_RNG_RESEED_POLL_BITS 256
#define BOTAN_RNG_AUTO_RESEED_TIMEOUT std::chrono::milliseconds(10)
#define BOTAN_RNG_RESEED_DEFAULT_TIMEOUT std::chrono::milliseconds(50)

/*
* Specifies (in order) the list of entropy sources that will be used
* to seed an in-memory RNG. The first in the default list: "rdseed" and "rdrand"
* do not count as contributing any entropy
* but are included as they are fast and help protect against a
* seriously broken system RNG.
*/
#define BOTAN_ENTROPY_DEFAULT_SOURCES \
   { "rdseed", "rdrand", "darwin_secrandom", "getentropy", "dev_random", \
    "win32_cryptoapi", "proc_walk", "system_stats" }


/* Multiplier on a block cipher's native parallelism */
#define BOTAN_BLOCK_CIPHER_PAR_MULT 4

/*
* These control the RNG used by the system RNG interface
*/
#define BOTAN_SYSTEM_RNG_DEVICE "/dev/urandom"
#define BOTAN_SYSTEM_RNG_CRYPTOAPI_PROV_TYPE PROV_RSA_FULL

/*
* These paramaters control how many bytes to read from the system
* PRNG, and how long to block if applicable.
*
* Timeout is ignored on Windows as CryptGenRandom doesn't block
*/
#define BOTAN_SYSTEM_RNG_POLL_DEVICES { "/dev/urandom", "/dev/random", "/dev/srandom" }

/*
* This directory will be monitored by ProcWalking_EntropySource and
* the contents provided as entropy inputs to the RNG. May also be
* usefully set to something like "/sys", depending on the system being
* deployed to. Set to an empty string to disable.
*/
#define BOTAN_ENTROPY_PROC_FS_PATH "/proc"

#define BOTAN_SYSTEM_RNG_POLL_REQUEST 64
#define BOTAN_SYSTEM_RNG_POLL_TIMEOUT_MS 20


/*
How many times to read from the RDRAND/RDSEED RNGs.
Each read generates 32 bits of output
*/
#define BOTAN_ENTROPY_INTEL_RNG_POLLS 32

// According to Intel, RDRAND is guaranteed to generate a random number within 10 retries on a working CPU
#define BOTAN_ENTROPY_RDRAND_RETRIES 10

/*
* RdSeed is not guaranteed to generate a random number within a specific number of retries
* Define the number of retries here
*/
#define BOTAN_ENTROPY_RDSEED_RETRIES 20

/*
* If no way of dynamically determining the cache line size for the
* system exists, this value is used as the default. Used by the side
* channel countermeasures rather than for alignment purposes, so it is
* better to be on the smaller side if the exact value cannot be
* determined. Typically 32 or 64 bytes on modern CPUs.
*/
#if !defined(BOTAN_TARGET_CPU_DEFAULT_CACHE_LINE_SIZE)
  #define BOTAN_TARGET_CPU_DEFAULT_CACHE_LINE_SIZE 32
#endif

/**
* Controls how AutoSeeded_RNG is instantiated
*/
#if !defined(BOTAN_AUTO_RNG_HMAC)

  #if defined(BOTAN_HAS_SHA2_64)
    #define BOTAN_AUTO_RNG_HMAC "HMAC(SHA-384)"
  #elif defined(BOTAN_HAS_SHA2_32)
    #define BOTAN_AUTO_RNG_HMAC "HMAC(SHA-256)"
  #elif defined(BOTAN_HAS_SHA3)
    #define BOTAN_AUTO_RNG_HMAC "HMAC(SHA-3(256))"
  #elif defined(BOTAN_HAS_SHA1)
    #define BOTAN_AUTO_RNG_HMAC "HMAC(SHA-1)"
  #endif
  // Otherwise, no hash found: leave BOTAN_AUTO_RNG_HMAC undefined

#endif

// Append to macros living outside of functions, so that invocations must end with a semicolon.
// The struct is only declared to force the semicolon, it is never defined.
#define BOTAN_FORCE_SEMICOLON struct BOTAN_DUMMY_STRUCT

// Check for a common build problem:

#if defined(BOTAN_TARGET_ARCH_IS_X86_64) && ((defined(_MSC_VER) && !defined(_WIN64)) || \
                                             (defined(__clang__) && !defined(__x86_64__)) || \
                                             (defined(__GNUG__) && !defined(__x86_64__)))
    #error "Trying to compile Botan configured as x86_64 with non-x86_64 compiler."
#endif

#if defined(BOTAN_TARGET_ARCH_IS_X86_32) && ((defined(_MSC_VER) && defined(_WIN64)) || \
                                             (defined(__clang__) && !defined(__i386__)) || \
                                             (defined(__GNUG__) && !defined(__i386__)))

    #error "Trying to compile Botan configured as x86_32 with non-x86_32 compiler."
#endif

#include <botan/compiler.h>

#endif
