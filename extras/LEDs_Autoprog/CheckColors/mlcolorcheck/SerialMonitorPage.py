# -*- coding: utf-8 -*-
#
#         MobaLedCheckColors: Color checker for WS2812 and WS2811 based MobaLedLib
#
#         SerialMonitorPage
#
# * Version: 1.00
# * Author: Harold Linke
# * Date: December 25th, 2019
# * Copyright: Harold Linke 2019
# *
# *
# * MobaLedCheckColors on Github: https://github.com/haroldlinke/MobaLedCheckColors
# *
# *
# * History of Change
# * V1.00 25.12.2019 - Harold Linke - first release
# *
# *
# * MobaLedCheckColors supports the MobaLedLib by Hardi Stengelin
# * https://github.com/Hardi-St/MobaLedLib
# *
# * MobaLedCheckColors is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * MobaLedCheckColors is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.
# *
# * MobaLedCheckColors is based on tkColorPicker by Juliette Monsel
# * https://sourceforge.net/projects/tkcolorpicker/
# *
# * tkcolorpicker - Alternative to colorchooser for Tkinter.
# * Copyright 2017 Juliette Monsel <j_4321@protonmail.com>
# *
# * tkcolorpicker is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * tkcolorpicker is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.
# *
# * The code for changing pages was derived from: http://stackoverflow.com/questions/7546050/switch-between-two-frames-in-tkinter
# * License: http://creativecommons.org/licenses/by-sa/3.0/
# ***************************************************************************

import tkinter as tk
from tkinter import ttk,messagebox
#from tkcolorpicker.functions import tk, ttk, round2, create_checkered_image, \
#    overlay, hsv_to_rgb, hexa_to_rgb, rgb_to_hexa, col2hue, rgb_to_hsv, convert_K_to_RGB
#from tkcolorpicker.alphabar import AlphaBar
#from tkcolorpicker.gradientbar import GradientBar
#from tkcolorpicker.lightgradientbar import LightGradientBar
#from tkcolorpicker.colorsquare import ColorSquare
#from tkcolorpicker.colorwheel import ColorWheel
#from tkcolorpicker.spinbox import Spinbox
#from tkcolorpicker.limitvar import LimitVar
from mlcolorcheck.configfile import ConfigFile
from locale import getdefaultlocale
#import re
#import math
import os
import serial
import sys
import threading
import queue
import time
import logging
#import concurrent.futures
#import random
#import webbrowser
from datetime import datetime
#import json

VERSION ="V01.17 - 25.12.2019"
LARGE_FONT= ("Verdana", 12)
VERY_LARGE_FONT = ("Verdana", 14)
SMALL_FONT= ("Verdana", 8)




# --- Translation - not used
EN = {}
FR = {"Red": "Rouge", "Green": "Vert", "Blue": "Bleu",
      "Hue": "Teinte", "Saturation": "Saturation", "Value": "Valeur",
      "Cancel": "Annuler", "Color Chooser": "Sélecteur de couleur",
      "Alpha": "Alpha"}
DE = {"Red": "Rot", "Green": "Grün", "Blue": "Blau",
      "Hue": "Farbton", "Saturation": "Sättigung", "Value": "Helligkeit",
      "Cancel": "Beenden", "Color Chooser": "Farbwähler",
      "Alpha": "Alpha", "Configuration": "Einstellungen"}

try:
    TR = EN
    if getdefaultlocale()[0][:2] == 'fr':
        TR = FR
    else:
        if getdefaultlocale()[0][:2] == 'de':
            TR = DE
except ValueError:
    TR = EN


def _(text):
    """Translate text."""
    return TR.get(text, text)

ThreadEvent = None
            
