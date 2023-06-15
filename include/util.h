/*
**             Copyright 2023 by Kvaser AB, Molndal, Sweden
**                         http://www.kvaser.com
**
** This software is dual licensed under the following two licenses:
** BSD-new and GPLv2. You may use either one. See the included
** COPYING file for details.
**
** License: BSD-new
** ==============================================================================
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the <organization> nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
** IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
**
**
** License: GPLv2
** ==============================================================================
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
**
**
** IMPORTANT NOTICE:
** ==============================================================================
** This source code is made available for free, as an open license, by Kvaser AB,
** for use with its applications. Kvaser AB does not accept any liability
** whatsoever for any third party patent or other immaterial property rights
** violations that may result from any usage of this source code, regardless of
** the combination of source code and various applications that it can be used
** in, or with.
**
** -----------------------------------------------------------------------------
*/

#ifndef UTIL_H
#define UTIL_H

#ifdef __linux__
#include <linux/usb.h>
#include <linux/jiffies.h>
#include <linux/ktime.h>
#include <linux/time.h>
#include <linux/version.h>
#endif /* __linux__ */

// Suppress warning about unused variable
#define NOT_USED(p) ((void)p)

#ifdef __linux__

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 1, 0)
#define static_assert(expr, ...)
#endif /* LINUX_VERSION_CODE < 5.1 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35)
static inline s64 ktime_to_ms(const ktime_t kt)
{
    struct timeval tv = ktime_to_timeval(kt);
    return (s64) tv.tv_sec * MSEC_PER_SEC + tv.tv_usec / USEC_PER_MSEC;
}
#endif /* LINUX_VERSION_CODE < 2.6.35 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
static inline s64 ktime_ms_delta(const ktime_t later, const ktime_t earlier)
{
    return ktime_to_ms(ktime_sub(later, earlier));
}
#endif /* LINUX_VERSION_CODE < 4.0 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
#define MSLEEP_SHORT(ms) (msleep(ms))
#else
// Sleep 1 - 10 ms with increased precision
#define MSLEEP_SHORT(ms) (usleep_range(1000 * (ms)-100, 1000 * (ms) + 100))
#endif /* LINUX_VERSION_CODE < 2.6.36 */

// Wrapper macros for portability
#define SYSTICK_ADD_MSEC(ms)          (jiffies + msecs_to_jiffies((ms)))
#define SYSTICK_IS_AFTER(systick)     (time_after(jiffies, (systick)))
#define SYSTICK_IS_AFTER_EQ(systick)  (time_after_eq(jiffies, (systick)))
#define SYSTICK_IS_BEFORE(systick)    (time_before(jiffies, (systick)))
#define SYSTICK_IS_BEFORE_EQ(systick) (time_before_eq(jiffies, (systick)))

unsigned int get_usb_root_hub_id(struct usb_device *udev);
#else
// TODO: Define macros for Windows

// Wrapper macros for portability, millisecond resolution or better required!
#define SYSTICK_ADD_MSEC(ms)
#define SYSTICK_IS_AFTER(systick)
#define SYSTICK_IS_AFTER_EQ(systick)
#define SYSTICK_IS_BEFORE(systick)
#define SYSTICK_IS_BEFORE_EQ(systick)
#endif /* __linux__ */

void packed_EAN_to_BCD_with_csum(unsigned char *ean, unsigned char *bcd);
unsigned int calculateCRC32(void *buf, unsigned int bufsiz);

#endif /* UTIL_H */
