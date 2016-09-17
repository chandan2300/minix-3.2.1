#include "pm.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct plog
{
	int p_id;
	time_t start_t;
	time_t end_t;
} plog;

typedef struct
{
	plog* arr[PLOG_BUFFER_SIZE];
	int cur_index;
	size_t size;
} circularBuffer;

circularBuffer* buffer;

bool started = false;

/* Entry point into functionality */
int do_plog()
{
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
	if (started)
		return (EXIT_FAILURE);

	plog_clear();
	started = true;
	return EXIT_SUCCESS;
}

/* Stops process logger process */
int plog_stop()
{
	if (!started)
		return (EXIT_FAILURE);

	started = false;
	return EXIT_SUCCESS;
}

/* Adds a new process to the buffer */
int log_start(int id)
{
	if (!started)
		return EXIT_FAILURE;

	plog* tmp = (plog*)calloc(1, sizeof(plog) );
	tmp->p_id = id;
	tmp->start_t = do_time();
	tmp->end_t = -1;
	buffer->arr[buffer->cur_index++] = tmp;
	++(buffer->size);
	if (buffer->cur_index == PLOG_BUFFER_SIZE)
		buffer->cur_index = 0;
	return (EXIT_SUCCESS);
}

/* Adds termination time */
int log_end(int id)
{
	plog* tmp = find_by_PID(id);
	if (tmp && started)
	{
		tmp->end_t = do_time();
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

/* Clears entire buffer */
int plog_clear()
{
	if (buffer)
	{
		/* For each value in the array we want to free the memory */
		for (int i = 0; i < buffer->size - 1; i++)
		{
			/* Sanity check for null pointers (may be unneccesary) */
			if (buffer->arr[i]){free(buffer->arr[i]);}
		}
		/* Finally free the pointer used for the buffer */
		free(buffer);
	}
	/* Allocate memory for the buffer */
	buffer = (circularBuffer*) calloc(1, sizeof(circularBuffer));
	return buffer == NULL;
}

/* Get current size of buffer */
int plog_get_size()
{
	if (!buffer)
		return EXIT_FAILURE;

	m_in.m2_i1 = buffer->size;
	
	return EXIT_SUCCESS;
}

/* Get process by PID */
int plog_PIDget()
{
	plog* found = find_by_PID(m_in.m1_i2);
	fprintf(stderr, found->p_id);
	if (found)
	{
		m_in.m2_l1 = found->start_t;
		m_in.m2_l2 = found->end_t;
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

/* Get process by index */
int plog_IDXget()
{
	if (!buffer || buffer->size < m_in.m1_i3)
		return EXIT_FAILURE;

	m_in.m2_l1 = buffer->arr[m_in.m1_i3]->start_t;
	m_in.m2_l2 = buffer->arr[m_in.m1_i3]->end_t;
	m_in.m1_i2 = buffer->arr[m_in.m1_i3]->p_id;

	return EXIT_SUCCESS;
}

plog* find_by_PID(int id)
{
	if (!buffer)
		return NULL;
	for (int i = 0; i < buffer->size - 1; i++)
	{
		if (id == buffer->arr[i]->p_id)
			return buffer->arr[i];
	}
	return NULL;
}