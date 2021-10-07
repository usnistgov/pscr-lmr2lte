#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2021 NIST PSCR.
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from PyQt5.QtWidgets import QFrame, QVBoxLayout, QLabel, QSizePolicy
from PyQt5.QtGui import QFontMetricsF
from PyQt5 import QtCore
from PyQt5.QtCore import Qt as Qtc
from PyQt5.QtCore import QSize

import numpy
from gnuradio import gr
import pmt
import inspect

class Label(QFrame):
	# Positions: 1 = top, 2=bottom, 3=left, 4=right
	def __init__(self, id='', lbl='', alignment=1, valignment=1, parent=None):
		#print("2. [Label] Label__init__")
		QFrame.__init__(self, parent)
				
		lbl = lbl.strip()
		self.lbl = lbl
		self.id = id
		self.lblcontrol = QLabel(lbl, self)
		#print("id " + self.id)
		
		layout =  QVBoxLayout()
		layout.addWidget(self.lblcontrol)

		if alignment == 1:        
			halign = Qtc.AlignCenter
			self.lblcontrol.setAlignment(Qtc.AlignCenter)
		elif alignment == 2:
			halign = Qtc.AlignLeft
			self.lblcontrol.setAlignment(Qtc.AlignLeft)
		else:
			halign = Qtc.AlignRight
			self.lblcontrol.setAlignment(Qtc.AlignRight)

		if valignment == 1:
			valign = Qtc.AlignVCenter
		elif valignment == 2:
			valign = Qtc.AlignTop
		else:
			valign = Qtc.AlignBottom
			
		layout.setAlignment(halign | valign)
		self.setLayout(layout)

		self.show()

	def setLabel(self,label):
		#print("4. [Label] setLabel")
		lbl = label.strip()
		self.lbl = lbl
		self.lblcontrol.setText(lbl)
			
class label(gr.sync_block, Label):
	"""
	Label defined by a message
	"""
	def __init__(self, id='', label='',alignment=1,valignment=1, parent=None):
		try:
			#print("1. [Label] label__init__")
			gr.sync_block.__init__(self, name="Label", in_sig = None, out_sig = None)
			Label.__init__(self, id, label, alignment, valignment, parent)
			self.lbl = label
			
			self.message_port_register_in(pmt.intern("label"))
			self.set_msg_handler(pmt.intern("label"), self.msgHandler)   
		except Exception as e:
			print("[Label] Error with message conversion: %s" % str(e))

	def msgHandler(self, msg):
		#print("3. [Label] msgHandler")
		try:
			if pmt.is_pair(msg):
				if (pmt.to_python(pmt.car(msg)) == self.alias() or pmt.to_python(pmt.car(msg)) == self.id):
					newVal = pmt.to_python(pmt.cdr(msg))
					if type(newVal) == str:
							super().setLabel(newVal)
					else:
						print("[Label] Error: Value received was not a string: %s" % type(newVal))
				
		except Exception as e:
			print("[Label] Error with message conversion: %s" % str(e))
			pass

