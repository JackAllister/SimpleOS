dd if=/dev/zero of=output/fat.img bs=1k count=1440
mformat -i output/fat.img -f 1440 ::
mmd -i output/fat.img ::/EFI
mmd -i output/fat.img ::/EFI/BOOT
mcopy -i output/fat.img output/boot.efi ::/EFI/BOOT/BOOTX64.EFI