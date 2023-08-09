UID=$(shell id -u)
GID=$(shell id -g)

#######################################
### Building Software #################
#######################################

.PHONY: build.all
build.all: build.a9 build.x86_64

build.cmake/%/CMakeCache.txt:
	cmake --preset $*

build.cmake.%: | build.cmake/%/CMakeCache.txt
	cmake --build --parallel ${nproc} --preset $*

build.%: build.cmake.%
	echo "build $*"

build.graph:
	cmake --graphviz=test.dot . -DPLATFORM=CORTEX_A9

#######################################
### Container #########################
#######################################

.PHONY: container.pull
container.pull:
	docker pull ghcr.io/mitchellthompkins/embedded_sdk:latest

.PHONY: container.start
container.start:
	docker-compose -f docker-compose.yml run --rm dev_env 'sh -x'


#######################################
### Utility ###########################
#######################################

		#-serial mon:stdio
.PHONY: qemu-debug.%
qemu-debug.%:
	qemu-system-arm \
		-nographic \
		--no-reboot \
		-gdb tcp::1234 \
		-S \
		-machine xilinx-zynq-a9 \
		-cpu cortex-a9 \
		-m 12M \
		-d guest_errors\
		--semihosting \
		-semihosting-config enable=on,target=native \
		-kernel build/$*/test_os_task0.elf

.PHONY: qemu-run.%
qemu-run.%:
	qemu-system-arm \
		-nographic \
		--no-reboot \
		-serial stdio \
		-monitor none \
		-machine xilinx-zynq-a9 \
		-cpu cortex-a9 \
		-m 12M \
		--semihosting \
		-semihosting-config enable=on,target=native \
		-kernel build/$*/test_os_task0.elf

.PHONY: gdb-debug.%
gdb-debug.%:
	gdb build/$*/test.elf -ix .gdbinit

.PHONY: clean
clean:
	rm -rf build/

# All the Makefiles read themselves get matched if a target exists for them, so
# they will get matched by a Match anything target %:. This target is here
# to prevent the %: Match-anything target from matching, and do nothing.
Makefile:
	;
