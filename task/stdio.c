#include "x86playground/type.h"

/* Video 在从 0 开始的第一个 4m
 * 任务直接操作 Video
 */
#define VIDEO_START 0xB8000
#define COLOR 0x07

i32_t puts(const char *s)
{
	static i32_t tty_pos = 320;

	u8_t *write_pos;
	u8_t write;

	while (*s != '\0') {
		write = *s++;

		write_pos = (u8_t *) VIDEO_START + tty_pos * 2;
		*write_pos = write;
		*(write_pos + 1) = COLOR;

		if (++tty_pos >= 25 * 80) {
			tty_pos = 0;
		}
	}

	return 0;
}
