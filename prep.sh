#!/bin/bash

cp -v krumpax-linux-3.2.11/arch/x86/boot/bzImage /boot/vmlinuz-linux-pax ;
mkinitcpio -k 3.2.11-ARCH /boot/initramfs-linux-pax.img ;
cp -v krumpax-linux-3.2.11/System.map /boot/System.map-3.2.11-ARCH ;