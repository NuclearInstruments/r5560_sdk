# R5560 SDK
This is the SDK library for the CAEN / Nuclear Instruments R5560/SE DAQ board

## Linux Compiling

### Pre Requisites

R5560 SDK requires GCC build chain and LibZMQ to be compiled

Install following tools

```bash
sudo apt-get install libtool pkg-config build-essential autoconf automake
sudo apt-get install libzmq3-dev
```

### Compile and install library

Just enter make.

Install the library in /usr/local/lib
```bash
cd linux
sudo cp libr5560.so.1.0.0 /usr/local/lib
cd /usr/local/lib
sudo ln -s libr5560.so.1.0.0 librprng.so
```
Reload Linux library database
```bash
sudo ldconfig
```



