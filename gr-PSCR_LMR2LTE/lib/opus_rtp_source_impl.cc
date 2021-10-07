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
#include "opus_rtp_source_impl.h"

//#define DEBUG
#define ITEMSIZE sizeof(float)

namespace gr {
  namespace PSCR_LMR2LTE {

		opus_rtp_source::sptr
		opus_rtp_source::make(int port, int samprate, bool ipv6)
		{
			return gnuradio::get_initial_sptr
			(new opus_rtp_source_impl(port, samprate, ipv6));
		}

		void opus_rtp_source_impl::command(pmt::pmt_t msg)
		{
			pmt::pmt_t val;
			bool reset_udp = false;
			if (pmt::is_dict(msg)) {
				//make sure the message is for us (ID must match)
				val = pmt::dict_ref (msg, pmt::intern("ID"), pmt::PMT_NIL);
				if (pmt::is_number(val) && pmt::to_long(val)==unique_id()){
					val=pmt::dict_ref (msg, pmt::intern("local port"), pmt::PMT_NIL);
					if (pmt::is_number(val)) {
						d_port=(pmt::to_long(val));
						reset_udp = true;
					}
					val=pmt::dict_ref (msg, pmt::intern("pt"), pmt::PMT_NIL);
					if (pmt::is_number(val)) {
						d_rtp_ver=0x8000|(pmt::to_long(val));
					}
					val=pmt::dict_ref (msg, pmt::intern("Floor"), pmt::PMT_NIL);
					if (pmt::is_symbol(val)) {
						if (val == pmt::string_to_symbol("taken")){
							d_floor = true; //We only transmit if the floor has been taken
							#ifdef DEBUG
							GR_LOG_INFO(d_logger, "Floor taken");
							#endif
						}else{
							d_floor = false;
							#ifdef DEBUG
							GR_LOG_INFO(d_logger, "Floor not taken");
							#endif
						}
					}
				}
				if (reset_udp){
					opus_rtp_source_impl::set_udpsocket(d_port);
				}
			}else if (pmt::is_symbol(msg)){
				std::string s = pmt::symbol_to_string(msg);
				send_status();
			}
		}

		void opus_rtp_source_impl::send_status(void)
		{
			pmt::pmt_t dict(pmt::make_dict());
			dict = pmt::dict_add(dict, pmt::string_to_symbol("codec"), pmt::string_to_symbol("opus"));
			dict = pmt::dict_add(dict, pmt::string_to_symbol("Tx offset"), pmt::from_double(tx_offset));
			if (sip_user.length())
			dict = pmt::dict_add(dict, pmt::string_to_symbol("SIP user"), pmt::string_to_symbol(sip_user));
			if (sip_pw.length())
			dict = pmt::dict_add(dict, pmt::string_to_symbol("SIP password"), pmt::string_to_symbol(sip_pw));
			if (sip_uri.length())
			dict = pmt::dict_add(dict, pmt::string_to_symbol("SIP URI"), pmt::string_to_symbol(sip_uri));
			dict = pmt::dict_add(dict, pmt::string_to_symbol("alias"), pmt::string_to_symbol(alias()));
			dict = pmt::dict_add(dict, pmt::string_to_symbol("RTP source"), pmt::from_long(unique_id()));
			if (sip_user.length() && sip_pw.length() && sip_uri.length())
			message_port_pub(d_cmd_out, dict);
		}

		void opus_rtp_source_impl::set_chan(float tx_freq_offset,
		const std::string& SIP_user, const std::string& SIP_pw, const std::string& uri){
			//save info
			tx_offset=tx_freq_offset;
			sip_user=SIP_user;
			sip_pw=SIP_pw;
			sip_uri=uri;
		}

