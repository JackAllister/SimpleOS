cd output

# Create an IMF file, this should be enough for QEMU: qemu-system-x86_64 -L . -bios utils/OVMF-pure-efi.fd -usb -usbdevice disk::output/built.img 
dd if=/dev/zero of=built.img bs=512 count=93750 status=progress

parted built.img -s -a minimal mklabel gpt
parted built.img -s -a minimal mkpart EFI FAT32 2048s 93716s
parted built.img -s -a minimal toggle 1 boot

# Loop back the built image to this OS as a temporary device. So we can write to the device.
sudo losetup --offset 1048576 --sizelimit 46934528 /dev/loop100 built.img

# Make the file system as FAT32 for the EFI partition.
sudo mkdosfs -F 32 /dev/loop100

# Mount the loop device to a path.
sudo mkdir /mnt/build-loop
sudo mount /dev/loop100 /mnt/build-loop

# Create the directory and copy all required files to the filesystem.
sudo mkdir -p /mnt/build-loop/EFI/BOOT/

sudo cp boot.efi /mnt/build-loop/EFI/BOOT/BOOTX64.EFI

# Tidy up.
sudo umount /mnt/build-loop
sudo rm -r /mnt/build-loop
sudo losetup -d /dev/loop100

# Also create an ISO which can be used for QEMU: qemu-system-x86_64 -L . -bios utils/OVMF-pure-efi.fd -cdrom output/built.iso
mkdir iso
cp built.img iso
xorriso -as mkisofs -R -f -e built.img -no-emul-boot -o built.iso iso
rm -r iso/

cd ..