
#include <stdlib.h>
#include "mbug_2811.h"

//------------------------------------------------------------------------------
// Get a list of all available mbug-2811 devices. 
const mbug_device_list  mbug_2811_list( void )
{
	return mbug_get_device_list(2811);
}

//------------------------------------------------------------------------------
// Open a device specified by it's serial number (as int,
// last digits of the serial number are matched only).
mbug_device mbug_2811_open( int serial_num )
{
	return mbug_open_int(2811, serial_num);
}

//------------------------------------------------------------------------------
// Open a device specified by it's id string, as returned
// by mbug_2811_list()
mbug_device mbug_2811_open_str( const char *id )
{
	unsigned int type = mbug_type_from_id( id );
	if (type != 2811) return NULL;
	return mbug_open_str( id );
}

//------------------------------------------------------------------------------
// Close a previously opened device.
void mbug_2811_close( mbug_device dev )
{
	mbug_close(dev);
}

//------------------------------------------------------------------------------
// Read raw sensor value
int mbug_2811_read_raw( mbug_device dev )
{
	unsigned char in[16] = {0};

	if (mbug_write( dev, "\xFA\xA0\xF2", 3) <3)	// Raw format (0xF2 is the old revisions read cmd)
		return NOT_A_TEMPERATURE;
	if (mbug_read( dev, in, 8 ) <8)
		return NOT_A_TEMPERATURE;
	return in[0]+(in[1]<<8);
}

//------------------------------------------------------------------------------
// Read the current temperature (ascii mode)
double mbug_2811_read_ascii( mbug_device dev )
{
	char tem[8] = {0};
	if (mbug_write( dev, "read", 5) <5)
		return NOT_A_TEMPERATURE;
	if (mbug_read( dev, tem, 8 ) <8)
		return NOT_A_TEMPERATURE;
	return atof(tem);
}

//------------------------------------------------------------------------------

// Default read mode is ascii 
double mbug_2811_read( mbug_device dev )
{
	return mbug_2811_read_ascii( dev );
}

//------------------------------------------------------------------------------
// Set the acquisition mode:
//  	MODE_INST: Use last measured value if not read before, else wait for next.
//		MODE_LAST: Always use last valid value, never block.
//		MODE_NEXT: Always wait for next incoming value.
int mbug_2811_set_mode( mbug_device dev, mbug_2811_mode mode )
{
	char* cmd = "";

	switch (mode)
	{
		case ACQ_MODE_INST: cmd = "MODE INS"; break;
		case ACQ_MODE_LAST: cmd = "MODE LAS"; break;
		case ACQ_MODE_NEXT: cmd = "MODE NEX"; break;
		default: return -1;
	}

	if (mbug_write(dev, cmd, 10) <= 0)
		return -1;
	return 0;
}

