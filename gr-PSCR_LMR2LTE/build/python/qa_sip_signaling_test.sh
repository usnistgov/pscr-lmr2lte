#!/usr/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir="/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/python"
export GR_CONF_CONTROLPORT_ON=False
export PATH="/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/python":$PATH
export LD_LIBRARY_PATH="":$LD_LIBRARY_PATH
export PYTHONPATH=/home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/build/swig:$PYTHONPATH
/usr/bin/python3 /home/jonc/lmr2lte/gnuradio/gr-PSCR_LMR2LTE/python/qa_sip_signaling.py 
