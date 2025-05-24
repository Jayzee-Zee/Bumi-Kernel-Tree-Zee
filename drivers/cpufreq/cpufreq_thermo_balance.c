// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/cpufreq.h>
#include <linux/init.h>
#include <linux/tick.h>
#include <linux/sched.h>
#include <linux/jiffies.h>
#include <linux/slab.h>

struct thermo_tunables {
	struct gov_attr_set attr_set;
	unsigned int rate_limit_us;
};

struct thermo_policy {
	struct cpufreq_policy *policy;
	struct thermo_tunables *tunables;
	struct list_head tunables_hook;
	u64 last_update;
	unsigned int last_freq;
};

static DEFINE_PER_CPU(struct thermo_policy *, thermo_data);

static void thermo_update_freq(struct cpufreq_policy *policy)
{
	struct thermo_policy *tp = per_cpu(thermo_data, policy->cpu);
	unsigned int load = 0;
	unsigned int next_freq;

	if (!tp || !policy->cur)
		return;

	// use jiffies to fake utilization estimation (4.19-compatible)
	load = (policy->cpuinfo.max_freq - policy->cur) * 100 / policy->cpuinfo.max_freq;

	if (load > 70)
		next_freq = policy->max;
	else if (load > 40)
		next_freq = (policy->max + policy->min) / 2;
	else
		next_freq = policy->min;

	if (next_freq != tp->last_freq) {
		__cpufreq_driver_target(policy, next_freq, CPUFREQ_RELATION_H);
		tp->last_freq = next_freq;
		tp->last_update = get_jiffies_64();
	}
}

static void thermo_input_event(struct input_handle *handle, unsigned int type,
			       unsigned int code, int value)
{
	if (type == EV_SYN) {
		int cpu = smp_processor_id();
		struct cpufreq_policy *policy = cpufreq_cpu_get(cpu);
		if (!policy)
			return;
		thermo_update_freq(policy);
		cpufreq_cpu_put(policy);
	}
}

static struct cpufreq_governor thermo_gov;

static int thermo_start(struct cpufreq_policy *policy)
{
	struct thermo_policy *tp;

	tp = kzalloc(sizeof(*tp), GFP_KERNEL);
	if (!tp)
		return -ENOMEM;

	tp->policy = policy;
	tp->last_freq = policy->cur;
	tp->last_update = get_jiffies_64();
	per_cpu(thermo_data, policy->cpu) = tp;

	return 0;
}

static void thermo_stop(struct cpufreq_policy *policy)
{
	kfree(per_cpu(thermo_data, policy->cpu));
	per_cpu(thermo_data, policy->cpu) = NULL;
}

static struct cpufreq_governor thermo_gov = {
	.name = "thermo_balance",
	.owner = THIS_MODULE,
	.init = thermo_start,
	.exit = thermo_stop,
};

static int __init thermo_init(void)
{
	return cpufreq_register_governor(&thermo_gov);
}

static void __exit thermo_exit(void)
{
	cpufreq_unregister_governor(&thermo_gov);
}

module_init(thermo_init);
module_exit(thermo_exit);

MODULE_AUTHOR("NoFilterGPT");
MODULE_DESCRIPTION("Thermal-less dynamic CPU governor");
MODULE_LICENSE("GPL");
