/*
 * Copyright (C) 2017 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#ifndef _MT_GPUFREQ_CORE_H_
#define _MT_GPUFREQ_CORE_H_

/**************************************************
 * MT6768/MT6767 : GPU DVFS OPP table Setting
 **************************************************/
#define SEG_GPU_DVFS_FREQ0			(1209000)	/* KHz - OC */
#define SEG_GPU_DVFS_FREQ1			(1100000)	/* KHz - OC safe */
#define SEG_GPU_DVFS_FREQ2			(1000000)	/* KHz */
#define SEG_GPU_DVFS_FREQ3			(975000)	/* KHz */
#define SEG_GPU_DVFS_FREQ4			(950000)	/* KHz */
#define SEG_GPU_DVFS_FREQ5			(925000)	/* KHz */
#define SEG_GPU_DVFS_FREQ6			(900000)	/* KHz */
#define SEG_GPU_DVFS_FREQ7			(875000)	/* KHz */
#define SEG_GPU_DVFS_FREQ8			(850000)	/* KHz */
#define SEG_GPU_DVFS_FREQ9			(823000)	/* KHz */
#define SEG_GPU_DVFS_FREQ10			(796000)	/* KHz */
#define SEG_GPU_DVFS_FREQ11			(769000)	/* KHz */
#define SEG_GPU_DVFS_FREQ12			(743000)	/* KHz */
#define SEG_GPU_DVFS_FREQ13			(716000)	/* KHz */
#define SEG_GPU_DVFS_FREQ14			(690000)	/* KHz */
#define SEG_GPU_DVFS_FREQ15			(663000)	/* KHz */
#define SEG_GPU_DVFS_FREQ16			(637000)	/* KHz */
#define SEG_GPU_DVFS_FREQ17			(611000)	/* KHz */
#define SEG_GPU_DVFS_FREQ18			(586000)	/* KHz */
#define SEG_GPU_DVFS_FREQ19			(560000)	/* KHz */
#define SEG_GPU_DVFS_FREQ20			(535000)	/* KHz */
#define SEG_GPU_DVFS_FREQ21			(509000)	/* KHz */
#define SEG_GPU_DVFS_FREQ22			(484000)	/* KHz */
#define SEG_GPU_DVFS_FREQ23			(467000)	/* KHz */
#define SEG_GPU_DVFS_FREQ24			(450000)	/* KHz */
#define SEG_GPU_DVFS_FREQ25			(434000)	/* KHz */
#define SEG_GPU_DVFS_FREQ26			(417000)	/* KHz */
#define SEG_GPU_DVFS_FREQ27			(400000)	/* KHz */
#define SEG_GPU_DVFS_FREQ28			(383000)	/* KHz */
#define SEG_GPU_DVFS_FREQ29			(366000)	/* KHz */
#define SEG_GPU_DVFS_FREQ30			(349000)	/* KHz */
#define SEG_GPU_DVFS_FREQ31			(332000)	/* KHz */
#define SEG_GPU_DVFS_FREQ32			(316000)	/* KHz */
#define SEG_GPU_DVFS_FREQ33			(300000)	/* KHz - lowest */

