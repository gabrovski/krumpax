#!/bin/bash

cd krumpax-linux-3.2.11 ;
make ;
make modules_install ;
cd .. ;
cp -v krumpax-linux-3.2.11/arch/x86/boot/bzImage /boot/vmlinuz-linux-pax ;
mkinitcpio -k 3.2.11-pax -g /boot/initramfs-linux-pax.img ;
cp -v krumpax-linux-3.2.11/System.map /boot/System.map-3.2.11-pax ;