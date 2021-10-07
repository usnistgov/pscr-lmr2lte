#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2021 NIST PSCR.
# modified from ledindicator.python
# Copyright 2019
# ghostop14
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


from PyQt5.QtWidgets import QFrame, QWidget, QHBoxLayout, QVBoxLayout, QLabel
from PyQt5.QtGui import QPainter, QPixmap,	QBrush, QColor, QPen, QFontMetricsF
from PyQt5 import Qt
from PyQt5 import QtCore
from PyQt5.QtCore import Qt as Qtc
from PyQt5.QtCore import QSize, QPoint, QRect
from PyQt5.QtGui import QPixmap,QPainter, QPainterPath,QLinearGradient, QRadialGradient, QConicalGradient

import numpy
from gnuradio import gr
import pmt

class LabeledLEDIndicator(QFrame):
	# Positions: 1 = above, 2=below, 3=left, 4=right
	def __init__(self, id='', lbl='', Color='green', maxSize=80, position=1, alignment=1, valignment=1, parent=None):
		#print("2. LabeledLEDIndicator __init__")
		QFrame.__init__(self, parent)
		self.id = id
		#print("id " + self.id)
		if (maxSize > 0):
			self.colorControl = LEDIndicator(Color, maxSize, parent)
			
			if position < 3:
				layout =  QVBoxLayout()
			else:
				layout = QHBoxLayout()
				
			lbl = lbl.strip()
			self.lbl = lbl
			self.lblcontrol = QLabel(lbl, self)
			if alignment == 1:		  
				self.lblcontrol.setAlignment(Qtc.AlignHCenter)
			elif alignment == 2:
				self.lblcontrol.setAlignment(Qtc.AlignLeft)
			else:
				self.lblcontrol.setAlignment(Qtc.AlignRight)

			# add top or left
			if len(lbl) > 0:
				if position == 1 or position == 3:
					layout.addWidget(self.lblcontrol)

			layout.addWidget(self.colorControl)
			
			# Add bottom or right
			if len(lbl) > 0:
				if position == 2 or position == 4:
					layout.addWidget(self.lblcontrol)
					
			if alignment == 1:		  
				halign = Qtc.AlignCenter
			elif alignment == 2:
				halign = Qtc.AlignLeft
			else:
				halign = Qtc.AlignRight

			if valignment == 1:
				valign = Qtc.AlignVCenter
			elif valignment == 2:
				valign = Qtc.AlignTop
			else:
				valign = Qtc.AlignBottom
				
			layout.setAlignment(halign | valign)
			self.setLayout(layout)
			left, top, right, bottom = layout.getContentsMargins()
			#print( "left", left, "right", right, "top", top, "bottom", bottom)
			try:
				if (len(lbl) > 0):
					textfont = self.lblcontrol.font()
					metrics = QFontMetricsF(textfont)
					#print("label width", metrics.width(lbl), metrics.boundingRect(lbl))
					if position < 3: 
						#label top or bottom
						maxWidth = max(maxSize,metrics.width(lbl)) + left + right
						maxHeight = maxSize + metrics.height() + top + bottom
					else:
						#label left or right
						maxWidth = maxSize + metrics.width(lbl) + left + right
						maxHeight = max(maxSize,metrics.height()) + top + bottom
				else:
					maxWidth = maxSize + left + right
					maxHeight = maxSize + top + bottom  

				if Color=='transparent':
					self.setMinimumSize(QSize(0, 0))
					self.hide()
				else:
					self.show()
					#must set size to zero before changing size
					self.setMinimumSize(QSize(0, 0))
					self.setMinimumSize(maxWidth, maxHeight)
					#print("id ", self.id, " maxWidth ", maxWidth, " maxHeight ", maxHeight)
			except Exception as e:
				print("[LEDIndicator] Error: %s" % str(e))
		
	def setLEDcolor(self,Color):
		#print("5. LabeledLEDIndicator setLEDcolor")
		self.colorControl.setLEDcolor(Color)
		if Color=='transparent':
			self.hide()
		else:	
			self.show()
		
