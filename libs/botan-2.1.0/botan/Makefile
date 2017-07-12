# Compiler Options
CXX            = g++ -fstack-protector -m64 -pthread
LINKER         = $(CXX)

CXXFLAGS       = -std=c++11 -D_REENTRANT -O3 -momit-leaf-frame-pointer
WARN_FLAGS     = -Wall -Wextra -Wpedantic -Wstrict-aliasing -Wstrict-overflow=5 -Wcast-align -Wmissing-declarations -Wpointer-arith -Wcast-qual -Wzero-as-null-pointer-constant -Wnon-virtual-dtor
SO_OBJ_FLAGS   = -fPIC -fvisibility=hidden

LIB_LINK_CMD   = $(CXX) -shared -fPIC -Wl,-soname,$(SONAME_ABI)
CLI_LINK_CMD   = $(LINKER) -Wl,-rpath=\$$ORIGIN
TEST_LINK_CMD  = $(LINKER) -Wl,-rpath=\$$ORIGIN

LIB_LINKS_TO   = -lrt
CLI_LINKS_TO   = $(LIB_LINKS_TO)
TEST_LINKS_TO  = $(LIB_LINKS_TO)

LIB_FLAGS      = $(SO_OBJ_FLAGS) $(CXXFLAGS) $(WARN_FLAGS)
CLI_FLAGS      = $(CXXFLAGS) $(WARN_FLAGS)
TEST_FLAGS     = $(CXXFLAGS) $(WARN_FLAGS)

SCRIPTS_DIR    = ./src/scripts
INSTALLED_LIB_DIR = /usr/local/lib

CLI_POST_LINK_CMD  = 
TEST_POST_LINK_CMD = 


# Program aliases
AR               = ar crs
COPY             = cp
COPY_R           = cp -r
CD               = @cd
ECHO             = @echo
INSTALL_CMD_EXEC = install -m 755
INSTALL_CMD_DATA = install -m 644
LN               = ln -fs
MKDIR            = @mkdir
MKDIR_INSTALL    = @umask 022; mkdir -p -m 755
RANLIB           = true
RM               = @rm -f
RM_R             = @rm -rf


# Executable targets
CLI           = ./botan
TEST          = ./botan-test

# Library targets
LIB_BASENAME  = libbotan-2
STATIC_LIB    = ./$(LIB_BASENAME).a
LIBRARIES     = $(STATIC_LIB)

# File Lists
INCLUDE_DIR   = build/include/botan

LIBOBJS       = build/obj/lib/botan_all.o

CLIOBJS       = build/obj/cli/asn1.o \
                build/obj/cli/cc_enc.o \
                build/obj/cli/compress.o \
                build/obj/cli/encryption.o \
                build/obj/cli/main.o \
                build/obj/cli/math.o \
                build/obj/cli/pubkey.o \
                build/obj/cli/speed.o \
                build/obj/cli/timing_tests.o \
                build/obj/cli/tls_client.o \
                build/obj/cli/tls_proxy.o \
                build/obj/cli/tls_server.o \
                build/obj/cli/tls_utils.o \
                build/obj/cli/utils.o \
                build/obj/cli/x509.o

TESTOBJS      = build/obj/test/main.o \
                build/obj/test/test_aead.o \
                build/obj/test/test_asn1.o \
                build/obj/test/test_bigint.o \
                build/obj/test/test_block.o \
                build/obj/test/test_c25519.o \
                build/obj/test/test_certstor.o \
                build/obj/test/test_compression.o \
                build/obj/test/test_cryptobox.o \
                build/obj/test/test_dh.o \
                build/obj/test/test_dl_group.o \
                build/obj/test/test_dlies.o \
                build/obj/test/test_dsa.o \
                build/obj/test/test_ecc_pointmul.o \
                build/obj/test/test_ecdh.o \
                build/obj/test/test_ecdsa.o \
                build/obj/test/test_ecgdsa.o \
                build/obj/test/test_ecies.o \
                build/obj/test/test_eckcdsa.o \
                build/obj/test/test_ed25519.o \
                build/obj/test/test_elg.o \
                build/obj/test/test_entropy.o \
                build/obj/test/test_ffi.o \
                build/obj/test/test_filters.o \
                build/obj/test/test_fpe.o \
                build/obj/test/test_fuzzer.o \
                build/obj/test/test_gf2m.o \
                build/obj/test/test_gost_3410.o \
                build/obj/test/test_hash.o \
                build/obj/test/test_kdf.o \
                build/obj/test/test_keywrap.o \
                build/obj/test/test_mac.o \
                build/obj/test/test_mceliece.o \
                build/obj/test/test_modes.o \
                build/obj/test/test_mp.o \
                build/obj/test/test_name_constraint.o \
                build/obj/test/test_newhope.o \
                build/obj/test/test_ocb.o \
                build/obj/test/test_ocsp.o \
                build/obj/test/test_octetstring.o \
                build/obj/test/test_os_utils.o \
                build/obj/test/test_otp.o \
                build/obj/test/test_package_transform.o \
                build/obj/test/test_pad.o \
                build/obj/test/test_passhash.o \
                build/obj/test/test_pbkdf.o \
                build/obj/test/test_pk_pad.o \
                build/obj/test/test_pkcs11.o \
                build/obj/test/test_pkcs11_high_level.o \
                build/obj/test/test_pkcs11_low_level.o \
                build/obj/test/test_pubkey.o \
                build/obj/test/test_rfc6979.o \
                build/obj/test/test_rng.o \
                build/obj/test/test_rsa.o \
                build/obj/test/test_simd.o \
                build/obj/test/test_sm2.o \
                build/obj/test/test_srp6.o \
                build/obj/test/test_stream.o \
                build/obj/test/test_tls_messages.o \
                build/obj/test/test_tpm.o \
                build/obj/test/test_tss.o \
                build/obj/test/test_utils.o \
                build/obj/test/test_workfactor.o \
                build/obj/test/test_x509_dn.o \
                build/obj/test/test_x509_path.o \
                build/obj/test/test_xmss.o \
                build/obj/test/tests.o \
                build/obj/test/unit_ecc.o \
                build/obj/test/unit_ecdh.o \
                build/obj/test/unit_ecdsa.o \
                build/obj/test/unit_tls.o \
                build/obj/test/unit_tls_policy.o \
                build/obj/test/unit_x509.o

