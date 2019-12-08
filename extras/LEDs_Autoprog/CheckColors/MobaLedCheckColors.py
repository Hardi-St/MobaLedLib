# -*- coding: utf-8 -*-
#
#         MobaLedCheckColors: Color checker for WS2812 and WS2811 based MobaLedLib
#
# * Version: 1.10
# * Author: Harold Linke
# * Date: November 23rd, 2019
# * Copyright: Harold Linke 2019
# *
# *
# * MobaLedCheckColors on Github: https://github.com/haroldlinke/MobaLedCheckColors
# *
# *
# * History of Change
# * V1.00 23.11.2019 - Harold Linke - first release
# * V1.09 29.11.2019
# * V1.10 03.12.2019 - Hardi
# *                  - Brightness could be shown in pecent if the constant PERCENT_BRIGHTNESS is set to 1
# *                  - Write a finish file at the end of the program. This is needed by the calling excel program.
# *                    Harold: Is this file also written in case of a crash ?
# *                  - Some more detailed error messages
# *       04.12.2019 - Added COM Ports 7..40 because on some systems such high com ports are used (Hardi: COM12)
# *                    Hint: For test purposes the COM Port could be changed in the device manager: "devmgmt.msc" in windows
# *                  - Send "#End" when the program is closed to restart the arduino. Prior if was still in the ColorChecker mode (LED was flashing fast)
# *                  - Moved the sending og the "#BEGIN" command down because the Arduino is restarted if the serial connection is opened
# *                    => The "#BEGIN" command was not received
# *                    With this change the first press to a color button is raccepted (Prior a second press was needed)
# *                  - The serial monitor could be started in the json file with "startpage": 2,
# *                  - Increased the size of the serial monitor window
# *                  - Added "End" button to the serial monitor
# * V1.11 08.12.19:  - Writing the finis file also in "def disconnect(self):"
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
from tkinter import ttk
from tkcolorpicker.functions import tk, ttk, round2, create_checkered_image, \
    overlay, hsv_to_rgb, hexa_to_rgb, rgb_to_hexa, col2hue, rgb_to_hsv
from tkcolorpicker.alphabar import AlphaBar
from tkcolorpicker.gradientbar import GradientBar
from tkcolorpicker.lightgradientbar import LightGradientBar
from tkcolorpicker.colorsquare import ColorSquare
from tkcolorpicker.colorwheel import ColorWheel
from tkcolorpicker.spinbox import Spinbox
from tkcolorpicker.limitvar import LimitVar
from locale import getdefaultlocale
import re
import math
import os
import serial
import sys
from tkinter import messagebox
import threading
import queue
import time
import logging
import concurrent.futures
import random
import webbrowser
from datetime import datetime

VERSION ="V01.11 - 08.12.2019"
LARGE_FONT= ("Verdana", 12)
VERY_LARGE_FONT = ("Verdana", 14)
SMALL_FONT= ("Verdana", 8)

PARAM_FILENAME = 'MobaLedTest_param.json'
CONFIG_FILENAME = 'MobaLedTest_config.json'
DISCONNECT_FILENAME = 'MobaLedTest_disconnect.txt'
CLOSE_FILENAME = 'MobaLedTest_close.txt'
HELPPAGE_FILENAME = "MobaLedCheckColorsHelp01.htm"

FINISH_FILE = "Finished.txt"                                                 # 03.12.19:

PERCENT_BRIGHTNESS = 1  # 1 = Show the brightnes as percent, 0 = Show the brightnes as ">>>"# 03.12.19:

PALETTE = {
        "ROOM_COL0": "#0F0D03",
        "ROOM_COL1": "#162C1B",
        "ROOM_COL2": "#9B4905",
        "ROOM_COL3": "#271201",
        "ROOM_COL4": "#1E0000",
        "ROOM_COL5": "#4F2707",
        "GAS_LIGHT D": "#323232",
        "GAS LIGHT": "#FFFFFF",
        "NEON_LIGHT D": "#14141B",
        "NEON_LIGHT M": "#464650",
        "NEON_LIGHT": "#F5F5FF",
        "ROOM_TV0 A": "#323214",
        "ROOM_TV0 B": "#46461E",
        "ROOM_TV1 A": "#323208",
        "ROOM_TV1 B": "#323208",
        "SINGLE_LED": "#FFFFFF",
        "SINGLE_LED D": "#323232"
        }


# --- Translation - not used
EN = {}
FR = {"Red": "Rouge", "Green": "Vert", "Blue": "Bleu",
      "Hue": "Teinte", "Saturation": "Saturation", "Value": "Valeur",
      "Cancel": "Annuler", "Color Chooser": "Sélecteur de couleur",
      "Alpha": "Alpha"}
DE = {"Red": "Rot", "Green": "Grün", "Blue": "Blau",
      "Hue": "Farbton", "Saturation": "Sättigung", "Value": "Helligkeit",
      "Cancel": "Beenden", "Color Chooser": "Farbwähler",
      "Alpha": "Alpha"}

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


import json


# ----------------------------------------------------------------
# Class ConfigFile
# ----------------------------------------------------------------
class ConfigFile():
    """ Configuration File """

    def __init__(self):
        # type:
        """ SDConfig Constructor Method (__init__)

        Arguments:
            None

        Raises:
            None
        """

        filedir = os.path.dirname(os.path.realpath(__file__))
        filepath = os.path.join(filedir, CONFIG_FILENAME)

        try:
            with open(filepath, "r") as read_file:
                data = json.load(read_file)
        except:
            data = ""

        self.serportnumber    = 0       # define serial port
        self.serportname      = "No Device"  # define serial port name
        self.maxLEDcount      = 255     # defines, the maximum LED count
        self.lastLed          = 0
        self.lastLedCount     = 1
        self.pos_x            = 100
        self.pos_y            = 100
        self.colorview        = 1
        self.startpage        = 0
        self.led_correction_r = 100
        self.led_correction_g = 69
        self.led_correction_b = 94
        self.use_led_correction = True
        self.autoconnect      = False
        self.old_color        = (255,255,255)
        self.palette          = PALETTE
        # self.palette          = {"Room_Col0": "#FF6C00","Room_Col1":"#FFA24F","Room_Col2":"#FFCDA6K","Room_Col3":"#FF0000", "Room_Col4":"#00FF00", "Room_Col5":"#0000FF", "Gas Light1":"#323232",
        #            "Gas Light2":"#FFFFFF", "Neon Dark":"#14141B", "Neon Medium":"#464650", "Neon bright":"#F5F5FF", "TV0-Ch A":"#323214", "TV0-Ch B":"#46461E", "TV1-Ch A":"#323208", "TV2-Ch B":"#323208","Single 1":"#FFFFFF",
        #            "Single 1":"#323232"}

        #        self.palette = ("Room_Col0\n 1500K", "Room_Col1\n 2600k", "Room_Col2\n 4000K", "Room_Col3\n #FF0000", "Room_Col4\n #00FF00", "Room_Col5\n #0000FF", "Gas Light1\n #323232",
        #                   "Gas Light2\n #FFFFFF", "Neon Dark\n #14141B", "Neon Medium\n #464650", "Neon bright\n #F5F5FF", "TV0-Ch A \n #323214", "TV0-Ch B \n #46461E", "TV1-Ch A \n #323208", "TV2-Ch B \n #323208","Single 1 \n #FFFFFF",
        #                   "Single 1 \n #323232")

        #self.palette = ("Kerze \n 1500K", "Natriumlampe \n 2000K", "Gluehlampe \n 2600K", "Hallogenlampe \n 3000K", "Fotolampe \n 3400K", "Neonroehre \n 4000K", "Mondlicht \n 4120K",
        #           "Xeonlampe \n 4500K", "Morgensonne \n 5000K", "Nachmtgsonne \n 5500K", "Mittagssonne \n 5800K", "Bed. Himmel \n 7000K", "Nebel \n 8000K",
        #           "Blauer Himmel \n 10000K")


        try:
            if "serportnumber" in data:
                self.serportnumber = data["serportnumber"]    # define serial port
            if "serportname" in data:
                self.serportname = data["serportname"]    # define serial port
            if "maxLEDcount" in data:
                self.maxLEDcount = data["maxLEDcount"]         # defines, the maximum LED count
            if "lastLedCount" in data:
                self.lastLedCount = data["lastLedCount"]         # stores the last LED count
            if "lastLed" in data:
                self.lastLed = data["lastLed"]         # store last led numt
            if "pos_x" in data:
                self.pos_x = data["pos_x"]
            if "pos_y" in data:
                self.pos_y = data["pos_y"]
            if "colorview" in data:
                self.colorview = data["colorview"]
            if "startpage" in data:
                self.startpage = data["startpage"]
            if "led_correction_r" in data:
                self.led_correction_r = data["led_correction_r"]
            if "led_correction_g" in data:
                self.led_correction_g = data["led_correction_g"]
            if "led_correction_b" in data:
                self.led_correction_b = data["led_correction_b"]
            if "use_led_correction" in data:
                self.use_led_correction = data["use_led_correction"]
            if "autoconnect" in data:
                self.autoconnect = data["autoconnect"]
            if "old_color" in data:
                self.old_color = data["old_color"]
            if "palette" in data:
                self.palette = data["palette"]


        except:
            print ("Error in Config File")
            print(data)


    def save(self):

        filedir = os.path.dirname(os.path.realpath(__file__))
        filepath = os.path.join(filedir, CONFIG_FILENAME)

        data=dict(serportnumber=self.serportnumber,serportname=self.serportname,maxLEDcount=self.maxLEDcount,lastLedCount=self.lastLedCount,lastLed=self.lastLed,pos_x=self.pos_x,pos_y=self.pos_y,
                  colorview=self.colorview, startpage = self.startpage, led_correction_r = self.led_correction_r,
                  led_correction_g = self.led_correction_g,led_correction_b = self.led_correction_b,
                  use_led_correction = self.use_led_correction, old_color=self.old_color, palette = self.palette, autoconnect=self.autoconnect)

        # Write JSON file
        with open(filepath, 'w', encoding='utf8') as outfile:
            json.dump(data, outfile, ensure_ascii=False, indent=4)