class LEDIndicator(QFrame):
	def __init__(self, Color='green', maxSize=80, parent=None):
		#print("3. LEDIndicator __init__")
		QFrame.__init__(self, parent)

		self.maxSize = maxSize
		self.Color = QColor(Color)

		self.setMinimumSize(maxSize, maxSize)	   
		self.setMaximumSize(maxSize, maxSize)  

	def setLEDcolor(self,Color):
		#print("6. LEDIndicator setLEDcolor")
		if (self.maxSize > 0):
			self.Color = QColor(Color)
			super().update()
		
	def paintEvent(self, event):
		#print("7. LEDIndicator paintEvent")
		super().paintEvent(event)
		
		painter = QPainter(self)
		
		#self.painter.setPen(self.backgroundColor)
		size = self.size()
		brush = QBrush()

		#rect = QtCore.QRect(2, 2, size.width()-4, size.height()-4)
		rect = QtCore.QRect(0, 0, size.width(), size.height())
		#painter.fillRect(rect, brush)
		smallestDim = size.width()
		if smallestDim > size.height():
			smallestDim = size.height();
			
		smallestDim = smallestDim/2
		smallestDim -= 2
		## rect.moveCenter(QPoint(size.width()/2,size.height()/2))
		center_x = size.width()/2
		center_y = size.height()/2
		centerpoint = QPoint(center_x,center_y)
		
		# Draw the border
		#circle_path = QPainterPath()
		radius = smallestDim
		#circle_path.addEllipse(centerpoint,radius,radius)
		painter.setPen(QPen(QColor('lightgray'),0))
		brush.setStyle(Qtc.SolidPattern)

		radial = QRadialGradient(center_x,center_y/2, radius)
		#radial = QConicalGradient(centerpoint, 50)	 # Last number is the angle
		radial.setColorAt(0, Qtc.white)
		radial.setColorAt(0.8, Qtc.darkGray)
		painter.setBrush(QBrush(radial))
		painter.drawEllipse(centerpoint,radius,radius)
		#painter.setBrush(QColor('gray'))
		# painter.drawPath(circle_path)
		
		# Draw the colored center		 
		radial = QRadialGradient(center_x,center_y/2, radius)
		#radial = QRadialGradient(center_x*2/3,center_y*2/3, radius)
		#radial = QConicalGradient(centerpoint, 0)	# Last number is the angle
		radial.setColorAt(0, Qtc.white)
		
		radial.setColorAt(.7, self.Color)
		brush.setColor(self.Color)
		painter.setPen(QPen(self.Color,0))
			
		brush.setStyle(Qtc.SolidPattern)
		#painter.setBrush(brush)
		painter.setBrush(QBrush(radial))
		#radius = radius - 9
		if (smallestDim <= 30):
			radius = radius - 3
		elif (smallestDim <= 60):
			radius = radius - 4
		elif (smallestDim <=100):
			  radius = radius - 5
		elif (smallestDim <= 200):
			radius = radius - 6
		elif (smallestDim <= 300):
			radius = radius - 7
		else:
			radius = radius - 9
		painter.drawEllipse(centerpoint,radius,radius)

class led(gr.sync_block, LabeledLEDIndicator):
	def __init__(self, id='', lbl='', Color='green', maxSize=80, position=1, alignment=1, valignment=1, parent=None):
		#print("1. led __init__")
		gr.sync_block.__init__(self, name = "LEDIndicator", in_sig = None, out_sig = None)
		LabeledLEDIndicator.__init__(self, id, lbl, Color, maxSize, position, alignment, valignment, parent)
		self.lbl = lbl
		
		self.message_port_register_in(pmt.intern("color"))
		self.set_msg_handler(pmt.intern("color"), self.msgHandler)	 


	def msgHandler(self, msg):
		#print("4. led msgHandler")
		try:
			if pmt.is_pair(msg):
				if (pmt.to_python(pmt.car(msg)) == self.lbl or pmt.to_python(pmt.car(msg)) == self.alias() or pmt.to_python(pmt.car(msg)) == self.id):
					newVal = pmt.to_python(pmt.cdr(msg))
					#https://doc.qt.io/qt-5/qcolor.html#QColor-7
					# color can be any from https://www.w3.org/TR/SVG11/types.html#ColorKeywords
					# or #RRGGBB, etc.
					if type(newVal) == str:
							super().setLEDcolor(newVal)
					else:
						print("[LEDIndicator] Error: Value received was not a string: %s" % type(newVal))
				
		except Exception as e:
			#print("[LEDIndicator] Error with message conversion: %s" % str(e))
			pass

