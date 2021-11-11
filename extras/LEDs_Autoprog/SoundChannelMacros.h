#ifndef __SOUND_CHANNEL_MACROS_H__
#define __SOUND_CHANNEL_MACROS_H__


/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2021  Hardi Stengelin: MobaLedLib@gmx.de
 
 this file: Copyright (C) 2021 Juergen Winkler: MobaLedLib@a1.net

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 -------------------------------------------------------------------------------------------------------------
 
 
 Revision History :
~~~~~~~~~~~~~~~~~
07.10.21:  Versions 1.0 (Juergen)

*/

// the third byte is combines ModuleNumber and Command
// update four bit are the module number
// lower four bits are the command number
// if highest bit of command number is set one additional argument is expected
// so commands 8-15 have one argument byte
// and commands 0-7 have none

// Loop mode parameters for SOUND_CHANNEL_LOOP_MODE command
#define LOOP_ALL    0           
#define LOOP_RND    1
#define LOOP_ONE    2
#define LOOP_OFF    4

// set the default type id for sound channel macros
// this id is used by the MobaLebLib state machine to identify macro types and call appropriate handling code
// the type id may not overlap with predefined types of MobaLedLib
#ifndef SOUND_CHANNEL_TYPE_T
  // take the lowest id allowed for external command processing
  #define SOUND_CHANNEL_TYPE_T EXT_PROC_BASE_T
#endif

// define the internal command ids
#define SOUND_CHANNEL_CMD_INCREASE_VOLUME 0
#define SOUND_CHANNEL_CMD_DECREASE_VOLUME 1
#define SOUND_CHANNEL_CMD_PAUSE           2
#define SOUND_CHANNEL_CMD_CONTINUE        3
#define SOUND_CHANNEL_CMD_PLAY_TRACK      8
#define SOUND_CHANNEL_CMD_SET_VOLUME      9
#define SOUND_CHANNEL_CMD_LOOP_MODE       10
#define SOUND_CHANNEL_CMD_PLAY_RANDOM     15

/*************************************/
/*  Command without argument (0-7)   */
/*************************************/
// command 0: Increase volume
#define SOUND_CHANNEL_INCREASE_VOLUME(Module, InCh) SOUND_CHANNEL_TYPE_T,InCh,(SOUND_CHANNEL_##Module<<4)+SOUND_CHANNEL_CMD_INCREASE_VOLUME,
// command 1: decrease volume
#define SOUND_CHANNEL_DECREASE_VOLUME(Module, InCh) SOUND_CHANNEL_TYPE_T,InCh,(SOUND_CHANNEL_##Module<<4)+SOUND_CHANNEL_CMD_DECREASE_VOLUME,
// command 2: pause
#define SOUND_CHANNEL_PAUSE(Module, InCh) SOUND_CHANNEL_TYPE_T,InCh,(SOUND_CHANNEL_##Module<<4)+SOUND_CHANNEL_CMD_PAUSE,
// command 3: continue
#define SOUND_CHANNEL_CONTINUE(Module, InCh) SOUND_CHANNEL_TYPE_T,InCh,(SOUND_CHANNEL_##Module<<4)+SOUND_CHANNEL_CMD_CONTINUE,


/**************************************/
/*  Command with one argument (8-13)  */
/**************************************/
// command 8: PlayTrack, one argument TrackNumber
#define SOUND_CHANNEL_PLAY_TRACK(Module, InCh, Track) SOUND_CHANNEL_TYPE_T,InCh,(SOUND_CHANNEL_##Module<<4)+SOUND_CHANNEL_CMD_PLAY_TRACK,Track,
// command 9: Set Volume to percentage level
#define SOUND_CHANNEL_SET_VOLUME(Module, InCh, VolumePercent) SOUND_CHANNEL_TYPE_T,InCh,(SOUND_CHANNEL_##Module<<4)+SOUND_CHANNEL_CMD_SET_VOLUME,VolumePercent,   
// command 10: Set loop mode
#define SOUND_CHANNEL_LOOP_MODE(Module, InCh, LoopMode) SOUND_CHANNEL_TYPE_T,InCh,(SOUND_CHANNEL_##Module<<4)+SOUND_CHANNEL_CMD_LOOP_MODE,LoopMode,   

/***************************************/
/*  Command with two arguments (14-15) */
/***************************************/
#define SOUND_CHANNEL_PLAY_RANDOM(Module, InCh, TrackMin, TrackMax) SOUND_CHANNEL_TYPE_T,InCh,(SOUND_CHANNEL_##Module<<4)+SOUND_CHANNEL_CMD_PLAY_RANDOM,TrackMin,TrackMax-TrackMin,

#endif