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

#ifndef INCLUDED_PSCR_LMR2LTE_SIP_SIGNALING_IMPL_H
#define INCLUDED_PSCR_LMR2LTE_SIP_SIGNALING_IMPL_H

#include <PSCR_LMR2LTE/sip_signaling.h>
#include <pjsua2.hpp>
#include <pjsua-lib/pjsua.h>
#include <pjsip/sip_transport.h>
#include <pjmedia.h>
#include <pjsua-lib/pjsua_internal.h>
#include <iostream>
#include <atomic>
#include <boost/algorithm/string/case_conv.hpp>

#define ANS_MODE_HDR "Answer-Mode"
#define X_MCS_TYPE_HDR "X-Mcx-Type"

namespace gr {
  namespace PSCR_LMR2LTE {

		struct RFchannel {
			uint8_t chan;
			int src_id;
			int sink_id;
			uint8_t rxpt;
			uint8_t txpt;
			uint16_t lport;
			uint16_t rport;
			std::string rhost;
			std::string sip_user;
			std::string sip_pw;
			std::string uri;
			pmt::pmt_t codec;
			pmt::pmt_t squelch;
			pjsua_acc_id acc_id;
			pjsua_call_id call_id;
			pj::Call *call;
			pj::Account *acc=NULL;
			bool del_call_scheduled;
		};
			
		class pjsua2Account : public pj::Account
		{
		public:
			~pjsua2Account();
			
			virtual void onRegState(pj::OnRegStateParam &prm);
			virtual void onIncomingCall(pj::OnIncomingCallParam &iprm);
		};

		class pjsua2Call : public pj::Call
		{
		public:
			pjsua2Call(pj::Account &acc, int call_id = PJSUA_INVALID_ID)
			: pj::Call(acc, call_id)
			{ }
			
			// Notification when call's state has changed.
			virtual void onCallState(pj::OnCallStateParam &prm);
			// Notification when call's media state has changed.
			virtual void onCallMediaState(pj::OnCallMediaStateParam &prm);
			// Notification whenever a transaction within the call has changed
			virtual void onCallTsxState(pj::OnCallTsxStateParam &prm);
			
		private:
			std::string lastMessage;
		};

		class sip_signaling_impl : public sip_signaling
		{
		private:
			boost::shared_ptr<gr::thread::thread> d_thread;
			void msg_in(pmt::pmt_t msg);
			
			void run();

			struct RFchannel RFchan[PJSUA_MAX_ACC];
			
			pj::Endpoint *ep;
			
			pmt::pmt_t d_cmd_out;
			pmt::pmt_t d_subscribers;
			
			pmt::pmt_t d_codec;

			std::atomic<bool> d_finished;
			bool d_run;
			std::string s_SIP_domain;
			std::string s_PCSCF_addr;

			int d_argc;
			char* d_argv;

		public:
			sip_signaling_impl(char* SIP_domain, char* PCSCF_addr);
			~sip_signaling_impl();

			struct RFchannel* getChanInfo(pjsua_acc_id acc_id);
			void cmd_out(pmt::pmt_t cmd);
			bool parseSockAddr(std::string sockAddress, std::string* host, uint16_t* port);
			void set_ports(pjsua_acc_id acc_id);
			int make_call(int acctnum);
			static sip_signaling_impl *instance_;	// static instance
			static gr::logger_ptr g_logger;  // static copy of d_logger

			// Overloading these to start and stop the internal thread that
			// periodically checks for SIP messages.
			bool start();
			bool stop();
			
		};

  } // namespace PSCR_LMR2LTE
} // namespace gr

#endif /* INCLUDED_PSCR_LMR2LTE_SIP_SIGNALING_IMPL_H */

