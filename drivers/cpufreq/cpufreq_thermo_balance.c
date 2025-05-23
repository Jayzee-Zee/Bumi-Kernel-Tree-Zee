// SPDX-License-Identifier: GPL-2.0
#include <linux/cpufreq.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/power_supply.h>
#include <linux/slab.h>
#include <linux/jiffies.h>

#define GOV_NAME "thermaless_balance"
#define GOV_CHECK_INTERVAL (HZ / 2)

static unsigned int get_battery_capacity(void)
{
	struct power_supply *psy;
	union power_supply_propval val;
	unsigned int cap = 100;

	psy = power_supply_get_by_name("battery");
	if (!psy)
		return cap;

	if (!power_supply_get_property(psy, POWER_SUPPLY_PROP_CAPACITY, &val))
		cap = val.intval;

	power_supply_put(psy);
	return cap;
}

static void cpufreq_governor_adjust(struct cpufreq_policy *policy)
{
	unsigned int load = 0;
	unsigned int target_freq;
	unsigned int battery = get_battery_capacity();

	// Get average load
	load = cpufreq_quick_get_load(policy->cpu);
	if (!load)
		return;

	if (battery <= 20) {
		// Battery saving logic kicks in
		if (load > 85)
			target_freq = policy->max;
		else if (load > 60)
			target_freq = policy->max * 80 / 100;
		else if (load > 40)
			target_freq = policy->max * 60 / 100;
		else
			target_freq = policy->min;
	} else {
		// Normal dynamic behavior
		if (load > 80)
			target_freq = policy->max;
		else if (load > 60)
			target_freq = policy->max * 90 / 100;
		else if (load > 40)
			target_freq = policy->max * 70 / 100;
		else
			target_freq = policy->min;
	}

	__cpufreq_driver_target(policy, target_freq, CPUFREQ_RELATION_H);
}

static void thermaless_balance_work(struct work_struct *work)
{
	struct cpufreq_policy *policy = container_of(work, struct cpufreq_policy, governor_data);
	cpufreq_governor_adjust(policy);
	mod_timer(&policy->governor_data_timer, jiffies + GOV_CHECK_INTERVAL);
}

static int cpufreq_gov_thermaless_balance_start(struct cpufreq_policy *policy)
{
	timer_setup(&policy->governor_data_timer, thermaless_balance_work, 0);
	mod_timer(&policy->governor_data_timer, jiffies + GOV_CHECK_INTERVAL);
	return 0;
}

static void cpufreq_gov_thermaless_balance_stop(struct cpufreq_policy *policy)
{
	del_timer_sync(&policy->governor_data_timer);
}

static struct cpufreq_governor thermaless_balance_gov = {
	.name		= GOV_NAME,
	.owner		= THIS_MODULE,
	.init		= cpufreq_gov_thermaless_balance_start,
	.exit		= cpufreq_gov_thermaless_balance_stop,
};

static int __init thermaless_balance_init(void)
{
	return cpufreq_register_governor(&thermaless_balance_gov);
}

static void __exit thermaless_balance_exit(void)
{
	cpufreq_unregister_governor(&thermaless_balance_gov);
}

module_init(thermaless_balance_init);
module_exit(thermaless_balance_exit);

MODULE_AUTHOR("Jayzee");
MODULE_DESCRIPTION("CPUFreq Governor - Battery-aware, Thermal-Free Dynamic Scaling");
MODULE_LICENSE("GPL");
