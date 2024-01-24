/* huawei-mib.c - subdriver to monitor Huawei SNMP devices with NUT
 *
 *  Copyright (C)
 *  2011 - 2012	Arnaud Quette <arnaud.quette@free.fr>
 *  2015	Stuart Henderson <stu@spacehopper.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "huawei-mib.h"

#define HUAWEI_MIB_VERSION  "0.4"

#define HUAWEI_SYSOID       ".1.3.6.1.4.1.8072.3.2.10"
#define HUAWEI_UPSMIB       ".1.3.6.1.4.1.2011"
#define HUAWEI_OID_MODEL_NAME ".1.3.6.1.4.1.2011.6.174.1.2.100.1.2.1"

/* To create a value lookup structure (as needed on the 2nd line of the example
 * below), use the following kind of declaration, outside of the present snmp_info_t[]:
 * static info_lkp_t huawei_onbatt_info[] = {
 * 	{ 1, "OB"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
 * 	{ 2, "OL"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
 * 	{ 0, NULL
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	}
 * };
 */

static info_lkp_t huawei_supplymethod_info[] = {
	{ 1, ""
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},		/* no supply */
	{ 2, "OL BYPASS"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 3, "OL"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 4, "OB"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 5, ""
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},		/* combined */
	{ 6, "OL ECO"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 7, "OB ECO"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 0, NULL
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	}
};

static info_lkp_t huawei_battstate_info[] = {
	{ 1, ""
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},		/* not connected */
	{ 2, ""
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},		/* not charging or discharging */
	{ 3, ""
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},		/* hibernation */
	{ 4, ""
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},		/* float */
	{ 5, "CHRG"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},		/* equalized charging */
	{ 6, "DISCHRG"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 0, NULL
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	}
};

static info_lkp_t huawei_phase_info[] = {
	{ 1, "1"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 2, "3"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 0, NULL
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	}
};

static info_lkp_t huawei_voltrating_info[] = {
	{ 1, "200"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 2, "208"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 3, "220"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 4, "380"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 5, "400"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 6, "415"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 7, "480"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 8, "600"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 9, "690"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 0, NULL
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	}
};

static info_lkp_t huawei_freqrating_info[] = {
	{ 1, "50"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 2, "60"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 0, NULL
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	}
};

static info_lkp_t huawei_pwrrating_info[] = {
	{ 1, "80000"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 2, "100000"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 3, "120000"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 4, "160000"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 5, "200000"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 6, "30000"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 7, "40000"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 8, "60000"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 9, "2400000"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 10, "2500000"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 11, "2800000"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 12, "3000000"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 0, NULL
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	}
};

/* Note: This is currently identical to ietf_test_result_info from IETF MIB
 * We rename it here to a) allow evolution that may become incompatible;
 * b) avoid namespace conflicts, especially with DMF loader of named objects
 */
static info_lkp_t huawei_test_result_info[] = {
	{ 1, "done and passed"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 2, "done and warning"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 3, "done and error"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 4, "aborted"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 5, "in progress"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 6, "no test initiated"
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	},
	{ 0, NULL
#if WITH_SNMP_LKP_FUN
		, NULL, NULL, NULL, NULL
#endif
	}
};