#define SEG_GPU_DVFS_VOLT0			(97500)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT1			(96250)
#define SEG_GPU_DVFS_VOLT2			(95000)
#define SEG_GPU_DVFS_VOLT3			(92500)
#define SEG_GPU_DVFS_VOLT4			(90000)
#define SEG_GPU_DVFS_VOLT5			(87500)
#define SEG_GPU_DVFS_VOLT6			(85000)
#define SEG_GPU_DVFS_VOLT7			(82500)
#define SEG_GPU_DVFS_VOLT8			(80000)
#define SEG_GPU_DVFS_VOLT9			(79375)
#define SEG_GPU_DVFS_VOLT10			(78125)
#define SEG_GPU_DVFS_VOLT11			(76875)
#define SEG_GPU_DVFS_VOLT12			(75625)
#define SEG_GPU_DVFS_VOLT13			(75000)
#define SEG_GPU_DVFS_VOLT14			(73750)
#define SEG_GPU_DVFS_VOLT15			(72500)
#define SEG_GPU_DVFS_VOLT16			(71250)
#define SEG_GPU_DVFS_VOLT17			(70625)
#define SEG_GPU_DVFS_VOLT18			(70000)
#define SEG_GPU_DVFS_VOLT19			(69375)
#define SEG_GPU_DVFS_VOLT20			(68750)
#define SEG_GPU_DVFS_VOLT21			(68125)
#define SEG_GPU_DVFS_VOLT22			(66875)
#define SEG_GPU_DVFS_VOLT23			(66250)
#define SEG_GPU_DVFS_VOLT24			(65625)
#define SEG_GPU_DVFS_VOLT25			(65000)
#define SEG_GPU_DVFS_VOLT26			(64375)
#define SEG_GPU_DVFS_VOLT27			(63750)
#define SEG_GPU_DVFS_VOLT28			(62500)
#define SEG_GPU_DVFS_VOLT29			(61250)
#define SEG_GPU_DVFS_VOLT30			(60000)
#define SEG_GPU_DVFS_VOLT31			(58750)
#define SEG_GPU_DVFS_VOLT32			(57500)
#define SEG_GPU_DVFS_VOLT33			(56250)

#define SEG_GPU_DVFS_VSRAM0			(107500)	/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM1			(106250)
#define SEG_GPU_DVFS_VSRAM2			(105000)
#define SEG_GPU_DVFS_VSRAM3			(102500)
#define SEG_GPU_DVFS_VSRAM4			(100000)
#define SEG_GPU_DVFS_VSRAM5			(97500)
#define SEG_GPU_DVFS_VSRAM6			(95000)
#define SEG_GPU_DVFS_VSRAM7			(92500)
#define SEG_GPU_DVFS_VSRAM8			(90000)
#define SEG_GPU_DVFS_VSRAM9			(89375)
#define SEG_GPU_DVFS_VSRAM10		(88125)
#define SEG_GPU_DVFS_VSRAM11		(86875)
#define SEG_GPU_DVFS_VSRAM12		(85625)
#define SEG_GPU_DVFS_VSRAM13		(85000)
#define SEG_GPU_DVFS_VSRAM14		(83750)
#define SEG_GPU_DVFS_VSRAM15		(82500)
#define SEG_GPU_DVFS_VSRAM16		(81250)
#define SEG_GPU_DVFS_VSRAM17		(80625)
#define SEG_GPU_DVFS_VSRAM18		(80000)
#define SEG_GPU_DVFS_VSRAM19		(79375)
#define SEG_GPU_DVFS_VSRAM20		(78750)
#define SEG_GPU_DVFS_VSRAM21		(78125)
#define SEG_GPU_DVFS_VSRAM22		(77500)
#define SEG_GPU_DVFS_VSRAM23		(76875)
#define SEG_GPU_DVFS_VSRAM24		(76250)
#define SEG_GPU_DVFS_VSRAM25		(75625)
#define SEG_GPU_DVFS_VSRAM26		(75000)
#define SEG_GPU_DVFS_VSRAM27		(74375)
#define SEG_GPU_DVFS_VSRAM28		(73125)
#define SEG_GPU_DVFS_VSRAM29		(72500)
#define SEG_GPU_DVFS_VSRAM30		(71250)
#define SEG_GPU_DVFS_VSRAM31		(70000)
#define SEG_GPU_DVFS_VSRAM32		(68750)
#define SEG_GPU_DVFS_VSRAM33		(67500)		/* mV x 100 */

#define FIXED_VSRAM_VOLT			(85000)		/* mV x 100 */
#define FIXED_VSRAM_VOLT_THSRESHOLD	(75000)		/* mV x 100 */

/**************************************************
 * PMIC Setting
 **************************************************/
