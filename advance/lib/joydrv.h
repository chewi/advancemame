/*
 * This file is part of the Advance project.
 *
 * Copyright (C) 1999-2002 Andrea Mazzoleni
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * In addition, as a special exception, Andrea Mazzoleni
 * gives permission to link the code of this program with
 * the MAME library (or with modified versions of MAME that use the
 * same license as MAME), and distribute linked combinations including
 * the two.  You must obey the GNU General Public License in all
 * respects for all of the code used other than MAME.  If you modify
 * this file, you may extend this exception to your version of the
 * file, but you are not obligated to do so.  If you do not wish to
 * do so, delete this exception statement from your version.
 */

#ifndef __JOYDRV_H
#define __JOYDRV_H

#include "device.h"
#include "conf.h"

#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/
/* Driver */

#define JOYSTICK_DRIVER_FLAGS_USER_BIT0 0x10000
#define JOYSTICK_DRIVER_FLAGS_USER_MASK 0xFFFF0000

struct joystickb_driver_struct {
	const char* name; /**< Name of the driver */
	const device* device_map; /**< List of supported devices */

	/** Load the configuration options. Call before init() */
	video_error (*load)(struct conf_context* context);

	/** Register the load options. Call before load(). */
	void (*reg)(struct conf_context* context);

	video_error (*init)(int device_id); /**< Initialize the driver */
	void (*done)(void); /**< Deinitialize the driver */

	unsigned (*flags)(void); /**< Get the capabilities of the driver */

	unsigned (*count_get)(void);
	unsigned (*stick_count_get)(unsigned j);
	unsigned (*stick_axe_count_get)(unsigned j, unsigned s);
	unsigned (*button_count_get)(unsigned j);
	const char* (*stick_name_get)(unsigned j, unsigned s);
	const char* (*stick_axe_name_get)(unsigned j, unsigned s, unsigned a);
	const char* (*button_name_get)(unsigned j, unsigned b);
	unsigned (*button_get)(unsigned j, unsigned b);
	unsigned (*stick_axe_digital_get)(unsigned j, unsigned s, unsigned a, unsigned d);
	int (*stick_axe_analog_get)(unsigned j, unsigned s, unsigned a);
	void (*calib_start)(void);
	const char* (*calib_next)(void);
	void (*poll)(void);
};

typedef struct joystickb_driver_struct joystickb_driver;

#define JOYSTICK_DRIVER_MAX 8

struct joystickb_state_struct {
	video_bool is_initialized_flag;
	video_bool is_active_flag;
	unsigned driver_mac;
	joystickb_driver* driver_map[JOYSTICK_DRIVER_MAX];
	joystickb_driver* driver_current;
	char name[DEVICE_NAME_MAX];
};

extern struct joystickb_state_struct joystickb_state;

void joystickb_reg(struct conf_context* config_context, video_bool auto_detect);
void joystickb_reg_driver(struct conf_context* config_context, joystickb_driver* driver);
video_error joystickb_load(struct conf_context* config_context);
video_error joystickb_init(void);
void joystickb_done(void);
void joystickb_abort(void);

static __inline__ unsigned joystickb_count_get(void) {
	assert( joystickb_state.is_active_flag );

	return joystickb_state.driver_current->count_get();
}

static __inline__ unsigned joystickb_stick_count_get(unsigned j) {
	assert( joystickb_state.is_active_flag );

	return joystickb_state.driver_current->stick_count_get(j);
}

static __inline__ unsigned joystickb_stick_axe_count_get(unsigned j, unsigned s) {
	assert( joystickb_state.is_active_flag );

	return joystickb_state.driver_current->stick_axe_count_get(j,s);
}

static __inline__ unsigned joystickb_button_count_get(unsigned j) {
	assert( joystickb_state.is_active_flag );

	return joystickb_state.driver_current->button_count_get(j);
}

static __inline__ const char* joystickb_stick_name_get(unsigned j, unsigned s) {
	assert( joystickb_state.is_active_flag );

	return joystickb_state.driver_current->stick_name_get(j,s);
}

static __inline__ const char* joystickb_stick_axe_name_get(unsigned j, unsigned s, unsigned a) {
	assert( joystickb_state.is_active_flag );

	return joystickb_state.driver_current->stick_axe_name_get(j,s,a);
}

static __inline__ const char* joystickb_button_name_get(unsigned j, unsigned b) {
	assert( joystickb_state.is_active_flag );

	return joystickb_state.driver_current->button_name_get(j,b);
}

static __inline__ unsigned joystickb_button_get(unsigned j, unsigned b) {
	assert( joystickb_state.is_active_flag );

	return joystickb_state.driver_current->button_get(j, b);
}

static __inline__ unsigned joystickb_stick_axe_digital_get(unsigned j, unsigned s, unsigned a, unsigned d) {
	assert( joystickb_state.is_active_flag );

	return joystickb_state.driver_current->stick_axe_digital_get(j,s,a,d);
}

static __inline__ int joystickb_stick_axe_analog_get(unsigned j, unsigned s, unsigned a) {
	assert( joystickb_state.is_active_flag );

	return joystickb_state.driver_current->stick_axe_analog_get(j,s,a);
}

static __inline__ void joystickb_calib_start(void) {
	assert( joystickb_state.is_active_flag );

	return joystickb_state.driver_current->calib_start();
}

static __inline__ const char* joystickb_calib_next(void) {
	assert( joystickb_state.is_active_flag );

	return joystickb_state.driver_current->calib_next();
}

static __inline__ void joystickb_poll(void) {
	assert( joystickb_state.is_active_flag );

	joystickb_state.driver_current->poll();
}

static __inline__ const char* joystickb_name(void) {
	return joystickb_state.name;
}

#ifdef __cplusplus
}
#endif

#endif