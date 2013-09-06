/*
 * berrno.h
 *
 *  Created on: 2013-4-9
 *      Author: Administrator
 */

#ifndef BERRNO_H_
#define BERRNO_H_

#ifdef ENOENT
#undef ENOENT
#endif
#define ENOENT 2

#ifdef ENOMEM
#undef ENOMEM
#endif
#define ENOMEM 12

#ifdef	EPIPE
#undef	EPIPE
#endif
#define	EPIPE	32


#endif /* BERRNO_H_ */
