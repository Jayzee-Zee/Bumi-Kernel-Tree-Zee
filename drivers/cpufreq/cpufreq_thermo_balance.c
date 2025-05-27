// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cpufreq.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/slab.h>

struct thermo_tunables {
	struct gov_attr_set attr_set;
	unsigned int rate_limit_us;
};

struct thermo_policy {
	struct cpufreq_policy *policy;
	struct thermo_tunables *tunables;
	unsigned int last_freq;
	u64 last_update;
};

static DEFINE_PER_CPU(struct thermo_policy *, thermo_data);

static void thermo_update_freq(struct cpufreq_policy *policy)
{
	struct thermo_policy *tp = per_cpu(thermo_data, policy->cpu);
	unsigned int util = 0;
	unsigned int next_freq;

	if (!tp || !policy->cur)
		return;

	// Estimating utilization as percent of max freq
	util = policy->cur * 100 / policy->cpuinfo.max_freq;

	if (util > 80)
		next_freq = policy->max;
	else if (util > 50)
		next_freq = (policy->max + policy->min) / 2;
	else
		next_freq = policy->min;

	if (next_freq != tp->last_freq) {
		__cpufreq_driver_target(policy, next_freq, CPUFREQ_RELATION_H);
		tp->last_freq = next_freq;
		tp->last_update = get_jiffies_64();
	}
}

static int thermo_start(struct cpufreq_policy *policy)
{
	struct thermo_policy *tp;

	tp = kzalloc(sizeof(*tp), GFP_KERNEL);
	if (!tp)
		return -ENOMEM;

	tp->policy = policy;
	tp->last_freq = policy->cur;
	tp->last_update = get_jiffies_64();

	tp->tunables = kzalloc(sizeof(*tp->tunables), GFP_KERNEL);
	tp->tunables->rate_limit_us = 20000;

	per_cpu(thermo_data, policy->cpu) = tp;

	return 0;
}

static void thermo_stop(struct cpufreq_policy *policy)
{
	struct thermo_policy *tp = per_cpu(thermo_data, policy->cpu);

	if (tp) {
		kfree(tp->tunables);
		kfree(tp);
		per_cpu(thermo_data, policy->cpu) = NULL;
	}
}

static void thermo_limits(struct cpufreq_policy *policy)
{
	thermo_update_freq(policy);
}

static int thermo_policy_update(struct cpufreq_policy *policy)
{
	thermo_update_freq(policy);
	return 0;
}

static struct cpufreq_governor thermo_balance_gov = {
	.name = "thermo_balance",
	.owner = THIS_MODULE,
	.init = thermo_start,
	.exit = thermo_stop,
	.start = thermo_policy_update,
	.stop = thermo_stop,
	.limits = thermo_limits,
	.dynamic_switching = true,
};

static int __init thermo_balance_init(void)
{
	return cpufreq_register_governor(&thermo_balance_gov);
}

static void __exit thermo_balance_exit(void)
{
	cpufreq_unregister_governor(&thermo_balance_gov);
}

module_init(thermo_balance_init);
module_exit(thermo_balance_exit);

MODULE_AUTHOR("Jayzee");
MODULE_DESCRIPTION("Dynamic CPUFreq governor for performance and battery");
MODULE_LICENSE("GPL");