# ----------------------------------------------------------------

# ----------------------------------------------------------------
# Global Var sdConfig
# ----------------------------------------------------------------
sdConfig = ConfigFile()

# ----------------------------------------------------------------
# Class ML_Param
#
# transfer of params between MobaLedLib via JSON file
# ----------------------------------------------------------------
class ParamFile():
    """ Configurationfile """

    def __init__(self):
        # type:
        """ SDConfig Constructor Method (__init__)

        Arguments:
            None

        Raises:
            None
        """
        self.filename = PARAM_FILENAME

        filedir = os.path.dirname(os.path.realpath(__file__))
        self.filepath = os.path.join(filedir, self.filename)

        try:
            with open(self.filepath, "r") as read_file:
                data = json.load(read_file)
            self.fileexists     = True
        except:
            data = ""
            self.fileexists     = False


        self.color          = ""  # color param
        self.cor_color      = ""  # corrected color for ARDUINO - only output
        self.Lednum         = -1
        self.LedCount       = 0
        self.comport        = ""
        self.coltab         = {}

        try:
            if "color" in data:
                self.color = data["color"]
            if "cor_color" in data:
                self.cor_color = data["cor_color"]
            if "Lednum" in data:
                self.Lednum = data["Lednum"]
            if "LedCount" in data:
                self.LedCount = data["LedCount"]
            if "comport" in data:
                self.comport = data["comport"]
            if "coltab" in data:
                self.coltab = data["coltab"]


        except:
            print ("Error in Param File")
            print(data)


    def save(self):

        filedir = os.path.dirname(os.path.realpath(__file__))
        filepath = os.path.join(filedir, self.filename)

        data=dict(color=self.color,cor_color=self.cor_color,Lednum=self.Lednum,LedCount=self.LedCount,comport=self.comport,coltab=self.coltab)

        # Write JSON file
        with open(filepath, 'w', encoding='utf8') as outfile:
            json.dump(data, outfile, ensure_ascii=False, indent=4)

    def check(self):
        return os.path.isfile(self.filename)


# ----------------------------------------------------------------

# ----------------------------------------------------------------
# Global Var MLLParam - transfers params
# ----------------------------------------------------------------
MLLparam = ParamFile()

# --- replace config data with paramters from MLLparam file if the data was transferred
if MLLparam.fileexists:
    if MLLparam.color != "":
        sdConfig.old_color = MLLparam.color
    if MLLparam.Lednum >= 0:
        sdConfig.lastLed = MLLparam.Lednum
    if MLLparam.LedCount > 0:
        sdConfig.lastLedCount = MLLparam.LedCount
    if MLLparam.comport != "":
        sdConfig.serportname = MLLparam.comport
    if MLLparam.coltab != {}:
        sdConfig.palette = MLLparam.coltab

# ----------------------------------------------------------------
# Class LEDColorTest
# ----------------------------------------------------------------

class LEDColorTest(tk.Tk):

    # ----------------------------------------------------------------
    # LEDColorTest __init__
    # ----------------------------------------------------------------
    def __init__(self, *args, **kwargs):

        tk.Tk.__init__(self, *args, **kwargs)

        self.arduino = None
        self.queue = None

        screen_width = self.winfo_screenwidth()
        screen_height = self.winfo_screenheight()

        if sdConfig.pos_x < screen_width and sdConfig.pos_y < screen_height:

            self.geometry('+%d+%d' % (sdConfig.pos_x, sdConfig.pos_y))
        else:
            self.geometry("+100+100")

