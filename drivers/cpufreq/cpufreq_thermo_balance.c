// SPDX-License-Identifier: GPL-2.0
/*
 * Thermo Balance Governor - Based on Interactive Governor Logic
 * Focus: Lower heat, preserve battery, keep UI smooth
 */

#include <linux/cpufreq.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/notifier.h>
#include <linux/suspend.h>
#include <linux/input.h>

#define BOOST_FREQ_STEP 200000
#define FREQ_DROP_STEP 100000
#define IDLE_TIMEOUT_MS 250
#define BOOST_TIMEOUT_MS 1500
#define LOW_BATTERY_THRESHOLD 20

struct thermo_cpuinfo {
	struct cpufreq_policy *policy;
	struct timer_list idle_timer;
	struct timer_list boost_timer;
	struct mutex lock;
	unsigned int target_freq;
	bool boosted;
	bool screen_on;
};

static struct thermo_cpuinfo *g_info;
static int battery_level = 100;
static bool is_charging = false;

static void apply_freq(struct thermo_cpuinfo *info, unsigned int freq)
{
	if (!info || !info->policy)
		return;

	freq = clamp(freq, info->policy->min, info->policy->max);
	cpufreq_driver_target(info->policy, freq, CPUFREQ_RELATION_L);
	info->target_freq = freq;
}

static void boost_timeout(struct timer_list *t)
{
	struct thermo_cpuinfo *info = from_timer(info, t, boost_timer);

	mutex_lock(&info->lock);
	if (info->boosted) {
		info->boosted = false;
		apply_freq(info, info->policy->min + FREQ_DROP_STEP);
	}
	mutex_unlock(&info->lock);
}

static void idle_handler(struct timer_list *t)
{
	struct thermo_cpuinfo *info = from_timer(info, t, idle_timer);

	mutex_lock(&info->lock);
	if (!info->boosted) {
		unsigned int drop = FREQ_DROP_STEP;

		if (!is_charging && battery_level <= LOW_BATTERY_THRESHOLD)
			drop += 50000; // Save more battery when low

		if (!info->screen_on)
			drop += 100000;

		if (info->target_freq > info->policy->min + drop)
			apply_freq(info, info->target_freq - drop);
	}
	mod_timer(&info->idle_timer, jiffies + msecs_to_jiffies(IDLE_TIMEOUT_MS));
	mutex_unlock(&info->lock);
}

static int thermo_input_event(struct input_handle *handle, unsigned int type,
                              unsigned int code, int value)
{
	if (type == EV_KEY || type == EV_ABS || type == EV_REL) {
		struct thermo_cpuinfo *info = g_info;

		mutex_lock(&info->lock);
		if (!info->boosted) {
			unsigned int new_freq = info->policy->max;
			apply_freq(info, new_freq);
			info->boosted = true;
			mod_timer(&info->boost_timer, jiffies + msecs_to_jiffies(BOOST_TIMEOUT_MS));
		}
		mutex_unlock(&info->lock);
	}
	return 0;
}

static struct input_handle thermo_input_handle;
static struct input_handler thermo_input_handler = {
	.event = thermo_input_event,
	.name = "thermo_input_handler",
	.id_table = NULL,
};

static int thermo_start(struct cpufreq_policy *policy)
{
	struct thermo_cpuinfo *info;

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->policy = policy;
	info->target_freq = policy->min + FREQ_DROP_STEP;
	info->boosted = false;
	info->screen_on = true;
	mutex_init(&info->lock);

	timer_setup(&info->idle_timer, idle_handler, 0);
	timer_setup(&info->boost_timer, boost_timeout, 0);

	mod_timer(&info->idle_timer, jiffies + msecs_to_jiffies(IDLE_TIMEOUT_MS));

	g_info = info;

	input_register_handler(&thermo_input_handler);

	return 0;
}

static void thermo_stop(struct cpufreq_policy *policy)
{
	struct thermo_cpuinfo *info = g_info;

	del_timer_sync(&info->idle_timer);
	del_timer_sync(&info->boost_timer);
	input_unregister_handler(&thermo_input_handler);
	kfree(info);
}

static struct cpufreq_governor cpufreq_gov_thermo_balance = {
	.name = "thermo_balance",
	.init = thermo_start,
	.exit = thermo_stop,
	.owner = THIS_MODULE,
};

static int __init thermo_governor_init(void)
{
	return cpufreq_register_governor(&cpufreq_gov_thermo_balance);
}

static void __exit thermo_governor_exit(void)
{
	cpufreq_unregister_governor(&cpufreq_gov_thermo_balance);
}

MODULE_AUTHOR("Jayzee");
MODULE_DESCRIPTION("Thermo Balance CPUFreq Governor");
MODULE_LICENSE("GPL");

fs_initcall(thermo_governor_init);
module_exit(thermo_governor_exit);
