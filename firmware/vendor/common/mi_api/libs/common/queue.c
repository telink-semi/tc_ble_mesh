#include <string.h>
#include "queue.h"
#include "mible_port.h"

mible_status_t queue_init(queue_t *q, void *buf, char queue_size, char item_size)
{
    if (buf == NULL || q == NULL)
        return MI_ERR_INVALID_PARAM;

    q->buf = buf;
    q->size = queue_size;
    q->offset = item_size;
    q->rd_pos = 0;
    q->wr_pos = 0;
    q->cnt = 0;

    return MI_SUCCESS;
}

mible_status_t enqueue(queue_t *q, void *in)
{
    if (q->cnt == q->size) {
        return MI_ERR_NO_MEM;
    }
    CRITICAL_SECTION_ENTER();
    /* q->buf[q->wr_ptr++] = in; */
    memcpy((char*)q->buf + q->wr_pos * q->offset, in, q->offset);
    q->wr_pos = (q->wr_pos + 1) % q->size;
    q->cnt++;
    CRITICAL_SECTION_EXIT();
    return MI_SUCCESS;
}

mible_status_t dequeue(queue_t *q, void *out)
{
    if (q->cnt > 0) {
        CRITICAL_SECTION_ENTER();
        /* *out = q->buf[q->rd_ptr++]; */
        memcpy(out, (char*)q->buf + q->rd_pos * q->offset, q->offset);
        q->rd_pos = (q->rd_pos + 1) % q->size;
        q->cnt--;
        CRITICAL_SECTION_EXIT();
        return MI_SUCCESS;
    } else
        return MI_ERR_NOT_FOUND;
}

void queue_flush(queue_t *q)
{
    q->rd_pos = 0;
    q->wr_pos = 0;
    q->cnt = 0;
}