#        tk.Tk.iconbitmap(self,default='clienticon.ico')
        tk.Tk.wm_title(self, "MobaLedLib " + VERSION)

        self.title("MobaLedLib " + VERSION)
        self.transient(self.master)
        self.resizable(False, False)
        self.rowconfigure(1, weight=1)

        self.color = ""
        style = ttk.Style(self)
        style.map("palette.TFrame", relief=[('focus', 'sunken')],
                  bordercolor=[('focus', "#4D4D4D")])
        self.configure(background=style.lookup("TFrame", "background"))

        # --- define container for configuration and colortest frame
        container = tk.Frame(self)

        container.pack(side="top", fill="both", expand = True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0, weight=1)

        self.frames = {}

        for F in (ColorCheckPage, ConfigurationPage, SerialMonitorPage):

            frame = F(container, self)
            self.frames[F] = frame
            frame.grid(row=0, column=0, sticky="nsew")

        if sdConfig.startpage == 1:
            self.show_frame(ColorCheckPage)
        else:
            if sdConfig.startpage == 2:                                      # 04.12.19:
                self.show_frame(SerialMonitorPage)
            else:
                self.show_frame(ConfigurationPage)

    # ----------------------------------------------------------------
    # LEDColorTest show_frame
    # ----------------------------------------------------------------
    def show_frame(self, cont):

        #frame = self.frames[cont]
        #if cont == SerialMonitorPage or cont == ColorCheckPage:
        #    self.frames[SerialMonitorPage].start_process_serial()
        #else:
        #    self.frames[SerialMonitorPage].stop_process_serial()
        frame = self.frames[cont]
        frame.tkraise()

    # ----------------------------------------------------------------
    # LEDColorTest cancel
    # ----------------------------------------------------------------
    def cancel(self):
        sdConfig.pos_x = self.winfo_x()
        sdConfig.pos_y = self.winfo_y()
        sdConfig.save()
        open(FINISH_FILE, 'a').close()                                       # 03.12.19:
        self.disconnect()
        self.destroy()


    # ----------------------------------------------------------------
    # LEDColorTest connect ARDUINO
    # ----------------------------------------------------------------
    def connect(self):

        print("connect")

        port = sdConfig.serportname

        self.ARDUINO_message = ""
        self.set_pipeline(pipeline)
        timeout = False

        if port!="NO DEVICE" and not self.arduino:

            try:

                self.arduino = serial.Serial(port,baudrate=115200,timeout=2,parity=serial.PARITY_NONE,stopbits=serial.STOPBITS_ONE,bytesize=serial.EIGHTBITS)

                print("connected to: " + self.arduino.portstr)
                '''                                                          # 04.12.19: Moved down because the arduino has to boot befor he can receive messages
                message = "#BEGIN\n"
                self.arduino.write(message.encode())
                print("Message send to ARDUINO: ",message)
                '''
                seq = []
                count = 1
                timeout = False
                while count==1 and not timeout:
                    timeout = True
                    for c in self.arduino.read():
                        timeout = False
                        seq.append(chr(c)) #convert from ANSII
                        joined_seq = ''.join(str(v) for v in seq) #Make a string from array

                        if chr(c) == '\n':
                            print("Feedback from ARDUINO: " + joined_seq)
                            self.ARDUINO_message = joined_seq
                            seq = []
                            count += 1
                            break
                if timeout:         # 03.12.19: more detailed error message
                    messagebox.showerror("Timeout waiting for the Arduino answer",
                                         "ARDUINO is not answering.\n"
                                         "\n"
                                         "Check if the correct program is loaded to the arduino.\n"
                                         "It must becompiled with the compiler switch:\n"
                                         "  #define RECEIVE_LED_COLOR_PER_RS232")
                    self.ARDUINO_message = "ARDUINO is not answering"
                    port = "NO DEVICE"
                    self.arduino = None
                    return False
                else:
                    if sdConfig.startpage == 1: # Just send the begin if the LED page shown at the start
                        message = "#BEGIN\n"                                     # 04.12.19: New location
                        self.arduino.write(message.encode())
                        print("Message send to ARDUINO: ",message)
                    return True

            except:                 # 03.12.19: more detailed error message
                messagebox.showerror("Error connecting to the serial port " + sdConfig.serportname,
                                     "Serial Interface to ARDUINO could not be opened!\n"
                                     "\n"
                                     "Check if an other program is using the serial port\n"
                                     "This could be the Ardiono IDE/serial monitor or an\n"
                                     "other instance of this program.")
                self.ARDUINO_message = "ERROR! Connection could not be opened"
                port = "NO DEVICE"
                self.arduino = None
                return False

            return not timeout
        else:
            return False

    def set_pipeline(self,queue):
        self.queue = queue


    # ----------------------------------------------------------------
    # LEDColorTest disconnect ARDUINO
    # ----------------------------------------------------------------
    def disconnect(self):

        print("disconnect")
        open(FINISH_FILE, 'a').close()                                       # 08.12.19: Moved to this proc
        if self.arduino:
            message = "#END\n"
            self.arduino.write(message.encode())
            print("Message send to ARDUINO: ",message)

            self.arduino.close()
            self.arduino = None

    # ----------------------------------------------------------------
    # LEDColorTest disconnect ARDUINO
    # ----------------------------------------------------------------
    def send_to_ARDUINO(self, message):

        if self.arduino:
            self.arduino.write(message.encode())
            print("Message send to ARDUINO: ",message)
            self.queue.put( ">> " + str(message))

    def call_helppage(self,event=None):
        webbrowser.open_new_tab(HELPPAGE_FILENAME)

    def switch_to_serial_mon(self):
        self.unbind("F")
        self.unbind("f")
        self.unbind("S")
        self.unbind("s")
        self.unbind("H")
        self.unbind("h")
        self.unbind("R")
        self.unbind("r")
        self.unbind("G")
        self.unbind("g")
        self.unbind("B")
        self.unbind("b")
        self.unbind("+")
        self.unbind("-")
        self.unbind("A")
        self.unbind("a")
        self.unbind("T")
        self.unbind("t")
        self.show_frame(SerialMonitorPage)


    def switch_to_color_check(self):
        frame = self.frames[ColorCheckPage]
        self.bind("F",frame.s_h.invoke_buttonup)
        self.bind("f",frame.s_h.invoke_buttondown)
        self.bind("S",frame.s_s.invoke_buttonup)
        self.bind("s",frame.s_s.invoke_buttondown)
        self.bind("H",frame.s_v.invoke_buttonup)
        self.bind("h",frame.s_v.invoke_buttondown)
        self.bind("R",frame.s_red.invoke_buttonup)
        self.bind("r",frame.s_red.invoke_buttondown)
        self.bind("G",frame.s_green.invoke_buttonup)
        self.bind("g",frame.s_green.invoke_buttondown)
        self.bind("B",frame.s_blue.invoke_buttonup)
        self.bind("b",frame.s_blue.invoke_buttondown)
        self.bind("+",frame.s_led.invoke_buttonup)
        self.bind("-",frame.s_led.invoke_buttondown)
        self.bind("A",frame.s_ledcount.invoke_buttonup)
        self.bind("a",frame.s_ledcount.invoke_buttondown)
        self.bind("T",frame.s_ct.invoke_buttonup)
        self.bind("t",frame.s_ct.invoke_buttondown)
        self.show_frame(ColorCheckPage)

# ----------------------------------------------------------------
# Class ConfigurationPage
# ----------------------------------------------------------------

class ConfigurationPage(tk.Frame):

    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)


        title_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        title_frame.columnconfigure(0, weight=1)

        label = ttk.Label(title_frame, text="Konfiguration", font=LARGE_FONT)
        label.pack(padx=50,pady=(10,10))


        cfg_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        cfg_frame.columnconfigure(0, weight=1)

        label2text = tk.Label(cfg_frame, text="ARDUINO Port:")
        label2text.pack(side="left", padx=10, pady=(10,10))

        self.combo = ttk.Combobox(cfg_frame)
        self.combo["value"] = ("NO DEVICE", "COM1",  "COM2",  "COM3",  "COM4",  "COM5",  "COM6",  "COM7",  "COM8",  "COM9",  "COM10",
                                            "COM11", "COM12", "COM13", "COM14", "COM15", "COM16", "COM17", "COM18", "COM19", "COM20",
                                            "COM21", "COM22", "COM23", "COM24", "COM25", "COM26", "COM27", "COM28", "COM29", "COM30",
                                            "COM31", "COM32", "COM33", "COM34", "COM35", "COM36", "COM37", "COM38", "COM39", "COM40")   # 04.12.19: Added com ports 7..

        self.combo.current(sdConfig.serportnumber) #set the selected item

        self.combo.pack(side="right", padx=10, pady=(10,10) )

        view_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        view_frame.columnconfigure(0, weight=1)

        label3text = tk.Label(view_frame, text="Farbauswahlansicht (Neustart notwendig):")
        label3text.pack(side="left", padx=10, pady=(10,10))

        self.comboview = ttk.Combobox(view_frame)
        self.comboview["value"] = ("Farbton, Sättigung, Helligkeit", "Farbrad und Helligkeit")
        self.comboview.current(sdConfig.colorview) #set the selected colorview

#        self.comboview.pack(side="right", padx=10, pady=(10,10))

        page_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        page_frame.columnconfigure(0, weight=1)

        label4text = tk.Label(page_frame, text="Startseite:")
        label4text.pack(side="left", padx=10, pady=(10,10))

        self.combopage = ttk.Combobox(page_frame)
        self.combopage["value"] = ("Konfiguration", "LED Farbauswahl", "Serieller Monitor")   # 04.12.19: Added: "Serieller Monitor"
        self.combopage.current(sdConfig.startpage) #set the selected colorview

        self.combopage.pack(side="right", padx=10, pady=(10,10))


        # --- LED MaxCount

        maxcount_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        maxcount_frame.columnconfigure(0, weight=1)

        self.ledmaxcount = LimitVar(1, 256, self)

        self.ledmaxcount.set(sdConfig.maxLEDcount)

        self.s_ledmaxcount = Spinbox(maxcount_frame, from_=1, to=256, width=5, name='spinbox',
                          textvariable=self.ledmaxcount, command=self._update_led_count)
        self.s_ledmaxcount.delete(0, 'end')
        self.s_ledmaxcount.insert(1, sdConfig.maxLEDcount)
        self.s_ledmaxcount.pack(side="right", padx=10, pady=(10,10))


        ttk.Label(maxcount_frame, text=_('Maximale LED Anzahl')).pack(side="left", padx=10, pady=(10,10))



        # --- Buttons
        button_frame = ttk.Frame(self)
        ttk.Button(button_frame, text="Zur LED Farbauswahlseite",
                            command=controller.switch_to_color_check).pack(side="right", padx=10)
        ttk.Button(button_frame, text="Beenden",
                            command=controller.cancel).pack(side="right", padx=10)
        ttk.Button(button_frame, text=_("Konfiguration Speichern"), command=self.save_config).pack(side="right", padx=10)

        ttk.Button(button_frame, text=_("Hilfe"),
                            command=controller.call_helppage).pack(side="right", padx=10)


        # --- autoconnect checkbox
        autocn_frame = ttk.Frame(self, relief="ridge", borderwidth=2)
        autocn_frame.columnconfigure(0, weight=1)
        self.s_autocnvar = tk.IntVar()
        self.s_autocn = ttk.Checkbutton(autocn_frame,text=_("Automatisch verbinden"),variable=self.s_autocnvar,onvalue = 1, offvalue = 0, command=self.autocn)
        self.s_autocn.grid(sticky='w', padx=4, pady=4, row=0,column=0)
        self.s_autocnvar.set(sdConfig.autoconnect)

        # --- placement
        title_frame.grid(row=1, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="new")
        cfg_frame.grid(row=2, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="new")

        #view_frame.grid(row=5, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="new")
        page_frame.grid(row=6, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="new")
        maxcount_frame.grid(row=7, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="new")
        autocn_frame.grid(row=8, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="new")

        button_frame.grid(row=9, columnspan=2, pady=(20, 30), padx=10)

    # ----------------------------------------------------------------
    # ConfigurationPage save_config
    # ----------------------------------------------------------------

    def save_config(self):

        sdConfig.pos_x = self.winfo_x()
        sdConfig.pos_y = self.winfo_y()
        sdConfig.serportnumber= self.combo.current()
        sdConfig.serportname= self.combo.get()
        sdConfig.maxLEDcount = self.s_ledmaxcount.get()
        sdConfig.colorview = self.comboview.current()
        sdConfig.startpage = self.combopage.current()
        sdConfig.save()

    # ----------------------------------------------------------------
    # ConfigurationPage _update_led_count
    # ----------------------------------------------------------------

    def _update_led_count(self, event=None):
        if event is None or event.widget.old_value != event.widget.get():
            maxledcount = self.s_ledmaxcount.get()

    def autocn(self,event=None):
        if self.s_autocnvar.get() == 1:
            sdConfig.autoconnect = True
        else:
            sdConfig.autoconnect = False




