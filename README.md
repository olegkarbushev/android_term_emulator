-----------------------------------------------------
# android_term_emulator
-----------------------------------------------------

Native android application reads STDIN and prints STDOUT to adb. 
Android java application with service holds server socket. 

Both apps communicating using Unix domain socket 


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
Hardcoded socket name: "com.soft.penguin.localServerSock"

Probably has to be changed, if so - please change in both application!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Folder structure:

|
|-- android - android application
|
`-- native - native application
    |
    |-- jni - folder with src and makefile
    |-- obj - contains obj
    `-- libs - contains compiled BIN file

-----------------------------------------------------
run:
-----------------------------------------------------

1. run android application (will create socket server side); 

2. click "Start service" to start service, which will create server socket

3. open adb shell and start native appliction;

4. open one more adb shell with logcat: 
adb logcat -b main | egrep 'Emulator!'

5. grab some coffee, enjoy;

-----------------------------------------------------
install:
-----------------------------------------------------

Android:

app installation is straight forward.

Native:

adb push /libs/<cpu/abi>/bin_name /data/local/tmp
adb shell chmod 777 /data/local/tmp/bin_name
adb shell /data/local/tmp/bin_name

example:
adb push /libs/armeabi-v7a/term_emu /data/local/tmp
adb shell chmod 777 /data/local/tmp/term_emu
adb shell /data/local/tmp/term_emu

-----------------------------------------------------
build:
-----------------------------------------------------

Android: 
build is straight forward

Native:

android-ndk required (install, add to the system PATH)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
for android V < lollipop just follow instructions
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
1. cd native

2. nkd-build

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
for android V > lollipop just follow instructions
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
1. uncomment lines
# LOCAL_CFLAGS += -fPIE
# LOCAL_LDFLAGS += -fPIE -pie
in jni/Android.mk

2. cd native

3. nkd-build

example:

ndk-build 
[armeabi-v7a] Compile thumb  : term_emu <= main.c
[armeabi-v7a] Executable     : term_emu
[armeabi-v7a] Install        : term_emu => libs/armeabi-v7a/term_emu

binary file is placed in "libs/armeabi-v7a/term_emu"
