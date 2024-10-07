Перед сборкой initramfs.cpio.gz в папку _install Busybox'a я скопировал /dev/null и создал файл /etc/passwd, в который записал:

root:x:0:0:root:/home:/bin/sh
daemon:x:1:1:daemon:/usr/sbin:/bin/sh
sampleUser:x:0:0:Linux User,,,:/home/sampleUser:/bin/sh

Также скопировал содержимое папок в _install/ openssh'а в папку _install/ Busybox'а.