# ----------------------------------------------------------------
# Class ColorCheckPage
# ----------------------------------------------------------------

class ColorCheckPage(tk.Frame):

    # ----------------------------------------------------------------
    # ColorCheckPage __init__
    # ----------------------------------------------------------------

    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
#        label = ttk.Label(self, text="Page One!!!", font=LARGE_FONT)
#        label.pack(pady=10,padx=10)

        color = sdConfig.old_color
        self.controller = controller
        self.parent = parent

        if isinstance(color, str):
            if re.match(r"^#[0-9A-F]{8}$", color.upper()):
                col = hexa_to_rgb(color)
                self._old_color = col[:3]
                old_color = color[:7]
            elif re.match(r"^#[0-9A-F]{6}$", color.upper()):
                self._old_color = hexa_to_rgb(color)
                old_color = color
            else:
                col = self.winfo_rgb(color)
                self._old_color = tuple(round2(c * 255 / 65535) for c in col)
                args = self._old_color
                old_color = rgb_to_hexa(*args)
        else:
            self._old_color = color[:3]
            old_color = rgb_to_hexa(*color)

        led = sdConfig.lastLed

        ledcount = sdConfig.lastLedCount

        serport = controller.arduino

        self.cor_red = sdConfig.led_correction_r
        self.cor_green = sdConfig.led_correction_g
        self.cor_blue = sdConfig.led_correction_b

        title=_("MobaLedLib LED Farbentester " + VERSION)

        # --- Buttons
        button_frame = ttk.Frame(self)

        button_width = 15
        ttk.Button(button_frame, text=_("Einstellungen\nvornehmen"),width=button_width, command=lambda: controller.show_frame(ConfigurationPage)).grid(row=0, column=0, padx=10, pady=(10, 4), sticky='n')
        ttk.Button(button_frame, text=_("Programm\nbeenden"),width=button_width, command=self.cancel).grid(row=0, column=4, padx=10, pady=(10, 4), sticky='n')
        ttk.Button(button_frame, text=_("Serial\nMonitor"), width=button_width,command=controller.switch_to_serial_mon).grid(row=0, column=1, padx=10, pady=(10, 4), sticky='n')
        #ttk.Button(button_frame, text=_("Hilfe\n"), command=lambda: webbrowser.open_new_tab(HELPPAGE_FILENAME)).grid(row=0, column=3, padx=10, pady=(10, 4), sticky='n')
        ttk.Button(button_frame, text=_("Hilfe\n"), width=button_width,command=controller.call_helppage).grid(row=0, column=2, padx=10, pady=(10, 4), sticky='n')
        ttk.Button(button_frame, text=_("Farben\nzurücksetzen"), width=button_width, command=self._palette_reset_colors).grid(row=0, column=3, padx=10, pady=(10, 4), sticky='n')

        self.main_frame = ttk.Frame(self)
        self.main_frame.columnconfigure(1, weight=1)
        self.main_frame.rowconfigure(1, weight=1)

        # --- Colorwheel or Colorquare ?
        if sdConfig.colorview == 0:
            # --- GradientBar
            hue = col2hue(*self._old_color)
            bar = ttk.Frame(self, borderwidth=2, relief='groove')
            self.bar = GradientBar(bar, hue=hue, width=200, highlightthickness=0)
            self.bar.pack()

            # --- ColorSquare
            square = ttk.Frame(self, borderwidth=2, relief='groove')
            self.square = ColorSquare(square, hue=hue, width=200, height=200,
                                      color=rgb_to_hsv(*self._old_color),
                                      highlightthickness=0)
            self.square.pack()


        else:
            # --- LightGradientBar
            hue = 0
            h,s,v=rgb_to_hsv(*self._old_color)
            bar = ttk.Frame(self, borderwidth=2, relief='groove')
            self.bar = LightGradientBar(bar, hue=h, value=v, width=200, highlightthickness=0)
            self.bar.pack()

            # --- ColorWheel
            square = ttk.Frame(self, borderwidth=2, relief='groove')
            self.square = ColorWheel(square, hue=hue, width=200, height=200,
                                      color=rgb_to_hsv(*self._old_color),
                                      highlightthickness=0, cr = int(sdConfig.led_correction_r), cg=int(sdConfig.led_correction_g), cb=int(sdConfig.led_correction_b))
            self.square.pack()


        # --- color preview: initial color and currently selected color side by side
        preview_frame = ttk.Frame(self.main_frame, relief="raised", borderwidth=1)
        preview_frame.grid(row=0, column=0, sticky="nw", pady=2)
        old_color_disp, brightness_text = self.keycolor_to_dispcolor(old_color[:7])
        self.old_color_prev = tk.Label(preview_frame, background=old_color_disp,
                                  width=12, highlightthickness=0, height=2,
                                  padx=2, pady=2,text=_("alte Farbe")+"\n"+brightness_text)
        self.color_preview = tk.Label(preview_frame, width=12, height=2,
                                      pady=2, background=old_color_disp,
                                      padx=2, highlightthickness=0, text=_("aktuelle Farbe")+"\n"+brightness_text)
        self.old_color_prev.bind("<1>", self._reset_preview)
        self.old_color_prev.grid(row=0, column=0)
        self.color_preview.grid(row=1, column=0)

        # --- palette
        self.palette = sdConfig.palette
        self.palette_frame = ttk.Frame(self.main_frame)
        self.palette_frame.grid(row=0, column=1, rowspan=2, sticky="ne")
        for i, key in enumerate(self.palette):
            keycolor = self.palette[key]
            color_disp,brightness_text = self.keycolor_to_dispcolor(keycolor)
            fontcolor = "#000000"
            text = key + "\n" + brightness_text
            f = ttk.Frame(self.palette_frame, borderwidth=1, relief="raised",
                          style="palette.TFrame")
            l = tk.Label(f, background=color_disp, width=12, height=2,text=text,fg=fontcolor)
            l.bind("<Button-1>", self._palette_cmd)
            l.bind("<Button-3>", self._palette_save_cmd)
            f.bind("<FocusOut>", lambda e: e.widget.configure(relief="raised"))
            l.pack()
            f.grid(row=i % 4, column=i // 4, padx=2, pady=2)

        # --- hsv

        col_frame = ttk.Frame(self)

        hsv_frame = ttk.Frame(col_frame, relief="ridge", borderwidth=2)
        hsv_frame.pack(pady=(0, 4), fill="x")
        hsv_frame.columnconfigure(0, weight=1)
        self.hue = LimitVar(0, 360, self)
        self.saturation = LimitVar(0, 100, self)
        self.value = LimitVar(0, 100, self)

        self.s_h = Spinbox(hsv_frame, from_=0, to=360, width=5, name='spinbox',
                      textvariable=self.hue, command=self._update_color_hsv)
        self.s_s = Spinbox(hsv_frame, from_=0, to=100, width=5,
                      textvariable=self.saturation, name='spinbox',
                      command=self._update_color_hsv)
        self.s_v = Spinbox(hsv_frame, from_=0, to=100, width=5, name='spinbox',
                      textvariable=self.value, command=self._update_color_hsv)
        h, s, v = rgb_to_hsv(*self._old_color)
        self.s_h.delete(0, 'end')
        self.s_h.insert(0, h)
        self.s_s.delete(0, 'end')
        self.s_s.insert(0, s)
        self.s_v.delete(0, 'end')
        self.s_v.insert(0, v)
        self.s_h.grid(row=0, column=1, sticky='w', padx=4, pady=4)
        self.s_s.grid(row=1, column=1, sticky='w', padx=4, pady=4)
        self.s_v.grid(row=2, column=1, sticky='w', padx=4, pady=4)
        ttk.Label(hsv_frame, text=_('Farbton [F/f]')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(hsv_frame, text=_('Sättigung [S/s]')).grid(row=1, column=0, sticky='e',
                                                        padx=4, pady=4)
        ttk.Label(hsv_frame, text=_('Helligkeit [H/h]')).grid(row=2, column=0, sticky='e',
                                                   padx=4, pady=4)

        # --- rgb
        rgb_frame = ttk.Frame(col_frame, relief="ridge", borderwidth=2)
        rgb_frame.pack(pady=4, fill="x")
        rgb_frame.columnconfigure(0, weight=1)
        self.red = LimitVar(0, 255, self)
        self.green = LimitVar(0, 255, self)
        self.blue = LimitVar(0, 255, self)

        self.s_red = Spinbox(rgb_frame, from_=0, to=255, width=5, name='spinbox',
                        textvariable=self.red, command=self._update_color_rgb)
        self.s_green = Spinbox(rgb_frame, from_=0, to=255, width=5, name='spinbox',
                          textvariable=self.green, command=self._update_color_rgb)
        self.s_blue = Spinbox(rgb_frame, from_=0, to=255, width=5, name='spinbox',
                         textvariable=self.blue, command=self._update_color_rgb)
        self.s_red.delete(0, 'end')
        self.s_red.insert(0, self._old_color[0])
        self.s_green.delete(0, 'end')
        self.s_green.insert(0, self._old_color[1])
        self.s_blue.delete(0, 'end')
        self.s_blue.insert(0, self._old_color[2])
        self.s_red.grid(row=0, column=1, sticky='e', padx=4, pady=4)
        self.s_green.grid(row=1, column=1, sticky='e', padx=4, pady=4)
        self.s_blue.grid(row=2, column=1, sticky='e', padx=4, pady=4)
        ttk.Label(rgb_frame, text=_('Rot (LED 1)[R/r]')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(rgb_frame, text=_('Grün (LED2) [G/g]')).grid(row=1, column=0, sticky='e',
                                                   padx=4, pady=4)
        ttk.Label(rgb_frame, text=_('Blau (LED3) [B/b]')).grid(row=2, column=0, sticky='e',
                                                  padx=4, pady=4)

        # --- hexa
        hexa_frame = ttk.Frame(col_frame)
        hexa_frame.pack(fill="x")
        self.hexa = ttk.Entry(hexa_frame, justify="center", width=10, name='entry')
        self.hexa.insert(0, old_color.upper())
        ttk.Label(hexa_frame, text="HEXA").pack(side="left", padx=4, pady=(4, 1))
        self.hexa.pack(side="left", padx=6, pady=(4, 1), fill='x', expand=True)


        # --- Colortemperature
        ct_frame = ttk.Frame(col_frame, relief="ridge", borderwidth=2)
        ct_frame.pack(pady=4, fill="x")
        ct_frame.columnconfigure(0, weight=1)
        ct_min = 1000
        ct_max = 20000
        self.ct = LimitVar(ct_min, ct_max, self)

        self.s_ct = Spinbox(ct_frame, from_=ct_min, to=ct_max, width=5, name='spinbox',
                        textvariable=self.ct, command=self._update_ct, increment = 25)
        self.s_ct.delete(0, 'end')
        self.s_ct.insert(0, ct_min)
        self.s_ct.grid(row=0, column=1, sticky='e', padx=4, pady=4)

        ttk.Label(ct_frame, text=_('Farbtemperature (K) [T/t]')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        # --- ARDUINO Steuerung

        arduino_frame = ttk.Frame(self,relief="ridge", borderwidth=2,width=500)
#        ttk.Label(arduino_frame, text=_('ARDUINO Steuerung')).grid(row=0, column=0, columnspan=3, sticky='', padx=4, pady=4)
        self.arduino_status = tk.Label(arduino_frame, text=_('Nicht verbunden'), bg="lightgray",width=20)
        self.arduino_status.grid(row=0, column=2, sticky='', padx=4, pady=(10, 4))

        # --- ARDUINO LED
        led_frame = ttk.Frame(arduino_frame, relief="ridge", borderwidth=2)
        led_frame.columnconfigure(0, weight=1)

        self.lednum = LimitVar(0, 255, self)
        self.ledcount = LimitVar(1, 256, self)

        self.lednum.set(sdConfig.lastLed)
        self.ledcount.set(sdConfig.lastLedCount)

        self.s_led = Spinbox(led_frame, from_=0, to=255, width=5, name='spinbox',
                        textvariable=self.lednum, command=self._update_led_num)
        self.s_ledcount = Spinbox(led_frame, from_=1, to=256, width=5, name='spinbox',
                          textvariable=self.ledcount, command=self._update_led_count)
        self.s_led.delete(0, 'end')
        self.s_led.insert(0, led)
        self.s_led.grid(row=0, column=1, sticky='e', padx=4, pady=4)
        self.s_ledcount.delete(0, 'end')
        self.s_ledcount.insert(1, ledcount)
        self.s_ledcount.grid(row=0, column=3, sticky='e', padx=4, pady=4)

        ttk.Label(led_frame, text=_('LED Adresse [+/-]')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(led_frame, text=_('Anzahl [A/a]')).grid(row=0, column=2, sticky='e',
                                                 padx=4, pady=4)


        button_width = 15
        ttk.Button(arduino_frame, text=_("Verbinden"),width=button_width, command=self.connect).grid(row=0, column=0, padx=10, pady=(10, 4), sticky='')
        ttk.Button(arduino_frame, text=_("Trennen"),width=button_width, command=self.disconnect).grid(row=0, column=1, padx=10, pady=(10, 4), sticky='')

        led_frame.grid(row=1,column=0,columnspan=2,padx=10,pady=(10, 4),sticky="w")
        ttk.Button(arduino_frame, text=_("Alle LED aus [CTRL-c]"),width=20, command=self.led_off).grid(row=1, column=2, padx=10, pady=(10, 4), sticky='')


        # --- LED Color correction
        ledcor_frame = ttk.Frame(arduino_frame, relief="ridge", borderwidth=2)
        ledcor_frame.columnconfigure(0, weight=1)
        #self.s_ledcorvar = tk.IntVar()
        #self.s_ledcor = ttk.Checkbutton(ledcor_frame,text=_("LED Farbstichkorrektur %"),variable=self.s_ledcorvar,onvalue = 1, offvalue = 0, command=self.cb)
        self.s_ledcor = ttk.Label(ledcor_frame,text=_("LED Farbstichkorrektur %"))
        self.s_ledcor.grid(sticky='w', padx=4, pady=4, row=0,column=0)
        #self.s_ledcorvar.set(sdConfig.use_led_correction)

        ledcor_frame.grid(row=5,column=0,columnspan=3,padx=10,pady=(10, 4),sticky="w")

        rgb_frame = ttk.Frame(ledcor_frame, relief="ridge", borderwidth=2)
        rgb_frame.grid(row=0,column=1,columnspan=3,padx=10,pady=(10, 4),sticky="w")
        rgb_frame.columnconfigure(0, weight=1)
        self.c_red = LimitVar(0, 100, self)
        self.c_green = LimitVar(0, 100, self)
        self.c_blue = LimitVar(0, 100, self)

        self.cs_red = Spinbox(rgb_frame, from_=0, to=100, width=5, name='spinbox',
                        textvariable=self.c_red, command=self._update_cor_rgb)
        self.cs_green = Spinbox(rgb_frame, from_=0, to=100, width=5, name='spinbox',
                          textvariable=self.c_green, command=self._update_cor_rgb)
        self.cs_blue = Spinbox(rgb_frame, from_=0, to=100, width=5, name='spinbox',
                         textvariable=self.c_blue, command=self._update_cor_rgb)
        self.cs_red.delete(0, 'end')
        self.cs_red.insert(0, sdConfig.led_correction_r)
        self.cs_green.delete(0, 'end')
        self.cs_green.insert(0, sdConfig.led_correction_g)
        self.cs_blue.delete(0, 'end')
        self.cs_blue.insert(0, sdConfig.led_correction_b)
        self.cs_red.grid(row=0, column=1, sticky='e', padx=4, pady=4)
        self.cs_green.grid(row=0, column=3, sticky='e', padx=4, pady=4)
        self.cs_blue.grid(row=0, column=5, sticky='e', padx=4, pady=4)

        ttk.Label(rgb_frame, text=_('Rot')).grid(row=0, column=0, sticky='e',
                                                 padx=4, pady=4)
        ttk.Label(rgb_frame, text=_('Grün')).grid(row=0, column=2, sticky='e',
                                                   padx=0, pady=4)
        ttk.Label(rgb_frame, text=_('Blau')).grid(row=0, column=4, sticky='e',
                                                  padx=4, pady=4)

        ttk.Label(ledcor_frame, text=_('ARDUINO Message:')).grid(row=4, column=0, columnspan=1, sticky='w', rowspan=2, padx=4, pady=4)
        self.s_ledcorvalue = ttk.Label(ledcor_frame, text=_(' '))
        self.s_ledcorvalue.grid(row=4, column=2, columnspan=1, sticky='w', padx=4, pady=4)
        self.ledcorvalue = " "


        # --- placement
        bar.grid(row=0, column=0, padx=10, pady=(10, 4), sticky='n')
        square.grid(row=1, column=0, padx=10, pady=(9, 0), sticky='n')
        col_frame.grid(row=0, rowspan=2, column=1, padx=(4, 10), pady=(10, 4))
        self.main_frame.grid(row=3, column=0, columnspan=2, pady=(4, 10), padx=10, sticky="nsew")
        arduino_frame.grid(row=4, column=0, columnspan=2,pady=(0, 10), padx=10)
        button_frame.grid(row=5, column=0, columnspan=2,pady=(0, 10), padx=10)

        # --- bindings
        self.bar.bind("<ButtonRelease-1>", self._change_color, True)
        self.bar.bind("<B1-Motion>", self._change_color, True)
        self.bar.bind("<Button-1>", self._unfocus, True)
        self.square.bind("<Button-1>", self._unfocus, True)
        self.square.bind("<ButtonRelease-1>", self._change_sel_color, True)
        self.square.bind("<B1-Motion>", self._change_sel_color, True)
        self.s_red.bind('<FocusOut>', self._update_color_rgb)
        self.s_green.bind('<FocusOut>', self._update_color_rgb)
        self.s_blue.bind('<FocusOut>', self._update_color_rgb)
        self.s_red.bind('<Return>', self._update_color_rgb)
        self.s_green.bind('<Return>', self._update_color_rgb)
        self.s_blue.bind('<Return>', self._update_color_rgb)
        self.s_red.bind('<Control-a>', self._select_all_spinbox)
        self.s_green.bind('<Control-a>', self._select_all_spinbox)
        self.s_blue.bind('<Control-a>', self._select_all_spinbox)
        self.cs_red.bind('<FocusOut>', self._update_cor_rgb)
        self.cs_green.bind('<FocusOut>', self._update_cor_rgb)
        self.cs_blue.bind('<FocusOut>', self._update_cor_rgb)
        self.cs_red.bind('<Return>', self._update_cor_rgb)
        self.cs_green.bind('<Return>', self._update_cor_rgb)
        self.cs_blue.bind('<Return>', self._update_cor_rgb)
        self.cs_red.bind('<Control-a>', self._select_all_spinbox)
        self.cs_green.bind('<Control-a>', self._select_all_spinbox)
        self.cs_blue.bind('<Control-a>', self._select_all_spinbox)
        self.s_led.bind('<Control-a>', self._select_all_spinbox)
        self.s_ledcount.bind('<Control-a>', self._select_all_spinbox)
        self.s_h.bind('<FocusOut>', self._update_color_hsv)
        self.s_s.bind('<FocusOut>', self._update_color_hsv)
        self.s_v.bind('<FocusOut>', self._update_color_hsv)
        self.s_h.bind('<Return>', self._update_color_hsv)
        self.s_s.bind('<Return>', self._update_color_hsv)
        self.s_v.bind('<Return>', self._update_color_hsv)
        self.s_h.bind('<Control-a>', self._select_all_spinbox)
        self.s_s.bind('<Control-a>', self._select_all_spinbox)
        self.s_v.bind('<Control-a>', self._select_all_spinbox)
        self.s_ct.bind("<FocusOut>", self._update_ct)
        self.s_ct.bind("<Return>", self._update_ct)
        self.s_ct.bind("<Control-a>", self._select_all_entry)
        self.hexa.bind("<FocusOut>", self._update_color_hexa)
        self.hexa.bind("<Return>", self._update_color_hexa)
        self.hexa.bind("<Control-a>", self._select_all_entry)

        self.controller.bind("F",self.s_h.invoke_buttonup)
        self.controller.bind("f",self.s_h.invoke_buttondown)
        self.controller.bind("S",self.s_s.invoke_buttonup)
        self.controller.bind("s",self.s_s.invoke_buttondown)
        self.controller.bind("H",self.s_v.invoke_buttonup)
        self.controller.bind("h",self.s_v.invoke_buttondown)
        self.controller.bind("R",self.s_red.invoke_buttonup)
        self.controller.bind("r",self.s_red.invoke_buttondown)
        self.controller.bind("G",self.s_green.invoke_buttonup)
        self.controller.bind("g",self.s_green.invoke_buttondown)
        self.controller.bind("B",self.s_blue.invoke_buttonup)
        self.controller.bind("b",self.s_blue.invoke_buttondown)
        self.controller.bind("+",self.s_led.invoke_buttonup)
        self.controller.bind("-",self.s_led.invoke_buttondown)
        self.controller.bind("A",self.s_ledcount.invoke_buttonup)
        self.controller.bind("a",self.s_ledcount.invoke_buttondown)
        self.controller.bind("T",self.s_ct.invoke_buttonup)
        self.controller.bind("t",self.s_ct.invoke_buttondown)
        self.controller.bind("<F1>",self.controller.call_helppage)
        #self.bind("<Escape>",self.cancel)
        self.controller.bind("<Control-c>",self.led_off)

        self._update_color_hsv()

        self.onCheckDisconnectFile()
        #        self.focus_set()
        #        self.lift()
        #        self.grab_set()

        if sdConfig.autoconnect:
            self.connect()



    def cb(self):

        self._update_cor_rgb()

        #self._update_preview()

    def connect(self):
        if self.controller.connect():
            self.arduino_status.configure(bg="green",text=_("Verbunden"))

        max_len = 40
        if len(self.controller.ARDUINO_message) > max_len:
            message = self.controller.ARDUINO_message[:max_len]
        else:
            message = self.controller.ARDUINO_message

        self.s_ledcorvalue.configure(text=message)


    def disconnect(self):
        self.controller.disconnect()
        self.arduino_status.configure(bg="lightgray",text=_("Nicht Verbunden"))

    def set_ARDUINO_message(self,readtext):
        self.s_ledcorvalue.configure(text=readtext)


    def get_color(self):
        """Return selected color, return an empty string if no color is selected."""
        return self.color


    def _correct_rgb_disp(self, r, g, b):
        crf = 100/int(self.cor_red)
        cgf = 100/int(self.cor_green)
        cbf = 100/int(self.cor_blue)
        rcor = int(r*crf)
        gcor = int(g*cgf)
        bcor = int(b*cbf)
        if rcor > 255: rcor = 255
        if gcor > 255: gcor = 255
        if bcor > 255: bcor = 255
        return rcor, gcor, bcor

    def keycolor_to_dispcolor(self, keycolor):
        r,g,b = hexa_to_rgb(keycolor)
        args = (r,g,b)
        h,s,v = rgb_to_hsv(*args)
        r2,g2,b2 = self._correct_rgb_disp(*args)
        #---  palette colors will be displayed with brightness = 100
        args2 = (r2,g2,b2)
        h2,s2,v2 = rgb_to_hsv(*args2)
        r3,g3,b3 = hsv_to_rgb(h2,s2,100)
        args3 = (r3,g3,b3)
        disp_color = rgb_to_hexa(*args3)
        if PERCENT_BRIGHTNESS:                                           # 03.12.19:
            brightness = str(v) + " %"
        else:
            brightness = ""
            v3 = v/10
            for j in range(10):
                if j<v3:
                    brightness = brightness + ">"

        return disp_color,brightness


    @staticmethod
    def _select_all_spinbox(event):
        """Select all entry content."""
        event.widget.selection('range', 0, 'end')
        return "break"

    @staticmethod
    def _select_all_entry(event):
        """Select all entry content."""
        event.widget.selection_range(0, 'end')
        return "break"

    def _unfocus(self, event):
        """Unfocus palette items when click on bar or square."""
        w = self.focus_get()
        if w != self and 'spinbox' not in str(w) and 'entry' not in str(w):
            self.focus_set()

    def _update_preview(self):
        """Update color preview."""
        red=self.red.get()
        green= self.green.get()
        blue = self.blue.get()
        args = (red,green,blue)
        color = rgb_to_hexa(*args)
        color_disp,brightness_text = self.keycolor_to_dispcolor(color)
        self.color_preview.configure(background=color_disp)
        self.color_preview.configure(text="aktuelle Farbe\n"+brightness_text)
        ledcount = self.ledcount.get()
        if ledcount >0:
            lednum = self.lednum.get()
            message = "#L" + '{:02x}'.format(self.lednum.get()) + " " + '{:02x}'.format(red) + " " + '{:02x}'.format(green) + " " + '{:02x}'.format(blue) + " " + '{:02x}'.format(self.ledcount.get()) + "\n"
            #self.ledcorvalue = "("+str(red)+","+str(green)+","+str(blue)+")  "+ "#"+ '{:02x}'.format(red) + '{:02x}'.format(green) + '{:02x}'.format(blue)
            #self.s_ledcorvalue.configure(text=self.ledcorvalue.upper())
            self.controller.send_to_ARDUINO(message)

    def _reset_preview(self, event):
        """Respond to user click on old_color item."""
        current_color = self.hexa.get()
        label = event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        args = self._old_color
        color = rgb_to_hexa(*args)
        h, s, v = rgb_to_hsv(*self._old_color)
        self.red.set(self._old_color[0])
        self.green.set(self._old_color[1])
        self.blue.set(self._old_color[2])
        self.hue.set(h)
        self.saturation.set(s)
        self.value.set(v)
        self.hexa.delete(0, "end")
        self.hexa.insert(0, color.upper())
        self.bar.set(h, s, v)
        self.square.set_hsv((h, s, v))
        old_color_disp, brightness_text = self.keycolor_to_dispcolor(current_color)
        self.old_color_prev.configure(background=old_color_disp)
        self.old_color_prev.configure(text="alte Farbe\n"+brightness_text)
        self._old_color = hexa_to_rgb(current_color)
        self._update_preview()

    def _palette_cmd(self, event):
        """Respond to user click on a palette item."""
        label = event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        text = event.widget["text"]
        palettekey,dummy = text.split("\n")
        keycolor = self.palette[palettekey]
        r,g,b = hexa_to_rgb(keycolor)
        self.ct.set(0)
        args = (r, g, b)
        color = rgb_to_hexa(*args)
        h, s, v = rgb_to_hsv(r, g, b)
        self.red.set(r)
        self.green.set(g)
        self.blue.set(b)
        self.hue.set(h)
        self.saturation.set(s)
        self.value.set(v)
        self.hexa.delete(0, "end")
        self.hexa.insert(0, color.upper())
        self.bar.set(h,s,v)
        self.square.set_hsv((h, s, v))
        self._update_preview()

    def _palette_save_cmd(self, event):
        """Respond to user right click on a palette item."""

        label = event.widget
        label.master.focus_set()
        label.master.configure(relief="sunken")
        r=self.red.get()
        g= self.green.get()
        b = self.blue.get()
        args = (r,g,b)
        color = rgb_to_hexa(*args)
        old_text = event.widget["text"]
        linebreak=old_text.find("\n")
        key = old_text[:linebreak]
        old_color = self.palette[key]

        color_disp,brightness_text = self.keycolor_to_dispcolor(color)

        fontcolor = "#000000"
        event.widget["foreground"] = fontcolor
        event.widget["background"] = color_disp

        new_text = key + "\n "+ brightness_text #color
        event.widget["text"] = new_text

        old_color_disp, brigthness_text = self.keycolor_to_dispcolor(old_color)

        self.old_color_prev.configure(background=old_color_disp)
        self.old_color_prev.configure(text="alte Farbe\n"+brigthness_text)
        self._old_color = hexa_to_rgb(old_color)
        self.palette[key] = color
        self._update_preview()

    def _palette_redraw_colors(self):

        self.palette_frame.destroy()
        self.palette_frame = ttk.Frame(self.main_frame)
        self.palette_frame.grid(row=0, column=1, rowspan=2, sticky="ne")
        for i, key in enumerate(self.palette):
            keycolor = self.palette[key]
            color_disp,brightness_text = self.keycolor_to_dispcolor(keycolor)
            fontcolor = "#000000"
            text = key + "\n" + brightness_text
            f = ttk.Frame(self.palette_frame, borderwidth=1, relief="raised",
                          style="palette.TFrame")
            l = tk.Label(f, background=color_disp, width=12, height=2,text=text,fg=fontcolor)
            l.bind("<Button-1>", self._palette_cmd)
            l.bind("<Button-3>", self._palette_save_cmd)
            f.bind("<FocusOut>", lambda e: e.widget.configure(relief="raised"))
            l.pack()
            f.grid(row=i % 4, column=i // 4, padx=2, pady=2)

    def _palette_reset_colors(self):

        self.palette = PALETTE
        self._palette_redraw_colors()



    def _change_sel_color(self, event):
        """Respond to motion of the color selection cross."""
        (r, g, b), (h, s, v), color = self.square.get()
        self.red.set(r)
        self.green.set(g)
        self.blue.set(b)
        self.saturation.set(s)
        self.hue.set(h)
        self.value.set(v)
        self.hexa.delete(0, "end")
        self.hexa.insert(0, color.upper())
        self._update_preview()

    def _change_color(self, event):
        """Respond to motion of the hsv/brightness cursor."""
        hv = self.bar.get()
        self.square.set_hue(hv)
        (r, g, b), (h, s, v), sel_color = self.square.get()
        self.red.set(r)
        self.green.set(g)
        self.blue.set(b)
        self.hue.set(h)
        self.saturation.set(s)
        self.value.set(v)
        self.hexa.delete(0, "end")
        self.hexa.insert(0, sel_color.upper())
        self._update_preview()

    def _change_alpha(self, event):
        """Respond to motion of the alpha cursor."""
        a = self.alphabar.get()
        self.alpha.set(a)
        hexa = self.hexa.get()
        hexa = hexa[:7] + ("%2.2x" % a).upper()
        self.hexa.delete(0, 'end')
        self.hexa.insert(0, hexa)
        self._update_preview()

    def _update_color_hexa(self, event=None):
        """Update display after a change in the HEX entry."""
        color = self.hexa.get().upper()
        self.hexa.delete(0, 'end')
        self.hexa.insert(0, color)
        if re.match(r"^#[0-9A-F]{6}$", color):
            r, g, b = hexa_to_rgb(color)
            self.red.set(r)
            self.green.set(g)
            self.blue.set(b)
            h, s, v = rgb_to_hsv(r, g, b)
            self.hue.set(h)
            self.saturation.set(s)
            self.value.set(v)
            self.bar.set(h,s,v)
            self.square.set_hsv((h, s, v))
        else:
            self._update_color_rgb()
        self._update_preview()

    def _update_color_hsv(self, event=None):
        """Update display after a change in the HSV spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            h = self.hue.get()
            s = self.saturation.get()
            v = self.value.get()
            sel_color = hsv_to_rgb(h, s, v)
            self.red.set(sel_color[0])
            self.green.set(sel_color[1])
            self.blue.set(sel_color[2])
            hexa = rgb_to_hexa(*sel_color)
            self.hexa.delete(0, "end")
            self.hexa.insert(0, hexa)
            self.square.set_hsv((h, s, v))
            self.bar.set(h,s,v)
            self._update_preview()

    def _update_color_rgb(self, event=None):
        """Update display after a change in the RGB spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            r = self.red.get()
            g = self.green.get()
            b = self.blue.get()
            h, s, v = rgb_to_hsv(r, g, b)
            self.hue.set(h)
            self.saturation.set(s)
            self.value.set(v)
            args = (r, g, b)
            hexa = rgb_to_hexa(*args)
            self.hexa.delete(0, "end")
            self.hexa.insert(0, hexa)
            self.square.set_hsv((h, s, v))
            self.bar.set(h,s,v)
            self._update_preview()

    def _update_cor_rgb(self, event=None):
        """Update display after a change in the RGB spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            r = self.cs_red.get()
            g = self.cs_green.get()
            b = self.cs_blue.get()
            sdConfig.led_correction_r=r
            sdConfig.led_correction_g=g
            sdConfig.led_correction_b=b
            self.cor_red = r
            self.cor_green = g
            self.cor_blue = b
            self._update_preview()
            self.square.set_colorcorrection(r,g,b)
            self._palette_redraw_colors()


    def _update_led_num(self, event=None):
        """Update display after a change in the LED spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            led = self.lednum.get()
            self._update_preview()

    def _update_led_count(self, event=None):
        """Update display after a change in the LED count spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            ledcount = self.ledcount.get()
            self._update_preview()

    def _convert_K_to_RGB(self, colour_temperature):
        """
        Converts from K to RGB, algorithm courtesy of
        http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
        """
        #range check
        if colour_temperature < 1000:
            colour_temperature = 1000
        elif colour_temperature > 40000:
            colour_temperature = 40000

        tmp_internal = colour_temperature / 100.0

        # red
        if tmp_internal <= 66:
            red = 255
        else:
            tmp_red = 329.698727446 * math.pow(tmp_internal - 60, -0.1332047592)
            if tmp_red < 0:
                red = 0
            elif tmp_red > 255:
                red = 255
            else:
                red = int(tmp_red)

        # green
        if tmp_internal <=66:
            tmp_green = 99.4708025861 * math.log(tmp_internal) - 161.1195681661
            if tmp_green < 0:
                green = 0
            elif tmp_green > 255:
                green = 255
            else:
                green = int(tmp_green)
        else:
            tmp_green = 288.1221695283 * math.pow(tmp_internal - 60, -0.0755148492)
            if tmp_green < 0:
                green = 0
            elif tmp_green > 255:
                green = 255
            else:
                green = int(tmp_green)

        # blue
        if tmp_internal >=66:
            blue = 255
        elif tmp_internal <= 19:
            blue = 0
        else:
            tmp_blue = 138.5177312231 * math.log(tmp_internal - 10) - 305.0447927307
            if tmp_blue < 0:
                blue = 0
            elif tmp_blue > 255:
                blue = 255
            else:
                blue = int(tmp_blue)

        return red, green, blue

    def _update_ct(self, event=None):
        """Update display after a change in the ct spinboxes."""
        if event is None or event.widget.old_value != event.widget.get():
            ct = self.ct.get()
            r,g,b = self._convert_K_to_RGB(ct)
            self.red.set(r)
            self.green.set(g)
            self.blue.set(b)
            h, s, v = rgb_to_hsv(r, g, b)
            self.hue.set(h)
            self.saturation.set(s)
            self.value.set(v)
            args = (r, g, b)
            hexa = rgb_to_hexa(*args)
            self.hexa.delete(0, "end")
            self.hexa.insert(0, hexa)
            self.square.set_hsv((h, s, v))
            self.bar.set(h,s,v)
            self._update_preview()


    def led_off(self,_event=None):
    # switch off all LED
        message = "#L00 00 00 00 FF\n"
        self.controller.send_to_ARDUINO(message)


    def cancel(self,_event=None):
        try:
            self.led_off()
        except:
            pass

        sdConfig.lastLed = self.lednum.get()
        sdConfig.lastLedCount = self.ledcount.get()

        #sdConfig.use_led_correction = self.s_ledcorvar.get()

        sdConfig.led_correction_r = self.cs_red.get()
        sdConfig.led_correction_g = self.cs_green.get()
        sdConfig.led_correction_b = self.cs_blue.get()

        sdConfig.old_color = self.hexa.get()

        sdConfig.palette = self.palette

        if MLLparam.fileexists: # write params only when the file existed
            MLLparam.color = self.hexa.get() # current color
            MLLparam.cor_color = self.ledcorvalue.upper()[-7:] # corrected color
            MLLparam.Lednum = self.lednum.get()
            MLLparam.LedCount = self.ledcount.get()
            MLLparam.coltab = sdConfig.palette

            MLLparam.save()

        self.controller.cancel()


    def onCheckDisconnectFile(self):
        # checks every 1 Second if the file DISCONNECT_FILENAME is found.
        # if yes: then disconnect ARDUINO - is used by external program to request a disconnect of the ARDUINO
        if os.path.isfile(DISCONNECT_FILENAME):
            self.led_off()
            self.disconnect()

            # delete the file to avoid a series of disconnects
            try:
                os.remove(DISCONNECT_FILENAME)
            except:
                pass

        if os.path.isfile(CLOSE_FILENAME):
            self.led_off()
            self.disconnect()
            self.cancel()

            # delete the file to avoid a series of disconnects
            try:
                os.remove(CLOSE_FILENAME)
            except:
                pass
        self.after(1000, self.onCheckDisconnectFile)




class SerialThread(threading.Thread):
    def __init__(self, queue, serialport):
        threading.Thread.__init__(self)
        self.queue = queue
        self.serialport = serialport

    def run(self):
        logging.info("SerialThread started")
        if self.serialport:

            while not event.is_set() and self.serialport:
                try:
                    text = self.serialport.readline()
                except:
                    pass
                if len(text)>0:
                    try:
                        self.queue.put(text)
                    except:
                        pass
                    logging.info("SerialThread got message: %s", text)

        logging.info("SerialThread received event. Exiting")


class SerialMonitorPage(tk.Frame):
    def __init__(self, parent, controller):
        self.controller = controller
        tk.Frame.__init__(self, parent)
        title_frame = ttk.Frame(self)

        ttk.Label(title_frame, text="Simple Serial Monitor").pack(side="left", padx=4, pady=(4, 1))
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

        frameLabel = tk.Frame(self, padx=40, pady =40)
#        self.text = tk.Text(frameLabel, wrap='word', font='TimesNewRoman 37',
#                            bg=self.cget('bg'), relief='flat', height=2, width=10)
        self.text = tk.Text(frameLabel, wrap='word',
                            bg=self.cget('bg'), height=40, width=80)         # 04.12.19: Old: height=10, width=50
        frameLabel.grid(row=2, column=0,padx=10, pady=(10, 4))
#        self.text.pack()


        # add a vertical scroll bar to the text area
        scroll=tk.Scrollbar(frameLabel)
        self.text.configure(yscrollcommand=scroll.set)
        scroll.config(command=self.text.yview)


        #pack everything
        self.text.pack(side=tk.LEFT)
        scroll.pack(side=tk.RIGHT,fill=tk.Y)


        button_frame = ttk.Frame(self)

        button_width = 20
        ttk.Button(button_frame, text=_("zurück zu Einstellungen"),width=button_width, command=lambda: controller.show_frame(ConfigurationPage)).grid(row=0, column=0, padx=10, pady=(10, 4), sticky='n')
        ttk.Button(button_frame, text=_("zur LED Farbtestseite"),  width=button_width, command=controller.switch_to_color_check).grid(                row=0, column=1, padx=10, pady=(10, 4), sticky='n')
        ttk.Button(button_frame, text=_("Beenden"),                width=button_width ,command=controller.cancel).grid(                               row=0, column=2, padx=10, pady=(10, 4), sticky='n') # 04.12.19:

        button_frame.grid(row=3, column=0,padx=10, pady=(10, 4))

        self.queue = queue.Queue()
        self.monitor_serial = True
        self.start_process_serial()

    def send(self,event=None):
        message = self.input.get() +"\r\n"
        self.controller.send_to_ARDUINO(message)

    def process_serial(self):

        if self.controller.queue:
            while self.controller.queue.qsize():
                try:
                    readtext = self.controller.queue.get()
                    date_time = datetime.now()
                    d = date_time.strftime("%H:%M:%S")
                    textmessage = d + "  " + str(readtext)
                    if not textmessage.endswith("\n"):
                        textmessage = textmessage +"\n"
                    self.text.insert("end", textmessage)
                    self.text.yview("end")
                    frame = self.controller.frames[ColorCheckPage]
                    frame.set_ARDUINO_message(textmessage)
                except:
                    pass
        if self.monitor_serial:
            self.after(100, self.process_serial)

    def start_process_serial(self):
        event.set()
        time.sleep(2)
        event.clear()
        self.monitor_serial = True
        thread = SerialThread(self.controller.queue,self.controller.arduino)
        thread.start()
        self.process_serial()

    def stop_process_serial(self):
        self.monitor_serial = False
        event.set()
        time.sleep(1)


#-------------------------------------------


format = "%(asctime)s: %(message)s"
logging.basicConfig(format=format, level=logging.INFO,
                    datefmt="%H:%M:%S")

event = threading.Event()
pipeline = queue.Queue(maxsize=20)
app = LEDColorTest()

serialport = app.arduino

#app.set_pipeline(pipeline)

app.mainloop()
logging.info("Main: about to set event")

app.disconnect()                            # 04.12.19: send the "#END" Message to restart the arduino

event.set()
