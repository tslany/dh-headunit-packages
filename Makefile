SHELL := /bin/sh

CC := gcc
LD := ld

CFLAGS := -m32 -O2 -fPIC -fvisibility=hidden -fno-stack-protector \
	-fno-builtin -ffreestanding -Wall -Wextra -Wshadow -Wconversion \
	-Wsign-conversion -Werror
LDFLAGS := -m elf_i386 -shared --hash-style=sysv --build-id=none \
	-z noexecstack

FEATURE_PACKAGES_DIR := $(CURDIR)/feature_packages
LIBFE := $(FEATURE_PACKAGES_DIR)/vehicle-information/payload/app/lib/libFE.so
LIBPG := $(FEATURE_PACKAGES_DIR)/projected-guidance/payload/app/lib/libPG.so
LIBVH := $(FEATURE_PACKAGES_DIR)/vehicle-information/payload/app/lib/libVH.so

.PHONY: all fuel projection vehicle-home clean

all: fuel projection vehicle-home

fuel: $(LIBFE)

projection: $(LIBPG)

vehicle-home: $(LIBVH)

define build_library
	@set -eu; \
	object=$$(mktemp /tmp/dh-headunit-package.XXXXXX.o); \
	trap 'rm -f -- "$$object"' 0 HUP INT TERM; \
	mkdir -p -- "$(@D)"; \
	$(CC) $(CFLAGS) -c "$<" -o "$$object"; \
	$(LD) $(LDFLAGS) -soname "$(@F)" -o "$@" "$$object"; \
	chmod 0755 "$@"; \
	printf '%s\n' "$@"
endef

$(LIBFE): sources/fuel_economy/native/fuel_economy.c Makefile
	$(build_library)

$(LIBPG): sources/projected_guidance/projected_guidance.c Makefile
	$(build_library)

$(LIBVH): sources/vehicle_home/vehicle_home.c Makefile
	$(build_library)

clean:
	rm -f -- "$(LIBFE)" "$(LIBPG)" "$(LIBVH)"
