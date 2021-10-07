#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Narrowband 8 channel FM Transceiver
# GNU Radio version: v3.8.2.0-118-g8b2012ab

from distutils.version import StrictVersion

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print("Warning: failed to XInitThreads()")

import os
import sys
sys.path.append(os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))

from PyQt5 import Qt
from gnuradio import qtgui
from gnuradio.filter import firdes
import sip
from gnuradio import blocks
from gnuradio import gr
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio import uhd
import time
from gnuradio.qtgui import Range, RangeWidget
from led_array import led_array  # grc-generated hier_block
from xcvr_array import xcvr_array  # grc-generated hier_block
import PSCR_LMR2LTE

from gnuradio import qtgui

class NBFM_xcvr(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Narrowband 8 channel FM Transceiver")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Narrowband 8 channel FM Transceiver")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "NBFM_xcvr")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass

        ##################################################
        # Variables
        ##################################################
        self.squelch = squelch = -40
        self.samp_ratio = samp_ratio = 21
        self.max_deviation = max_deviation = 2500
        self.gain = gain = 10
        self.audio_samp_rate = audio_samp_rate = 48000
        self.UL_freq = UL_freq = 158.7525e6
        self.SDR_address = SDR_address = "type=b200"
        self.NumChans = NumChans = 2
        self.DL_freq = DL_freq = 162.075e6

        ##################################################
        # Blocks
        ##################################################
        self.qtgui_tab_widget_0 = Qt.QTabWidget()
        self.qtgui_tab_widget_0_widget_0 = Qt.QWidget()
        self.qtgui_tab_widget_0_layout_0 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.qtgui_tab_widget_0_widget_0)
        self.qtgui_tab_widget_0_grid_layout_0 = Qt.QGridLayout()
        self.qtgui_tab_widget_0_layout_0.addLayout(self.qtgui_tab_widget_0_grid_layout_0)
        self.qtgui_tab_widget_0.addTab(self.qtgui_tab_widget_0_widget_0, 'Status')
        self.qtgui_tab_widget_0_widget_1 = Qt.QWidget()
        self.qtgui_tab_widget_0_layout_1 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.qtgui_tab_widget_0_widget_1)
        self.qtgui_tab_widget_0_grid_layout_1 = Qt.QGridLayout()
        self.qtgui_tab_widget_0_layout_1.addLayout(self.qtgui_tab_widget_0_grid_layout_1)
        self.qtgui_tab_widget_0.addTab(self.qtgui_tab_widget_0_widget_1, 'Advanced')
        self.top_grid_layout.addWidget(self.qtgui_tab_widget_0, 0, 0, 1, 1)
        for r in range(0, 1):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._squelch_range = Range(-100, -20, 1, -40, 100)
        self._squelch_win = RangeWidget(self._squelch_range, self.set_squelch, 'Power Squelch', "counter_slider", float)
        self.qtgui_tab_widget_0_grid_layout_1.addWidget(self._squelch_win, 0, 0, 1, 2)
        for r in range(0, 1):
            self.qtgui_tab_widget_0_grid_layout_1.setRowStretch(r, 1)
        for c in range(0, 2):
            self.qtgui_tab_widget_0_grid_layout_1.setColumnStretch(c, 1)
        self.qtgui_freq_sink_x_0_0 = qtgui.freq_sink_c(
            2**14-1, #size
            firdes.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            audio_samp_rate*samp_ratio, #bw
            "Tx Spectrum", #name
            1
        )
        self.qtgui_freq_sink_x_0_0.set_update_time(0.10)
        self.qtgui_freq_sink_x_0_0.set_y_axis(-140, 10)
        self.qtgui_freq_sink_x_0_0.set_y_label("", 'dB')
        self.qtgui_freq_sink_x_0_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, 0, 0, "")
        self.qtgui_freq_sink_x_0_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0_0.enable_grid(True)
        self.qtgui_freq_sink_x_0_0.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0_0.enable_axis_labels(True)
        self.qtgui_freq_sink_x_0_0.enable_control_panel(False)

        self.qtgui_freq_sink_x_0_0.disable_legend()


        labels = ["", '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
            "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_0_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_0_0.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_0_0.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_0_0.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_0_0.set_line_alpha(i, alphas[i])

        self._qtgui_freq_sink_x_0_0_win = sip.wrapinstance(self.qtgui_freq_sink_x_0_0.pyqwidget(), Qt.QWidget)
        self.qtgui_tab_widget_0_grid_layout_1.addWidget(self._qtgui_freq_sink_x_0_0_win, 2, 0, 1, 2)
        for r in range(2, 3):
            self.qtgui_tab_widget_0_grid_layout_1.setRowStretch(r, 1)
        for c in range(0, 2):
            self.qtgui_tab_widget_0_grid_layout_1.setColumnStretch(c, 1)
        self.qtgui_freq_sink_x_0 = qtgui.freq_sink_c(
            4096, #size
            firdes.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            audio_samp_rate*samp_ratio, #bw
            "Rx Spectrum", #name
            1
        )
        self.qtgui_freq_sink_x_0.set_update_time(0.10)
        self.qtgui_freq_sink_x_0.set_y_axis(-140, 10)
        self.qtgui_freq_sink_x_0.set_y_label("", 'dB')
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_AUTO, squelch, 0, "")
        self.qtgui_freq_sink_x_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0.enable_grid(True)
        self.qtgui_freq_sink_x_0.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0.enable_axis_labels(True)
        self.qtgui_freq_sink_x_0.enable_control_panel(False)

        self.qtgui_freq_sink_x_0.disable_legend()


        labels = ["", '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
            "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_freq_sink_x_0_win = sip.wrapinstance(self.qtgui_freq_sink_x_0.pyqwidget(), Qt.QWidget)
        self.qtgui_tab_widget_0_grid_layout_1.addWidget(self._qtgui_freq_sink_x_0_win, 1, 0, 1, 2)
        for r in range(1, 2):
            self.qtgui_tab_widget_0_grid_layout_1.setRowStretch(r, 1)
        for c in range(0, 2):
            self.qtgui_tab_widget_0_grid_layout_1.setColumnStretch(c, 1)
        self.led_array_0 = led_array()

        self.qtgui_tab_widget_0_grid_layout_0.addWidget(self.led_array_0, 0, 0, 1, 1)
        for r in range(0, 1):
            self.qtgui_tab_widget_0_grid_layout_0.setRowStretch(r, 1)
        for c in range(0, 1):
            self.qtgui_tab_widget_0_grid_layout_0.setColumnStretch(c, 1)
        self.xcvr_array_0 = xcvr_array(
            SIPuser1="proxy-client-A",
            SIPpw1="proxy-client-A",
            SIPuri1="sip:mcptt_id_clientB@mcptt01.pscr.lz",
            RxOffset_1=0,
            TxOffset_1=0,
            ctcss_1=179.9,
            SIPuser2="",
            SIPpw2="",
            SIPuri2="",
            RxOffset_2=0,
            TxOffset_2=0,
            ctcss_2=67,
            SIPuser3="",
            SIPpw3="",
            SIPuri3="",
            RxOffset_3=0,
            TxOffset_3=0,
            ctcss_3=0,
            SIPuser4="",
            SIPpw4="",
            SIPuri4="",
            RxOffset_4=0,
            TxOffset_4=0,
            ctcss_4=0,
            SIPuser5="",
            SIPpw5="",
            SIPuri5="",
            RxOffset_5=0,
            TxOffset_5=0,
            ctcss_5=0,
            SIPuser6="",
            SIPpw6="",
            SIPuri6="",
            RxOffset_6=0,
            TxOffset_6=0,
            ctcss_6=0,
            SIPuser7="",
            SIPpw7="",
            SIPuri7="",
            RxOffset_7=0,
            TxOffset_7=0,
            ctcss_7=0,
            SIPuser8="",
            SIPpw8="",
            SIPuri8="",
            RxOffset_8=0,
            TxOffset_8=0,
            ctcss_8=0,
            aud_to_rf=samp_ratio,
            audio_samp_rate=audio_samp_rate,
            squelch=squelch,
            Max_deviation=2500,
        )
        self.uhd_usrp_source_0 = uhd.usrp_source(
            ",".join((SDR_address, "recv_buff_size=816000")),
            uhd.stream_args(
                cpu_format="fc32",
                args='',
                channels=list(range(0,1)),
            ),
        )
        self.uhd_usrp_source_0.set_center_freq(uhd.tune_request(DL_freq, 200e3, args=uhd.device_addr("mode_n=integer")), 0)
        self.uhd_usrp_source_0.set_rx_agc(False, 0)
        self.uhd_usrp_source_0.set_gain(gain, 0)
        self.uhd_usrp_source_0.set_antenna('RX2', 0)
        self.uhd_usrp_source_0.set_samp_rate(audio_samp_rate*samp_ratio)
        # No synchronization enforced.
        self.uhd_usrp_sink_0 = uhd.usrp_sink(
            ",".join((SDR_address, "recv_buff_size=816000")),
            uhd.stream_args(
                cpu_format="fc32",
                args='',
                channels=list(range(0,1)),
            ),
            '',
        )
        self.uhd_usrp_sink_0.set_center_freq(uhd.tune_request(UL_freq, 200e3, args=uhd.device_addr("mode_n=integer")), 0)
        self.uhd_usrp_sink_0.set_gain(-30, 0)
        self.uhd_usrp_sink_0.set_antenna('TX/RX', 0)
        self.uhd_usrp_sink_0.set_samp_rate(audio_samp_rate*samp_ratio)
        self.uhd_usrp_sink_0.set_time_unknown_pps(uhd.time_spec())
        self.blocks_multiply_const_xx_0 = blocks.multiply_const_cc(1.0/NumChans, 1)
        self.PSCR_LMR2LTE_sip_signaling_0 = PSCR_LMR2LTE.sip_signaling('mcptt01.pscr.lz', 'sip:10.8.140.190:5050')


        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.PSCR_LMR2LTE_sip_signaling_0, 'cmd out'), (self.led_array_0, 'msg in'))
        self.msg_connect((self.PSCR_LMR2LTE_sip_signaling_0, 'cmd out'), (self.xcvr_array_0, 'CMD_in'))
        self.msg_connect((self.xcvr_array_0, 'MSG_out'), (self.PSCR_LMR2LTE_sip_signaling_0, 'msg in'))
        self.connect((self.blocks_multiply_const_xx_0, 0), (self.qtgui_freq_sink_x_0_0, 0))
        self.connect((self.blocks_multiply_const_xx_0, 0), (self.uhd_usrp_sink_0, 0))
        self.connect((self.uhd_usrp_source_0, 0), (self.qtgui_freq_sink_x_0, 0))
        self.connect((self.uhd_usrp_source_0, 0), (self.xcvr_array_0, 0))
        self.connect((self.xcvr_array_0, 0), (self.blocks_multiply_const_xx_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "NBFM_xcvr")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_squelch(self):
        return self.squelch

    def set_squelch(self, squelch):
        self.squelch = squelch
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_AUTO, self.squelch, 0, "")
        self.xcvr_array_0.set_squelch(self.squelch)

    def get_samp_ratio(self):
        return self.samp_ratio

    def set_samp_ratio(self, samp_ratio):
        self.samp_ratio = samp_ratio
        self.qtgui_freq_sink_x_0.set_frequency_range(0, self.audio_samp_rate*self.samp_ratio)
        self.qtgui_freq_sink_x_0_0.set_frequency_range(0, self.audio_samp_rate*self.samp_ratio)
        self.uhd_usrp_sink_0.set_samp_rate(self.audio_samp_rate*self.samp_ratio)
        self.uhd_usrp_source_0.set_samp_rate(self.audio_samp_rate*self.samp_ratio)
        self.xcvr_array_0.set_aud_to_rf(self.samp_ratio)

    def get_max_deviation(self):
        return self.max_deviation

    def set_max_deviation(self, max_deviation):
        self.max_deviation = max_deviation

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain
        self.uhd_usrp_source_0.set_gain(self.gain, 0)
        self.uhd_usrp_source_0.set_gain(self.gain, 1)

    def get_audio_samp_rate(self):
        return self.audio_samp_rate

    def set_audio_samp_rate(self, audio_samp_rate):
        self.audio_samp_rate = audio_samp_rate
        self.qtgui_freq_sink_x_0.set_frequency_range(0, self.audio_samp_rate*self.samp_ratio)
        self.qtgui_freq_sink_x_0_0.set_frequency_range(0, self.audio_samp_rate*self.samp_ratio)
        self.uhd_usrp_sink_0.set_samp_rate(self.audio_samp_rate*self.samp_ratio)
        self.uhd_usrp_source_0.set_samp_rate(self.audio_samp_rate*self.samp_ratio)
        self.xcvr_array_0.set_audio_samp_rate(self.audio_samp_rate)

    def get_UL_freq(self):
        return self.UL_freq

    def set_UL_freq(self, UL_freq):
        self.UL_freq = UL_freq
        self.uhd_usrp_sink_0.set_center_freq(uhd.tune_request(self.UL_freq, 200e3, args=uhd.device_addr("mode_n=integer")), 0)
        self.uhd_usrp_sink_0.set_center_freq(uhd.tune_request(self.UL_freq, 200e3, args=uhd.device_addr("mode_n=integer")), 1)

    def get_SDR_address(self):
        return self.SDR_address

    def set_SDR_address(self, SDR_address):
        self.SDR_address = SDR_address

    def get_NumChans(self):
        return self.NumChans

    def set_NumChans(self, NumChans):
        self.NumChans = NumChans
        self.blocks_multiply_const_xx_0.set_k(1.0/self.NumChans)

    def get_DL_freq(self):
        return self.DL_freq

    def set_DL_freq(self, DL_freq):
        self.DL_freq = DL_freq
        self.uhd_usrp_source_0.set_center_freq(uhd.tune_request(self.DL_freq, 200e3, args=uhd.device_addr("mode_n=integer")), 0)
        self.uhd_usrp_source_0.set_center_freq(uhd.tune_request(self.DL_freq, 200e3, args=uhd.device_addr("mode_n=integer")), 1)





def main(top_block_cls=NBFM_xcvr, options=None):
    if gr.enable_realtime_scheduling() != gr.RT_OK:
        print("Error: failed to enable real-time scheduling.")

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()

    tb.start()

    tb.show()

    def sig_handler(sig=None, frame=None):
        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    def quitting():
        tb.stop()
        tb.wait()

    qapp.aboutToQuit.connect(quitting)
    qapp.exec_()

if __name__ == '__main__':
    main()
