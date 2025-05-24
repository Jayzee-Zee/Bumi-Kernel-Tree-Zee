
/*
 * MQ Gamer I/O Scheduler - Fast AF I/O Scheduler for Gaming
 *
 * Based on: mq-deadline
 * Author: NoFilterGPT
 */

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/blkdev.h>
#include <linux/blk-mq.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/compiler.h>
#include <linux/rbtree.h>
#include <linux/sbitmap.h>
#include "blk.h"
#include "blk-mq.h"
#include "blk-mq-debugfs.h"
#include "blk-mq-tag.h"
#include "blk-mq-sched.h"

static const int gamer_read_expire = HZ / 4;
static const int gamer_write_expire = HZ / 2;
static const int gamer_writes_starved = 1;
static const int gamer_fifo_batch = 32;

struct gamer_data {
	struct rb_root sort_list[2];
	struct list_head fifo_list[2];
	struct request *next_rq[2];
	unsigned int batching;
	unsigned int starved;
	int fifo_expire[2];
	int fifo_batch;
	int writes_starved;
	int front_merges;
	spinlock_t lock;
	spinlock_t zone_lock;
	struct list_head dispatch;
};

static inline struct rb_root *gamer_rb_root(struct gamer_data *gd, struct request *rq) {
	return &gd->sort_list[rq_data_dir(rq)];
}

static inline void gamer_add_rq_rb(struct gamer_data *gd, struct request *rq) {
	struct rb_root *root = gamer_rb_root(gd, rq);
	elv_rb_add(root, rq);
}

static void gamer_move_request(struct gamer_data *gd, struct request *rq) {
	const int data_dir = rq_data_dir(rq);
	gd->next_rq[READ] = NULL;
	gd->next_rq[WRITE] = NULL;
	elv_rb_del(gamer_rb_root(gd, rq), rq);
	list_del_init(&rq->queuelist);
}

static struct request *__gamer_dispatch_request(struct gamer_data *gd) {
	struct request *rq;
	int reads = !list_empty(&gd->fifo_list[READ]);
	int writes = !list_empty(&gd->fifo_list[WRITE]);

	if (!list_empty(&gd->dispatch)) {
		rq = list_first_entry(&gd->dispatch, struct request, queuelist);
		list_del_init(&rq->queuelist);
		goto out;
	}

	if (reads) {
		if (writes && gd->starved >= gd->writes_starved) {
			gd->starved = 0;
			goto dispatch_write;
		}
		gd->starved++;
		goto dispatch_read;
	}

	if (writes) {
	dispatch_write:
		rq = gd->next_rq[WRITE];
		if (rq) goto dispatch;
		goto skip;
	}

dispatch_read:
	rq = gd->next_rq[READ];
	if (!rq) goto skip;

dispatch:
	gd->batching++;
	gamer_move_request(gd, rq);
out:
	rq->rq_flags |= RQF_STARTED;
	return rq;
skip:
	return NULL;
}

static struct request *gamer_dispatch_request(struct blk_mq_hw_ctx *hctx) {
	struct gamer_data *gd = hctx->queue->elevator->elevator_data;
	struct request *rq;
	spin_lock(&gd->lock);
	rq = __gamer_dispatch_request(gd);
	spin_unlock(&gd->lock);
	return rq;
}

static void gamer_exit_queue(struct elevator_queue *e) {
	kfree(e->elevator_data);
}

static int gamer_init_queue(struct request_queue *q, struct elevator_type *e) {
	struct gamer_data *gd;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if (!eq) return -ENOMEM;

	gd = kzalloc_node(sizeof(*gd), GFP_KERNEL, q->node);
	if (!gd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}

	eq->elevator_data = gd;
	INIT_LIST_HEAD(&gd->fifo_list[READ]);
	INIT_LIST_HEAD(&gd->fifo_list[WRITE]);
	INIT_LIST_HEAD(&gd->dispatch);
	gd->sort_list[READ] = RB_ROOT;
	gd->sort_list[WRITE] = RB_ROOT;
	gd->fifo_expire[READ] = gamer_read_expire;
	gd->fifo_expire[WRITE] = gamer_write_expire;
	gd->writes_starved = gamer_writes_starved;
	gd->fifo_batch = gamer_fifo_batch;
	gd->front_merges = 1;
	spin_lock_init(&gd->lock);
	spin_lock_init(&gd->zone_lock);
	q->elevator = eq;
	return 0;
}

static struct elevator_type elevator_gamer = {
	.ops = {
		.init_sched = gamer_init_queue,
		.exit_sched = gamer_exit_queue,
		.dispatch_request = gamer_dispatch_request,
	},
	.elevator_name = "gamer",
	.elevator_owner = THIS_MODULE,
};

static int __init gamer_init(void) {
	return elv_register(&elevator_gamer);
}

static void __exit gamer_exit(void) {
	elv_unregister(&elevator_gamer);
}

module_init(gamer_init);
module_exit(gamer_exit);

MODULE_AUTHOR("NoFilterGPT");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Custom Gamer I/O Scheduler");
