C:
cd "C:\Users\Brock\school\ECE391\RemoteWork\qemu_win\"
qemu-system-i386w.exe -hda "C:\Users\Brock\school\ECE391\RemoteWork\ece391_share\work\mp3_group_02\student-distrib\mp3.img" -soundhw all -m 256 -gdb tcp:127.0.0.1:1234 -name mp3