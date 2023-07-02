# Update these three lines for your local setup
TOOLCHAIN_PATH=
TOOLCHAIN_PREFIX=riscv-none-elf
CARAVEL_BOARD_REPO=/work/caravel_board/gf180

# ---- Test patterns for project raven ----

.SUFFIXES:

PATTERN = tiny_user_project_ctrl

hex:  ${PATTERN:=.hex}

%.elf: %.c ../sections.lds ../crt0_vex.S
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-gcc -I../ -I../generated/ -O0 -mabi=ilp32 -march=rv32i -D__vexriscv__ -Wl,-Bstatic,-T,$(CARAVEL_BOARD_REPO)/sections.lds,--strip-debug -ffreestanding -nostdlib -o $@ $(CARAVEL_BOARD_REPO)/crt0_vex.S $(CARAVEL_BOARD_REPO)/isr.c $<
	${TOOLCHAIN_PATH}$(TOOLCHAIN_PREFIX)-objdump -D tiny_user_project_ctrl.elf > tiny_user_project_ctrl.lst

%.hex: %.elf
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy -O verilog $< $@
	sed -ie 's/@1000/@0000/g' $@

%.bin: %.elf
	$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)-objcopy -O binary $< $@

client: client.c
	gcc client.c -o client

flash: tiny_user_project_ctrl.hex
	python3 $(CARAVEL_BOARD_REPO)/util/caravel_hkflash.py tiny_user_project_ctrl.hex
	python3 $(CARAVEL_BOARD_REPO)/util/caravel_hkstop.py

flash2: tiny_user_project_ctrl.hex
	python3 $(CARAVEL_BOARD_REPO)/util/caravel_flash.py tiny_user_project_ctrl.hex

# ---- Clean ----

clean:
	rm -f *.elf *.hex *.bin *.vvp *.vcd

.PHONY: clean hex all flash

