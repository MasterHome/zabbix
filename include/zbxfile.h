/*
** Zabbix
** Copyright (C) 2001-2022 Zabbix SIA
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

#ifndef ZABBIX_ZBXFILE_H
#define ZABBIX_ZBXFILE_H


#if defined(_WINDOWS) || defined(__MINGW32__)
#	define zbx_open(pathname, flags)	__zbx_open(pathname, flags | O_BINARY)
#	define PATH_SEPARATOR	'\\'
#else
#	define zbx_open(pathname, flags)	open(pathname, flags)
#	define PATH_SEPARATOR	'/'
#endif

#if defined(_WINDOWS) || defined(__MINGW32__)
int     __zbx_open(const char *pathname, int flags);
#endif

typedef struct
{
	zbx_fs_time_t	modification_time;	/* time of last modification */
	zbx_fs_time_t	access_time;		/* time of last access */
	zbx_fs_time_t	change_time;		/* time of last status change */
}
zbx_file_time_t;

int	zbx_get_file_time(const char *path, int sym, zbx_file_time_t *time);
void	find_cr_lf_szbyte(const char *encoding, const char **cr, const char **lf, size_t *szbyte);
int	zbx_read(int fd, char *buf, size_t count, const char *encoding);
int	zbx_is_regular_file(const char *path);
char	*zbx_fgets(char *buffer, int size, FILE *fp);
int	zbx_write_all(int fd, const char *buf, size_t n);


typedef struct {
	ULONGLONG	LowPart;
	ULONGLONG	HighPart;
} ZBX_EXT_FILE_ID_128;

typedef struct {
	ULONGLONG		VolumeSerialNumber;
	ZBX_EXT_FILE_ID_128	FileId;
} ZBX_FILE_ID_INFO;

#endif /* ZABBIX_ZBXFILE_H */
