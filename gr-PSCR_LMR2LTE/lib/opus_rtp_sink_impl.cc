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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "opus_rtp_sink_impl.h"
#include <boost/array.hpp>
#include <chrono>
#include <random>

//#define DEBUG
#define CHANNELS 2
#define APPLICATION OPUS_APPLICATION_AUDIO
#define BITRATE 24000*CHANNELS //https://wiki.xiph.org/index.php?title=Opus_Recommended_Settings

#define MAX_PACKET_SIZE (3*1276)

#define ITEMSIZE sizeof(float)

namespace gr {
  namespace PSCR_LMR2LTE {

		opus_rtp_sink::sptr
		opus_rtp_sink::make(const std::string& host, int rport, int lport, int samprate)
		{
			return gnuradio::get_initial_sptr
			(new opus_rtp_sink_impl(host, rport, lport, samprate));
		}
		
		void opus_rtp_sink_impl::command(pmt::pmt_t msg)
		{
			pmt::pmt_t val;
			std::string host;
			bool reset_udp = false;
			int dest_id;
			if (pmt::is_dict(msg)) {
				//make sure the message is for us (ID must match)
				val = pmt::dict_ref (msg, pmt::intern("ID"), pmt::PMT_NIL);
				if (pmt::is_number(val) && pmt::to_long(val)==unique_id()){
					val=pmt::dict_ref (msg, pmt::intern("remote port"), pmt::PMT_NIL);
					if (pmt::is_number(val)) {
						d_rport=(pmt::to_long(val));
						reset_udp = true;
					}
					val=pmt::dict_ref (msg, pmt::intern("pt"), pmt::PMT_NIL);
					if (pmt::is_number(val)) {
						d_rtp_ver=0x8000|(pmt::to_long(val));
					}
					val=pmt::dict_ref (msg, pmt::intern("remote host"), pmt::PMT_NIL);
					if (pmt::is_symbol(val)) {
						host = symbol_to_string (val);
						reset_udp = true;
					}
					val=pmt::dict_ref (msg, pmt::intern("local port"), pmt::PMT_NIL);
					if (pmt::is_number(val)) {
						d_lport=(pmt::to_long(val));
						reset_udp = true;
					}
					val=pmt::dict_ref (msg, pmt::intern("Floor"), pmt::PMT_NIL);
					if (pmt::is_symbol(val)) {
						#ifdef DEBUG
						GR_LOG_INFO(d_logger, "Floor message received " + symbol_to_string (val));
						#endif
						if (val == pmt::string_to_symbol("taken")){
							d_floor = false; //We do not receive if the floor has been taken (that's when we transmit)
							#ifdef DEBUG
							GR_LOG_INFO(d_logger, "Floor taken");
							#endif
						}else{
							d_floor = true;
							#ifdef DEBUG
							GR_LOG_INFO(d_logger, "Floor not taken");
							#endif
						}
					}
				}
				if (reset_udp){
					opus_rtp_sink_impl::set_udpsocket(host);
				}
			}else if (pmt::is_symbol(msg)){
				std::string s = pmt::symbol_to_string(msg);
				send_status(b_squelch);
			}
		}

		void opus_rtp_sink_impl::send_status(pmt::pmt_t squelch)
		{
			if (sip_user.length() && sip_pw.length() && sip_uri.length()){
				pmt::pmt_t dict(pmt::make_dict());
				dict = pmt::dict_add(dict, pmt::string_to_symbol("codec"), pmt::string_to_symbol("opus"));
				dict = pmt::dict_add(dict, pmt::string_to_symbol("Rx offset"), pmt::from_double(rx_offset));
				if (sip_user.length())
				dict = pmt::dict_add(dict, pmt::string_to_symbol("SIP user"), pmt::string_to_symbol(sip_user));
				if (sip_pw.length())
				dict = pmt::dict_add(dict, pmt::string_to_symbol("SIP password"), pmt::string_to_symbol(sip_pw));
				if (sip_uri.length())
				dict = pmt::dict_add(dict, pmt::string_to_symbol("SIP URI"), pmt::string_to_symbol(sip_uri));
				dict = pmt::dict_add(dict, pmt::string_to_symbol("squelch"), squelch);
				//dict = pmt::dict_add(dict, pmt::string_to_symbol("alias"), pmt::string_to_symbol(alias()));
				//dict = pmt::dict_add(dict, pmt::string_to_symbol("d_name"), pmt::string_to_symbol(d_name));
				//dict = pmt::dict_add(dict, pmt::string_to_symbol("d_symbol_name"), pmt::string_to_symbol(d_symbol_name));
				//dict = pmt::dict_add(dict, pmt::string_to_symbol("symbol_name"), pmt::string_to_symbol(symbol_name()));
				//dict = pmt::dict_add(dict, pmt::string_to_symbol("identifier"), pmt::string_to_symbol(identifier()));
				//dict = pmt::dict_add(dict, pmt::string_to_symbol("d_symbolic_id"), pmt::from_long(d_symbolic_id));
				dict = pmt::dict_add(dict, pmt::string_to_symbol("RTP sink"), pmt::from_long(unique_id()));
				message_port_pub(m_squelch, dict);
			}
		}

