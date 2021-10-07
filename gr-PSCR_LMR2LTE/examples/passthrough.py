#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Passthrough
# GNU Radio version: v3.8.2.0-117-gb29473d5

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

from PyQt5 import Qt
from gnuradio import qtgui
from gnuradio.filter import firdes
import sip
from gnuradio import analog
from gnuradio import blocks
from gnuradio import filter
from gnuradio import gr
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio import uhd
import time
from gnuradio.qtgui import Range, RangeWidget

from gnuradio import qtgui

class passthrough(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Passthrough")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Passthrough")
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

        self.settings = Qt.QSettings("GNU Radio", "passthrough")

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
        self.uplink_freq = uplink_freq = 162.075e6
        self.squelch = squelch = -50
        self.samp_rate = samp_rate = 2e6
        self.mute = mute = 0
        self.gain = gain = 30
        self.downlink_freq = downlink_freq = 158.7525e6
        self.ctcss_e = ctcss_e = 0
        self.ctcss = ctcss = 179.9
        self.aud_to_rf = aud_to_rf = 125
        self.SDR_address = SDR_address = type=b200
        self.Audio_samp_rate = Audio_samp_rate = 16000

        ##################################################
        # Blocks
        ##################################################
        self.qtgui_freq_sink_x_0_0 = qtgui.freq_sink_c(
            4096, #size
            firdes.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            samp_rate, #bw
            "B210 Uplink Spectrum", #name
            1
        )
        self.qtgui_freq_sink_x_0_0.set_update_time(0.10)
        self.qtgui_freq_sink_x_0_0.set_y_axis(-140, 10)
        self.qtgui_freq_sink_x_0_0.set_y_label('Relative Gain', 'dB')
        self.qtgui_freq_sink_x_0_0.set_trigger_mode(qtgui.TRIG_MODE_AUTO, squelch, 0, "")
        self.qtgui_freq_sink_x_0_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0_0.enable_grid(True)
        self.qtgui_freq_sink_x_0_0.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0_0.enable_axis_labels(True)
        self.qtgui_freq_sink_x_0_0.enable_control_panel(False)



        labels = ["Rx", '', '', '', '',
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
        self.top_grid_layout.addWidget(self._qtgui_freq_sink_x_0_0_win, 0, 3, 3, 1)
        for r in range(0, 3):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(3, 4):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.qtgui_freq_sink_x_0 = qtgui.freq_sink_c(
            4096, #size
            firdes.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            samp_rate, #bw
            "B210 Downlink Spectrum", #name
            1
        )
        self.qtgui_freq_sink_x_0.set_update_time(0.10)
        self.qtgui_freq_sink_x_0.set_y_axis(-140, 10)
        self.qtgui_freq_sink_x_0.set_y_label('Relative Gain', 'dB')
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_AUTO, squelch, 0, "")
        self.qtgui_freq_sink_x_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0.enable_grid(True)
        self.qtgui_freq_sink_x_0.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0.enable_axis_labels(True)
        self.qtgui_freq_sink_x_0.enable_control_panel(False)



        labels = ["Rx", '', '', '', '',
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
        self.top_grid_layout.addWidget(self._qtgui_freq_sink_x_0_win, 0, 2, 3, 1)
        for r in range(0, 3):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(2, 3):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._squelch_range = Range(-100, -20, 1, -50, 100)
        self._squelch_win = RangeWidget(self._squelch_range, self.set_squelch, 'Power Squelch', "counter_slider", float)
        self.top_layout.addWidget(self._squelch_win)
        _mute_check_box = Qt.QCheckBox('Mute')
        self._mute_choices = {True: 1, False: 0}
        self._mute_choices_inv = dict((v,k) for k,v in self._mute_choices.items())
        self._mute_callback = lambda i: Qt.QMetaObject.invokeMethod(_mute_check_box, "setChecked", Qt.Q_ARG("bool", self._mute_choices_inv[i]))
        self._mute_callback(self.mute)
        _mute_check_box.stateChanged.connect(lambda i: self.set_mute(self._mute_choices[bool(i)]))
        self.top_layout.addWidget(_mute_check_box)
        _ctcss_e_check_box = Qt.QCheckBox('CTCSS')
        self._ctcss_e_choices = {True: 1, False: 0}
        self._ctcss_e_choices_inv = dict((v,k) for k,v in self._ctcss_e_choices.items())
        self._ctcss_e_callback = lambda i: Qt.QMetaObject.invokeMethod(_ctcss_e_check_box, "setChecked", Qt.Q_ARG("bool", self._ctcss_e_choices_inv[i]))
        self._ctcss_e_callback(self.ctcss_e)
        _ctcss_e_check_box.stateChanged.connect(lambda i: self.set_ctcss_e(self._ctcss_e_choices[bool(i)]))
        self.top_layout.addWidget(_ctcss_e_check_box)
        self._ctcss_range = Range(67, 255, .1, 179.9, 200)
        self._ctcss_win = RangeWidget(self._ctcss_range, self.set_ctcss, 'CTCSS frequency', "slider", float)
        self.top_layout.addWidget(self._ctcss_win)
        self.uhd_usrp_source_1 = uhd.usrp_source(
            ",".join(("SDR_address", "")),
            uhd.stream_args(
                cpu_format="fc32",
                args='',
                channels=list(range(0,1)),
            ),
        )
        self.uhd_usrp_source_1.set_center_freq(uhd.tune_request(downlink_freq, 200e3, args=uhd.device_addr("mode_n=integer")), 0)
        self.uhd_usrp_source_1.set_gain(30, 0)
        self.uhd_usrp_source_1.set_antenna('RX2', 0)
        self.uhd_usrp_source_1.set_samp_rate(samp_rate)
        # No synchronization enforced.
        self.uhd_usrp_sink_0 = uhd.usrp_sink(
            ",".join(("type=b200", "")),
            uhd.stream_args(
                cpu_format="fc32",
                args='',
                channels=list(range(0,1)),
            ),
            '',
        )
        self.uhd_usrp_sink_0.set_center_freq(uhd.tune_request(uplink_freq, 200e3, args=uhd.device_addr("mode_n=integer")), 0)
        self.uhd_usrp_sink_0.set_gain(50, 0)
        self.uhd_usrp_sink_0.set_antenna('TX/RX', 0)
        self.uhd_usrp_sink_0.set_samp_rate(samp_rate)
        self.uhd_usrp_sink_0.set_time_unknown_pps(uhd.time_spec())
        self.rational_resampler_xxx_0 = filter.rational_resampler_fff(
                interpolation=aud_to_rf,
                decimation=1,
                taps=None,
                fractional_bw=0.4)
        self.qtgui_freq_sink_x_0_1 = qtgui.freq_sink_f(
            4096, #size
            firdes.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            Audio_samp_rate, #bw
            "NBFM Spectrum", #name
            1
        )
        self.qtgui_freq_sink_x_0_1.set_update_time(0.10)
        self.qtgui_freq_sink_x_0_1.set_y_axis(-140, 10)
        self.qtgui_freq_sink_x_0_1.set_y_label('Relative Gain', 'dB')
        self.qtgui_freq_sink_x_0_1.set_trigger_mode(qtgui.TRIG_MODE_AUTO, squelch, 0, "")
        self.qtgui_freq_sink_x_0_1.enable_autoscale(False)
        self.qtgui_freq_sink_x_0_1.enable_grid(True)
        self.qtgui_freq_sink_x_0_1.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0_1.enable_axis_labels(True)
        self.qtgui_freq_sink_x_0_1.enable_control_panel(False)


        self.qtgui_freq_sink_x_0_1.set_plot_pos_half(not True)

        labels = ["Rx", '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
            "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_0_1.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_0_1.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_0_1.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_0_1.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_0_1.set_line_alpha(i, alphas[i])

        self._qtgui_freq_sink_x_0_1_win = sip.wrapinstance(self.qtgui_freq_sink_x_0_1.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_freq_sink_x_0_1_win)
        self.high_pass_filter_0 = filter.fir_filter_fff(
            1,
            firdes.high_pass(
                1,
                Audio_samp_rate,
                275,
                25,
                firdes.WIN_HAMMING,
                6.76))
        self._gain_range = Range(0, 55, 1, 30, 100)
        self._gain_win = RangeWidget(self._gain_range, self.set_gain, 'Tx gain', "counter_slider", float)
        self.top_layout.addWidget(self._gain_win)
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_cc(mute)
        self.blocks_add_xx_0 = blocks.add_vff(1)
        self.analog_sig_source_x_0_0 = analog.sig_source_f(Audio_samp_rate, analog.GR_COS_WAVE, ctcss, 0.15*ctcss_e, 0, 0)
        self.analog_pwr_squelch_xx_1 = analog.pwr_squelch_cc(squelch, 0.0001, int(0.05*Audio_samp_rate*aud_to_rf), False)
        self.analog_noise_source_x_0 = analog.noise_source_f(analog.GR_GAUSSIAN, .5, 0)
        self.analog_nbfm_tx_0 = analog.nbfm_tx(
        	audio_rate=Audio_samp_rate*aud_to_rf,
        	quad_rate=Audio_samp_rate*aud_to_rf,
        	tau=75e-6,
        	max_dev=2.5e3,
        	fh=-1.0,
                )
        self.analog_nbfm_rx_0 = analog.nbfm_rx(
        	audio_rate=Audio_samp_rate,
        	quad_rate=Audio_samp_rate*aud_to_rf,
        	tau=75e-12,
        	max_dev=2500,
          )


        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_nbfm_rx_0, 0), (self.qtgui_freq_sink_x_0_1, 0))
        self.connect((self.analog_nbfm_tx_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.analog_noise_source_x_0, 0), (self.high_pass_filter_0, 0))
        self.connect((self.analog_pwr_squelch_xx_1, 0), (self.analog_nbfm_rx_0, 0))
        self.connect((self.analog_sig_source_x_0_0, 0), (self.blocks_add_xx_0, 1))
        self.connect((self.blocks_add_xx_0, 0), (self.rational_resampler_xxx_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.qtgui_freq_sink_x_0_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.uhd_usrp_sink_0, 0))
        self.connect((self.high_pass_filter_0, 0), (self.blocks_add_xx_0, 0))
        self.connect((self.rational_resampler_xxx_0, 0), (self.analog_nbfm_tx_0, 0))
        self.connect((self.uhd_usrp_source_1, 0), (self.analog_pwr_squelch_xx_1, 0))
        self.connect((self.uhd_usrp_source_1, 0), (self.qtgui_freq_sink_x_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "passthrough")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_uplink_freq(self):
        return self.uplink_freq

    def set_uplink_freq(self, uplink_freq):
        self.uplink_freq = uplink_freq
        self.uhd_usrp_sink_0.set_center_freq(uhd.tune_request(self.uplink_freq, 200e3, args=uhd.device_addr("mode_n=integer")), 0)

    def get_squelch(self):
        return self.squelch

    def set_squelch(self, squelch):
        self.squelch = squelch
        self.analog_pwr_squelch_xx_1.set_threshold(self.squelch)
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_AUTO, self.squelch, 0, "")
        self.qtgui_freq_sink_x_0_0.set_trigger_mode(qtgui.TRIG_MODE_AUTO, self.squelch, 0, "")
        self.qtgui_freq_sink_x_0_1.set_trigger_mode(qtgui.TRIG_MODE_AUTO, self.squelch, 0, "")

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.qtgui_freq_sink_x_0.set_frequency_range(0, self.samp_rate)
        self.qtgui_freq_sink_x_0_0.set_frequency_range(0, self.samp_rate)
        self.uhd_usrp_sink_0.set_samp_rate(self.samp_rate)
        self.uhd_usrp_source_1.set_samp_rate(self.samp_rate)

    def get_mute(self):
        return self.mute

    def set_mute(self, mute):
        self.mute = mute
        self._mute_callback(self.mute)
        self.blocks_multiply_const_vxx_0_0.set_k(self.mute)

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain

    def get_downlink_freq(self):
        return self.downlink_freq

    def set_downlink_freq(self, downlink_freq):
        self.downlink_freq = downlink_freq
        self.uhd_usrp_source_1.set_center_freq(uhd.tune_request(self.downlink_freq, 200e3, args=uhd.device_addr("mode_n=integer")), 0)

    def get_ctcss_e(self):
        return self.ctcss_e

    def set_ctcss_e(self, ctcss_e):
        self.ctcss_e = ctcss_e
        self._ctcss_e_callback(self.ctcss_e)
        self.analog_sig_source_x_0_0.set_amplitude(0.15*self.ctcss_e)

    def get_ctcss(self):
        return self.ctcss

    def set_ctcss(self, ctcss):
        self.ctcss = ctcss
        self.analog_sig_source_x_0_0.set_frequency(self.ctcss)

    def get_aud_to_rf(self):
        return self.aud_to_rf

    def set_aud_to_rf(self, aud_to_rf):
        self.aud_to_rf = aud_to_rf

    def get_SDR_address(self):
        return self.SDR_address

    def set_SDR_address(self, SDR_address):
        self.SDR_address = SDR_address

    def get_Audio_samp_rate(self):
        return self.Audio_samp_rate

    def set_Audio_samp_rate(self, Audio_samp_rate):
        self.Audio_samp_rate = Audio_samp_rate
        self.analog_sig_source_x_0_0.set_sampling_freq(self.Audio_samp_rate)
        self.high_pass_filter_0.set_taps(firdes.high_pass(1, self.Audio_samp_rate, 275, 25, firdes.WIN_HAMMING, 6.76))
        self.qtgui_freq_sink_x_0_1.set_frequency_range(0, self.Audio_samp_rate)





def main(top_block_cls=passthrough, options=None):

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
