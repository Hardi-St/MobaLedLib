# -*- coding: utf-8 -*-
#
#         MobaLedCheckColors: Color checker for WS2812 and WS2811 based MobaLedLib
#
#         DefaultData
#
# * Version: 1.25
# * Author: Harold Linke
# * Date: January 04th, 2020
# * Copyright: Harold Linke 2020
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

VERSION ="V01.26 - 13.01.2020"
LARGE_FONT= ("Verdana", 12)
VERY_LARGE_FONT = ("Verdana", 14)
SMALL_FONT= ("Verdana", 8)

COLORCOR_MAX = 255

# colorwheel view
DELTA_H = 270
INVERT_WHEEL = True

PARAM_FILENAME = 'MobaLedTest_param.json'
CONFIG_FILENAME = 'MobaLedTest_config.json'
DISCONNECT_FILENAME = 'MobaLedTest_disconnect.txt'
CLOSE_FILENAME = 'MobaLedTest_close.txt'
HELPPAGE_FILENAME = "MobaLedCheckColorsHelp01.htm"

FINISH_FILE = "Finished.txt"                                                 # 03.12.19:

SerialIF_teststring1 = "LEDs_AutoProg"
SerialIF_teststring2 = "#Color Test LED"

PERCENT_BRIGHTNESS = 1  # 1 = Show the brightnes as percent, 0 = Show the brightnes as ">>>"# 03.12.19:

DEFAULT_PALETTE = {
                    "ROOM_COL0"   : "#0F0D03",
                    "ROOM_COL1"   : "#162C1B",
                    "ROOM_COL2"   : "#9B4905",
                    "ROOM_COL3"   : "#271201",
                    "ROOM_COL4"   : "#1E0000",
                    "ROOM_COL5"   : "#4F2707",
                    "GAS_LIGHT D" : "#323232",
                    "GAS LIGHT"   : "#FFFFFF",
                    "NEON_LIGHT D": "#14141B",
                    "NEON_LIGHT M": "#464650",
                    "NEON_LIGHT"  : "#F5F5FF",
                    "ROOM_TV0 A"  : "#323214",
                    "ROOM_TV0 B"  : "#46461E",
                    "ROOM_TV1 A"  : "#323208",
                    "ROOM_TV1 B"  : "#323208",
                    "SINGLE_LED"  : "#FFFFFF",
                    "SINGLE_LED D": "#323232"
                    }

DEFAULT_CONFIG = {
                    "serportnumber": 0,
                    "serportname": "No Device",
                    "baudrate": 115200,         # baudrate, possible values: 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200
                    "maxLEDcount": 255,
                    "lastLedCount": 1,
                    "lastLed": 0,
                    "pos_x": 100,
                    "pos_y": 100,
                    "colorview": 1,
                    "startpage": -1,
                    "startpagename" : "ColorCheckPage",
                    "led_correction_r": COLORCOR_MAX,
                    "led_correction_g": int(69*COLORCOR_MAX/100),
                    "led_correction_b": int(94*COLORCOR_MAX/100),
                    "use_led_correction": True,
                    "old_color": "#FFFFFF",
                    "palette": DEFAULT_PALETTE,
                    "autoconnect": False
                }

DEFAULT_PARAM = {}

CONFIG2PARAMKEYS = {
                    "old_color"   :"color",
                    "lastLed"     :"Lednum",
                    "lastLedCount":"LedCount", 
                    "serportname" :"comport",
                    "palette"     :"coltab", 
                    }

TOOLTIPLIST = {
                   "Alte Farbe": "Alte Farbe",
                   "Aktuelle Farbe": "Aktuelle Farbe",
                   "ColorWheel":"Farbton und Sättigung auswählen durch Mausklick"
                   }
                   

#tabpageslist = ("ColorCheckPage","SerialMonitorPage","ConfigurationPage")

BLINKFRQ     = 2 # Blinkfrequenz in Hz

ARDUINO_WAITTIME = 0.01  # Wartezeit zwischen 2 Farbkommandos in Sekunden 


