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
#include "sip_signaling_impl.h"
#include <chrono>

#define SINK 1
#define SRC 2

#define PJ_LOG_LEVEL 1 //logging level for pjsua2 (1 for errors only, 6 maximum)

#define ANS_MODE_HDR "Answer-Mode"
#define X_MCS_TYPE_HDR "X-Mcx-Type"

//#define DEBUG

namespace gr {
  namespace PSCR_LMR2LTE {

		sip_signaling_impl *sip_signaling_impl::instance_;
		gr::logger_ptr sip_signaling_impl::g_logger;

		sip_signaling::sptr
		sip_signaling::make(char* SIP_domain, char* PCSCF_addr)
		{
			
			return gnuradio::get_initial_sptr
			(new sip_signaling_impl(SIP_domain, PCSCF_addr));
		}

		bool sip_signaling_impl::parseSockAddr(std::string sockAddress, std::string* host, uint16_t* port){
			size_t last_colon = sockAddress.rfind(":");
			if (last_colon==std::string::npos) {
				return false; //port number not found
			}
			if (port){
				*port = (uint16_t)std::stoi(sockAddress.substr(last_colon+1));
			}
			if (host){
				//look for ipv6 delimiters
				size_t hoststart = sockAddress.find("[");
				size_t hoststop = sockAddress.find("]");
				if (hoststart >= hoststop){
					hoststart=-1;
					hoststop=last_colon;
				}
				*host = sockAddress.substr(hoststart+1,hoststop-hoststart-1);
			}
			return true;
		}