		/*
	* The private constructor
	*/
		opus_rtp_source_impl::opus_rtp_source_impl(int port, int samprate, bool ipv6)
		: gr::sync_block("opus_rtp_source",
		gr::io_signature::make(0, 0, 0),
		gr::io_signature::make(1, 1, ITEMSIZE)),
		d_udp_key(pmt::intern("udp_data")),
		is_ipv6(ipv6),
		d_port(port),
		d_seq_num(0),
		d_samprate(samprate)
		{
			//GR_LOG_INFO(d_logger, "The private constructor");
			message_port_register_in(pmt::mp("command"));
			set_msg_handler(pmt::mp("command"), boost::bind(&opus_rtp_source_impl::command, this, _1));
			
			d_cmd_out = pmt::string_to_symbol("cmd out");
			message_port_register_out(d_cmd_out);
			d_freq = pmt::string_to_symbol("freq");
			message_port_register_out(d_freq);

			d_localqueue = new boost::circular_buffer<float>(d_samprate);

			d_udpsocket = NULL;
			d_udpsocket1 = NULL;

			set_udpsocket(d_port);
			d_start = std::chrono::steady_clock::now();
			
			d_data_coming=false;
			d_startup=true;
			unsigned int d_burst_bytes=0;
			unsigned int d_num_packets=0;

			/*Create a new decoder state */
			int error;
			decoder = opus_decoder_create(d_samprate, 1, &error);
			if (error<0){
				GR_LOG_ERROR(d_logger, "Unable to create opus decoder");
			}
		}

		/*
	* Our virtual destructor.
	*/
		opus_rtp_source_impl::~opus_rtp_source_impl()  { stop(); }
		
		bool opus_rtp_source_impl::stop()
		{
			free_udpsocket();

			if (d_localqueue) {
				delete d_localqueue;
				d_localqueue = NULL;
			}
			opus_decoder_destroy(decoder);

			return true;
		}
		
