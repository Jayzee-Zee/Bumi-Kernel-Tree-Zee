// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cpufreq.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/power_supply.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

#define GOVERNOR_NAME "thermo_balance"

static struct cpufreq_policy *global_policy;
static struct input_handler thermo_input_handler;
static struct timer_list thermo_timer;
static struct work_struct thermo_work;

static unsigned int touch_boost_freq = 0;
static unsigned int idle_freq = 0;
static unsigned int battery_threshold = 20;
static bool on_battery = true;

static void thermo_update_freq(struct work_struct *work)
{
	unsigned int target_freq = idle_freq;

	if (touch_boost_freq && on_battery) {
		target_freq = touch_boost_freq;
	}

	if (global_policy)
		__cpufreq_driver_target(global_policy, target_freq, CPUFREQ_RELATION_H);
}

static void thermo_timer_callback(struct timer_list *t)
{
	schedule_work(&thermo_work);
	mod_timer(&thermo_timer, jiffies + msecs_to_jiffies(500));
}

static void thermo_input_event(struct input_handle *handle,
                               unsigned int type,
                               unsigned int code,
                               int value)
{
	if (type == EV_ABS || type == EV_KEY)
		schedule_work(&thermo_work);
}

static int thermo_power_event(struct notifier_block *nb,
                              unsigned long event,
                              void *data)
{
	struct power_supply *psy = data;
	union power_supply_propval val;

	if (!psy || !psy->get_property)
		return NOTIFY_OK;

	if (psy->get_property(psy, POWER_SUPPLY_PROP_PRESENT, &val))
		return NOTIFY_OK;

	on_battery = !val.intval;

	return NOTIFY_OK;
}

static struct notifier_block thermo_psy_notifier = {
	.notifier_call = thermo_power_event,
};

static int thermo_input_connect(struct input_handler *handler,
                                struct input_dev *dev,
                                const struct input_device_id *id)
{
	struct input_handle *handle;
	int error;

	handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
	if (!handle)
		return -ENOMEM;

	handle->dev = dev;
	handle->handler = handler;
	handle->name = "thermo_input_handle";

	error = input_register_handle(handle);
	if (error)
		goto err_free;

	error = input_open_device(handle);
	if (error)
		goto err_unregister;

	return 0;

err_unregister:
	input_unregister_handle(handle);
err_free:
	kfree(handle);
	return error;
}

static void thermo_input_disconnect(struct input_handle *handle)
{
	input_close_device(handle);
	input_unregister_handle(handle);
	kfree(handle);
}

static const struct input_device_id thermo_ids[] = {
	{ .driver_info = 1 }, /* Match all devices */
	{ },
};

static struct input_handler thermo_input_handler = {
	.event = thermo_input_event,
	.connect = thermo_input_connect,
	.disconnect = thermo_input_disconnect,
	.name = "thermo_input_handler",
	.id_table = thermo_ids,
};

static int thermo_start(struct cpufreq_policy *policy)
{
	global_policy = policy;

	touch_boost_freq = policy->max;
	idle_freq = policy->min;

	timer_setup(&thermo_timer, thermo_timer_callback, 0);
	mod_timer(&thermo_timer, jiffies + msecs_to_jiffies(500));

	INIT_WORK(&thermo_work, thermo_update_freq);
	schedule_work(&thermo_work);

	power_supply_reg_notifier(&thermo_psy_notifier);
	input_register_handler(&thermo_input_handler);

	return 0;
}

static void thermo_stop(struct cpufreq_policy *policy)
{
	del_timer_sync(&thermo_timer);
	cancel_work_sync(&thermo_work);
	input_unregister_handler(&thermo_input_handler);
	power_supply_unreg_notifier(&thermo_psy_notifier);
	global_policy = NULL;
}

static struct cpufreq_governor thermo_gov = {
	.name = GOVERNOR_NAME,
	.owner = THIS_MODULE,
	.init = thermo_start,
	.exit = thermo_stop,
};

static int __init thermo_gov_init(void)
{
	return cpufreq_register_governor(&thermo_gov);
}

static void __exit thermo_gov_exit(void)
{
	cpufreq_unregister_governor(&thermo_gov);
}

module_init(thermo_gov_init);
module_exit(thermo_gov_exit);

MODULE_AUTHOR("Jayzee");
MODULE_DESCRIPTION("Thermo Balance CPUFreq Governor - Minimal Lag & Heat");
MODULE_LICENSE("GPL");
