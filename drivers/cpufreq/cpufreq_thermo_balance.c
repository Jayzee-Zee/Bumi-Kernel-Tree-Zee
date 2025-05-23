#include <linux/cpufreq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/power_supply.h>
#include <linux/timer.h>

#define THERMO_LOAD_HIGH 75
#define THERMO_LOAD_LOW 30

static void thermo_balance_limits(struct cpufreq_policy *policy)
{
	unsigned int freq;
	unsigned int max_freq = policy->cpuinfo.max_freq;
	unsigned int min_freq = policy->cpuinfo.min_freq;
	unsigned int mid_freq = (max_freq + min_freq) / 2;
	unsigned int cur_freq = policy->cur;
	unsigned int load_pct = 100 * (cur_freq - min_freq) / (max_freq - min_freq + 1);

	// Default: assume not charging
	bool is_charging = false;
	int battery_pct = 100;

	struct power_supply *bat = power_supply_get_by_name("battery");
	union power_supply_propval val;

	if (bat) {
		if (!power_supply_get_property(bat, POWER_SUPPLY_PROP_CAPACITY, &val))
			battery_pct = val.intval;
		if (!power_supply_get_property(bat, POWER_SUPPLY_PROP_STATUS, &val))
			is_charging = (val.intval == POWER_SUPPLY_STATUS_CHARGING || val.intval == POWER_SUPPLY_STATUS_FULL);
	}

	if (battery_pct <= 20 && !is_charging) {
		// Save more when battery is low and NOT charging
		freq = (load_pct > THERMO_LOAD_HIGH) ? mid_freq :
		       (load_pct < THERMO_LOAD_LOW) ? min_freq :
		       mid_freq - (mid_freq / 6);
	} else {
		// Normal scaling behavior
		freq = (load_pct > THERMO_LOAD_HIGH) ? max_freq :
		       (load_pct < THERMO_LOAD_LOW) ? mid_freq :
		       (max_freq + min_freq) / 2;
	}

	cpufreq_driver_target(policy, freq, CPUFREQ_RELATION_H);
}

static struct cpufreq_governor thermo_governor = {
	.name = "thermo_balance",
	.owner = THIS_MODULE,
	.limits = thermo_balance_limits,
};

static int __init thermo_governor_init(void)
{
	return cpufreq_register_governor(&thermo_governor);
}

static void __exit thermo_governor_exit(void)
{
	cpufreq_unregister_governor(&thermo_governor);
}

MODULE_AUTHOR("Jayzee");
MODULE_DESCRIPTION("Dynamic CPU governor for perf/battery, no thermal dependency");
MODULE_LICENSE("GPL");

fs_initcall(thermo_governor_init);
module_exit(thermo_governor_exit);