		void opus_rtp_sink_impl::set_chan(float rx_freq_offset,
		const std::string& SIP_user, const std::string& SIP_pw, const std::string& uri){
			//save info
			rx_offset=rx_freq_offset;
			sip_user=SIP_user;
			sip_pw=SIP_pw;
			sip_uri=uri;
		}
		
		/*
	* The private constructor
	*/
		opus_rtp_sink_impl::opus_rtp_sink_impl(const std::string& host, int rport, int lport, int samprate)
		: gr::sync_block("opus_rtp_sink",
		gr::io_signature::make(1, 1, ITEMSIZE),
		gr::io_signature::make(0, 0, 0))
		{
			d_rx_start=std::chrono::time_point<std::chrono::steady_clock>::max();
			d_send_ok=std::chrono::steady_clock::now();
			message_port_register_in(pmt::mp("command"));
			set_msg_handler(pmt::mp("command"), boost::bind(&opus_rtp_sink_impl::command, this, _1));
			
			m_squelch = pmt::string_to_symbol("squelch");
			message_port_register_out(m_squelch);
			m_cmd_out = pmt::string_to_symbol("cmd out");
			message_port_register_out(m_cmd_out);

			d_udpsocket = NULL;
			d_udpsocket1 = NULL;
			s_host = host;
			d_rport = rport;
			d_samprate = samprate;
			d_framesize = int(0.01*d_samprate); //10ms frame
			d_header_size = 0;
			d_rtp_ver=0x8061; //dynamic payload type 97
			
			xmit_sob=UINT64_MAX;
			xmit_eob=0;
			b_squelch = pmt::PMT_NIL;
			std::random_device rd;
			d_header_size = sizeof(header_rtp);
			d_ssrc = rd() & 0xffffffff; //32 bit random number
			d_timestamp = rd() & 0xffffffff;  //32 bit random number
			d_seq_num = rd() & 0xffff; //16 bit random number
			
			d_localqueue = new boost::circular_buffer<float>(d_samprate);

			int out_multiple = 0.001*d_samprate; //1ms of data

			if (out_multiple & 1)
			out_multiple += 1; // Ensure we get pairs, for instance complex -> ichar pairs

			gr::block::set_output_multiple(out_multiple);
			
			/*Create a new encoder state */
			int error;
			printf("samprate %d\n", d_samprate);
			encoder = opus_encoder_create(d_samprate, CHANNELS, OPUS_APPLICATION_VOIP, &error);
			if (error<0){
				GR_LOG_ERROR(d_logger, "Unable to create opus encoder");
			}
			error = opus_encoder_ctl(encoder, OPUS_SET_BITRATE(BITRATE));
			if (error<0)
			{
				GR_LOG_ERROR(d_logger, "Unable to set opus bitrate");
			}
			error = opus_encoder_ctl(encoder, OPUS_SET_MAX_BANDWIDTH(OPUS_BANDWIDTH_WIDEBAND));
			if (error<0)
			{
				GR_LOG_ERROR(d_logger, "Unable to set opus bandwidth");
			}
		}

		/*
	* Our virtual destructor.
	*/
		opus_rtp_sink_impl::~opus_rtp_sink_impl()
		{ stop(); }

		bool opus_rtp_sink_impl::stop()
		{
			free_udpsocket();

			if (d_localqueue) {
				delete d_localqueue;
				d_localqueue = NULL;
			}
			opus_encoder_destroy(encoder);

			return true;
		}

		void opus_rtp_sink_impl::build_header(char headr)
		{
			uint16_t ver = 0x800b;
			d_seq_num++;
			header_rtp rtp;
			if (headr==1) rtp.version_type = d_rtp_ver | 0x80; //mark the start packet
			else rtp.version_type = d_rtp_ver;
			rtp.seqnum=d_seq_num & 0xffff;
			rtp.ssrc = d_ssrc;
			rtp.timestamp = d_timestamp;
			memcpy((void*)d_tmpheaderbuff, (void*)&rtp, d_header_size);
			
		}
		
		void opus_rtp_sink_impl::free_udpsocket(){
			if (d_udpsocket) {
				gr::thread::scoped_lock guard(d_setlock);

				d_udpsocket->close();
				d_udpsocket = NULL;
				
				d_io_service.reset();
				d_io_service.stop();
			}
			
			if (d_udpsocket1){
				gr::thread::scoped_lock guard(d_setlock);
				
				d_udpsocket1->close();
				d_udpsocket1 = NULL;

				d_io_service1.reset();
				d_io_service1.stop();
			}

		}
		
