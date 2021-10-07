  # PSCR LMR2LTE

PSCR source code for the pscr-lmr2lte project Passthrough. The pscr-lmr2lte repository contains the GNURadio "gr-PSCR_LMR2LTE" Out of Tree Module(OOT), Hierarchical blocks, and example flowgraphs to support bidirectional communications between an Analog LTE system and a broadband MCPTT system. The following will present setup, install, and usage instructions to run the code targeting an analog FM LMR repeater.


### Dependencies

- GNU Radio. See the [GNU Radio
  Wiki](https://wiki.gnuradio.org/index.php/InstallingGR) for
  installation instructions.
```
sudo add-apt-repository ppa:gnuradio/gnuradio-releases 
sudo apt update
sudo apt install gnuradio
```

- pjsip. See the [pjsip
  Wiki](https://trac.pjsip.org/repos/wiki/Getting-Started/Autoconf) for
  installation instructions.
```
sudo apt install git libasound2-dev libssl-dev libv4l-dev libsdl2-dev libsdl2-gfx-dev libsdl2-image-dev libavdevice-dev libavfilter-dev libavresample-dev libavutil-dev libavcodec-extra libopus-dev libopencore-amrwb-dev libopencore-amrnb-dev libvo-amrwbenc-dev subversion python3.8-dev python3.8 swig default-jdk
```
--Build PJSIP
```
cd src
git clone https://github.com/pjsip/pjproject.git
cd pjproject
touch pjlib/include/pj/config_site.h
touch user.mak
echo "export CFLAGS += -fPIC" >> user.mak
./configure --enable-shared --with-opencore-amr=/usr/lib/x86_64-linux-gnu/
make clean
make dep
make
sudo make install
```
Install python modules
```
cd pjsip-apps/src/swig
sudo make
cd python     
sudo python setup.py install or $sudo python3 setup.py install)
```


### Installation
```
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
```

### Usage

open apps/rds_rx.grc example flow graph in GNU Radio Companion.


### Demos

Quick example:
<PSCR stakeholders link>

NIST-IR 8338:
https://nvlpubs.nist.gov/nistpubs/ir/2020/NIST.IR.8338.pdf

