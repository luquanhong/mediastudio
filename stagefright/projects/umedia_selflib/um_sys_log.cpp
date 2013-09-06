#include "um_sys_log.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <android/log.h>

#define LOG_BUFFER_SIZE		4096 //1024

typedef struct LogContext
{
	int type, fd;
	pthread_mutex_t mutex;
	char buf[0];
}LogContext;

static LogContext* g_log = NULL;

int um_log_init(int type, const char* file, const char* ip)
{
	if(!g_log && (((type&LOG_IO_FILE) && file) || (type&LOG_IO_CONSOLE)))
	{
		if((g_log = (LogContext*)malloc(sizeof(LogContext) + LOG_BUFFER_SIZE)) != NULL)
		{
			g_log->fd = -1;
			g_log->type = type;
			if(g_log->type & LOG_IO_FILE)
			{
				if((g_log->fd = open(file, O_CREAT|O_RDWR, 0666)) < 0)
				{
					perror("open() failed.");
				}
			}
			pthread_mutex_init(&g_log->mutex, NULL);
		}
	}
	return g_log ? 0 : -1;
}

int um_log_fini(void)
{
	if(g_log == NULL) return -1;
	
	if(g_log->fd > 0)
	{
		close(g_log->fd);
		g_log->fd = -1;
	}
	pthread_mutex_destroy(&g_log->mutex);
	free(g_log);
	g_log = NULL;
	return 0;
}

int um_log_print(const char *fmt, ...)
{
	int len = 0;
	va_list arg_ptr;
	
	if(g_log == NULL) return -1;

	pthread_mutex_lock(&g_log->mutex);
	va_start(arg_ptr, fmt);
	vsprintf(g_log->buf, fmt, arg_ptr);
	va_end(arg_ptr);
	len = (int)strlen(g_log->buf);
	if(g_log->type & LOG_IO_CONSOLE)
	{
		if(len >= LOG_BUFFER_SIZE)
		{
			sprintf(g_log->buf, "[%s:%d] error: log(len=%d) is too long.\r\n", __func__, __LINE__, len);
			__android_log_print(ANDROID_LOG_ERROR, "UM", g_log->buf);
			exit(1);
		}
		else
		{
			__android_log_print(ANDROID_LOG_ERROR, "UM", g_log->buf);
		}
	}
	if((g_log->type&LOG_IO_FILE) && g_log->fd>0)
	{
		if(len >= LOG_BUFFER_SIZE)
		{
			sprintf(g_log->buf, "[%s:%d] error: log(len=%d) is too long.\r\n", __func__, __LINE__, len);
			write(g_log->fd, g_log->buf, strlen(g_log->buf));
			exit(1);
		}
		else
		{
			write(g_log->fd, g_log->buf, len);
		}
	}
	pthread_mutex_unlock(&g_log->mutex);
	return 0;
}
