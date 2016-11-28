/*************************************************************************************

Emulation of the Alpha 8302 microcontroller.

All tables built manually from the analisis of the bootleg set.

The main purpose of the MCU is to control the program flow.
This tables are actually subroutine addresses, wich get pushed
onto the stack one after the other. Basically then the program
runs and when it returns on one of these subroutines, it expects
to land on the proper 'next' subroutine.

Ernesto Corvi - 10/30/98

- 09/12/2004 Pierpaolo Prazzoli
  added Exciting Soccer II protection
  (still at least one issue because you can't control the player)

*************************************************************************************/

#include "driver.h"


#define MCU_KEY_TABLE_SIZE 16

/* These are global */
unsigned char *exctsccr_mcu_ram;
WRITE8_HANDLER( exctsccr_mcu_w );
WRITE8_HANDLER( exctsccr_mcu_control_w );

/* Local stuff */
static int mcu_code_latch;

/* Exciting Soccer */

/* input = 0x6009 - data = 0x6170 */
static unsigned char mcu_table1[MCU_KEY_TABLE_SIZE] = {
	0x23, 0x05, 0xfb, 0x07, 0x4d, 0x3b,	0x7d, 0x03,
	0x9a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* input = 0x600d - data = 0x61f0 */
static unsigned char mcu_table2[MCU_KEY_TABLE_SIZE] = {
	0x75, 0x25, 0x4b, 0x10, 0xa6, 0x06,	0x9a, 0x02,
	0x04, 0x11, 0x09, 0x09,	0x2a, 0x10, 0x3a, 0x10
};

/* input = ?????? - data = 0x6300 */
static unsigned char mcu_table3[MCU_KEY_TABLE_SIZE*4] = {
	0xba, 0x26,	0x53, 0x0d, 0x01, 0x24, 0x1d, 0x15,
	0xd0, 0x1d, 0x35, 0x1f, 0x32, 0x22,	0xa0, 0x2c,

	0x5e, 0x2e, 0x92, 0x2e,	0xf9, 0x2e, 0x43, 0x2f,
	0x59, 0x2f,	0x6f, 0x2f, 0xd1, 0x38, 0xd8, 0x3e,

	0x9e, 0x0d, 0x4c, 0x47, 0x1f, 0x1f,	0x5a, 0x1f,
	0xbe, 0x22, 0xa2, 0x23,	0x33, 0x35, 0x69, 0x34,

	0x7c, 0x3a,	0x00, 0x00, 0xdc, 0x44, 0x78, 0x0b,
	0x65, 0x0d, 0xc4, 0xb2, 0xc4, 0xb2, 0xc4, 0xb2
};

/* input = 0x6007 - data = 0x629b */
static unsigned char mcu_table4[MCU_KEY_TABLE_SIZE] = {
	0xc5, 0x24, 0xc5, 0x24, 0x27, 0x3f,	0xd0, 0x07,
	0x07, 0x0b, 0xd8, 0x02, 0x9a, 0x08, 0x00, 0x00
};

/* input = 0x6007 - data = 0x629b */
static unsigned char mcu_table5[MCU_KEY_TABLE_SIZE] = {
	0x54, 0x43, 0xd0, 0x07, 0xd8, 0x02, 0x9a, 0x08,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* input = 0x6007 - data = 0x629b */
static unsigned char mcu_table6[MCU_KEY_TABLE_SIZE] = {
	0x2b, 0x03, 0xd0, 0x07, 0xd8, 0x02, 0x9a, 0x08,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* input = 0x6007 - data = 0x629b */
static unsigned char mcu_table7[MCU_KEY_TABLE_SIZE] = {
	0xfa, 0x0e, 0x9a, 0x08, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* input = 0x6011 - data = 0x607f */
static unsigned char mcu_table8[MCU_KEY_TABLE_SIZE*2] = {
	0x0b, 0x78, 0x47, 0x04, 0x36, 0x18, 0x23, 0xf1,
	0x10, 0xee, 0x16, 0x04, 0x11, 0x94, 0x16, 0xdd,

	0x13, 0x4e, 0x06, 0xa6, 0x13, 0x12, 0x08, 0x9a,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static void mcu_sort_list( unsigned char *src, unsigned char *dst, unsigned char *table ) {
	int i;

	for ( i = 0; i < 0x20; i++ ) {
		int where = src[i];

		if ( where < 0x20 ) {
			int offs = ( i << 1 );

			where <<= 1;
			dst[where] = table[offs];
			dst[where+1] = table[offs+1];
		}
	}
}

WRITE8_HANDLER( exctsccr_mcu_control_w ) {

	if ( data == 0xff ) { /* goes around the mcu checks */
		exctsccr_mcu_ram[0x0003] = 0x01; /* mcu state = running */

		exctsccr_mcu_ram[0x0005] = 0x08;
		exctsccr_mcu_ram[0x0380] = 0xf1;
		exctsccr_mcu_ram[0x0381] = 0x01;
		exctsccr_mcu_ram[0x02f8] = 0x1f; /* Selects song to play during gameplay */

		exctsccr_mcu_ram[0x0009] = 0x08;
		memcpy( &exctsccr_mcu_ram[0x0170], mcu_table1, MCU_KEY_TABLE_SIZE );

		if ( exctsccr_mcu_ram[0x000d] == 0x01 ) {
			exctsccr_mcu_ram[0x000d] = 0x08;

			memcpy( &exctsccr_mcu_ram[0x01f0], mcu_table2, MCU_KEY_TABLE_SIZE );
		}

		mcu_sort_list( &exctsccr_mcu_ram[0x03e0], &exctsccr_mcu_ram[0x0300], mcu_table3 );

		exctsccr_mcu_ram[0x02fb] = 0x7e;
		exctsccr_mcu_ram[0x02fd] = 0x7e;

		if ( mcu_code_latch ) {
			exctsccr_mcu_ram[0x02f9] = 0x7f;
			exctsccr_mcu_ram[0x02fa] = 0x0d;
			exctsccr_mcu_ram[0x02fc] = 0x05;
			exctsccr_mcu_ram[0x02fe] = 0x01;
		} else {
			exctsccr_mcu_ram[0x02f9] = 0x81;
			exctsccr_mcu_ram[0x02fa] = 0x2d;
			exctsccr_mcu_ram[0x02fc] = 0x25;
			exctsccr_mcu_ram[0x02fe] = 0x09;
		}

		if ( exctsccr_mcu_ram[0x000f] == 0x02 ) {
			exctsccr_mcu_ram[0x000f] = 0x08;
			exctsccr_mcu_ram[0x02f6] = 0xd6;
			exctsccr_mcu_ram[0x02f7] = 0x39;
		}

		if ( exctsccr_mcu_ram[0x0007] == 0x02 ) {
			exctsccr_mcu_ram[0x0007] = 0x08;

			if ( exctsccr_mcu_ram[0x0204] > 3 )
				exctsccr_mcu_ram[0x0204] = 0;

			switch( exctsccr_mcu_ram[0x0204] ) {
				case 0x00:
					memcpy( &exctsccr_mcu_ram[0x029b], mcu_table4, MCU_KEY_TABLE_SIZE );
				break;

				case 0x01:
					memcpy( &exctsccr_mcu_ram[0x029b], mcu_table5, MCU_KEY_TABLE_SIZE );
				break;

				case 0x02:
					memcpy( &exctsccr_mcu_ram[0x029b], mcu_table6, MCU_KEY_TABLE_SIZE );
				break;

				case 0x03:
					memcpy( &exctsccr_mcu_ram[0x029b], mcu_table7, MCU_KEY_TABLE_SIZE );
				break;
			}

			if ( exctsccr_mcu_ram[0x0204] != 3 )
				exctsccr_mcu_ram[0x0204]++;
		}

		exctsccr_mcu_ram[0x0011] = 0x08;
		memcpy( &exctsccr_mcu_ram[0x007f], mcu_table8, MCU_KEY_TABLE_SIZE*2 );
	}
}

WRITE8_HANDLER( exctsccr_mcu_w ) {

	if ( offset == 0x02f9 )
		mcu_code_latch = data;

	exctsccr_mcu_ram[offset] = data;
}


/* Exciting Soccer II */

/* input = 0x6009 - data = 0x6170 */
static unsigned char exctscc2_mcu_table1[MCU_KEY_TABLE_SIZE] = {
	0x56, 0x05, 0x90, 0x08, 0x19, 0x3e,	0xa8, 0x03,
	0x2f, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* input = 0x600d - data = 0x61f0 */
static unsigned char exctscc2_mcu_table2[MCU_KEY_TABLE_SIZE] = {
	0xff, 0x27, 0x35, 0x10, 0xd9, 0x06, 0xc3, 0x02,
	0x4d, 0x12, 0x9e, 0x09,	0x14, 0x10, 0x24, 0x10
};

/* input = ?????? - data = 0x6300 */
static unsigned char exctscc2_mcu_table3[MCU_KEY_TABLE_SIZE*4] = {
	0x41, 0x29,	0xb2, 0x12, 0x91, 0x26, 0xa6, 0x16, //0x12b2?
	0x5f, 0x20,	0xc4, 0x21, 0xc1, 0x24, 0x3e, 0x2f,

	0x03, 0x31,	0x37, 0x31, 0x9e, 0x31, 0xe8, 0x31,
	0xfe, 0x31,	0x14, 0x32, 0x93, 0x3b, 0xa4, 0x41,

	0x88, 0x0d,	0x55, 0x4b, 0xae, 0x21, 0xe9, 0x21,
	0x4d, 0x25,	0x32, 0x26, 0xd8, 0x37, 0x0e, 0x37,

	0x48, 0x3d,	0x00, 0x00, 0xd5, 0x48, 0x54, 0x0c,
	0xa0, 0x0c,	0xc4, 0xb2, 0xc4, 0xb2, 0xc4, 0xb2 //0x0ca0? //last 3 couples?
};

/* input = 0x6007 - data = 0x629b */
static unsigned char exctscc2_mcu_table4[MCU_KEY_TABLE_SIZE] = {
	0x55, 0x27, 0x55, 0x27, 0xf3, 0x41, 0x65, 0x08,
	0xe3, 0x0b, 0x01, 0x03, 0x2f, 0x09, 0x00, 0x00
};

/* input = 0x6007 - data = 0x629b */
static unsigned char exctscc2_mcu_table5[MCU_KEY_TABLE_SIZE] = {
	0xdd, 0x45, 0x65, 0x08, 0x01, 0x03, 0x2f, 0x09,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* input = 0x6007 - data = 0x629b */
static unsigned char exctscc2_mcu_table6[MCU_KEY_TABLE_SIZE] = {
	0x54, 0x03, 0x65, 0x08, 0x01, 0x03, 0x2f, 0x09,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* input = 0x6007 - data = 0x629b */
static unsigned char exctscc2_mcu_table7[MCU_KEY_TABLE_SIZE] = {
	0xe4, 0x0e, 0x2f, 0x09, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* input = 0x6011 - data = 0x607f */
static unsigned char exctscc2_mcu_table8[MCU_KEY_TABLE_SIZE*2] = {
	0x0c, 0x54, 0x4b, 0x0d, 0x38, 0xbd, 0x26, 0x81,
	0x12, 0x37, 0x18, 0x4a, 0x13, 0x0f, 0x19, 0x23,

	0x14, 0xd6, 0x06, 0xd9, 0x14, 0x9a, 0x09, 0x2f,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

WRITE8_HANDLER( exctscc2_mcu_control_w )
{
	if ( data == 0xff )  /* goes around the mcu checks */
	{
		exctsccr_mcu_ram[0x0003] = 0x01; /* mcu state = running */

		exctsccr_mcu_ram[0x0005] = 0x08;
		exctsccr_mcu_ram[0x0380] = 0x1a;
		exctsccr_mcu_ram[0x0381] = 0x02;
		exctsccr_mcu_ram[0x02f8] = 0x1f; /* Selects song to play during gameplay */

		exctsccr_mcu_ram[0x0009] = 0x08;
		memcpy( &exctsccr_mcu_ram[0x0170], exctscc2_mcu_table1, MCU_KEY_TABLE_SIZE );

		if ( exctsccr_mcu_ram[0x000d] == 0x01 ) {
			exctsccr_mcu_ram[0x000d] = 0x08;

			memcpy( &exctsccr_mcu_ram[0x01f0], exctscc2_mcu_table2, MCU_KEY_TABLE_SIZE );
		}

		mcu_sort_list( &exctsccr_mcu_ram[0x03e0], &exctsccr_mcu_ram[0x0300], exctscc2_mcu_table3 );

/* are these right ? */
		exctsccr_mcu_ram[0x02fb] = 0x7e;
		exctsccr_mcu_ram[0x02fd] = 0x7e;

		if ( mcu_code_latch ) {
			exctsccr_mcu_ram[0x02f9] = 0x7f;
			exctsccr_mcu_ram[0x02fa] = 0x0d;
			exctsccr_mcu_ram[0x02fc] = 0x05;
			exctsccr_mcu_ram[0x02fe] = 0x01;
		} else {
			exctsccr_mcu_ram[0x02f9] = 0x81;
			exctsccr_mcu_ram[0x02fa] = 0x2d;
			exctsccr_mcu_ram[0x02fc] = 0x25;
			exctsccr_mcu_ram[0x02fe] = 0x09;
		}
/*******/

		if ( exctsccr_mcu_ram[0x000f] == 0x02 ) {
			exctsccr_mcu_ram[0x000f] = 0x08;
			exctsccr_mcu_ram[0x02f6] = 0x98;
			exctsccr_mcu_ram[0x02f7] = 0x3c;
		}

		if ( exctsccr_mcu_ram[0x0007] == 0x02 ) {
			exctsccr_mcu_ram[0x0007] = 0x08;

			if ( exctsccr_mcu_ram[0x0204] > 3 )
				exctsccr_mcu_ram[0x0204] = 0;

			switch( exctsccr_mcu_ram[0x0204] ) {
				case 0x00:
					memcpy( &exctsccr_mcu_ram[0x029b], exctscc2_mcu_table4, MCU_KEY_TABLE_SIZE );
				break;

				case 0x01:
					memcpy( &exctsccr_mcu_ram[0x029b], exctscc2_mcu_table5, MCU_KEY_TABLE_SIZE );
				break;

				case 0x02:
					memcpy( &exctsccr_mcu_ram[0x029b], exctscc2_mcu_table6, MCU_KEY_TABLE_SIZE );
				break;

				case 0x03:
					memcpy( &exctsccr_mcu_ram[0x029b], exctscc2_mcu_table7, MCU_KEY_TABLE_SIZE );
				break;
			}

			if ( exctsccr_mcu_ram[0x0204] != 3 )
				exctsccr_mcu_ram[0x0204]++;

		}

		exctsccr_mcu_ram[0x0011] = 0x08;
		memcpy( &exctsccr_mcu_ram[0x007f], exctscc2_mcu_table8, MCU_KEY_TABLE_SIZE*2 );
	}
}