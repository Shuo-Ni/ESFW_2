#define NDEBUG
// This code comes from 
// https://github.com/embeddedartistry/embedded-resources/tree/master/examples/c/circular_buffer 
// It was published under the Creative Commons 0 License: free use 


#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "../FrameworkHeaders/circular_buffer.h"

// The definition of our circular buffer structure is hidden from the user
struct circular_buf_t {
  uint8_t * buffer;
	size_t head;
	size_t tail;
	size_t max; //of the buffer
};

// an array of buffer structures that we use to allow static memory allocation
circular_buf_t frameworkCircularBuffers[MAX_CIRC_BUFFERS];
uint8_t numBuffersAllocated = 0;

#pragma mark - Private Functions -

static void advance_pointer(cbuf_handle_t cbuf)
{
	assert(cbuf);

	if(circular_buf_full(cbuf))
  {
    if(++(cbuf->tail) == cbuf->max)
    {
      cbuf->tail = 0;
    }
  }

  if(++(cbuf->head) == cbuf->max)
	{
		cbuf->head = 0;
	}
}

static void retreat_pointer(cbuf_handle_t cbuf)
{
	assert(cbuf);

	if(++(cbuf->tail) == cbuf->max)
	{
		cbuf->tail = 0;
	}
}

#pragma mark - APIs -

cbuf_handle_t circular_buf_init(uint8_t* buffer, size_t size)
{
	assert(buffer && size > 1);

	// buf_handle_t cbuf = malloc(sizeof(circular_buf_t));
  // The original code, above, malloc'd space, but I want a static allocation
  // so we'll use an array of structures and make sure that we don't allocate
  // more than we have room for.
  
	cbuf_handle_t cbuf = &frameworkCircularBuffers[numBuffersAllocated];
  numBuffersAllocated++;
	assert(MAX_CIRC_BUFFERS >= numBuffersAllocated);

	cbuf->buffer = buffer;
	cbuf->max = size;
	circular_buf_reset(cbuf);

	assert(circular_buf_empty(cbuf));

	return cbuf;
}

// since we are using static buffers, we can't free them
#if 0
void circular_buf_free(cbuf_handle_t cbuf)
{
	assert(cbuf);
	free(cbuf);
}
#endif

void circular_buf_reset(cbuf_handle_t cbuf)
{
  assert(cbuf);

  cbuf->head = 0;
  cbuf->tail = 0;
}

size_t circular_buf_size(cbuf_handle_t cbuf)
{
	assert(cbuf);

	size_t size = cbuf->max;

	if(!circular_buf_full(cbuf))
	{
		if(cbuf->head >= cbuf->tail)
		{
			size = (cbuf->head - cbuf->tail);
		}
		else
		{
			size = (cbuf->max + cbuf->head - cbuf->tail);
		}

	}

	return size;
}

size_t circular_buf_capacity(cbuf_handle_t cbuf)
{
	assert(cbuf);

	return cbuf->max;
}

void circular_buf_put(cbuf_handle_t cbuf, uint8_t data)
{
	assert(cbuf && cbuf->buffer);

  cbuf->buffer[cbuf->head] = data;

  advance_pointer(cbuf);
}

int circular_buf_put2(cbuf_handle_t cbuf, uint8_t data)
{
  int r = -1;

  assert(cbuf && cbuf->buffer);

  if(!circular_buf_full(cbuf))
  {
    cbuf->buffer[cbuf->head] = data;
    advance_pointer(cbuf);
    r = 0;
  }

  return r;
}

int circular_buf_get(cbuf_handle_t cbuf, uint8_t * data)
{
  assert(cbuf && data && cbuf->buffer);

  int r = -1;

  if(!circular_buf_empty(cbuf))
  {
    *data = cbuf->buffer[cbuf->tail];
    retreat_pointer(cbuf);

    r = 0;
  }

  return r;
}

bool circular_buf_empty(cbuf_handle_t cbuf)
{
	assert(cbuf);

  return cbuf->head == cbuf->tail;
}

bool circular_buf_full(circular_buf_t* cbuf)
{
	// We need to handle the wraparound case
	size_t head = cbuf->head + 1;
	if(head == cbuf->max)
	{
		head = 0;
	}

	return head == cbuf->tail;
}