#define VGPU_MAX_VOLT		(SEG_GPU_DVFS_VOLT0)
#define VSRAM_GPU_MAX_VOLT	(SEG_GPU_DVFS_VSRAM0)
#define DELAY_FACTOR		(625)
#define BUCK_DIFF_MAX		(25000)		/* mV x 100 */
#define BUCK_DIFF_MIN		(10000)		/* mV x 100 */
#define NUM_OF_OPP_IDX		(32)

/**************************************************
 * efuse Setting
 **************************************************/
#define GPUFREQ_EFUSE_INDEX			(8)
#define EFUSE_MFG_SPD_BOND_SHIFT		(8)
#define EFUSE_MFG_SPD_BOND_MASK			(0xF)
#define FUNC_CODE_EFUSE_INDEX			(22)

/**************************************************
 * Clock Setting
 **************************************************/
#define POST_DIV_2_MAX_FREQ			(1900000)
#define POST_DIV_2_MIN_FREQ			(750000)
#define POST_DIV_4_MAX_FREQ			(950000)
#define POST_DIV_4_MIN_FREQ			(375000)
#define POST_DIV_8_MAX_FREQ			(475000)
#define POST_DIV_8_MIN_FREQ			(187500)
#define POST_DIV_16_MAX_FREQ			(237500)
#define POST_DIV_16_MIN_FREQ			(93750)
#define POST_DIV_MASK				(0x70000000)
#define POST_DIV_SHIFT				(24)
#define TO_MHz_HEAD				(100)
#define TO_MHz_TAIL				(10)
#define ROUNDING_VALUE				(5)
#define DDS_SHIFT				(14)
#define GPUPLL_FIN				(26)
#define GPUPLL_CON1				(g_apmixed_base + 0x24C)

/**************************************************
 * Reference Power Setting
 **************************************************/
#define GPU_ACT_REF_POWER			(1285)		/* mW  */
#define GPU_ACT_REF_FREQ			(900000)	/* KHz */
#define GPU_ACT_REF_VOLT			(90000)		/* mV x 100 */
#define GPU_DVFS_PTPOD_DISABLE_VOLT	(80000)		/* mV x 100 */
#define GPU_DVFS_PTPOD_DISABLE_VSRAM_VOLT (90000)

/**************************************************
 * Log Setting
 **************************************************/
#define GPUFERQ_TAG				"[GPU/DVFS]"
#define gpufreq_pr_err(fmt, args...)		pr_err(GPUFERQ_TAG"[ERROR]"fmt, ##args)
#define gpufreq_pr_warn(fmt, args...)		pr_warn(GPUFERQ_TAG"[WARNING]"fmt, ##args)
#define gpufreq_pr_info(fmt, args...)		pr_info(GPUFERQ_TAG"[INFO]"fmt, ##args)
#define gpufreq_pr_debug(fmt, args...)		pr_debug(GPUFERQ_TAG"[DEBUG]"fmt, ##args)

/**************************************************
 * Condition Setting
 **************************************************/
#define MT_GPUFREQ_STATIC_PWR_READY2USE
#define MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
#define MT_GPUFREQ_BATT_PERCENT_PROTECT /* todo: disable it */
#define MT_GPUFREQ_BATT_OC_PROTECT
#define MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE

/**************************************************
 * Battery Over Current Protect
 **************************************************/
#ifdef MT_GPUFREQ_BATT_OC_PROTECT
#define MT_GPUFREQ_BATT_OC_LIMIT_FREQ		(485000)	/* KHz */
#endif

/**************************************************
 * Battery Percentage Protect
 **************************************************/
#ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT
#define MT_GPUFREQ_BATT_PERCENT_LIMIT_FREQ	(485000)	/* KHz */
#endif

/**************************************************
 * Low Battery Volume Protect
 **************************************************/
#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
#define MT_GPUFREQ_LOW_BATT_VOLT_LIMIT_FREQ	(485000)	/* KHz */
#endif

/**************************************************
 * Proc Node Definition
 **************************************************/
