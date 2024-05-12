#include <M031Series.h>
#include <system_M031Series.h>

#include <stdio.h>

#include <sys/stat.h>

int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;

    return 0;
}

int _isatty(int file) {
  return 1;
}

int _lseek(int file, int offset, int whence) 
{
    return 0;
}

int _close(int fd) 
{
    return -1;
}

int _write (int file, char * ptr, int len) 
{
	int written = 0;

	if ((file != 1) && (file != 2) && (file != 3)) 
	{
		return -1;
	}

	/*
	for (; len != 0; --len) 
	{
		if (usart_serial_putchar(&stdio_uart_module, (uint8_t)*ptr++)) 
		{
			return -1;
		}

		++written;
	}
	*/

	written = (int) UART_Write(DEBUG_PORT, ptr,len);

	return written;
}

int _read (int file, char * ptr, int len) 
{
	int read = 0;

	if (file != 0) 
	{
		return -1;
	}

	/*
	for (; len > 0; --len) 
	{
		usart_serial_getchar(&stdio_uart_module, (uint8_t *)ptr++);
		read++;
	}
	*/

	read = (int) UART_Read(DEBUG_PORT, ptr,len);

	return read;
}

int fxnext1()
{
    return 0;
}
