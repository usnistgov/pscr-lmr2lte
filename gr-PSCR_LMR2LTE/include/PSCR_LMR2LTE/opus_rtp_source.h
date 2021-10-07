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

#ifndef INCLUDED_PSCR_LMR2LTE_OPUS_RTP_SOURCE_H
#define INCLUDED_PSCR_LMR2LTE_OPUS_RTP_SOURCE_H

#include <PSCR_LMR2LTE/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace PSCR_LMR2LTE {

    /*!
     * \brief This block receives opus encoded data
	 * over RTP streams and outputs the decoded audio
     * \ingroup PSCR_LMR2LTE
     *
     */
    class PSCR_LMR2LTE_API opus_rtp_source : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<opus_rtp_source> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of PSCR_LMR2LTE::opus_rtp_source.
       *
       * To avoid accidental use of raw pointers, PSCR_LMR2LTE::opus_rtp_source's
       * constructor is in a private implementation
       * class. PSCR_LMR2LTE::opus_rtp_source::make is the public interface for
       * creating new instances.
       */
      static sptr make(int port, int samprate, bool ipv6);
	  virtual void set_chan(float tx_freq_offset,
                      const std::string& SIP_user, const std::string& SIP_pw, const std::string& uri) = 0;    
	};

  } // namespace PSCR_LMR2LTE
} // namespace gr

#endif /* INCLUDED_PSCR_LMR2LTE_OPUS_RTP_SOURCE_H */

