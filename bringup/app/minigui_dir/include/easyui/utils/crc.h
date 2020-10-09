/*
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef _UBOOT_CRC_H
#define _UBOOT_CRC_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;
typedef unsigned int uint;

typedef unsigned char  Bytef;  /* 8 bits */
typedef unsigned int   uInt;  /* 16 bits or more */

//__LITTLE_ENDIAN
#define cpu_to_le32(x)		(x)
#define le32_to_cpu(x)		(x)

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* lib/crc32.c */
uint32_t crc32 (uint32_t, const unsigned char *, uint);
uint32_t crc32_wd (uint32_t, const unsigned char *, uint, uint);
uint32_t crc32_no_comp (uint32_t, const unsigned char *, uint);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _UBOOT_CRC_H */
