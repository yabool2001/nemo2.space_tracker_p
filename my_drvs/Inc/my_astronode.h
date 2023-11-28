/*
 * my_astronode.h
 *
 *  Created on: Oct 23, 2023
 *      Author: mzeml
 */

#ifndef ASTROCAST_INC_MY_ASTRONODE_H_
#define ASTROCAST_INC_MY_ASTRONODE_H_

#define MY_TIMER			&htim6

#include "main.h"

void my_astro_write_coordinates ( int32_t , int32_t ) ;
bool my_astro_add_payload_2_queue ( uint16_t , char* ) ;
bool my_astro_read_evt_reg ( void ) ;
bool my_astro_log ( void ) ;

#endif /* ASTROCAST_INC_MY_ASTRONODE_H_ */
