After the OOT has been installed the following "Hierarchical" blocks must be generated before the main solution flowgraph can be run:

*CTCSS Detection from CTCSSdetect.grc
*QT GUI LED Array from LED_array.grc
*Analog NBFM receive from NBFM receive.grc
*Analog NBFM transmit from NBFM transmit.grc
*NBFM Transceiver Array from NBFM_xcvr.grc

To generate the hierarchical blocks run each of their flowgraphs included in the lmr2lte code repository, found in... /pscr_lmr2lte/H-blocks

<h2>Other config changes</h2>
In order to run the GNURadio scheduler in realtime...
 sudo nano /etc/security/limits.conf
 @employee-vpn    - rtprio    99


<h1>Blocks:</h1>

<h2>label</h2>
This block makes a basic label. It can be controlled with a message pair.The car of the message must match the alias and the cdr supplies the label. 

Used in: 

<h2>led </h2>
This block makes a basic LED indicator. It can be controlled with a message pair. The car of the message must match the label and the cdr supplies the color.

Used in: 

*"LED_array.grc" (Hierarchical)


<h2>opus_rtp_sink</h2>
This block provides opus encoded RTP transmission capabilities. Takes a float audio stream and sends opus encoded samples to an IP address. The sample rate must be one of 8000, 12000, 160000, 24000 or 48000.
The command port takes a PMT dict with the following pairs:
"ID", number which must match the unique_id() of the block (required)
"remote port", port number on remote host for IP packets
"remote host", IP address of remote host
"pt", payload type for RTP packest
"freq offset", value in Hz which is output from cmd out.  Intended to be sent to Frequency Xlating FIR Filter block
If the command port receives a PMT symbol it will trigger the squelch port. 

The squelch port outputs the unique_id() of the block and the squelch status from the squelch message port whenever a squelch_sob or squelch_sob tag is received or triggered by a command.
  
NOTES: This block does support connecting to IPv6 addresses.  If an IPv6 address is detected as the destination IP address, the block will automatically adjust for proper connection.  Just make sure your IPv6 stack is enabled.

For best performance and to ensure RTP packets are not dropped, add the following lines to your /etc/sysctl.conf and reboot (the reboot is required).

 net.core.rmem_default=26214400
 net.core.rmem_max=104857600
 net.core.wmem_default=65536
 net.core.wmem_max=104857600

Network_opus_rtp_sink
 make(std::string const & host, int port, int samprate) -> opus_rtp_sink_sptr
        Constructor Specific Documentation:
        Return a shared_ptr to a new instance of Network::opus_rtp_sink.
        To avoid accidental use of raw pointers, Network::opus_rtp_sink's constructor is in a private implementation class. Network::opus_rtp_sink::make is the public interface for creating new instances.
        Args:
            host : 
            port : 
            samprate :
Used in: 

*"NBFM receive.grc" (Hierarchical)

<h2>opus_rtp_source</h2>
This block provides opus decoding RTP transmission capabilities. Takes opus encoded samples from an IP port and sends a float audio stream. The sample rate must be one of 8000, 12000, 160000, 24000 or 48000. The command port takes a PMT dict with the following pairs:
"ID", number which must match the unique_id() of the block (required)
"local port", port number on local host for IP packets
"pt", payload type for RTP packest
"freq offset", value in Hz which is output from freq.  Intended to be sent to an analog signal sourceblock cmd port
If the command port receives a PMT symbol it will trigger the cmd out port. The cmd out port outputs the unique_id() of the block when triggered by a command.
NOTE: For best performance and to ensure UDP packets are not dropped, add the following lines to your /etc/sysctl.conf and reboot(the reboot is required).

 net.core.rmem_default=26214400
 net.core.rmem_max=104857600
 net.core.wmem_default=65536
 net.core.wmem_max=104857600

Network_opus_rtp_source
 make(int port, int samprate, bool ipv6) -> opus_rtp_source_sptr
        This block receives opus encoded data over RTP streams and outputs the decoded audio.
        Constructor Specific Documentation:
        Return a shared_ptr to a new instance of Network::opus_rtp_source.
        To avoid accidental use of raw pointers, Network::opus_rtp_source's constructor is in a private implementation class. Network::opus_rtp_source::make is the public interface for creating new instances.
        Args:
            port : 
            samprate : 
            ipv6 :

Used in: 

*"NBFM transmit.grc" (Hierarchical)

<h2>sip_signaling</h2>
Network_sip_signaling
 make(char * SIP_domain, char * PCSCF_addr) -> sip_signaling_sptr
        <+description of block+>
        Constructor Specific Documentation:
        Return a shared_ptr to a new instance of Network::sip_signaling.
        To avoid accidental use of raw pointers, Network::sip_signaling's constructor is in a private implementation class. Network::sip_signaling::make is the public interface for creating new instances.
        Args:
            SIP_domain : 
            PCSCF_addr :

Used in: 

*NBFM_xcvr_array.grc (Hierarchical)


<h1>GRC Hierarchical blocks:</h1>

<h2>Analog NBFM receive</h2>
Used in: 

*NBFM_xcvr_array.grc (Hierarchical)

OOT blocks:

*opus_rtp_sink

<h2>Analog NBFM transmit</h2>
Used in: 

*NBFM_xcvr_array.grc (Hierarchical)

OOT blocks:

*opus_rtp_source

<h2>CTCSS Detection</h2>


<h2>NBFM Transceiver Array</h2>
OOT blocks:

*NBFM Transceiver Array (Hierarchical)
*(Hierarchical)

Parameter:

*aud_to_rf, sample Rate to Audio Rate ratio
*audio_samp_rate, audio sample rate
*Max_deviation
*squelch, squelch level (dB)
*Set all CTCSS tone definitions
*Set 1 SIP URI per conventional LMR RF channel
*Set all SDR tx offsets TODO: why??

OOT Blocks:

*Analog NBFM receiver
*Analog NBFM transmitter

<h2>QT GUI LED Array</h2>
OOT blocks:

*led

<h1>GRC Flowgraphs:</h1>
<h2>passthrough.grc</h2>
This flow can be used to verify the Kenwood analog repeater in the Analog-lmr2lte PSCR test-bed, additionally this flow uses no out-of-tree modules. By default the flowgraph is set to use the first b200 SDR the driver finds available, this can be changed by updating the "SDR_address" variable. 

<h2>NBFM_xcvr.grc</h2>

==Next Steps==
Install [[LMR2LTE/UHD|UHD]] or go back to [[LMR2LTE]]
[[Category:MCV]]
