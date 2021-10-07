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

#ifndef INCLUDED_PSCR_LMR2LTE_SIP_SIGNALING_H
#define INCLUDED_PSCR_LMR2LTE_SIP_SIGNALING_H

#include <PSCR_LMR2LTE/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace PSCR_LMR2LTE {

    /*!
     * \brief <+description of block+>
     * \ingroup PSCR_LMR2LTE
     *
     */
    class PSCR_LMR2LTE_API sip_signaling : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<sip_signaling> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of PSCR_LMR2LTE::sip_signaling.
       *
       * To avoid accidental use of raw pointers, PSCR_LMR2LTE::sip_signaling's
       * constructor is in a private implementation
       * class. PSCR_LMR2LTE::sip_signaling::make is the public interface for
       * creating new instances.
       */
      static sptr make(char* SIP_domain, char* PCSCF_addr);
    };

  } // namespace PSCR_LMR2LTE
} // namespace gr

#endif /* INCLUDED_PSCR_LMR2LTE_SIP_SIGNALING_H */