# First make target. Will be used by default
all: $(CLI) $(TEST)

# Build Commands
build/obj/lib/botan_all.o: botan_all.cpp
	$(CXX) $(LIB_FLAGS) -Ibuild/include -c botan_all.cpp -o $@


build/obj/cli/asn1.o: ./src/cli/asn1.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/asn1.cpp -o $@

build/obj/cli/cc_enc.o: ./src/cli/cc_enc.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/cc_enc.cpp -o $@

build/obj/cli/compress.o: ./src/cli/compress.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/compress.cpp -o $@

build/obj/cli/encryption.o: ./src/cli/encryption.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/encryption.cpp -o $@

build/obj/cli/main.o: ./src/cli/main.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/main.cpp -o $@

build/obj/cli/math.o: ./src/cli/math.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/math.cpp -o $@

build/obj/cli/pubkey.o: ./src/cli/pubkey.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/pubkey.cpp -o $@

build/obj/cli/speed.o: ./src/cli/speed.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/speed.cpp -o $@

build/obj/cli/timing_tests.o: ./src/cli/timing_tests.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/timing_tests.cpp -o $@

build/obj/cli/tls_client.o: ./src/cli/tls_client.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/tls_client.cpp -o $@

build/obj/cli/tls_proxy.o: ./src/cli/tls_proxy.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/tls_proxy.cpp -o $@

build/obj/cli/tls_server.o: ./src/cli/tls_server.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/tls_server.cpp -o $@

build/obj/cli/tls_utils.o: ./src/cli/tls_utils.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/tls_utils.cpp -o $@

build/obj/cli/utils.o: ./src/cli/utils.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/utils.cpp -o $@

build/obj/cli/x509.o: ./src/cli/x509.cpp
	$(CXX) $(CLI_FLAGS) -Ibuild/include -c ./src/cli/x509.cpp -o $@


build/obj/test/main.o: ./src/tests/main.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/main.cpp -o $@

build/obj/test/test_aead.o: ./src/tests/test_aead.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_aead.cpp -o $@

build/obj/test/test_asn1.o: ./src/tests/test_asn1.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_asn1.cpp -o $@

build/obj/test/test_bigint.o: ./src/tests/test_bigint.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_bigint.cpp -o $@

build/obj/test/test_block.o: ./src/tests/test_block.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_block.cpp -o $@

build/obj/test/test_c25519.o: ./src/tests/test_c25519.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_c25519.cpp -o $@

build/obj/test/test_certstor.o: ./src/tests/test_certstor.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_certstor.cpp -o $@

build/obj/test/test_compression.o: ./src/tests/test_compression.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_compression.cpp -o $@

build/obj/test/test_cryptobox.o: ./src/tests/test_cryptobox.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_cryptobox.cpp -o $@

build/obj/test/test_dh.o: ./src/tests/test_dh.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_dh.cpp -o $@

build/obj/test/test_dl_group.o: ./src/tests/test_dl_group.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_dl_group.cpp -o $@

build/obj/test/test_dlies.o: ./src/tests/test_dlies.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_dlies.cpp -o $@

build/obj/test/test_dsa.o: ./src/tests/test_dsa.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_dsa.cpp -o $@

