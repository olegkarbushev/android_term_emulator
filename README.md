# android_term_emulator

native android application to read STDIN and print STDOUT to adb. 
Android java application with service. 
Both communicates using Unix domain socket 

!!! 
Hardcoded socket name: "com.soft.penguin.localServerSock"

Probably has to be changed, if so - please change in both application!
!!!

Folder structure:

|
|-- android - android application
|
`-- native - native application
    |
    |-- jni - folder with src and makefile
    |-- obj - contains obj
    `-- libs - contains compiled BIN file

-- run:

1. run android application (will create socket server side);

2. open adb shell adb start native appliction;

3. open one more adb shell with logcat: 
adb logcat -b main | egrep 'emulator'

4. grab some coffee, enjoy;

-- install:
Android: 
app installation is straight forward.

Native:

adb push bin_name /data/local/tmp
adb shell chmod 751 /data/local/tmp/bin_name
adb shell /data/local/tmp/bin_name

example:
adb push term_emu /data/local/
adb shell chmod 751 /data/local/tmp/term_emu
adb shell /data/local/tmp/term_emu

--build:
Android: 
build is straight forward

Native:
android-ndk required (install, add to the system PATH)
1. cd native
2. nkd-build

example:

ndk-build 
[armeabi-v7a] Compile thumb  : term_emu <= main.c
[armeabi-v7a] Executable     : term_emu
[armeabi-v7a] Install        : term_emu => libs/armeabi-v7a/term_emu

binary file is placed in "libs/armeabi-v7a/term_emu"
