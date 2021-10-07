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

#ifndef INCLUDED_PSCR_LMR2LTE_OPUS_RTP_SOURCE_IMPL_H
#define INCLUDED_PSCR_LMR2LTE_OPUS_RTP_SOURCE_IMPL_H

#include <PSCR_LMR2LTE/opus_rtp_source.h>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/circular_buffer.hpp>
#include <PSCR_LMR2LTE/packet_headers.h>
#include <chrono>
#include <opus.h>

namespace gr {
  namespace PSCR_LMR2LTE {

		class opus_rtp_source_impl : public opus_rtp_source
		{
		private:
			const pmt::pmt_t d_udp_key;
			std::chrono::time_point<std::chrono::steady_clock> d_start;


			/*!
			* \brief Messages are received in this port.
			*
			* This port receives messages from the scheduler's message
			* handling mechanism. Messages are used to set the 
			* port and payload type.
			*
			* \param msg A pmt message passed from the scheduler's message handling.
			*/
			void command(pmt::pmt_t msg);
			void send_status(void);

		protected:
			bool is_ipv6;
			int d_port;
			int d_samprate;
			uint64_t d_seq_num;

			bool d_data_coming;
			bool d_startup;
			bool d_floor=false;

			unsigned int d_burst_bytes;
			unsigned int d_num_packets;
			uint16_t d_rtp_ver;
			pmt::pmt_t d_cmd_out;
			pmt::pmt_t d_freq;

			double tx_offset;
			double ctcss;
			std::string sip_user;
			std::string sip_pw;
			std::string sip_uri;

			boost::asio::io_service d_io_service;
			boost::asio::ip::udp::endpoint d_endpoint;
			boost::asio::ip::udp::socket* d_udpsocket;
			boost::asio::io_service d_io_service1;
			boost::asio::ip::udp::endpoint d_endpoint1;
			boost::asio::ip::udp::socket* d_udpsocket1;

			boost::asio::streambuf d_read_buffer;
			boost::asio::streambuf d_read_buffer1;

			// A queue is required because we have 2 different timing
			// domains: The network packets and the GR work()/scheduler
			boost::circular_buffer<float>* d_localqueue;
			void set_udpsocket(unsigned short port);
			void free_udpsocket();
			void set_chan(float tx_freq_offset, const std::string& SIP_user, const std::string& SIP_pw, const std::string& uri);

			OpusDecoder *decoder;

		public:
			opus_rtp_source_impl(int port, int samprate, bool ipv6);
			~opus_rtp_source_impl();

			bool stop();

			inline size_t netdata_available();
			inline size_t rtcpdata_available();

			// Where all the action really happens
			int work(
			int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items
			);
		};

  } // namespace PSCR_LMR2LTE
} // namespace gr

#endif /* INCLUDED_PSCR_LMR2LTE_OPUS_RTP_SOURCE_IMPL_H */