build/obj/test/test_ecc_pointmul.o: ./src/tests/test_ecc_pointmul.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_ecc_pointmul.cpp -o $@

build/obj/test/test_ecdh.o: ./src/tests/test_ecdh.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_ecdh.cpp -o $@

build/obj/test/test_ecdsa.o: ./src/tests/test_ecdsa.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_ecdsa.cpp -o $@

build/obj/test/test_ecgdsa.o: ./src/tests/test_ecgdsa.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_ecgdsa.cpp -o $@

build/obj/test/test_ecies.o: ./src/tests/test_ecies.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_ecies.cpp -o $@

build/obj/test/test_eckcdsa.o: ./src/tests/test_eckcdsa.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_eckcdsa.cpp -o $@

build/obj/test/test_ed25519.o: ./src/tests/test_ed25519.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_ed25519.cpp -o $@

build/obj/test/test_elg.o: ./src/tests/test_elg.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_elg.cpp -o $@

build/obj/test/test_entropy.o: ./src/tests/test_entropy.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_entropy.cpp -o $@

build/obj/test/test_ffi.o: ./src/tests/test_ffi.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_ffi.cpp -o $@

build/obj/test/test_filters.o: ./src/tests/test_filters.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_filters.cpp -o $@

build/obj/test/test_fpe.o: ./src/tests/test_fpe.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_fpe.cpp -o $@

build/obj/test/test_fuzzer.o: ./src/tests/test_fuzzer.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_fuzzer.cpp -o $@

build/obj/test/test_gf2m.o: ./src/tests/test_gf2m.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_gf2m.cpp -o $@

build/obj/test/test_gost_3410.o: ./src/tests/test_gost_3410.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_gost_3410.cpp -o $@

build/obj/test/test_hash.o: ./src/tests/test_hash.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_hash.cpp -o $@

build/obj/test/test_kdf.o: ./src/tests/test_kdf.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_kdf.cpp -o $@

build/obj/test/test_keywrap.o: ./src/tests/test_keywrap.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_keywrap.cpp -o $@

build/obj/test/test_mac.o: ./src/tests/test_mac.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_mac.cpp -o $@

build/obj/test/test_mceliece.o: ./src/tests/test_mceliece.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_mceliece.cpp -o $@

build/obj/test/test_modes.o: ./src/tests/test_modes.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_modes.cpp -o $@

build/obj/test/test_mp.o: ./src/tests/test_mp.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_mp.cpp -o $@

build/obj/test/test_name_constraint.o: ./src/tests/test_name_constraint.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_name_constraint.cpp -o $@

build/obj/test/test_newhope.o: ./src/tests/test_newhope.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_newhope.cpp -o $@

build/obj/test/test_ocb.o: ./src/tests/test_ocb.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_ocb.cpp -o $@

build/obj/test/test_ocsp.o: ./src/tests/test_ocsp.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_ocsp.cpp -o $@

build/obj/test/test_octetstring.o: ./src/tests/test_octetstring.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_octetstring.cpp -o $@

build/obj/test/test_os_utils.o: ./src/tests/test_os_utils.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_os_utils.cpp -o $@

build/obj/test/test_otp.o: ./src/tests/test_otp.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_otp.cpp -o $@

build/obj/test/test_package_transform.o: ./src/tests/test_package_transform.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_package_transform.cpp -o $@

build/obj/test/test_pad.o: ./src/tests/test_pad.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_pad.cpp -o $@

build/obj/test/test_passhash.o: ./src/tests/test_passhash.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_passhash.cpp -o $@

build/obj/test/test_pbkdf.o: ./src/tests/test_pbkdf.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_pbkdf.cpp -o $@

build/obj/test/test_pk_pad.o: ./src/tests/test_pk_pad.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_pk_pad.cpp -o $@

build/obj/test/test_pkcs11.o: ./src/tests/test_pkcs11.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_pkcs11.cpp -o $@

build/obj/test/test_pkcs11_high_level.o: ./src/tests/test_pkcs11_high_level.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_pkcs11_high_level.cpp -o $@

build/obj/test/test_pkcs11_low_level.o: ./src/tests/test_pkcs11_low_level.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_pkcs11_low_level.cpp -o $@

build/obj/test/test_pubkey.o: ./src/tests/test_pubkey.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_pubkey.cpp -o $@

build/obj/test/test_rfc6979.o: ./src/tests/test_rfc6979.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_rfc6979.cpp -o $@

build/obj/test/test_rng.o: ./src/tests/test_rng.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_rng.cpp -o $@

build/obj/test/test_rsa.o: ./src/tests/test_rsa.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_rsa.cpp -o $@

build/obj/test/test_simd.o: ./src/tests/test_simd.cpp
	$(CXX) -msse2 $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_simd.cpp -o $@