		void opus_rtp_source_impl::free_udpsocket(){
			if (d_udpsocket) {
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

		void opus_rtp_source_impl::set_udpsocket(unsigned short port){
			d_startup=true;
			free_udpsocket();
			if (port==0 || port>=65536) return;
			#ifdef DEBUG
			GR_LOG_INFO(d_logger, "Setting UDP socket local port " + std::to_string(port));
			#endif
			//allow RTP sink to also use these sockets
			boost::asio::socket_base::reuse_address option_r(true);
			//setup udp socket
			d_udpsocket = new boost::asio::ip::udp::socket(d_io_service);
			if (is_ipv6) {
				d_udpsocket->open(boost::asio::ip::udp::v6());
				d_udpsocket->set_option(option_r);
				d_udpsocket->bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v6(), port));
			} else {
				d_udpsocket->open(boost::asio::ip::udp::v4());
				d_udpsocket->set_option(option_r);
				d_udpsocket->bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port));
			}
			//setup second socket for RTCP
			d_udpsocket1 = new boost::asio::ip::udp::socket(d_io_service1);
			if (is_ipv6) {
				d_udpsocket1->open(boost::asio::ip::udp::v6());
				d_udpsocket1->set_option(option_r);
				d_udpsocket1->bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v6(), port+1));
			} else {
				d_udpsocket1->open(boost::asio::ip::udp::v4());
				d_udpsocket1->set_option(option_r);
				d_udpsocket1->bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port+1));
			}
			std::stringstream msg_stream;
			msg_stream << unique_id() << " Listening for data on UDP port " << port << ".";
			boost::asio::socket_base::receive_buffer_size option_s(131072);
			d_udpsocket->set_option(option_s);	
			boost::asio::socket_base::receive_buffer_size option;
			d_udpsocket->get_option(option);
			int buffer_size = option.value();
			msg_stream << " Receive buffer size " << buffer_size << ".";
			GR_LOG_INFO(d_logger, msg_stream.str());
		}
		
		size_t opus_rtp_source_impl::netdata_available()
		{
			size_t bytes_readable = 0;
			if (d_udpsocket) {
				// Get amount of data available
				boost::asio::socket_base::bytes_readable command(true);
				d_udpsocket->io_control(command);
				bytes_readable = command.get();
			}

			return bytes_readable;
		}

		size_t opus_rtp_source_impl::rtcpdata_available()
		{
			size_t bytes_readable = 0;
			if (d_udpsocket1) {
				// Get amount of data available
				boost::asio::socket_base::bytes_readable command(true);
				d_udpsocket1->io_control(command);
				bytes_readable = command.get();
			}

			return bytes_readable;
		}

		int opus_rtp_source_impl::work(int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items)
		{
			int itemsreturned;
			gr::thread::scoped_lock guard(d_setlock);

			float* output = (float*)output_items[0];
			int skipped_packets = 0;
			bool send_eob=false;
			bool floor=d_floor;  //use a copy so we can ignore changes while processing this block of output items
			uint64_t eob_tag=0;
			double timeout=0.05;
			
			if (d_startup){
				d_startup=false;
				if (netdata_available()==0){
					d_data_coming=false;
					//let downstream blocks know that nothing is coming
					gr::block::add_item_tag(0, nitems_written(0), d_udp_key, pmt::from_double(0), pmt::intern("opus RTP source"));
					gr::block::add_item_tag(0, nitems_written(0), pmt::intern("tx_eob"), pmt::PMT_T, pmt::intern("opus RTP source"));
				}
			}

			while (floor && (netdata_available()>0)){
				d_start = std::chrono::steady_clock::now();
				if (!d_data_coming){
					d_data_coming=true;
					d_burst_bytes=0;
					d_num_packets=0;
					//send start of block tags
					gr::block::add_item_tag(0, nitems_written(0), d_udp_key, pmt::from_double(1), pmt::intern("udpsrc"));
					gr::block::add_item_tag(0, nitems_written(0), pmt::intern("tx_sob"), pmt::PMT_T, pmt::intern("udpsrc"));
					std::stringstream msg_stream;
					msg_stream << "Offset: " << nitems_written(0) << " Start of Block ";
					GR_LOG_INFO(d_logger, msg_stream.str());
				}

				int bytes_read;

				// read blocks
				boost::asio::streambuf::mutable_buffers_type buf =
				d_read_buffer.prepare(netdata_available());
				// http://stackoverflow.com/questions/28929699/boostasio-read-n-bytes-from-socket-to-streambuf
				bytes_read = d_udpsocket->receive_from(buf, d_endpoint);
				
				if (bytes_read > 0) {

					d_read_buffer.commit(bytes_read);
					#if 0
					GR_LOG_INFO(d_logger, std::to_string(bytes_read) + " bytes read");
					#endif

					// Get the data and add it to our local queue.  We have to maintain a
					// local queue in case we read more samples than noutput_items is asking
					// for.  In that case we'll only return noutput_items bytes
					const unsigned char* read_data =
					boost::asio::buffer_cast<const unsigned char*>(d_read_buffer.data());
					d_burst_bytes+=bytes_read;
					
					uint64_t pkt_seq_num = (((header_rtp*)read_data)->seqnum).value();

					if (d_num_packets > 0) { 
						if (pkt_seq_num > d_seq_num) {
							// Ideally pkt_seq_num = d_seq_num + 1.  Therefore this should do += 0
							// when no packets are dropped.
							skipped_packets += pkt_seq_num - d_seq_num - 1;
						}

						// Store as current for next pass.
						d_seq_num = pkt_seq_num;
					} else {
						// just starting.  Prime it for no loss on the first packet.
						d_seq_num = pkt_seq_num;
					}
					
					float pcm[int(0.12*d_samprate)]; //120ms is the maximum frame size for opus
					int samples = opus_decode_float(decoder, read_data+sizeof(header_rtp), bytes_read-sizeof(header_rtp), pcm, int(0.12*d_samprate), 0);
					
					if (d_localqueue->reserve() < samples) {
						std::stringstream msg_stream;
						msg_stream << "Offset: " << nitems_written(0) << " Circular buffer overflow!";
						GR_LOG_WARN(d_logger, msg_stream.str());
					}
					for (int i = 0; i < samples; i++) {
						d_localqueue->push_back(pcm[i]);
					}
					d_read_buffer.consume(bytes_read);
					
					double samp_period = (double)samples/d_samprate;
					if (samp_period>=0.01 && samp_period<=0.12){
						timeout=samp_period;
					}
					
				}
				d_num_packets++;
			}

			{
				int bytes_read;
				uint16_t version_type;
				if (rtcpdata_available()>0){
					// read blocks
					boost::asio::streambuf::mutable_buffers_type buf =
					d_read_buffer1.prepare(rtcpdata_available());
					// http://stackoverflow.com/questions/28929699/boostasio-read-n-bytes-from-socket-to-streambuf
					bytes_read = d_udpsocket1->receive_from(buf, d_endpoint1);
					d_read_buffer1.commit(bytes_read);
					const char* read_data = boost::asio::buffer_cast<const char*>(d_read_buffer1.data());
					version_type=(((header_rtcp*)read_data)->version_type).value();
					if ((version_type&0xff) == 0xcb){
						//Goodbye detected
						d_data_coming = false;
						send_eob=true;
						std::stringstream msg_stream;
						msg_stream << "Offset: " << nitems_written(0) + d_localqueue->size() << " GOODBYE received"; 
						GR_LOG_INFO(d_logger, msg_stream.str());
					}	
					d_read_buffer1.consume(bytes_read);
					#ifdef DEBUG
					GR_LOG_INFO(d_logger, std::to_string(bytes_read) + " rtcp bytes read");
					#endif
				}
			}
			
			//provide a bail out if packets stop coming
			if ((d_localqueue->size() < noutput_items)){
				auto end = std::chrono::steady_clock::now();
				std::chrono::duration<double> elapsed_seconds = end-d_start;
				//allow small gaps before signalling end of block and sending filler data
				if (elapsed_seconds.count()>timeout) {
					if (d_data_coming && nitems_written(0)){
						//the end was in the previous block sent so send tx_eob with this block of data
						std::stringstream msg_stream;
						msg_stream << "Offset: " << nitems_written(0) << " timeout";
						GR_LOG_INFO(d_logger, msg_stream.str());
						d_data_coming = false;
						send_eob=true;
					}
				}
			}
			
			if (send_eob){
				eob_tag=nitems_written(0) + d_localqueue->size();
				if (d_localqueue->size()>1) eob_tag--;
			}
			
			if (!d_data_coming){
				gr::block::add_item_tag(0, nitems_written(0), d_udp_key, pmt::from_double(0), pmt::intern("opus RTP source"));
				//output zeroes
				for (int i = d_localqueue->size(); i < noutput_items; i++) {
					d_localqueue->push_back(0);
				}
			}

			// let's figure out how much we have in relation to noutput_items

			if (d_localqueue->size() > noutput_items)
			itemsreturned=noutput_items;
			else
			itemsreturned=d_localqueue->size();
			
			// Move the data into the output[] array.

			for (int cur_item = 0; cur_item < itemsreturned; cur_item++) {
				output[cur_item] = d_localqueue->at(0);
				d_localqueue->pop_front();
			}

			if (send_eob){
				//send end of block tags
				//nitems_read/written will refer to the state of the data stream at the start of the work function
				gr::block::add_item_tag(0, eob_tag, d_udp_key, pmt::from_double(0), pmt::intern("opus RTP source"));
				gr::block::add_item_tag(0, eob_tag, pmt::intern("tx_eob"), pmt::PMT_T, pmt::intern("opus RTP source"));
				std::stringstream msg_stream;
				msg_stream << "Offset: " << eob_tag << " End of Block " << d_burst_bytes << " bytes " << d_num_packets << " packets " << itemsreturned << " itemsreturned ";
				//GR_LOG_INFO(d_logger, msg_stream.str());
			}
			
			if (skipped_packets > 0) {
				std::stringstream msg_stream;
				msg_stream << "Offset: " << nitems_written(0) << " [opus RTP source:" << d_port
				<< "] missed  packets: " << skipped_packets;
				GR_LOG_WARN(d_logger, msg_stream.str());
			}
			// If we had less data than requested, it'll be reflected in the return value.
			return itemsreturned;
		}

  } /* namespace PSCR_LMR2LTE */
} /* namespace gr */