		void sip_signaling_impl::msg_in(pmt::pmt_t msg)
		{
			pmt::pmt_t val;
			int uniqueID=-1;
			int acct;
			int* this_id;
			int* that_id;
			int sink_src=0;
			#ifdef DEBUG
			std::stringstream msg_stream;
			msg_stream << "Message received: " << msg;
			GR_LOG_INFO(d_logger, msg_stream.str());
			#endif
			if (pmt::is_dict(msg)) {
				val=pmt::dict_ref (msg, pmt::intern("RTP sink"), pmt::PMT_NIL);
				if (pmt::is_integer(val)){
					sink_src=SINK;
					uniqueID=pmt::to_long(val);
				}else{
					val=pmt::dict_ref (msg, pmt::intern("RTP source"), pmt::PMT_NIL);
					if (pmt::is_integer(val)){
						sink_src=SRC;
						uniqueID=pmt::to_long(val);
					}
				}
				if (uniqueID>0){
					val=pmt::dict_ref (msg, pmt::intern("SIP user"), pmt::PMT_NIL);
					if(pmt::is_symbol(val)){
						//determine which account is tied to this sink/source
						for (acct=0;acct<PJSUA_MAX_ACC;acct++){
							if (sink_src==SINK) {
								this_id=&(RFchan[acct].sink_id);
								that_id=&(RFchan[acct].src_id);
							}else{
								this_id=&(RFchan[acct].src_id);
								that_id=&(RFchan[acct].sink_id);
							}
							//add to list of accounts if not already there
							if (*this_id==uniqueID){
								//matching account already in list
								if (pmt::symbol_to_string(val)!=RFchan[acct].sip_user){
									GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " SIP user change not allowed!");
									throw std::runtime_error(std::string("[SIP Signaling] SIP user change not allowed!"));
									return;
								}
								break; //already in list
							}else if (*this_id==-1 && *that_id!=-1){
								//half of this account has been setup
								//check for matching SIP user
								if (pmt::symbol_to_string(val) == RFchan[acct].sip_user) {
									val=pmt::dict_ref (msg, pmt::intern("SIP password"), pmt::PMT_NIL);
									if (pmt::symbol_to_string(val)!=RFchan[acct].sip_pw){
										GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " SIP password change not allowed!");
										throw std::runtime_error(std::string("[SIP Signaling] SIP password change not allowed!"));
										return;
									}
									val=pmt::dict_ref (msg, pmt::intern("SIP URI"), pmt::PMT_NIL);
									if (pmt::symbol_to_string(val)!=RFchan[acct].uri){
										GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " SIP URI change not allowed!");
										throw std::runtime_error(std::string("[SIP Signaling] SIP URI change not allowed!"));
										return;
									}
									val=pmt::dict_ref (msg, pmt::intern("codec"), pmt::PMT_NIL);
									if (!pmt::equal(val,RFchan[acct].codec)){
										GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " codec change not allowed!");
										throw std::runtime_error(std::string("[SIP Signaling] codec change not allowed!"));
										return;
									}
									//set channel label & LEDs
									sip_signaling_impl::cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(acct+1) + "_Reg"),pmt::string_to_symbol("gray")));
									sip_signaling_impl::cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(acct+1) + "_Call"),pmt::string_to_symbol("gray")));
									sip_signaling_impl::cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(acct+1) + "_Floor"),pmt::string_to_symbol("gray")));
									sip_signaling_impl::cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(acct+1)), pmt::string_to_symbol("CH" + std::to_string(acct+1) + '\n' 
									+ RFchan[acct].sip_user + '\n' 
									+ RFchan[acct].uri + '\n'
									//+ RFchan[acct].rhost
									)));
									*this_id=uniqueID;
									std::stringstream msg_stream;
									msg_stream << "acct: " << acct << " " << RFchan[acct].sip_user << " sink_id " << RFchan[acct].sink_id << " src_id " << RFchan[acct].src_id;
									GR_LOG_INFO(d_logger, msg_stream.str());
									break;
								}
							}else if (*this_id==-1 && *that_id==-1) {
								//add a new account
								*this_id=uniqueID;
								if (pmt::is_symbol(val)){
									RFchan[acct].sip_user=pmt::symbol_to_string(val);
									val=pmt::dict_ref (msg, pmt::intern("SIP password"), pmt::PMT_NIL);
									if(pmt::is_symbol(val)){
										RFchan[acct].sip_pw=pmt::symbol_to_string(val);
									}else{
										GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " Invalid SIP password!");
										throw std::runtime_error(std::string("[SIP Signaling] Invalid SIP password!"));
										return;
									}
									val=pmt::dict_ref (msg, pmt::intern("SIP URI"), pmt::PMT_NIL);
									if(pmt::is_symbol(val)){
										RFchan[acct].uri=pmt::symbol_to_string(val);
									}else{
										GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " Invalid SIP URI!");
										throw std::runtime_error(std::string("[SIP Signaling] Invalid SIP URI!"));
										return;
									}
									val=pmt::dict_ref (msg, pmt::intern("codec"), pmt::PMT_NIL);
									if(pmt::is_symbol(val)){
										if (pmt::eq(d_codec, pmt::PMT_NIL)){
											d_codec=val;
										}
										if (pmt::eq(d_codec, val)){
											RFchan[acct].codec=val;
										}else{
											GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " All channels must use the same codec!");
											throw std::runtime_error(std::string("[SIP Signaling] All channels must use the same codec!"));
											return;
										}
									}else{
										GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " Invalid codec!");
										throw std::runtime_error(std::string("[SIP Signaling] Invalid codec!"));
										return;
									}
								}
								break;
							}
						}
						if (acct>=PJSUA_MAX_ACC){
							std::stringstream msg_stream;
							msg_stream << acct << " RTP sinks/sources exceeds PJSUA_MAX_ACC";
							for (acct=0;acct<PJSUA_MAX_ACC;acct++){
								msg_stream << "\nacct " << acct << " sink id " << RFchan[acct].sink_id << " source id " << RFchan[acct].src_id;
							}								
							GR_LOG_WARN(d_logger, msg_stream.str());
							return;
						}
						if (sink_src==SINK){
							val=pmt::dict_ref (msg, pmt::intern("squelch"), pmt::PMT_NIL);
							if (pmt::is_bool(val)){
								//squelch open/close reported. Time to send floor request/release
								if (val==pmt::PMT_F && RFchan[acct].squelch!=pmt::PMT_T){
									//ignore if squelch is closed without being opened
								}else{
									RFchan[acct].squelch=val;
								}
							}
						}
					}
				}else{
					//It's not a message we were expecting.  Ignore it.
				}
				
			}
		}
		
		void sip_signaling_impl::set_ports(pjsua_acc_id acc_id)
		{
			int acct;
			
			for (acct=0;acct<PJSUA_MAX_ACC && acc_id!=RFchan[acct].acc_id;acct++){}
			if (acct>=PJSUA_MAX_ACC){
				GR_LOG_ERROR(d_logger, "set_ports for unknown account! " + std::to_string(acct));
				return;
			}
			if (RFchan[acct].lport>0 && RFchan[acct].rport>0){
				pmt::pmt_t sink(pmt::make_dict());
				//send port, pt, host to sink block
				sink = pmt::dict_add(sink, pmt::string_to_symbol("ID"), pmt::from_long(RFchan[acct].sink_id));
				sink = pmt::dict_add(sink, pmt::string_to_symbol("remote host"), pmt::string_to_symbol(RFchan[acct].rhost));
				sink = pmt::dict_add(sink, pmt::string_to_symbol("remote port"), pmt::from_long(RFchan[acct].rport));
				sink = pmt::dict_add(sink, pmt::string_to_symbol("local port"), pmt::from_long(RFchan[acct].lport));
				sink = pmt::dict_add(sink, pmt::string_to_symbol("pt"), pmt::from_long(RFchan[acct].txpt));
				sink = pmt::dict_add(sink, pmt::string_to_symbol("SIP signaling"), pmt::from_long(unique_id()));
				sip_signaling_impl::cmd_out(sink);
				pmt::pmt_t source(pmt::make_dict());
				//send port, pt to source block
				source = pmt::dict_add(source, pmt::string_to_symbol("ID"), pmt::from_long(RFchan[acct].src_id));
				source = pmt::dict_add(source, pmt::string_to_symbol("local port"), pmt::from_long(RFchan[acct].lport));
				source = pmt::dict_add(source, pmt::string_to_symbol("pt"), pmt::from_long(RFchan[acct].rxpt));
				source = pmt::dict_add(source, pmt::string_to_symbol("SIP signaling"), pmt::from_long(unique_id()));
				sip_signaling_impl::cmd_out(source);
			}else{
				GR_LOG_WARN(d_logger, RFchan[acct].sip_user + " set_ports with lport:" + std::to_string(RFchan[acct].lport) + " and rport:" + std::to_string(RFchan[acct].rport));
			}
		}
		
		void sip_signaling_impl::cmd_out(pmt::pmt_t cmd)
		{
			message_port_pub(d_cmd_out, cmd);
			std::stringstream msg_stream;
			#ifdef DEBUG
			msg_stream << "Message sent: " << cmd;
			GR_LOG_INFO(d_logger, msg_stream.str());
			#endif
		}

		int sip_signaling_impl::make_call(int acctnum){
			pj_status_t status;
			pj::AccountInfo ai = RFchan[acctnum].acc->getInfo();

			if (ai.regIsActive==true && ai.regStatus>=200 && ai.regStatus<300){
				RFchan[acctnum].del_call_scheduled = false;
				if (!RFchan[acctnum].call){
					RFchan[acctnum].call = new pjsua2Call(*RFchan[acctnum].acc);
				}
				pj::CallOpParam prm(true);
				pj::SipHeader sHeader;
				prm.opt.audioCount = 1;
				prm.opt.videoCount = 0;
				sHeader.hName=X_MCS_TYPE_HDR;
				sHeader.hValue = "MCPTT;Private;WithFloor";
				prm.txOption.headers.push_back(sHeader);
				sHeader.hName=ANS_MODE_HDR;
				sHeader.hValue = "Auto";
				prm.txOption.headers.push_back(sHeader);
				//prm.options = 2;
				#ifdef DEBUG
				GR_LOG_INFO(d_logger, RFchan[acctnum].sip_user + " make call to " + RFchan[acctnum].uri);
				#endif
				try {
					RFchan[acctnum].call->makeCall(RFchan[acctnum].uri, prm);
					GR_LOG_INFO(d_logger, RFchan[acctnum].sip_user + " Calling " + RFchan[acctnum].uri);
					return PJ_SUCCESS;
				} catch(pj::Error& err) {
					GR_LOG_ERROR(d_logger, RFchan[acctnum].sip_user + " Call failed to " + RFchan[acctnum].uri + " " + err.info());
					sip_signaling_impl::cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(acctnum+1) + "_Call"),pmt::string_to_symbol("red")));
					sip_signaling_impl::cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(acctnum+1) + "_Floor"),pmt::string_to_symbol("gray")));
				}
			}
			return PJ_FALSE;

		}
		
		//match account ID with RF channel information
		struct RFchannel* sip_signaling_impl::getChanInfo(pjsua_acc_id acc_id){
			for (int i=0;i<PJSUA_MAX_ACC;i++){
				if (acc_id==RFchan[i].acc_id) return &RFchan[i];
			}
			return (RFchannel*) NULL;
		}
		
		/*
		* The private constructor
		*/
		sip_signaling_impl::sip_signaling_impl(char* SIP_domain, char* PCSCF_addr)
		: gr::block("sip_signaling",
		gr::io_signature::make(0, 0, 0),
		gr::io_signature::make(0, 0, 0)),
		d_finished(false)
		{
			GR_LOG_INFO(d_logger, "The private constructor");
			instance_ = this;
			g_logger = d_logger;
			s_SIP_domain = SIP_domain;
			s_PCSCF_addr = PCSCF_addr;
			
			d_argc = 1;
			d_argv = new char;
			d_argv[0] = '\0';
			d_codec=pmt::PMT_NIL;
			for (int i=0;i<PJSUA_MAX_ACC;i++){
				RFchan[i].chan=i;
				RFchan[i].src_id=-1;
				RFchan[i].sink_id=-1;
				RFchan[i].squelch=pmt::PMT_NIL;
				RFchan[i].rxpt=96;
				RFchan[i].txpt=96;
				RFchan[i].rhost="127.0.0.1";
				RFchan[i].lport=0;
				RFchan[i].rport=0;
				RFchan[i].del_call_scheduled=false;
				RFchan[i].call=NULL;
				RFchan[i].codec=pmt::PMT_NIL;
			}
			message_port_register_in(pmt::mp("msg in"));
			set_msg_handler(pmt::mp("msg in"), boost::bind(&sip_signaling_impl::msg_in, this, _1));
			d_cmd_out = pmt::string_to_symbol("cmd out");
			message_port_register_out(d_cmd_out);
		}

		bool sip_signaling_impl::start()
		{
			GR_LOG_INFO(d_logger, "sip_signaling_impl::start");
			//NOTE: any PJSUA function calls in here will cause a crash later
			
			// (copied from message_strobe_impl.cc)
			d_finished = false;
			d_run = false;
			d_thread = boost::shared_ptr<gr::thread::thread>(
			new gr::thread::thread(boost::bind(&sip_signaling_impl::run, this)));
			GR_LOG_INFO(d_logger, "return block::start()");

			return block::start();
		}

		void sip_signaling_impl::run()
		{
			#ifdef DEBUG
			GR_LOG_INFO(d_logger, "sip_signaling_impl::run()");
			#endif
			if (d_message_subscribers!=d_subscribers){
				//a channel has appeared (or disappeared)
				d_subscribers=d_message_subscribers;
				//pmt::print(d_message_subscribers);
				//get list of RTP sources and sinks
				sip_signaling_impl::cmd_out(pmt::string_to_symbol("?"));
				#ifdef DEBUG
				GR_LOG_INFO(d_logger, "? sent");
				#endif
			}

			//pjsua setup
			/* Instantiate the Endpoint */
			pj::Endpoint *ep = new pj::Endpoint;
			/* Create the library */
			try {
				ep->libCreate();
			} catch(pj::Error& err) {
				GR_LOG_ERROR(d_logger, "Error in pjsua2 libCreate():" + err.info());
				throw std::runtime_error(std::string("[SIP Signaling] Error in pjsua2 libCreate():" + err.info())); 
			}
			#ifdef DEBUG
			GR_LOG_INFO(d_logger, "Initialize the Library");
			#endif
			/* Initialize the Library and Configure the Settings */
			try {
				pj::EpConfig ep_cfg;
				ep_cfg.uaConfig.maxCalls=PJSUA_MAX_ACC; //only one call per account
				ep_cfg.uaConfig.outboundProxies.push_back(s_PCSCF_addr);
				ep_cfg.logConfig.level = PJ_LOG_LEVEL; //logging level for pjsua2 (1 for errors only, 6 maximum)
				// Specify customization of settings in ep_cfg
				ep->libInit(ep_cfg);
			} catch(pj::Error& err) {
				GR_LOG_ERROR(d_logger, "Error in pjsua2 libInit():" + err.info());
				throw std::runtime_error(std::string("[SIP Signaling] Error in pjsua2 libInit():" + err.info())); 
			}
			#ifdef DEBUG
			GR_LOG_INFO(d_logger, "Create UDP transport");
			#endif
			/* Create UDP transport for SIP signaling */
			try {
				pj::TransportConfig tcfg;
				tcfg.port = 5060;
				pj::TransportId tid = ep->transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
			} catch(pj::Error& err) {
				GR_LOG_ERROR(d_logger, "Error creating transport:" + err.info());
				throw std::runtime_error(std::string("[SIP Signaling] Error creating transport " + err.info()));
			}
			#ifdef DEBUG
			GR_LOG_INFO(d_logger, "Disable sound device");
			#endif
			/* Disable sound device in pjsua */
			pj::AudDevManager& aud_mgr = pj::Endpoint::instance().audDevManager();
			aud_mgr.setNoDev();
			//wait for codec to be known
			for (int i=0;i<1000 && !pmt::is_symbol(d_codec);i++){
				boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			}
			std::string codecName;
			if (!pmt::is_symbol(d_codec)){
				GR_LOG_WARN(d_logger, "Codec Not Defined! Using opus");
				codecName="OPUS";
			}else{
				codecName=pmt::symbol_to_string(d_codec);
			}
			/* Set codec priority */
			pj::Endpoint::instance().codecSetPriority("*", 0); //disable all codecs
			//set codec based on RTP block
			boost::algorithm::to_upper(codecName);
			pj::Endpoint::instance().codecSetPriority(codecName, PJMEDIA_CODEC_PRIO_NORMAL); //enable codec
			GR_LOG_INFO(d_logger, "Start the PJSUA Library " + codecName + " codec");
			/* Start the Library */
			try {
				ep->libStart();
			} catch(pj::Error& err) {
				GR_LOG_ERROR(d_logger, "Error starting pjsua2 library:" + err.info());
				throw std::runtime_error(std::string("[SIP Signaling] Error starting pjsua2 library " + err.info()));
			}
			int state = ep->libGetState();
			if (state!=PJSUA_STATE_RUNNING)
			GR_LOG_ERROR(d_logger, "PJSUA Library is not running! " + std::to_string(state));
			
			d_run=true;
			while (!d_finished) {
				try{
					//gnuradio keeps a list of listeners to our messages (d_message_subscribers)
					//if the list changes we will query the listeners
					if (d_message_subscribers!=d_subscribers){
						//a channel has appeared (or disappeared)
						d_subscribers=d_message_subscribers;
						//pmt::print(d_message_subscribers);
						//get list of RTP sources and sinks
						sip_signaling_impl::cmd_out(pmt::string_to_symbol("?"));
						#ifdef DEBUG
						GR_LOG_INFO(d_logger, "? sent after channel change");
						#endif
					}
					//register after source and sink are identified
					for (int acct=0;acct<PJSUA_MAX_ACC;acct++){
						if (!RFchan[acct].acc && RFchan[acct].src_id>=0 && RFchan[acct].sink_id>=0 && !RFchan[acct].sip_user.empty() && !RFchan[acct].sip_pw.empty() && !RFchan[acct].uri.empty()){
							pj::AccountConfig acc_cfg;
							GR_LOG_INFO(d_logger, RFchan[acct].sip_user + " Create account " + std::to_string(acct));
							acc_cfg.idUri = "sip:" + RFchan[acct].sip_user + "@" + s_SIP_domain;
							acc_cfg.regConfig.registrarUri = "sip:" + s_SIP_domain;
							acc_cfg.sipConfig.authCreds.push_back( pj::AuthCredInfo("digest", s_SIP_domain, RFchan[acct].sip_user + 
							"@" + s_SIP_domain, PJSIP_CRED_DATA_PLAIN_PASSWD, RFchan[acct].sip_pw) );
							acc_cfg.mediaConfig.srtpUse = PJMEDIA_SRTP_DISABLED;
							acc_cfg.mediaConfig.transportConfig.port = 16384 + acct*2;
							RFchan[acct].acc = new pjsua2Account;
							try {
								RFchan[acct].acc->create(acc_cfg);
							} catch(pj::Error& err) {
								GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " Account " + std::to_string(acct+1) + " creation error: " + err.info());
								//display channel in red
								sip_signaling_impl::cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(acct+1) + "_Reg"),pmt::string_to_symbol("red")));
								continue;
							}
							RFchan[acct].acc_id=RFchan[acct].acc->getId();
							
							{
								pj::AccountInfo ai;
								std::chrono::time_point<std::chrono::steady_clock> reg_start=std::chrono::steady_clock::now();
								std::chrono::duration<double> diff;
								bool reg_state=false;
								do {/*wait for registration*/
									ai = RFchan[acct].acc->getInfo();
									if (ai.regIsActive==true && ai.regStatus>=200 && ai.regStatus<300){
										reg_state=true;
									}
									diff = std::chrono::steady_clock::now()-reg_start;
								}while (!reg_state && ai.regStatus<300 && diff.count()<10.0);// timeout after 10sec., exit if fail
								if (reg_state){
									//display registration in green
									sip_signaling_impl::cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(acct+1) + "_Reg"),pmt::string_to_symbol("green")));
									GR_LOG_INFO(d_logger, RFchan[acct].sip_user + " Registration succeeded");
								#ifdef DEBUG
									//it's not necessary to make the calls here		
									make_call(acct);
								#endif
								}else{
									GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " Registration failed");
									//display registration in red
									sip_signaling_impl::cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(acct+1) + "_Reg"),pmt::string_to_symbol("red")));
								}
							}
						}
					}
					//pjsua monitoring for outside messages
					for (int acct=0;acct<PJSUA_MAX_ACC;acct++){
						if (pmt::is_bool(RFchan[acct].squelch)){
							//squelch open/close reported. Time to send floor request/release
							const pj_str_t SIP_INFO = pj_str((char*)"INFO");
							pjsua_msg_data msg_data;
							std::string body;
							std::stringstream msg_stream;
							msg_stream << RFchan[acct].sip_user;

							if (RFchan[acct].squelch==pmt::PMT_T){
								if (!RFchan[acct].call){
									GR_LOG_INFO(d_logger, RFchan[acct].sip_user + " Call is not up. Make call ");
									if (make_call(acct)!=PJ_SUCCESS){
										GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " Unable to complete call! "); 
										break;
									}
									pj::CallInfo ci = RFchan[acct].call->getInfo();
									GR_LOG_INFO(d_logger, RFchan[acct].sip_user + " Call state " + ci.stateText);
									//wait for call to go through
									for (int cntr=0;cntr<1000 && ci.state!=PJSIP_INV_STATE_CONFIRMED;cntr++){
										if (RFchan[acct].call){
											try{
												ci=RFchan[acct].call->getInfo();
											} catch(pj::Error& err) {
												GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " Call gone before completed! " + err.info());
												break;
											}
											boost::this_thread::sleep(boost::posix_time::milliseconds(1));
										}else{
											GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " Call gone before completed! "); 
											break;
										}
									}
									if (ci.state!=PJSIP_INV_STATE_CONFIRMED){
										GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " call failed!");
									}
								}
								if (RFchan[acct].call){
									msg_stream << " Squelch SOB - Request floor"; //request floor
									body = "Method=MCPTT-Control\r\naction=request\r\npriority=7";
								}
							}else{
								msg_stream << " Squelch EOB - Release floor"; //release floor
								if (RFchan[acct].call){
									body = "Method=MCPTT-Control\r\naction=release";
								}
							}
							#ifdef DEBUG
							GR_LOG_INFO(d_logger, RFchan[acct].sip_user + " Send floor request/release");
							#endif

							if (RFchan[acct].call){
								GR_LOG_INFO(d_logger, msg_stream.str());
								pjsua_msg_data_init(&msg_data);
								msg_data.content_type = pj_str((char*)"text/plain");
								msg_data.msg_body = pj_str((char*)body.c_str());
								pj_status_t status = pjsua_call_send_request(RFchan[acct].call_id, &SIP_INFO, &msg_data);
								if (status != PJ_SUCCESS) { 
									GR_LOG_ERROR(d_logger, RFchan[acct].sip_user + " Error sending floor request/release! " + 
									std::to_string(acct) + " callId " + std::to_string(RFchan[acct].call_id));
									//display call in red
									sip_signaling_impl::cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(acct+1) + "_Call"),pmt::string_to_symbol("red")));
									sip_signaling_impl::cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(acct+1) + "_Floor"),pmt::string_to_symbol("gray")));
								}
								#ifdef DEBUG
								GR_LOG_INFO(d_logger, RFchan[acct].sip_user + " floor request/release sent " + body);
								#endif
							}
							RFchan[acct].squelch=pmt::PMT_NIL;
						}
						if (RFchan[acct].del_call_scheduled==true) {
							GR_LOG_INFO(d_logger, RFchan[acct].sip_user + " Deleting call to " + RFchan[acct].uri);
							if (RFchan[acct].call) {
								#ifdef DEBUG
								GR_LOG_INFO(d_logger, RFchan[acct].sip_user + " Deleting call");
								#endif
								delete RFchan[acct].call;
								RFchan[acct].call=NULL;
							}
							RFchan[acct].del_call_scheduled=false;
						}
					}
					//wait a little time so the processor isn't always spinning here
					boost::this_thread::sleep(boost::posix_time::milliseconds(1));

				} catch(pj::Error& err) {
					GR_LOG_ERROR(sip_signaling_impl::g_logger, "Error in run():" + err.info());
				}
			}
			ep->hangupAllCalls();
			/* Destroy pjsua2 library */
			ep->libDestroy(); 
			delete ep;
			d_run=false;
		}

		/*
		* Our virtual destructor.
		*/
		sip_signaling_impl::~sip_signaling_impl()  { stop(); }
		bool sip_signaling_impl::stop()
		{
			//NOTE: any PJSUA function calls in here will cause a crash
			
			d_finished=true;
			//wait for run() to finish
			do{
				boost::this_thread::sleep(boost::posix_time::milliseconds(1));
			}while (d_run);

			return true;
		}
		
		pjsua2Account::~pjsua2Account()
		{
			// Invoke shutdown() first..
			shutdown();
			// ..before deleting any member objects.
		}

		/* Callback called by the pjsua library when registration status has changed */
		void pjsua2Account::onRegState(pj::OnRegStateParam &prm){
			#ifdef DEBUG
			GR_LOG_INFO(sip_signaling_impl::g_logger, "onRegState entered");
			#endif
			pj::AccountInfo ai = getInfo();
			struct RFchannel* thisChan=sip_signaling_impl::instance_->getChanInfo(ai.id);
			if (!thisChan){
				GR_LOG_ERROR(sip_signaling_impl::g_logger, "Registration status has changed for unknown account! " + std::to_string(ai.id));
				return;
			}
			if (ai.regIsActive){
				GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " Registered");
			}else{
				GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " Unregistered");
			}
			#ifdef DEBUG
			GR_LOG_INFO(sip_signaling_impl::g_logger, "Leaving onRegState");
			#endif
		}
		
		/* Callback called by the pjsua library when call is coming in */
		void pjsua2Account::onIncomingCall(pj::OnIncomingCallParam &iprm){
			#ifdef DEBUG
			GR_LOG_INFO(sip_signaling_impl::g_logger, "OnIncomingCallParam entered");
			#endif
			pj::AccountInfo ai = getInfo();
			struct RFchannel* thisChan=sip_signaling_impl::instance_->getChanInfo(ai.id);
			if (!thisChan){
				GR_LOG_ERROR(sip_signaling_impl::g_logger, "incoming call for unknown account! " + std::to_string(ai.id));
				return;
			}
			if (iprm.rdata.wholeMsg.length()) { //Parse custom SIP headers needed to interact with the enabler
				//TODO Parse Resource-Priority header
			}
			GR_LOG_INFO(sip_signaling_impl::g_logger, "incoming call for " + ai.uri);
			thisChan->call = new pjsua2Call(*this, iprm.callId);
			pj::CallOpParam prm;
			prm.statusCode = PJSIP_SC_OK;
			thisChan->call->answer(prm);
			thisChan->call_id=iprm.callId;
			#ifdef DEBUG
			GR_LOG_INFO(sip_signaling_impl::g_logger, "Leaving OnIncomingCallParam");
			#endif
		}	

		/* Callback called by the library when call's state has changed */
		void pjsua2Call::onCallState(pj::OnCallStateParam &prm){
			#ifdef DEBUG
			GR_LOG_INFO(sip_signaling_impl::g_logger, "onCallState entered");
			#endif
			pj::CallInfo ci = getInfo();
			struct RFchannel* thisChan=sip_signaling_impl::instance_->getChanInfo(ci.accId);
			if (!thisChan){
				GR_LOG_ERROR(sip_signaling_impl::g_logger, "call state change for unknown account! " + std::to_string(ci.accId));
				return;
			}
			thisChan->call_id=ci.id;
			
			GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " CallState " + ci.stateText + " " + ci.lastReason);
			switch (ci.state) {
			case PJSIP_INV_STATE_NULL:
				#ifdef DEBUG
				GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " onCallState: Before INVITE is sent or received");
				#endif
				break;
			case PJSIP_INV_STATE_CALLING:
				sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Call"),pmt::string_to_symbol("yellow")));
				#ifdef DEBUG
				GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " onCallState: INVITE sent");
				#endif
				break;
			case PJSIP_INV_STATE_INCOMING:
				sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Call"),pmt::string_to_symbol("yellow")));
				#ifdef DEBUG
				GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " onCallState: INVITE received");
				#endif
				break;
			case PJSIP_INV_STATE_EARLY:
				#ifdef DEBUG
				GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " onCallState: After response with To tag");
				#endif
				break;
			case PJSIP_INV_STATE_CONNECTING:
				#ifdef DEBUG
				GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " onCallState: 2xx sent/received");
				#endif
				break;
			case PJSIP_INV_STATE_CONFIRMED:
				sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Reg"),pmt::string_to_symbol("green")));
				sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Call"),pmt::string_to_symbol("green")));
				#ifdef DEBUG
				GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " onCallState: ACK sent/received");
				#endif
				break;
			case PJSIP_INV_STATE_DISCONNECTED:
				if (ci.lastStatusCode>=400){
					GR_LOG_ERROR(sip_signaling_impl::g_logger, thisChan->sip_user + " onCallState: disconnect on error " + std::to_string(ci.lastStatusCode));
					sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Call"),pmt::string_to_symbol("red")));
					sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Floor"),pmt::string_to_symbol("gray")));
				}else{
					sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Call"),pmt::string_to_symbol("gray")));
				}
				#ifdef DEBUG
				GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " onCallState: Session terminated code " + std::to_string(ci.lastStatusCode));
				#endif
				/* Schedule/Dispatch call deletion to another thread here */
				thisChan->del_call_scheduled = true;
				break;
			default:
				sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Call"),pmt::string_to_symbol("red")));
				sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Floor"),pmt::string_to_symbol("gray")));
				GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " onCallState: unknown state");
				break;
			}
		}
		
		// Callback when call's media state has changed.
		void pjsua2Call::onCallMediaState(pj::OnCallMediaStateParam &prm){
			#ifdef DEBUG
			GR_LOG_INFO(sip_signaling_impl::g_logger, "onCallMediaState entered");	
			#endif			
			pjsua_call_info pj_ci;
			unsigned mi;
			pj::CallInfo ci = getInfo();
			struct RFchannel* thisChan=sip_signaling_impl::instance_->getChanInfo(ci.accId);
			if (!thisChan){
				GR_LOG_ERROR(sip_signaling_impl::g_logger, "Media state has changed for unknown account! " + std::to_string(ci.accId));
				return;
			}
			if (pjsua_call_get_info(ci.id, &pj_ci) == PJ_SUCCESS)
			{
				GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " CallMediaState " + std::string(pj_ci.state_text.ptr, pj_ci.state_text.slen));
				if (pj_ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
					
					//get local and remote host and port information to pass to source/sink blocks
					pj::MediaTransportInfo  transInfo = getMedTransportInfo(0);
					sip_signaling_impl::instance_->parseSockAddr(transInfo.localRtpName,0,&(thisChan->lport));
					GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " local RTP:" + transInfo.localRtpName + " local RTCP:" + transInfo.localRtcpName + " local port:" + std::to_string(thisChan->lport));
					pj::StreamInfo streamInfo = getStreamInfo(0);
					if (pmt::is_symbol(thisChan->codec)){
						if ((pmt::symbol_to_string(thisChan->codec).compare(streamInfo.codecName))){
							GR_LOG_ERROR(sip_signaling_impl::g_logger, "Media stream codec does not match RTP block! for " + thisChan->sip_user);
							//display channel in red
							sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Call"),pmt::string_to_symbol("red")));
							sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Floor"),pmt::string_to_symbol("gray")));
						}
					}
					#ifdef DEBUG
					GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " parseSockAddr");
					#endif
					sip_signaling_impl::instance_->parseSockAddr(streamInfo.remoteRtpAddress,&(thisChan->rhost),&(thisChan->rport));
					thisChan->rxpt=streamInfo.rxPt;
					thisChan->txpt=streamInfo.txPt;
					GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " remote RTP:" + thisChan->rhost + " port:" + std::to_string(thisChan->rport) + " " + streamInfo.remoteRtpAddress + " remote RTCP:" + streamInfo.remoteRtcpAddress + " txPt:" + std::to_string(thisChan->txpt) + " rxPt:" + std::to_string(thisChan->rxpt) + " codec:" + streamInfo.codecName);
					
					pjsua_call *call = &pjsua_var.calls[ci.id];
					// Iterate all the call medias
					for (mi = 0; mi < call->med_cnt; ++mi) {
						pj_status_t status=PJ_SUCCESS;
						pjsua_call_media *call_med = &call->media[mi];
						// disconnect media so RTP block can use it
						
						if (call_med->tp_st > PJSUA_MED_TP_IDLE) {
							pjsua_set_media_tp_state(call_med, PJSUA_MED_TP_IDLE);
							GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " pjmedia_transport_media_stop");
							pjmedia_transport_media_stop(call_med->tp);
						}
						pjsua_set_media_tp_state(call_med, PJSUA_MED_TP_NULL);

						GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " pjmedia_transport_close");
						status = pjmedia_transport_close(call_med->tp);
						//call_med->tp = call_med->tp_orig = NULL;
						call->med_cnt=0;
						call->med_prov_cnt=0;
						if (status!=PJ_SUCCESS){
							GR_LOG_ERROR(sip_signaling_impl::g_logger, "Error disconnecting media pjmedia_transport_close for " + ci.localUri + " " + std::to_string(status));
							//display channel in red
							sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Call"),pmt::string_to_symbol("red")));
							sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Floor"),pmt::string_to_symbol("gray")));
						}
					}
					call->med_prov_cnt = 0;

					sip_signaling_impl::instance_->set_ports(ci.accId);

				}					
			}
		}
		
		// Callback whenever a transaction within the call has changed
		void pjsua2Call::onCallTsxState(pj::OnCallTsxStateParam &prm){
			std::string StateType[7]={"PJSIP_EVENT_UNKNOWN (Unidentified event)",
				"PJSIP_EVENT_TIMER (Timer event, normally only used internally in transaction)",
				"PJSIP_EVENT_TX_MSG (Message transmission event)",
				"PJSIP_EVENT_RX_MSG (Message received event)",
				"PJSIP_EVENT_TRANSPORT_ERROR (Transport error event)",
				"PJSIP_EVENT_TSX_STATE (Transaction state changed event)",
			"PJSIP_EVENT_USER (event was triggered by user action)"};
			#ifdef DEBUG
			GR_LOG_INFO(sip_signaling_impl::g_logger, "Entering onCallTsxState Method " + prm.e.body.tsxState.tsx.method + " State " + StateType[prm.e.body.tsxState.type]);
			#endif
			pj::CallInfo ci = getInfo();
			struct RFchannel* thisChan=sip_signaling_impl::instance_->getChanInfo(ci.accId);
			if (!thisChan){
				GR_LOG_ERROR(sip_signaling_impl::g_logger, "Transaction state has changed for unknown account! " + std::to_string(ci.accId));
				return;
			}
			try{
				if (prm.e.type == PJSIP_EVENT_TSX_STATE){
					if (prm.e.body.tsxState.type==PJSIP_EVENT_TIMER && prm.e.body.tsxState.tsx.statusCode>=400){
						//ignore Proxy Authentication Required
						GR_LOG_WARN(sip_signaling_impl::g_logger, thisChan->sip_user + " to " + thisChan->uri + " onCallTsxState Method " + prm.e.body.tsxState.tsx.method + " Status" + prm.e.body.tsxState.tsx.statusText + " :" + std::to_string(prm.e.body.tsxState.tsx.statusCode));
					}else if (prm.e.body.tsxState.tsx.statusCode>=400){
						GR_LOG_ERROR(sip_signaling_impl::g_logger, thisChan->sip_user + " to " + thisChan->uri + " onCallTsxState Method " + prm.e.body.tsxState.tsx.method + " Status " + prm.e.body.tsxState.tsx.statusText + " :" + std::to_string(prm.e.body.tsxState.tsx.statusCode));
						ci = getInfo();
						if (ci.state!=PJSIP_INV_STATE_DISCONNECTED){
							pj::CallOpParam cprm;
							try{
								hangup(&cprm); //error occurred.  hangup the call
								thisChan->del_call_scheduled = true;
							} catch(pj::Error& err) {
								GR_LOG_ERROR(sip_signaling_impl::g_logger, thisChan->sip_user + "onCallTsxState Error in hangup():" + err.info());
							}
						}
						//display channel in red
						sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Call"),pmt::string_to_symbol("red")));
						sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Floor"),pmt::string_to_symbol("gray")));
						return;
					}
					if(prm.e.body.tsxState.type==PJSIP_EVENT_RX_MSG){
						#ifdef DEBUG
						GR_LOG_INFO(sip_signaling_impl::g_logger, "prm.e.body.tsxState.type==PJSIP_EVENT_RX_MSG");
						#endif
						if (prm.e.body.tsxState.tsx.method=="INFO"){
							pjsip_rx_data *rdata = (pjsip_rx_data*)prm.e.body.tsxState.src.rdata.pjRxData;
							if (rdata->msg_info.msg->type == PJSIP_REQUEST_MSG){
								pjsip_dialog *dlg = pjsip_tsx_get_dlg((pjsip_transaction*)prm.e.body.tsxState.tsx.pjTransaction);
								#ifdef DEBUG
								GR_LOG_INFO(sip_signaling_impl::g_logger, "Entered onCallTsxState Method " + std::string((char*)rdata->msg_info.msg->body->data));
								#endif
								pj_status_t status;
								if ((std::string)((char*)rdata->msg_info.msg->body->data) != lastMessage){
									//I got tired of seeing repeated messages
									GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + 
									" CallTsxState RX_MSG " + (char*)rdata->msg_info.msg->body->data);
									pmt::pmt_t sink(pmt::make_dict());
									sink = pmt::dict_add(sink, pmt::string_to_symbol("ID"), pmt::from_long(thisChan->sink_id));
									pmt::pmt_t source(pmt::make_dict());
									source = pmt::dict_add(source, pmt::string_to_symbol("ID"), pmt::from_long(thisChan->src_id));
									if (strstr((char*)rdata->msg_info.msg->body->data,"action=granted")){
										//floor LED green
										sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Reg"),pmt::string_to_symbol("green")));
										sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Call"),pmt::string_to_symbol("green")));
										sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Floor"),pmt::string_to_symbol("green")));
										//send to RTP blocks
										sink = pmt::dict_add(sink, pmt::string_to_symbol("Floor"), pmt::string_to_symbol("granted"));
										sip_signaling_impl::instance_->cmd_out(sink);
										source = pmt::dict_add(source, pmt::string_to_symbol("Floor"), pmt::string_to_symbol("granted"));
										sip_signaling_impl::instance_->cmd_out(source);
									}
									if (strstr((char*)rdata->msg_info.msg->body->data,"action=taken")){
										//floor LED red
										sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Reg"),pmt::string_to_symbol("green")));
										sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Call"),pmt::string_to_symbol("green")));
										sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Floor"),pmt::string_to_symbol("red")));
										//send to RTP blocks
										sink = pmt::dict_add(sink, pmt::string_to_symbol("Floor"), pmt::string_to_symbol("taken"));
										sip_signaling_impl::instance_->cmd_out(sink);
										source = pmt::dict_add(source, pmt::string_to_symbol("Floor"), pmt::string_to_symbol("taken"));
										sip_signaling_impl::instance_->cmd_out(source);
									}
									if (strstr((char*)rdata->msg_info.msg->body->data,"action=idle")){
										//floor LED gray
										sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Floor"),pmt::string_to_symbol("gray")));
										//send to RTP blocks
										sink = pmt::dict_add(sink, pmt::string_to_symbol("Floor"), pmt::string_to_symbol("idle"));
										sip_signaling_impl::instance_->cmd_out(sink);
										source = pmt::dict_add(source, pmt::string_to_symbol("Floor"), pmt::string_to_symbol("idle"));
										sip_signaling_impl::instance_->cmd_out(source);
									}
								}
								lastMessage = (char*)rdata->msg_info.msg->body->data;
								try {
									#ifdef DEBUG
									GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " pjsip_dlg_respond send OK");
									#endif
									status = pjsip_dlg_respond(dlg, rdata, 200, NULL, NULL, NULL);
									if (status != PJ_SUCCESS) {
										GR_LOG_ERROR(sip_signaling_impl::g_logger, thisChan->sip_user + " Error answering incoming SIP INFO" + std::to_string(status));
										//display channel in red
										sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Call"),pmt::string_to_symbol("red")));
										sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Floor"),pmt::string_to_symbol("gray")));
									}
								} catch(pj::Error& err) {
									GR_LOG_ERROR(sip_signaling_impl::g_logger, thisChan->sip_user + " Error in pjsip_dlg_respond():" + err.info());
									//display channel in red
									sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Call"),pmt::string_to_symbol("red")));
									sip_signaling_impl::instance_->cmd_out(pmt::cons(pmt::string_to_symbol("CH" + std::to_string(1+thisChan->chan) + "_Floor"),pmt::string_to_symbol("gray")));
								}
							}
						}else{
							GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " CallTsxState RX_MSG INFO " + 
							prm.e.body.tsxState.tsx.method  + " " + std::to_string(prm.e.body.tsxState.tsx.statusCode) + " :" + prm.e.body.tsxState.tsx.statusText);
						}
					}else if (prm.e.body.tsxState.type==PJSIP_EVENT_TX_MSG){
						#ifndef DEBUG
						if (prm.e.body.tsxState.tsx.statusCode!=200){ //I got tired of seeing all the 200 OK
						#endif
							GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " CallTsxState TX_MSG " + prm.e.body.tsxState.tsx.method 
							+ " :" + std::to_string(prm.e.body.tsxState.tsx.statusCode) + " " + prm.e.body.tsxState.tsx.statusText);
						#ifndef DEBUG
						}
						#endif
					}else if (prm.e.body.tsxState.type==PJSIP_EVENT_TRANSPORT_ERROR){
						GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " CallTsxState TRANSPORT_ERROR Method " + prm.e.body.tsxState.tsx.method);
					}else if (prm.e.body.tsxState.type==PJSIP_EVENT_TIMER){
						GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " CallTsxState EVENT_TIMER Method " + prm.e.body.tsxState.tsx.method);
					}else if (prm.e.body.tsxState.type==PJSIP_EVENT_USER){
						GR_LOG_INFO(sip_signaling_impl::g_logger, thisChan->sip_user + " CallTsxState EVENT_USER Method " + prm.e.body.tsxState.tsx.method);
					}else{
						GR_LOG_WARN(sip_signaling_impl::g_logger, thisChan->sip_user + " onCallTsxState unknown event Method " + prm.e.body.tsxState.tsx.method);
					}
				}else{
					GR_LOG_WARN(sip_signaling_impl::g_logger, thisChan->sip_user + " onCallTsxState entered for unknown reason");
				}
			} catch(pj::Error& err) {
				GR_LOG_ERROR(sip_signaling_impl::g_logger, thisChan->sip_user + " Error in pjsip_dlg_respond()0:" + err.info());
			}
			#ifdef DEBUG
			GR_LOG_INFO(sip_signaling_impl::g_logger, "Leaving onCallTsxState Method " + prm.e.body.tsxState.tsx.method + " State " + StateType[prm.e.body.tsxState.type]);
			#endif
		}

  } /* namespace PSCR_LMR2LTE */
} /* namespace gr */

