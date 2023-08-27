.PHONY: %.qemu-run
%.qemu-run:
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
		-kernel $*

# For example: make a9/test_os_task0.elf.qemu-gdb
.PHONY: %.qemu-gdb
%.qemu-gdb:
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
		-kernel $*

.PHONY: %.gdb
%.gdb:
	gdb $* -ix .gdbinit

