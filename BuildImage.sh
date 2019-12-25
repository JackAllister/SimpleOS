cd output

# Create an IMG file, this should be enough for QEMU: qemu-system-x86_64 -L . -bios utils/OVMF-pure-efi.fd -usb -usbdevice disk::output/built.img 
dd if=/dev/zero of=built.img bs=1k count=1440
mformat -i built.img -f 1440 ::
mmd -i built.img ::/EFI
mmd -i built.img ::/EFI/BOOT
mcopy -i built.img boot.efi ::/EFI/BOOT/BOOTX64.EFI

# Also create an ISO which can be used for QEMU: qemu-system-x86_64 -L . -bios utils/OVMF-pure-efi.fd -cdrom output/built.iso
mkdir iso
cp built.img iso
xorriso -as mkisofs -R -f -e built.img -no-emul-boot -o built.iso iso

cd ..