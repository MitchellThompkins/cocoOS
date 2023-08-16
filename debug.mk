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
	gdb build/$*/test_os_task0.elf -ix .gdbinit