		void opus_rtp_sink_impl::set_udpsocket(std::string host){
			if (d_rport==0 || d_rport>=65536){
				GR_LOG_ERROR(d_logger, "Invalid remote port number " + std::to_string(d_rport));
				return;
			}
			if (d_lport==0 || d_lport>=65536){
				GR_LOG_ERROR(d_logger, "Invalid local port number " + std::to_string(d_lport));
				return;
			}
			free_udpsocket();
			//allow RTP source to also use these sockets
			boost::asio::socket_base::reuse_address option_r(true);
			#ifdef DEBUG
			GR_LOG_INFO(d_logger, "Setting UDP socket " + host + " local port " + std::to_string(d_lport) + " remote port " + std::to_string(d_rport));
			#endif
			//setup udp socket
			d_udpsocket = new boost::asio::ip::udp::socket(d_io_service);
			d_endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(host), d_rport);

			if (host.find(":") != std::string::npos) is_ipv6 = true;
			else {
				// This block supports a check that a name rather than an IP is provided.
				// the endpoint is then checked after the resolver is done.
				if (d_endpoint.address().is_v6())
				is_ipv6 = true;
				else
				is_ipv6 = false;
			}

			if (is_ipv6) {
				d_udpsocket->open(boost::asio::ip::udp::v6());
				d_udpsocket->set_option(option_r);
				d_udpsocket->bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v6(), d_lport));
			} else {
				d_udpsocket->open(boost::asio::ip::udp::v4());
				d_udpsocket->set_option(option_r);
				d_udpsocket->bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), d_lport));
			}

			//setup second socket for RTCP
			{
				d_udpsocket1 = new boost::asio::ip::udp::socket(d_io_service1);
				d_endpoint1 = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(host), d_rport+1);
				if (is_ipv6) {
					d_udpsocket1->open(boost::asio::ip::udp::v6());
					d_udpsocket1->set_option(option_r);
					d_udpsocket1->bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v6(), d_lport+1));
				} else {
					d_udpsocket1->open(boost::asio::ip::udp::v4());
					d_udpsocket1->set_option(option_r);
					d_udpsocket1->bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), d_lport+1));
				}
			}


			d_floor=true; //ready to start receiving
			d_send_ok=std::chrono::steady_clock::now();
			#ifdef DEBUG
			GR_LOG_INFO(d_logger, "UDP socket set " + host + " local port " + std::to_string(d_lport) + " remote port " + std::to_string(d_rport));
			#endif
		}

		int opus_rtp_sink_impl::work(int noutput_items, 
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
		{
			gr::thread::scoped_lock guard(d_setlock);
			const float* in_items = (const float*)input_items[0]; //this block has 1 input so input_items[1] doesn't exist
			bool eob_rcvd=false;
			bool floor=d_floor;  //use a copy so we can ignore changes while processing this block of input items
			std::chrono::time_point<std::chrono::steady_clock> timenow=std::chrono::steady_clock::now();
			std::vector<tag_t> tags;
			int hangtime=200;
			
			if (!floor) {
				#ifdef DEBUG
				if (timenow>=d_send_ok)	GR_LOG_INFO(d_logger, "Not OK to send RTP packets");
				#endif
				d_send_ok=timenow+std::chrono::hours(1);;  //keep from transmitting packets until after we have the floor
			}
			gr::block::get_tags_in_window(tags, 0, 0, noutput_items, pmt::intern("squelch_sob"));
			if (floor && tags.size()){
				#ifdef DEBUG
				GR_LOG_INFO(d_logger, "squelch_sob received");
				#endif
				if (timenow>=d_send_ok){
					d_sob_rcvd=true;
					std::random_device rd;
					std::stringstream msg_stream;
					d_rx_start=std::chrono::steady_clock::now(); //remember sob time
					msg_stream << "Offset "<< tags[0].offset << " Squelch SOB ";
					GR_LOG_INFO(d_logger, msg_stream.str());
					//we have a start of block so start transmitting packets
					d_localqueue->clear();	//clear any old data that didn't get transmitted			
					xmit_sob=tags[0].offset; //location of 1st sob in window
					xmit_eob=UINT64_MAX;
					d_timestamp = rd() & 0xffffffff;  //32 bit random number
					d_seq_num = rd() & 0xffff; //16 bit random number
					b_squelch = pmt::PMT_T;
					send_status(b_squelch);
				}
			}
			
			gr::block::get_tags_in_window(tags, 0, 0, noutput_items, pmt::intern("squelch_eob"));
			if (tags.size()){
				//we have an end of block
				xmit_eob=tags[tags.size()-1].offset; //location of last eob in window
				#ifdef DEBUG
				GR_LOG_INFO(d_logger, "eob received");
				#endif
				if (floor && (d_send_ok>timenow+std::chrono::milliseconds(hangtime))){
					d_send_ok=timenow+std::chrono::milliseconds(hangtime); //repeater is done sending signal
					#ifdef DEBUG
					GR_LOG_INFO(d_logger, "OK to send RTP packets after hangtime");
					#endif
				}
				if (timenow>=d_send_ok){
					eob_rcvd=true; //send goodbye with last packet
					std::chrono::duration<double> diff = timenow-d_rx_start;
					std::stringstream msg_stream;
					msg_stream << "Offset "<< tags[0].offset << " Squelch EOB "  << diff.count() <<"sec from SOB";
					GR_LOG_INFO(d_logger, msg_stream.str());
					d_rx_start=std::chrono::time_point<std::chrono::steady_clock>::max();
				}
			}
			// Build a long local queue to pull from so we can break it up easier

			int start_item=0;
			for (start_item=0; start_item<noutput_items && d_sob_rcvd; start_item++){
				if (in_items[start_item]!=0) break; //remove silence at beginning
			}
			for (int i = start_item; (i < noutput_items) && (b_squelch == pmt::PMT_T); i++) {
				//output from the first sob found in the window to the last eob. With only a couple ms in window that works well
				if ((nitems_read(0)+i)>=xmit_sob && (nitems_read(0)+i)<=xmit_eob){
					d_localqueue->push_back((in_items[i]));
				}
			}
			
			if (d_localqueue->size() && d_udpsocket == NULL){
				//if no message has come in causing the socket to be setup then setup using params
				GR_LOG_INFO(d_logger, "Setup socket using parameters");
				set_udpsocket(s_host);
			}
			#if 0
			if (d_localqueue->size()>=d_framesize || eob_rcvd){
				GR_LOG_INFO(d_logger, "d_localqueue->size() " + std::to_string(d_localqueue->size()) + " d_framesize " + std::to_string(d_framesize) + " eob_rcvd " + std::to_string(eob_rcvd));
			}
			#endif

			while (d_udpsocket && (d_localqueue->size()>=d_framesize || eob_rcvd)){
				unsigned char cbits[MAX_PACKET_SIZE];
				float localbuffer[CHANNELS*d_framesize];
				std::vector<boost::asio::const_buffer> transmitbuffer; // Local boost buffer for transmitting
				int nbBytes;
				int cntr;
				memset(localbuffer, 0, CHANNELS*d_framesize*sizeof(float));
				for (cntr=0;cntr<d_framesize && d_localqueue->size();cntr++) {
					for (int i=0;i<CHANNELS;i++){
						localbuffer[cntr*CHANNELS+i]=d_localqueue->at(0);
					}
					d_localqueue->pop_front();
				}
				/* Encode the 10ms frame. */
				nbBytes = opus_encode_float(encoder, localbuffer, d_framesize, cbits, MAX_PACKET_SIZE);

				//Transmit encoded data
				char headr=0;
				// Clear the next transmit buffer
				transmitbuffer.clear();

				if (d_sob_rcvd) {
					headr=1; //SOH
					d_sob_rcvd=false;
				}
				if (eob_rcvd && d_localqueue->size()==0) {
					headr=23; //ETB
					eob_rcvd=false;
				}
				build_header(headr);
				transmitbuffer.push_back(boost::asio::buffer((const void*)d_tmpheaderbuff, d_header_size));
				transmitbuffer.push_back(boost::asio::buffer((const void*)cbits, nbBytes));
				
				// Send
				#ifdef DEBUG
				if (d_localqueue->size()>=d_framesize || eob_rcvd){
					GR_LOG_INFO(d_logger, "send RTP");
				}
				#endif
				d_udpsocket->send_to(transmitbuffer, d_endpoint);
				if (headr==23){
					b_squelch = pmt::PMT_F;
					if (floor){
						send_status(b_squelch);
						//send RTCP goodbye
						header_rtcp rtcp;
						rtcp.version_type = 0x81cb; //Goodbye
						rtcp.length = 1; 
						rtcp.ssrc = d_ssrc;

						std::vector<boost::asio::const_buffer> transmitbuffer1;
						// Set up for transmit
						transmitbuffer1.push_back(
						boost::asio::buffer((const void*)&rtcp, sizeof(rtcp)));

						// Send
						#ifdef DEBUG
						GR_LOG_INFO(d_logger, "send RTCP");
						#endif
						d_udpsocket1->send_to(transmitbuffer1, d_endpoint1);
					}
				}
				headr=0;
				d_timestamp+=d_framesize;
			}
			
			if (eob_rcvd) d_sob_rcvd=false; //It's too late to send out anything stored in the buffer
			return noutput_items;
		}

  } /* namespace PSCR_LMR2LTE */
} /* namespace gr */

