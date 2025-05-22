// linux/drivers/cpufreq/cpufreq_maxoverdrive.c

#include <linux/cpufreq.h>
#include <linux/init.h>
#include <linux/module.h>

static void cpufreq_gov_maxoverdrive_limits(struct cpufreq_policy *policy)
{
	__cpufreq_driver_target(policy, policy->max, CPUFREQ_RELATION_H);
}

static struct cpufreq_governor cpufreq_gov_maxoverdrive = {
	.name		= "maxoverdrive",
	.owner		= THIS_MODULE,
	.limits		= cpufreq_gov_maxoverdrive_limits,
};

static int __init cpufreq_gov_maxoverdrive_init(void)
{
	return cpufreq_register_governor(&cpufreq_gov_maxoverdrive);
}

static void __exit cpufreq_gov_maxoverdrive_exit(void)
{
	cpufreq_unregister_governor(&cpufreq_gov_maxoverdrive);
}

module_init(cpufreq_gov_maxoverdrive_init);
module_exit(cpufreq_gov_maxoverdrive_exit);

MODULE_AUTHOR("Jayzee");
MODULE_DESCRIPTION("Aggressive CPU governor 'maxoverdrive'");
MODULE_LICENSE("GPL");