class SerialMonitorPage(tk.Frame):
    def __init__(self, parent, controller):
        self.controller = controller
        self.tabname = "Serial Monitor"
        self.tabClassName = "SerialMonitorPage"
        tk.Frame.__init__(self, parent)
        title_frame = ttk.Frame(self)

        ttk.Label(title_frame, text="Serial Monitor").pack(side="left", padx=4, pady=(4, 1))
        title_frame.grid(row=0, column=0,padx=10, pady=(10, 4))

        input_frame = ttk.Frame(self)

        self.input = ttk.Entry(input_frame, justify="center", width=30, name='entry')
        self.input.insert(0, "")
        ttk.Label(input_frame, text="Senden").pack(side="left", padx=4, pady=(4, 1))
        self.input.pack(side="left", padx=6, pady=(4, 1), fill='x', expand=True)
        self.send_button = ttk.Button(input_frame, text=_("Senden"),width=10, command=self.send)
        self.send_button.pack(side="left", padx=4, pady=(4, 1))

        input_frame.grid(row=1, column=0,padx=10, pady=(10, 4))
        
        self.input.bind("<Return>",self.send)

        frameLabel = tk.Frame(self, padx=10, pady= 10)

        self.text = tk.Text(frameLabel, wrap='word', bg=self.cget('bg'), height=25, width=70)         # 04.12.19: Old: height=10, width=50
        frameLabel.grid(row=2, column=0,padx=10, pady=(10, 4))

        # add a vertical scroll bar to the text area
        scroll=tk.Scrollbar(frameLabel)
        self.text.configure(yscrollcommand=scroll.set)
        scroll.config(command=self.text.yview)

        #pack everything
        self.text.pack(side=tk.LEFT)
        scroll.pack(side=tk.RIGHT,fill=tk.Y)

        # binding
        self.input.bind("T",self.dummy)
        self.input.bind("t",self.dummy)

        button_frame = ttk.Frame(self)

        button_width = 20
 
        self.queue = queue.Queue()
        self.monitor_serial = False

    def tabselected(self):
        self.controller.currentTabClass = self.tabClassName
        print(self.tabname)
        pass        

    def getConfigData(self, key):
        return self.controller.getConfigData(key)
    
    
    def readConfigData(self):
        self.controller.readConfigData()
        
    def setConfigData(self,key, value):
        self.controller.setConfigData(key, value)

    def setParamData(self,key, value):
        self.controller.setParamData(key, value)

    def MenuUndo(self,_event=None):
        pass
    
    def MenuRedo(self,_event=None):
        pass
    
    def dummy(self,event):
        print("dummy")
        

    def send(self,event=None):
        message = self.input.get() +"\r\n"
        self.controller.send_to_ARDUINO(message)

    def process_serial(self):

        #print("process_serial: Start")
        textmessage = self.controller.checkconnection()
        if textmessage:
            self.text.insert("end", textmessage)
            self.text.yview("end")
            self.controller.set_ARDUINOmessage(textmessage)
        else:
            if self.controller.queue:
                while self.controller.queue.qsize():
                    #print("process_serial: While loop")
                    try:
                        readtext = self.controller.queue.get()
                        date_time = datetime.now()
                        d = date_time.strftime("%H:%M:%S")
                        textmessage = d + "  " + readtext
                        if not textmessage.endswith("\n"):
                            textmessage = textmessage +"\n"
                        #write message into text window
                        self.text.insert("end", textmessage)
                        self.text.yview("end")
                        self.controller.set_ARDUINOmessage(textmessage)
                    except IOError:
                        pass
        
            
        if self.monitor_serial:
            self.after(100, self.process_serial)

    def start_process_serial(self):
        global ThreadEvent
        ThreadEvent = threading.Event()
        ThreadEvent.set()
        time.sleep(2)
        ThreadEvent.clear()
        self.monitor_serial = True
        self.thread = SerialThread(self.controller.queue,self.controller.arduino)
        self.thread.start()
        self.process_serial()

    def stop_process_serial(self):
        global ThreadEvent
        self.monitor_serial = False
        if ThreadEvent:
            ThreadEvent.set()
        #time.sleep(1)
        
    def cancel(self):
        self.stop_process_serial()
        
    def connect (self):
        self.start_process_serial()
    
    def disconnect (self):
        self.stop_process_serial()


class SerialThread(threading.Thread):
    def __init__(self, p_queue, p_serialport):
        #global serialport
        threading.Thread.__init__(self)
        self.queue = p_queue
        self.serialport = p_serialport

    def run(self):
        #global serialport
        logging.info("SerialThread started")
        if self.serialport:

            while not ThreadEvent.is_set() and self.serialport:
                # print("Event:",ThreadEvent.is_set())
                # print("Serialport:", serialport)
                # logging.info("SerialThread while loop")
                #logging.info("SerialThread running")
                try:
                    text = self.serialport.readline()
                except:
                    pass
                if len(text)>0:
                    try:
                        self.queue.put(text.decode('utf-8'))
                    except:
                        pass
                    logging.info("SerialThread got message: %s", text)

        logging.info("SerialThread received event. Exiting")

