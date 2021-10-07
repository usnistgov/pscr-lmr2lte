/* -*- c++ -*- */

#define PSCR_LMR2LTE_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "PSCR_LMR2LTE_swig_doc.i"

%{
#include "PSCR_LMR2LTE/opus_rtp_sink.h"
#include "PSCR_LMR2LTE/opus_rtp_source.h"
#include "PSCR_LMR2LTE/sip_signaling.h"
%}

%include "PSCR_LMR2LTE/opus_rtp_sink.h"
GR_SWIG_BLOCK_MAGIC2(PSCR_LMR2LTE, opus_rtp_sink);
%include "PSCR_LMR2LTE/opus_rtp_source.h"
GR_SWIG_BLOCK_MAGIC2(PSCR_LMR2LTE, opus_rtp_source);
%include "PSCR_LMR2LTE/sip_signaling.h"
GR_SWIG_BLOCK_MAGIC2(PSCR_LMR2LTE, sip_signaling);
