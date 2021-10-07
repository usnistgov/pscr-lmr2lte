/* -*- c++ -*- */
/*
 * Copyright 2021 NIST PSCR.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_PSCR_LMR2LTE_OPUS_RTP_SINK_H
#define INCLUDED_PSCR_LMR2LTE_OPUS_RTP_SINK_H

#include <PSCR_LMR2LTE/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace PSCR_LMR2LTE {

    /*!
	 * \brief This block can send opus encoded
	 * data over RTP streams to a specified host.
     * \ingroup PSCR_LMR2LTE
     *
     */
    class PSCR_LMR2LTE_API opus_rtp_sink : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<opus_rtp_sink> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of PSCR_LMR2LTE::opus_rtp_sink.
       *
       * To avoid accidental use of raw pointers, PSCR_LMR2LTE::opus_rtp_sink's
       * constructor is in a private implementation
       * class. PSCR_LMR2LTE::opus_rtp_sink::make is the public interface for
       * creating new instances.
       */
      static sptr make(const std::string& host, int rport, int lport, int samprate);
	  virtual void set_chan(float rx_freq_offset,
                      const std::string& SIP_user, const std::string& SIP_pw, const std::string& uri) = 0;    
    };

  } // namespace PSCR_LMR2LTE
} // namespace gr

#endif /* INCLUDED_PSCR_LMR2LTE_OPUS_RTP_SINK_H */

