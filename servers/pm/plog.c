#include "pm.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct plog
{
	int p_id;
	time_t start_t;
	time_t end_t;
} plog;

typedef struct circularBuffer
{
	int cur_index;
	size_t size;
	plog* arr[PLOG_BUFFER_SIZE];
} circularBuffer;

circularBuffer buffer;

bool dirtyBuf = true;
bool started = false;

/* Entry point into functionality */
int do_plog()
{
	printf("In do_plog\n");
	switch (m_in.m1_i1) {
	case PLOG_START:
		return plog_start();
	case PLOG_STOP:
		return plog_stop();
	case PLOG_RESETBUF:
		return plog_clear();
	case PLOG_GETBYIDX:
		return plog_IDXget();
	case PLOG_GETBYPID:
		return plog_PIDget();
	case PLOG_GETSIZE:
		return plog_get_size();
	}
	return (EXIT_FAILURE);
}

/* Starts process logger process */
int plog_start()
{
	if (!started)
	{
		printf("Starting\n");
		if (dirtyBuf)
			init_buffer();
		plog_clear();
		started = true;
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

/* Stops process logger process */
int plog_stop()
{
	if (!started)
		return (EXIT_FAILURE);

	printf("Stopping\n");

	started = false;
	return EXIT_SUCCESS;
}

/* Adds a new process to the buffer */
int log_start(int id)
{
	if (started)
	{
		printf("Logging Start\n");

		plog* tmp = buffer.arr[buffer.cur_index];
		if (tmp)
		{
			tmp->p_id = id;
			tmp->start_t = do_time();
			tmp->end_t = -1;
		}
		else
		{
			tmp = (plog*)malloc(sizeof(plog));
			tmp->p_id = id;
			tmp->start_t = do_time();
			tmp->end_t = -1;
			buffer.size += 1;
			buffer.arr[buffer.cur_index] = tmp;
		}
		++buffer.cur_index;
		if (buffer.cur_index == PLOG_BUFFER_SIZE)
			buffer.cur_index = 0;
		return (EXIT_SUCCESS);
	}
	return EXIT_FAILURE;
}

/* Adds termination time */
int log_end(int id)
{
	if (started)
	{
		printf("Logging End\n");
		plog* tmp = find_by_PID(id);
		if (tmp)
		{
			tmp->end_t = do_time();
			return EXIT_SUCCESS;
		}
	}

	return EXIT_FAILURE;
}

/* Clears entire buffer */
int plog_clear()
{
	printf("Clearing\n");
	if (started)
	{
		/* For each value in the array we want to free the memory */
		for (int i = 0; i < buffer.size; i++)
		{
			/* Sanity check for null pointers (may be unneccesary) */
			if (buffer.arr[i]) { free(buffer.arr[i]); }
		}
	}
	buffer.size = 0;
	buffer.cur_index = 0;
	return EXIT_SUCCESS;
}

/* Get current size of buffer */
int plog_get_size()
{
	printf("Getting log size.");
	m_in.m2_i1 = buffer.size;
	
	return EXIT_SUCCESS;
}

/* Get process by PID */
int plog_PIDget()
{
	plog* found = find_by_PID(m_in.m1_i2);
	if (found)
	{
		printf("in PIDget if found: %p", found);
		m_in.m2_l1 = found->start_t;
		m_in.m2_l2 = found->end_t;
		return EXIT_SUCCESS;
	}
	else
	{
		printf("in PIDget else: %p", found);
		m_in.m2_l1 = -1;
		m_in.m2_l2 = -1;
		return EXIT_FAILURE;
	}
}

/* Get process by index */
int plog_IDXget()
{
	if (buffer.size > m_in.m1_i3 && m_in.m1_i3 >= 0)
	{
		m_in.m2_l1 = buffer.arr[m_in.m1_i3]->start_t;
		m_in.m2_l2 = buffer.arr[m_in.m1_i3]->end_t;
		m_in.m1_i2 = buffer.arr[m_in.m1_i3]->p_id;

		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

plog* find_by_PID(int id)
{
	for (int i = 0; i < buffer.size; i++)
	{
		if (buffer.arr[i])
		{
			if (id == buffer.arr[i]->p_id)
			{
				return buffer.arr[i];
			}
		}
	}
	return NULL;
}

void init_buffer()
{
	printf("Starting Buffer Init\n");
	for (int i = 0; i < PLOG_BUFFER_SIZE; i++)
	{
		buffer.arr[i] = NULL;
	}
	dirtyBuf = false;
}