build/obj/test/test_sm2.o: ./src/tests/test_sm2.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_sm2.cpp -o $@

build/obj/test/test_srp6.o: ./src/tests/test_srp6.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_srp6.cpp -o $@

build/obj/test/test_stream.o: ./src/tests/test_stream.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_stream.cpp -o $@

build/obj/test/test_tls_messages.o: ./src/tests/test_tls_messages.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_tls_messages.cpp -o $@

build/obj/test/test_tpm.o: ./src/tests/test_tpm.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_tpm.cpp -o $@

build/obj/test/test_tss.o: ./src/tests/test_tss.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_tss.cpp -o $@

build/obj/test/test_utils.o: ./src/tests/test_utils.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_utils.cpp -o $@

build/obj/test/test_workfactor.o: ./src/tests/test_workfactor.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_workfactor.cpp -o $@

build/obj/test/test_x509_dn.o: ./src/tests/test_x509_dn.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_x509_dn.cpp -o $@

build/obj/test/test_x509_path.o: ./src/tests/test_x509_path.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_x509_path.cpp -o $@

build/obj/test/test_xmss.o: ./src/tests/test_xmss.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/test_xmss.cpp -o $@

build/obj/test/tests.o: ./src/tests/tests.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/tests.cpp -o $@

build/obj/test/unit_ecc.o: ./src/tests/unit_ecc.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/unit_ecc.cpp -o $@

build/obj/test/unit_ecdh.o: ./src/tests/unit_ecdh.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/unit_ecdh.cpp -o $@

build/obj/test/unit_ecdsa.o: ./src/tests/unit_ecdsa.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/unit_ecdsa.cpp -o $@

build/obj/test/unit_tls.o: ./src/tests/unit_tls.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/unit_tls.cpp -o $@

build/obj/test/unit_tls_policy.o: ./src/tests/unit_tls_policy.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/unit_tls_policy.cpp -o $@

build/obj/test/unit_x509.o: ./src/tests/unit_x509.cpp
	$(CXX) $(TEST_FLAGS) -Ibuild/include -c ./src/tests/unit_x509.cpp -o $@


# Link Commands
SONAME_PATCH    = libbotan-2.so.2.2.0
SONAME_ABI      = libbotan-2.so.2
SONAME_BASE     = libbotan-2.so

DARWIN_COMPATIBILITY_VER = 2002.2.0
DARWIN_CURRENT_VER       = 2002.2.0

SHARED_LIB      = ./$(SONAME_PATCH)

$(SHARED_LIB): $(LIBOBJS)
	$(LIB_LINK_CMD) $(LDFLAGS) $(LIBOBJS) $(LIB_LINKS_TO) -o $(SHARED_LIB)
	$(LN) $(SONAME_PATCH) ./$(SONAME_ABI)
	$(LN) $(SONAME_PATCH) ./$(SONAME_BASE)

LIBRARIES += $(SHARED_LIB)


libs: $(LIBRARIES)

$(CLI): $(LIBRARIES) $(CLIOBJS)
	$(CLI_LINK_CMD) $(LDFLAGS) $(CLIOBJS) -L. -lbotan-2 $(CLI_LINKS_TO) -o $(CLI)
	$(CLI_POST_LINK_CMD)

$(TEST): $(LIBRARIES) $(TESTOBJS)
	$(TEST_LINK_CMD) $(LDFLAGS) $(TESTOBJS) -L. -lbotan-2 $(TEST_LINKS_TO) -o $(TEST)
	$(TEST_POST_LINK_CMD)

$(STATIC_LIB): $(LIBOBJS)
	$(RM) $(STATIC_LIB)
	$(AR) $(STATIC_LIB) $(LIBOBJS)
	$(RANLIB) $(STATIC_LIB)

# Fake targets
.PHONY: clean distclean docs install valgrind lcov



SPHINX_CONFIG = ./src/build-data/sphinx
SPHINX_OPTS = -b html

clean:
	-$(RM) build/obj/lib/*
	-$(RM) build/obj/test/*
	-$(RM) build/obj/cli/*
	-$(RM) $(SONAME_ABI) $(SONAME_BASE)
	-$(RM) $(LIBRARIES) $(CLI) $(TEST)

distclean: clean
	$(RM) Makefile
	$(RM_R) build
	$(RM) botan_all.cpp botan_all.h

valgrind:
	valgrind --log-file=botan.%p.log -v --track-origins=yes --leak-check=full --show-reachable=yes ./botan-test

docs:
	$(COPY) ./doc/manual/*.rst build/docs/manual

install: $(CLI) docs
	$(SCRIPTS_DIR)/install.py --destdir=/usr/local --build-dir="build" --bindir=bin --libdir=lib --docdir=share/doc --includedir=include