/* HUAWEI Snmp2NUT lookup table */
static snmp_info_t huawei_mib[] = {

	/* Data format:
	 * snmp_info_default(info_type, info_flags, info_len, OID, dfl, flags, oid2info, setvar),
	 *
	 *	info_type:	NUT INFO_ or CMD_ element name
	 *	info_flags:	flags to set in addinfo
	 *	info_len:	length of strings if STR
	 *				cmd value if CMD, multiplier otherwise
	 *	OID: SNMP OID or NULL
	 *	dfl: default value
	 *	flags: snmp-ups internal flags (FIXME: ...)
	 *	oid2info: lookup table between OID and NUT values
	 *
	 * Example:
	 * snmp_info_default("input.voltage", 0, 0.1, ".1.3.6.1.4.1.705.1.6.2.1.2.1", "", SU_INPUT_1, NULL),
	 * snmp_info_default("ups.status", ST_FLAG_STRING, SU_INFOSIZE, ".1.3.6.1.4.1.705.1.7.3.0", "", SU_FLAG_OK | SU_STATUS_BATT, huawei_onbatt_info),
	 *
	 * To create a value lookup structure (as needed on the 2nd line), use the
	 * following kind of declaration, outside of the present snmp_info_t[]:
	 * static info_lkp_t huawei_onbatt_info[] = {
	 * 	{ 1, "OB" },
	 * 	{ 2, "OL" },
	 * 	{ 0, NULL }
	 * };
	 */

	/* standard MIB items */
	snmp_info_default("device.description", ST_FLAG_STRING | ST_FLAG_RW, SU_INFOSIZE, ".1.3.6.1.2.1.1.1.0", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("device.contact", ST_FLAG_STRING | ST_FLAG_RW, SU_INFOSIZE, ".1.3.6.1.2.1.1.4.0", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("device.location", ST_FLAG_STRING | ST_FLAG_RW, SU_INFOSIZE, ".1.3.6.1.2.1.1.6.0", NULL, SU_FLAG_OK, NULL),

	/* UPS page */

	snmp_info_default("ups.mfr", ST_FLAG_STRING, SU_INFOSIZE, NULL, "Huawei", SU_FLAG_ABSENT | SU_FLAG_OK, NULL),
	snmp_info_default("ups.model", ST_FLAG_STRING, SU_INFOSIZE, ".1.3.6.1.4.1.2011.6.174.1.2.100.1.2.1", "Generic SNMP UPS", SU_FLAG_STATIC | SU_FLAG_OK, NULL),
	snmp_info_default("ups.id", ST_FLAG_STRING, SU_INFOSIZE, ".1.3.6.1.4.1.2011.6.174.1.1.1.2.0", NULL, SU_FLAG_STATIC | SU_FLAG_OK, NULL),

	snmp_info_default("ups.time", 0, 1, ".1.3.6.1.4.1.2011.6.174.1.11.1.0", NULL, SU_FLAG_OK, NULL), /* seconds since epoch */

	snmp_info_default("ups.firmware", ST_FLAG_STRING, SU_INFOSIZE, ".1.3.6.1.4.1.2011.6.174.1.2.100.1.3.1", NULL, SU_FLAG_STATIC | SU_FLAG_OK, NULL),
	snmp_info_default("ups.serial", ST_FLAG_STRING, SU_INFOSIZE, ".1.3.6.1.4.1.2011.6.174.1.2.100.1.5.1", NULL, SU_FLAG_STATIC | SU_FLAG_OK, NULL),

	snmp_info_default("ups.status", 0, 1, ".1.3.6.1.4.1.2011.6.174.1.2.101.1.1.1", NULL, SU_FLAG_OK, huawei_supplymethod_info),
	snmp_info_default("ups.status", 0, 1, ".1.3.6.1.4.1.2011.6.174.1.2.101.1.3.1", NULL, SU_STATUS_BATT | SU_FLAG_OK, huawei_battstate_info),

	snmp_info_default("ups.test.result", ST_FLAG_STRING, SU_INFOSIZE, ".1.3.6.1.2.1.33.1.7.3.0", "", 0, huawei_test_result_info),


	/* Input page */

	/* hwUpsCtrlInputStandard listed in MIB but not present on tested UPS5000-E */
	snmp_info_default("input.phases", ST_FLAG_STRING, SU_INFOSIZE, ".1.3.6.1.4.1.2011.6.174.1.102.100.1.8", "3", SU_FLAG_ABSENT | SU_FLAG_OK, huawei_phase_info),

	snmp_info_default("input.L1-N.voltage", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.3.100.1.1.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("input.L2-N.voltage", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.3.100.1.2.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("input.L3-N.voltage", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.3.100.1.3.1", NULL, SU_FLAG_OK, NULL),

	snmp_info_default("input.frequency", 0, 0.01, ".1.3.6.1.4.1.2011.6.174.1.3.100.1.4.1", NULL, SU_FLAG_OK, NULL),

	snmp_info_default("input.L1.current", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.3.100.1.5.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("input.L2.current", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.3.100.1.6.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("input.L3.current", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.3.100.1.7.1", NULL, SU_FLAG_OK, NULL),

	snmp_info_default("input.L1.powerfactor", 0, 0.01, ".1.3.6.1.4.1.2011.6.174.1.3.100.1.8.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("input.L2.powerfactor", 0, 0.01, ".1.3.6.1.4.1.2011.6.174.1.3.100.1.9.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("input.L3.powerfactor", 0, 0.01, ".1.3.6.1.4.1.2011.6.174.1.3.100.1.10.1", NULL, SU_FLAG_OK, NULL),

	snmp_info_default("input.bypass.L1-N.voltage", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.5.100.1.1.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("input.bypass.L2-N.voltage", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.5.100.1.2.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("input.bypass.L3-N.voltage", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.5.100.1.3.1", NULL, SU_FLAG_OK, NULL),

	snmp_info_default("input.bypass.frequency", 0, 0.01, ".1.3.6.1.4.1.2011.6.174.1.5.100.1.4.1", NULL, SU_FLAG_OK, NULL),


	/* Output page */

	/* hwUpsCtrlOutputStandard listed in MIB but not present on tested UPS5000-E */
	snmp_info_default("output.phases", ST_FLAG_STRING, SU_INFOSIZE, ".1.3.6.1.4.1.2011.6.174.1.102.100.1.9", "3", SU_FLAG_ABSENT | SU_FLAG_OK, huawei_phase_info),

	snmp_info_default("output.L1-N.voltage", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.1.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("output.L2-N.voltage", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.2.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("output.L3-N.voltage", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.3.1", NULL, SU_FLAG_OK, NULL),

	snmp_info_default("output.L1.current", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.4.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("output.L2.current", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.5.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("output.L3.current", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.6.1", NULL, SU_FLAG_OK, NULL),

	snmp_info_default("output.frequency", 0, 0.01, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.7.1", NULL, SU_FLAG_OK, NULL),

	snmp_info_default("output.L1.realpower", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.8.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("output.L1.realpower", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.9.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("output.L1.realpower", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.10.1", NULL, SU_FLAG_OK, NULL),

	snmp_info_default("output.L1.power", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.11.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("output.L2.power", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.12.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("output.L3.power", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.13.1", NULL, SU_FLAG_OK, NULL),

	snmp_info_default("output.L1.power.percent", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.14.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("output.L2.power.percent", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.15.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("output.L3.power.percent", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.16.1", NULL, SU_FLAG_OK, NULL),

	snmp_info_default("output.voltage.nominal", ST_FLAG_STRING, SU_INFOSIZE, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.17.1", NULL, SU_FLAG_STATIC | SU_FLAG_OK, huawei_voltrating_info),
	snmp_info_default("output.frequency.nominal", ST_FLAG_STRING, SU_INFOSIZE, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.18.1", NULL, SU_FLAG_STATIC | SU_FLAG_OK, huawei_freqrating_info),
	snmp_info_default("output.power.nominal", ST_FLAG_STRING, SU_INFOSIZE, ".1.3.6.1.4.1.2011.6.174.1.2.100.1.6.1", NULL, SU_FLAG_STATIC | SU_FLAG_OK, huawei_pwrrating_info),

	snmp_info_default("output.L1.powerfactor", 0, 0.01, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.19.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("output.L2.powerfactor", 0, 0.01, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.20.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("output.L2.powerfactor", 0, 0.01, ".1.3.6.1.4.1.2011.6.174.1.4.100.1.21.1", NULL, SU_FLAG_OK, NULL),


	/* Battery page */

	snmp_info_default("battery.voltage", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.6.100.1.1.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("battery.current", 0, 0.1, ".1.3.6.1.4.1.2011.6.174.1.6.100.1.2.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("battery.charge", 0, 1, ".1.3.6.1.4.1.2011.6.174.1.6.100.1.3.1", NULL, SU_FLAG_OK, NULL),
	snmp_info_default("battery.runtime", 0, 1, ".1.3.6.1.4.1.2011.6.174.1.6.100.1.4.1", NULL, SU_FLAG_OK, NULL),

#if WITH_UNMAPPED_DATA_POINTS
	snmp_info_default("unmapped.hwUpsBattTest", 0, 1, ".1.3.6.1.4.1.2011.6.174.1.103.101.1.6.1", NULL, SU_FLAG_OK, NULL),
#endif	/* if WITH_UNMAPPED_DATA_POINTS */

	/* end of structure. */
	snmp_info_sentinel
};

mib2nut_info_t	huawei = { "huawei", HUAWEI_MIB_VERSION, NULL, HUAWEI_OID_MODEL_NAME, huawei_mib, HUAWEI_SYSOID, NULL };
