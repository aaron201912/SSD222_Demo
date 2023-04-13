#include <sys/time.h>

#include "st_framequeue.h"



int frame_queue_init(frame_queue_t *f, int max_size)
{
    int i;
    memset(f, 0, sizeof(frame_queue_t));
    
    CheckFuncResult(pthread_mutex_init(&f->mutex, NULL));
    CheckFuncResult(pthread_cond_init(&f->cond,NULL));
    
    f->max_size = max_size;
    
    for (i = 0; i < f->max_size; i++)
    {
        if (!(f->queue[i].frame = malloc(FRAME_WIDTH * FRAME_HEIGHT * 1.5)))
            return -1;
        CheckFuncResult(pthread_mutex_init(&f->queue[i].mutex, NULL));
    }
    return 0;
}

void frame_queue_putbuf(frame_queue_t *f,char *frame)
{
    struct timeval timeEnqueue;
    int windex;
    windex = f->windex;

    //printf("frame_queue_putbuf windex=%d \n",f->windex);
    pthread_mutex_lock(&f->mutex);
    pthread_mutex_lock(&f->queue[f->windex].mutex);
	
    gettimeofday(&timeEnqueue, NULL);
	if(f->queue[f->windex].enqueueTime != 0)    
		printf("----> frame_queue_putbuf can't write  this buffer[%d] now\n",f->windex);
    f->queue[f->windex].enqueueTime = (int64_t)timeEnqueue.tv_sec * 1000000 + timeEnqueue.tv_usec;
    memcpy(f->queue[f->windex].frame,frame,FRAME_WIDTH * FRAME_HEIGHT * 1.5);
    f->queue[f->windex].buf_size = FRAME_WIDTH * FRAME_HEIGHT * 1.5;
    //printf("putbuf: %p\n",f->queue[f->windex].frame);

   //printf("frame_queue_putbuf windex=%d time:%lld\n",f->windex, f->queue[f->windex].enqueueTime);
   if (++f->windex == f->max_size)
    {
        f->windex = 0;
    }
    f->size++;

    pthread_mutex_unlock(&f->queue[windex].mutex);
    pthread_mutex_unlock(&f->mutex);
	pthread_cond_signal(&f->cond);
}

#if 0

// 向队列尾部压入一帧，只更新计数与写指针，因此调用此函数前应将帧数据写入队列相应位置
void frame_queue_push(frame_queue_t *f)
{
    if (++f->windex == f->max_size)
        f->windex = 0;
    pthread_mutex_lock(&f->mutex);
    f->size++;
    //printf("wake up frame queue size: %d\n",f->size);
    pthread_cond_signal(&f->cond);
    pthread_mutex_unlock(&f->mutex);
}
#endif

frame_t *frame_queue_peek_last(frame_queue_t *f,int wait_ms)
{
    struct timespec now_time;
    struct timespec out_time;
    unsigned long now_time_us;

    clock_gettime(CLOCK_MONOTONIC, &now_time);
    out_time.tv_sec = now_time.tv_sec;
    out_time.tv_nsec = now_time.tv_nsec;
    out_time.tv_sec += wait_ms/1000;   //ms 可能超1s

    now_time_us = out_time.tv_nsec/1000 + 1000*(wait_ms%1000); //计算us
    out_time.tv_sec += now_time_us/1000000; //us可能超1s
    now_time_us = now_time_us%1000000;
    out_time.tv_nsec = now_time_us * 1000;//us

    pthread_mutex_lock(&f->mutex);
    //printf("frame_queue_peek_last begin rindex=%d \n",f->rindex);
    //pthread_cond_timedwait(&f->cond,&f->mutex,&out_time);
    pthread_cond_wait(&f->cond,&f->mutex);
    pthread_mutex_lock(&f->queue[f->rindex].mutex);
    //printf("frame_queue_peek_last end rindex=%d \n",f->rindex);
    return &f->queue[f->rindex];
}

void frame_queue_next(frame_queue_t *f)
{
    pthread_mutex_unlock(&f->queue[f->rindex].mutex);
    //printf("queue rindex: %d\n",f->rindex);
    
	if(f->queue[f->rindex].enqueueTime == 0)    
		printf("----> frame_queue_putbuf can't read  this buffer[%d] now\n", f->rindex);
    memset(f->queue[f->rindex].frame, 0, FRAME_WIDTH * FRAME_HEIGHT * 1.5);
    f->queue[f->rindex].enqueueTime = 0;

    if (++f->rindex == f->max_size)
        f->rindex = 0;
    //pthread_mutex_lock(&f->mutex);
    f->size--;
    //printf("queue next size: %d\n",f->size);
    //pthread_cond_signal(&f->cond);
    pthread_mutex_unlock(&f->mutex);
}

void frame_queue_signal(frame_queue_t *f)
{
    pthread_mutex_lock(&f->mutex);
    printf("send frm signal\n");
    pthread_cond_signal(&f->cond);
    pthread_mutex_unlock(&f->mutex);
}

void frame_queue_flush(frame_queue_t *f)
{
    //printf("queue valid size : %d, rindex : %d\n", f->size, f->rindex);
    pthread_mutex_lock(&f->mutex);
    for (; f->size > 0; f->size --)
    {
        frame_t *vp = &f->queue[(f->rindex ++) % f->max_size];
        if(vp->frame)
        {
            free(vp->frame);
            vp->frame = NULL;
        }
        if (f->rindex >= f->max_size)
            f->rindex = 0;
    }
    f->rindex = 0;
    f->windex = 0;
    f->size   = 0;
    pthread_mutex_unlock(&f->mutex);
}

void frame_queue_destory(frame_queue_t *f)
{
    int i;
    for (i = 0; i < f->max_size; i++) {
        frame_t *vp = &f->queue[i];
        if(vp->frame)
        {
            free(vp->frame);
            vp->frame = NULL;
        }
    }
    pthread_mutex_destroy(&f->mutex);
    pthread_cond_destroy(&f->cond);
}







