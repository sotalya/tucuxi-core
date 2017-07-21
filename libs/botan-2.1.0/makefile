
## ----------------------------------------------------------------------------
##
## TucuMath Makefile
##
## ----------------------------------------------------------------------------

NAME := botan
TYPE := LIB

SOURCES := src/lib/base/scan_name.cpp \
src/lib/base/symkey.cpp \
src/lib/block/block_cipher.cpp \
src/lib/codec/base64/base64.cpp \
src/lib/codec/hex/hex.cpp \
src/lib/entropy/entropy_srcs.cpp \
src/lib/filters/algo_filt.cpp \
src/lib/filters/basefilt.cpp \
src/lib/filters/buf_filt.cpp \
src/lib/filters/cipher_filter.cpp \
src/lib/filters/codec_filt/b64_filt.cpp \
src/lib/filters/codec_filt/hex_filt.cpp \
src/lib/filters/comp_filter.cpp \
src/lib/filters/data_snk.cpp \
src/lib/filters/fd_unix/fd_unix.cpp \
src/lib/filters/filter.cpp \
src/lib/filters/key_filt.cpp \
src/lib/filters/out_buf.cpp \
src/lib/filters/pipe.cpp \
src/lib/filters/pipe_io.cpp \
src/lib/filters/pipe_rw.cpp \
src/lib/filters/secqueue.cpp \
src/lib/filters/threaded_fork.cpp \
src/lib/hash/hash.cpp \
src/lib/hash/mdx_hash/mdx_hash.cpp \
src/lib/hash/sha2_32/sha2_32.cpp \
src/lib/mac/hmac/hmac.cpp \
src/lib/mac/mac.cpp \
src/lib/modes/cipher_mode.cpp \
src/lib/rng/auto_rng/auto_rng.cpp \
src/lib/rng/hmac_drbg/hmac_drbg.cpp \
src/lib/rng/rng.cpp \
src/lib/rng/stateful_rng/stateful_rng.cpp \
src/lib/stream/stream_cipher.cpp \
src/lib/utils/assert.cpp \
src/lib/utils/barrier.cpp \
src/lib/utils/calendar.cpp \
src/lib/utils/charset.cpp \
src/lib/utils/cpuid.cpp \
src/lib/utils/data_src.cpp \
src/lib/utils/filesystem.cpp \
src/lib/utils/mem_ops.cpp \
src/lib/utils/os_utils.cpp \
src/lib/utils/parsing.cpp \
src/lib/utils/read_cfg.cpp \
src/lib/utils/semaphore.cpp \
src/lib/utils/version.cpp 

INCLUDES := build/include

LIBS := 
EXTLIBS := 
DEFINES :=

include $(TUCUXI_ROOT)/make/rules.mak

prebuild: 
	$(COPY) makefile makefile.tmp
	python configure.py --minimized-build --enable-modules=fd_unix,codec_filt,filters,sha2_32,auto_rng,rng --disable-shared	
	$(DEL) Makefile
	$(COPY) makefile.tmp makefile
	$(DEL) makefile.tmp


