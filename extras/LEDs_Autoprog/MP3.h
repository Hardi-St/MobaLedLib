
// Min 64, max 69 - use something in the middle
#define MP3_BYTE(x) uint8_t( 0xff & (67 + (((int) x) * 6) ) )

#define MP3_NR_WORDH(nr, x) uint8_t( MP3_BYTE( (nr<<3) + (x>>5) ) )

#define MP3_WORDH(x) MP3_BYTE( ( (uint8_t) (x >> 5) ) )
#define MP3_WORDL(x) MP3_BYTE( ( (uint8_t) x & 31) )

// Equalizer
#define MP3_EQ_NORMAL 0
#define MP3_EQ_POP 1
#define MP3_EQ_ROCK 2
#define MP3_EQ_JAZZ 3
#define MP3_EQ_CLASSIC 4
#define MP3_EQ_BASS 5

// On/Off
#define MP3_ON 1
#define MP3_OFF 0

// Module types
#define MP3_TF_16p 1
#define MP3_JQ6500 2

// MP3 commands
/* META-COMMANDS (Executed by ATTiny) */
#define MP3_TYPE_CMD				MP3_BYTE(0x1e)
#define MP3_SELECT_CMD			MP3_BYTE(0x1f)

/* SIMPLE COMMANDS */
#define MP3_NOP							MP3_BYTE(0)			// Do nothing
#define MP3_NEXT            MP3_BYTE(0x01)  // Play next track
#define MP3_PREV            MP3_BYTE(0x02)  // Play previous track
#define MP3_INCREASE_VOLUME MP3_BYTE(0x04)  // Increase volume by 1
#define MP3_DECREASE_VOLUME MP3_BYTE(0x05)  // Decrease volume by 1
#define MP3_STANDBY         MP3_BYTE(0x0a)  // Set module to standby
#define MP3_RESET           MP3_BYTE(0x0c)  // Reset MP3-Module
#define MP3_PLAY            MP3_BYTE(0x0d)  // Play current track (e.g. after Pause)
#define MP3_PAUSE           MP3_BYTE(0x0e)  // Pause current track
#define MP3_STOP            MP3_BYTE(0x16)  // Stop current track
#define MP3_STOP_ADVERT     MP3_BYTE(0x15)  // Stop playing current advert and return to original track

/* Set commands with 8 bit parameter */
#define MP3_SET_VOLUME          MP3_BYTE(0x06)  // Set volume to 0-31
#define MP3_SET_EQ              MP3_BYTE(0x07)  // Set Equalizer, Parameter see MP3_EQ_??? above
#define MP3_SET_REPEAT_CURRENT  MP3_BYTE(0x19)  // Set repeat of current track, 0=No repeat, 1 = repeat

/* TRACK COMMANDS */
// Play track in root folder, ordered as copied to SD card:
#define MP3_PLAY_TRACK MP3_BYTE(0x03)
// Play track in root folder with repeat:
#define MP3_PLAY_TRACK_REPEAT MP3_BYTE(0x08)
// Interrupt current track and play advertisement \ADVERT\xxxx.mp3/wav, 1-1024
#define MP3_PLAY_ADVERT MP3_BYTE(0x13)
// Play folder with repeat, Folder 01 to 99
#define MP3_PLAY_FOLDER_REPEAT MP3_BYTE(0x17)
// Play track in mp3 folder, filename 0001.mp3/wav to 1024.mp3/wav
#define MP3_PLAY_MP3 MP3_BYTE(0x12)
// Play Track from root folder (1-256) on Module #
#define MP3_PLAY_TRACK_ON_CMD	MP3_BYTE(0x1d)
// Play Track from mp3 folder (1-256) on Module #
#define MP3_PLAY_MP3_ON_CMD	MP3_BYTE(0x1c)
// Play Track from root folder (1-256) endless repeat
#define MP3_PLAY_TRACK_REPEAT_ON_CMD	MP3_BYTE(0x1b)

/* Play track 001-032 from folder 01-32 */
#define MP3_PLAY_FOLDER_TRACK_CMD MP3_BYTE(0x0f)

#define MP3_CMD(LED, InCh, Cmd) ConstRGB(LED, InCh, 0, 0, 0, Cmd, 0, 0)

#define MP3_TRACK(LED, InCh, Cmd, Track) ConstRGB(LED, InCh, 0, 0, 0, Cmd, MP3_WORDH(uint16_t(Track-1)), MP3_WORDL(uint16_t(Track-1)))

#define MP3_SET(LED, InCh, Cmd, Value) ConstRGB(LED, InCh, 0, 0, 0, Cmd, MP3_BYTE(Value), 0)

#define MP3_PLAY_FOLDER_TRACK(LED, InCh, Folder, Track) ConstRGB(LED, InCh, 0, 0, 0, MP3_PLAY_FOLDER_TRACK_CMD, MP3_BYTE(Folder-1), MP3_BYTE(Track-1))

// Select Module 1-3
#define MP3_SELECT_MODULE(LED, InCh, MP3_Module) ConstRGB(LED, InCh, 0, 0, 0, MP3_SELECT_CMD, MP3_BYTE(MP3_Module), MP3_BYTE(MP3_Module))

#define MP3_SET_TYPE(LED, InCh, MP3_Module, MP3_Type)	ConstRGB(LED, InCh, 0, 0, 0, MP3_TYPE_CMD, MP3_BYTE(MP3_Module), MP3_BYTE(MP3_Type))

#define MP3_PLAY_TRACK_ON(LED, InCh, MP3_Module, Track) ConstRGB(LED, InCh, 0, 0, 0, MP3_PLAY_TRACK_ON_CMD, MP3_NR_WORDH(MP3_Module, uint16_t(Track-1)), MP3_WORDL(uint16_t(Track-1)))

#define MP3_PLAY_MP3_ON(LED, InCh, MP3_Module, Track) ConstRGB(LED, InCh, 0, 0, 0, MP3_PLAY_MP3_ON_CMD, MP3_NR_WORDH(MP3_Module, uint16_t(Track-1)), MP3_WORDL(uint16_t(Track-1)))

#define MP3_PLAY_TRACK_REPEAT_ON(LED, InCh, MP3_Module, Track) ConstRGB(LED, InCh, 0, 0, 0, MP3_PLAY_TRACK_REPEAT_ON_CMD, MP3_NR_WORDH(MP3_Module, uint16_t(Track-1)), MP3_WORDL(uint16_t(Track-1)))
