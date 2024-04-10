# R5560 SDK
This is the SDK library for the CAEN / Nuclear Instruments R5560/SE DAQ board
This is the low level library.
This library just offer basic hardware access method and tipically is not direclty used by users but it is a mandatory component of [SciSDK](https://github.com/NuclearInstruments/SCISDK) library.

You must install this library in the system in order to use R5560/DT5560/R5560SE with SciSDK.
While in windows the latest available version of this library is automatically installed with SciSDK by the SciSDK setup.

**In linux the library must be manually installed in order to use SciSDK**



## Linux Compiling

### Pre Requisites

R5560 SDK requires GCC build chain and LibZMQ to be compiled

Install following tools

```bash
sudo apt-get install libtool pkg-config build-essential autoconf automake
sudo apt-get install libzmq3-dev
```

### Compile and install library

Compile the library and install in 

```bash
cd linux
autoreconf  -f -i -Wall,no-obsolete
../configure
make -j16
```

Double check that the compilation process do not generate any error and than install simply copy the library /usr/local/lib
```bash
sudo cp libr5560.so.1.0.0 /usr/local/lib
cd /usr/local/lib
sudo ln -s libr5560.so.1.0.0 libr5560.so
```

Reload Linux library database
```bash
sudo ldconfig
```


If you are root of the machine you can copy the library directly in /usr/lib or /lib


