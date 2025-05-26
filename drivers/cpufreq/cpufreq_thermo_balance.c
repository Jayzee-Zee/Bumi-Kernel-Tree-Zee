// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cpufreq.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/sched.h>

#define GOV_NAME "thermo_balance"

struct thermo_policy {
	struct cpufreq_policy *policy;
	struct delayed_work work;
	u64 last_update;
	unsigned int last_freq;
};

static DEFINE_PER_CPU(struct thermo_policy *, thermo_data);

static unsigned int sampling_rate = 200; // in ms
module_param(sampling_rate, uint, 0644);
MODULE_PARM_DESC(sampling_rate, "Sampling rate in ms");

static void thermo_adjust_freq(struct work_struct *work)
{
	struct thermo_policy *tp = container_of(to_delayed_work(work), struct thermo_policy, work);
	struct cpufreq_policy *policy = tp->policy;
	unsigned int util = 0, freq;

	// Fake simplistic load (real CPU usage is better with schedutil hooks)
	util = (policy->cpuinfo.max_freq - policy->cur) * 100 / policy->cpuinfo.max_freq;

	if (util > 70)
		freq = policy->max;
	else if (util > 40)
		freq = (policy->max + policy->min) / 2;
	else
		freq = policy->min;

	if (freq != tp->last_freq) {
		__cpufreq_driver_target(policy, freq, CPUFREQ_RELATION_H);
		tp->last_freq = freq;
		tp->last_update = jiffies;
	}

	schedule_delayed_work_on(policy->cpu, &tp->work, msecs_to_jiffies(sampling_rate));
}

static int thermo_start(struct cpufreq_policy *policy)
{
	struct thermo_policy *tp;

	tp = kzalloc(sizeof(*tp), GFP_KERNEL);
	if (!tp)
		return -ENOMEM;

	tp->policy = policy;
	tp->last_freq = policy->cur;
	tp->last_update = jiffies;

	per_cpu(thermo_data, policy->cpu) = tp;
	INIT_DELAYED_WORK(&tp->work, thermo_adjust_freq);
	schedule_delayed_work_on(policy->cpu, &tp->work, msecs_to_jiffies(sampling_rate));

	return 0;
}

static void thermo_stop(struct cpufreq_policy *policy)
{
	struct thermo_policy *tp = per_cpu(thermo_data, policy->cpu);

	if (!tp)
		return;

	cancel_delayed_work_sync(&tp->work);
	kfree(tp);
	per_cpu(thermo_data, policy->cpu) = NULL;
}

static struct cpufreq_governor thermo_gov = {
	.name = GOV_NAME,
	.owner = THIS_MODULE,
	.init = thermo_start,
	.exit = thermo_stop,
};

static int __init thermo_init(void)
{
	pr_info("Thermo Balance governor loaded\n");
	return cpufreq_register_governor(&thermo_gov);
}

static void __exit thermo_exit(void)
{
	cpufreq_unregister_governor(&thermo_gov);
	pr_info("Thermo Balance governor unloaded\n");
}

module_init(thermo_init);
module_exit(thermo_exit);

MODULE_AUTHOR("Jayzee & NoFilterGPT");
MODULE_DESCRIPTION("No-thermal, performance-aware dynamic CPU governor");
MODULE_LICENSE("GPL");
