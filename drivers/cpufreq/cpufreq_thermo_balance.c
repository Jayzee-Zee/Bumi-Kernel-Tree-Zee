// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cpufreq.h>
#include <linux/sched/cpufreq.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/timer.h>

struct thermo_tunables {
	struct gov_attr_set attr_set;
	unsigned int up_rate_limit_us;
	unsigned int down_rate_limit_us;
};

struct thermo_policy {
	struct cpufreq_policy *policy;
	struct thermo_tunables *tunables;
	unsigned int last_freq;
	u64 last_update;
};

static DEFINE_PER_CPU(struct thermo_policy *, thermo_data);

static unsigned int compute_target_freq(struct cpufreq_policy *policy, unsigned int util)
{
	unsigned int max = policy->max;
	unsigned int min = policy->min;

	if (util >= 85)
		return max; // full boost
	else if (util >= 60)
		return max - (max - min) / 4; // 75%
	else if (util >= 30)
		return max - (max - min) / 2; // 50%
	else
		return min; // chill
}

static void thermo_balance_update(struct cpufreq_policy *policy)
{
	struct thermo_policy *tp = per_cpu(thermo_data, policy->cpu);
	unsigned int util;
	unsigned int next_freq;
	u64 now = jiffies;
	u64 delta;

	if (!tp)
		return;

	util = sched_cpu_util(policy->cpu);
	next_freq = compute_target_freq(policy, util);
	delta = jiffies_to_usecs(now - tp->last_update);

	if (next_freq > tp->last_freq &&
	    delta < tp->tunables->up_rate_limit_us)
		return;

	if (next_freq < tp->last_freq &&
	    delta < tp->tunables->down_rate_limit_us)
		return;

	__cpufreq_driver_target(policy, next_freq, CPUFREQ_RELATION_H);
	tp->last_freq = next_freq;
	tp->last_update = now;
}

static int thermo_balance_start(struct cpufreq_policy *policy)
{
	struct thermo_policy *tp;

	tp = kzalloc(sizeof(*tp), GFP_KERNEL);
	if (!tp)
		return -ENOMEM;

	tp->policy = policy;
	tp->last_freq = policy->cur;
	tp->last_update = jiffies;

	tp->tunables = kzalloc(sizeof(*tp->tunables), GFP_KERNEL);
	tp->tunables->up_rate_limit_us = 20000;
	tp->tunables->down_rate_limit_us = 40000;

	per_cpu(thermo_data, policy->cpu) = tp;

	return 0;
}

static void thermo_balance_stop(struct cpufreq_policy *policy)
{
	struct thermo_policy *tp = per_cpu(thermo_data, policy->cpu);

	if (tp) {
		kfree(tp->tunables);
		kfree(tp);
		per_cpu(thermo_data, policy->cpu) = NULL;
	}
}

static void thermo_balance_limits(struct cpufreq_policy *policy)
{
	thermo_balance_update(policy);
}

static int thermo_balance_update_policy(struct cpufreq_policy *policy)
{
	thermo_balance_update(policy);
	return 0;
}

static struct cpufreq_governor thermo_balance_gov = {
	.name = "thermo_balance",
	.owner = THIS_MODULE,
	.init = thermo_balance_start,
	.exit = thermo_balance_stop,
	.limits = thermo_balance_limits,
	.start = thermo_balance_update_policy,
	.stop = thermo_balance_stop,
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
MODULE_DESCRIPTION("Battery + Gaming Hybrid CPU Governor");
MODULE_LICENSE("GPL");
