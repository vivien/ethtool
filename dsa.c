#include <stdio.h>
#include <string.h>

#include "internal.h"

/* Macros and dump functions for the 16-bit mv88e6xxx per-port registers */

#define REG(_reg, _name, _val) \
	printf("%.02u: %-38.38s 0x%.4x\n", _reg, _name, _val)

#define FIELD(_name, _fmt, ...) \
	printf("      %-36.36s " _fmt "\n", _name, ##__VA_ARGS__)

#define FIELD_BITMAP(_name, _val) \
	FIELD(_name, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", \
	      ((_val) & 0x0001) ? "0 " : "", \
	      ((_val) & 0x0002) ? "1 " : "", \
	      ((_val) & 0x0004) ? "2 " : "", \
	      ((_val) & 0x0008) ? "3 " : "", \
	      ((_val) & 0x0010) ? "4 " : "", \
	      ((_val) & 0x0020) ? "5 " : "", \
	      ((_val) & 0x0040) ? "6 " : "", \
	      ((_val) & 0x0080) ? "7 " : "", \
	      ((_val) & 0x0100) ? "8 " : "", \
	      ((_val) & 0x0200) ? "9 " : "", \
	      ((_val) & 0x0400) ? "10 " : "", \
	      ((_val) & 0x0800) ? "11 " : "", \
	      ((_val) & 0x1000) ? "12 " : "", \
	      ((_val) & 0x2000) ? "13 " : "", \
	      ((_val) & 0x4000) ? "14 " : "", \
	      ((_val) & 0x8000) ? "15 " : "")

static void dsa_mv88e6185(int reg, u16 val)
{
	switch (reg) {
	case 0:
		REG(reg, "Port Status", val);
		break;
	case 1:
		REG(reg, "PCS Control", val);
		break;
	case 3:
		REG(reg, "Switch Identifier", val);
		break;
	case 4:
		REG(reg, "Port Control", val);
		break;
	case 5:
		REG(reg, "Port Control 1", val);
		break;
	case 6:
		REG(reg, "Port Base VLAN Map (Header)", val);
		break;
	case 7:
		REG(reg, "Default VLAN ID & Priority", val);
		break;
	case 8:
		REG(reg, "Port Control 2", val);
		break;
	case 9:
		REG(reg, "Rate Control", val);
		break;
	case 10:
		REG(reg, "Rate Control 2", val);
		break;
	case 11:
		REG(reg, "Port Association Vector", val);
		break;
	case 16:
		REG(reg, "InDiscardsLo Frame Counter", val);
		break;
	case 17:
		REG(reg, "InDiscardsHi Frame Counter", val);
		break;
	case 18:
		REG(reg, "InFiltered Frame Counter", val);
		break;
	case 19:
		REG(reg, "OutFiltered Frame Counter", val);
		break;
	case 24:
		REG(reg, "Tag Remap 0-3", val);
		break;
	case 25:
		REG(reg, "Tag Remap 4-7", val);
		break;
	default:
		REG(reg, "Reserved", val);
		break;
	}
};

struct dsa_mv88e6xxx_switch {
	void (*dump)(int reg, u16 val);
	const char *name;
	u16 id;
};

static const struct dsa_mv88e6xxx_switch dsa_mv88e6xxx_switches[] = {
	{ .id = 0x1a70, .name = "88E6185 ", .dump = dsa_mv88e6185 },
};

static int dsa_mv88e6xxx_dump_regs(struct ethtool_regs *regs)
{
	const struct dsa_mv88e6xxx_switch *sw = NULL;
	const u16 *data = (u16 *)regs->data;
	u16 id;
	int i;

	/* Marvell chips have 32 per-port 16-bit registers */
	if (regs->len < 32 * 2)
		return 1;

	id = data[3] & 0xfff0;

	for (i = 0; i < ARRAY_SIZE(dsa_mv88e6xxx_switches); i++) {
		if (id == dsa_mv88e6xxx_switches[i].id) {
			sw = &dsa_mv88e6xxx_switches[i];
			break;
		}
	}

	if (!sw)
		return 1;

	printf("%s Switch Port Registers\n", sw->name);
	printf("------------------------------\n");

	for (i = 0; i < 32; i++)
		if (sw->dump)
			sw->dump(i, data[i]);
		else
			REG(i, "", data[i]);

	return 0;
}

#undef FIELD_BITMAP
#undef FIELD
#undef REG

int dsa_dump_regs(struct ethtool_drvinfo *info, struct ethtool_regs *regs)
{
	/* DSA per-driver register dump */
	if (!dsa_mv88e6xxx_dump_regs(regs))
		return 0;

	/* Fallback to hexdump */
	return 1;
}
