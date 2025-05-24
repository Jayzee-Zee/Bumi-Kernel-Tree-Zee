// SPDX-License-Identifier: GPL-2.0
#include <linux/cpufreq.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/tick.h>
#include <trace/events/power.h>
#include <linux/cpumask.h>
#include <linux/mutex.h>

struct thermo_policy {
	struct cpufreq_policy *policy;
	u64 last_update;
	unsigned int prev_freq;
	unsigned int step_up;
	unsigned int step_down;
	unsigned int up_threshold;
	unsigned int down_threshold;
	bool battery_saver_mode;
};

static DEFINE_PER_CPU(struct thermo_policy, thermo_policies);

static void thermo_update_freq(struct update_util_data *data, u64 time, unsigned int flags)
{
	struct thermo_policy *tp = container_of(data, struct thermo_policy, policy->update_util);
	struct cpufreq_policy *policy = tp->policy;

	unsigned long util = arch_scale_freq_capacity(NULL, smp_processor_id());
	unsigned int target_freq = policy->cur;
	u64 delta = time - tp->last_update;

	if (delta < 1000000)
		return;

	tp->last_update = time;

	// Simulated utilization based scaling
	if (util > tp->up_threshold && policy->cur < policy->max) {
		target_freq = min(policy->cur + tp->step_up, policy->max);
	} else if (util < tp->down_threshold && policy->cur > policy->min) {
		target_freq = max(policy->cur - tp->step_down, policy->min);
	}

	// Battery saver tweak (simulate lower ceilings when under saver)
	if (tp->battery_saver_mode && target_freq > policy->max * 3 / 4)
		target_freq = policy->max * 3 / 4;

	if (target_freq != tp->prev_freq) {
		cpufreq_driver_target(policy, target_freq, CPUFREQ_RELATION_L);
		tp->prev_freq = target_freq;
	}
}

static int thermo_governor_start(struct cpufreq_policy *policy)
{
	struct thermo_policy *tp = &per_cpu(thermo_policies, policy->cpu);

	tp->policy = policy;
	tp->last_update = 0;
	tp->prev_freq = policy->cur;
	tp->step_up = 100000;     // 100 MHz
	tp->step_down = 80000;    // 80 MHz
	tp->up_threshold = 800;   // Simulated util threshold
	tp->down_threshold = 300;
	tp->battery_saver_mode = false;

	policy->governor_data = tp;
	cpufreq_register_util_update_callback(policy->cpu, thermo_update_freq);

	return 0;
}

static void thermo_governor_stop(struct cpufreq_policy *policy)
{
	cpufreq_unregister_util_update_callback(policy->cpu);
}

static struct cpufreq_governor cpufreq_gov_thermo_balance = {
	.name = "thermo_balance",
	.owner = THIS_MODULE,
	.start = thermo_governor_start,
	.stop = thermo_governor_stop,
};

static int __init thermo_governor_init(void)
{
	return cpufreq_register_governor(&cpufreq_gov_thermo_balance);
}

static void __exit thermo_governor_exit(void)
{
	cpufreq_unregister_governor(&cpufreq_gov_thermo_balance);
}

module_init(thermo_governor_init);
module_exit(thermo_governor_exit);

MODULE_AUTHOR("jayzee");
MODULE_DESCRIPTION("ThermoBalance CPUFreq Governor - Dynamic and Balanced");
MODULE_LICENSE("GPL");