#ifdef CONFIG_PROC_FS
#define PROC_FOPS_RW(name)	\
	static int mt_ ## name ## _proc_open(struct inode *inode, struct file *file)	\
	{	\
		return single_open(file, mt_ ## name ## _proc_show, PDE_DATA(inode));	\
	}	\
	static const struct file_operations mt_ ## name ## _proc_fops = {	\
		.owner = THIS_MODULE,	\
		.open = mt_ ## name ## _proc_open,	\
		.read = seq_read,	\
		.llseek = seq_lseek,	\
		.release = single_release,	\
		.write = mt_ ## name ## _proc_write,	\
	}
#define PROC_FOPS_RO(name)	\
	static int mt_ ## name ## _proc_open(struct inode *inode, struct file *file)	\
	{	\
		return single_open(file, mt_ ## name ## _proc_show, PDE_DATA(inode));	\
	}	\
	static const struct file_operations mt_ ## name ## _proc_fops =	\
	{	\
		.owner = THIS_MODULE,	\
		.open = mt_ ## name ## _proc_open,	\
		.read = seq_read,	\
		.llseek = seq_lseek,	\
		.release = single_release,	\
	}
#define PROC_ENTRY(name) \
	{__stringify(name), &mt_ ## name ## _proc_fops}
#endif

/**************************************************
 * Operation Definition
 **************************************************/
#define VOLT_NORMALIZATION(volt)		((volt % 625) ? (volt - (volt % 625) + 625) : volt)
#ifndef MIN
#define MIN(x, y)						(((x) < (y)) ? (x) : (y))
#endif
#define GPUOP(khz, volt, vsram)	\
	{	\
		.gpufreq_khz = khz,	\
		.gpufreq_volt = volt,	\
		.gpufreq_vsram = vsram,	\
	}

/**************************************************
 * Enumerations
 **************************************************/
enum g_segment_id_enum {
	MT6767_SEGMENT = 1,
	MT6768_SEGMENT,
	MT6769_SEGMENT,
	MT6769T_SEGMENT,
	MT6769Z_SEGMENT,
};
enum g_post_divider_power_enum  {
	POST_DIV2 = 1,
	POST_DIV4,
	POST_DIV8,
	POST_DIV16,
};
enum g_clock_source_enum  {
	CLOCK_MAIN = 0,
	CLOCK_SUB,
};
enum g_limited_idx_enum {
	IDX_THERMAL_PROTECT_LIMITED = 0,
	IDX_LOW_BATT_LIMITED,
	IDX_BATT_PERCENT_LIMITED,
	IDX_BATT_OC_LIMITED,
	IDX_PBM_LIMITED,
	NUMBER_OF_LIMITED_IDX,
};
enum g_volt_switch_enum {
	VOLT_FALLING = 0,
	VOLT_RISING,
};

/**************************************************
 * Structures
 **************************************************/
struct g_opp_table_info {
	unsigned int gpufreq_khz;
	unsigned int gpufreq_volt;
	unsigned int gpufreq_vsram;
};
struct g_clk_info {
	struct clk *clk_mux;		/* main clock for mfg setting */
	struct clk *clk_main_parent;	/* substitution clock for mfg transient mux setting */
	struct clk *clk_sub_parent;	/* substitution clock for mfg transient parent setting */
	struct clk *subsys_mfg_cg;	/* clock gating */
	struct clk *mtcmos_mfg_async;	/* */
	struct clk *mtcmos_mfg;		/* dependent on mtcmos_mfg_async */
	struct clk *mtcmos_mfg_core0;	/* dependent on mtcmos_mfg */
	struct clk *mtcmos_mfg_core1;	/* dependent on mtcmos_mfg */
};
struct g_pmic_info {
	struct regulator *reg_vgpu;
	struct regulator *reg_vsram_gpu;
};

/**************************************************
 * External functions declaration
 **************************************************/
extern bool mtk_get_gpu_loading(unsigned int *pLoading);
extern unsigned int mt_get_ckgen_freq(unsigned int);

#endif /* _MT_GPUFREQ_CORE_H_ */
