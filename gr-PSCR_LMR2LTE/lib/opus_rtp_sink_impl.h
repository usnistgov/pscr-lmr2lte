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

#ifndef INCLUDED_PSCR_LMR2LTE_OPUS_RTP_SINK_IMPL_H
#define INCLUDED_PSCR_LMR2LTE_OPUS_RTP_SINK_IMPL_H

#include <PSCR_LMR2LTE/opus_rtp_sink.h>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/circular_buffer.hpp>
#include <PSCR_LMR2LTE/packet_headers.h>
#include <chrono>
#include <opus.h>

namespace gr {
  namespace PSCR_LMR2LTE {

		class opus_rtp_sink_impl : public opus_rtp_sink
		{
		private:
			/*!
	* \brief Messages are received in this port.
	*
	* This port receives messages from the scheduler's message
	* handling mechanism. Messages are used to set the 
	* RTP address, port and payload type.
	*
	* \param msg A pmt message passed from the scheduler's message handling.
	*/
			boost::shared_ptr<gr::thread::thread> d_thread;
			void command(pmt::pmt_t msg);
			void send_status(pmt::pmt_t flowing);
			void run();

		protected:
			int d_rport;
			int d_lport;
			int d_samprate;
			int d_framesize;

			bool is_ipv6;
			bool d_floor=false;
			bool d_sob_rcvd=false;

			int d_header_size;
			uint64_t d_seq_num;
			uint32_t d_timestamp;
			uint32_t d_ssrc;
			
			uint64_t xmit_sob;
			uint64_t xmit_eob;

			uint16_t d_rtp_ver;
			
			pmt::pmt_t m_squelch;
			pmt::pmt_t b_squelch;
			pmt::pmt_t m_cmd_out;
 
			std::string s_host;
 
			double rx_offset;
			double ctcss;
			std::string sip_user;
			std::string sip_pw;
			std::string sip_uri;

			char d_tmpheaderbuff[12]; // Largest header is 10 bytes

			// A queue is required because we have 2 different timing
			// domains: The network packets and the GR work()/scheduler
			boost::circular_buffer<float>* d_localqueue;
			
			OpusEncoder *encoder;

			std::chrono::time_point<std::chrono::steady_clock> d_rx_start;
			std::chrono::time_point<std::chrono::steady_clock> d_send_ok;

			boost::asio::io_service d_io_service;
			boost::asio::ip::udp::endpoint d_endpoint;
			boost::asio::ip::udp::socket* d_udpsocket;
			boost::asio::io_service d_io_service1;
			boost::asio::ip::udp::endpoint d_endpoint1;
			boost::asio::ip::udp::socket* d_udpsocket1;

			virtual void
			build_header(char headr); // returns header size.  Header is stored in tmpHeaderBuff
			void set_udpsocket(std::string host);
			void free_udpsocket();
			void set_chan(float rx_freq_offset, const std::string& SIP_user, const std::string& SIP_pw, const std::string& uri);

		public:
			opus_rtp_sink_impl(const std::string& host, int rport, int lport, int samprate);
			~opus_rtp_sink_impl();

			bool stop();

			// Where all the action really happens
			int work(
			int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items
			);
		};

  } // namespace PSCR_LMR2LTE
} // namespace gr

#endif /* INCLUDED_PSCR_LMR2LTE_OPUS_RTP_SINK_IMPL_H */

