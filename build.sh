export ARCH=RISCV
export CROSS_COMPILE=$RISCV_LINUX_TOOLCHAIN
make saber_saber2020_defconfig
make all -j6
${CROSS_COMPILE}objdump -D u-boot > u-boot.objdump
