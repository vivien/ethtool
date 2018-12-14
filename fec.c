#include <stdio.h>
#include <string.h>

#include "internal.h"

/* Macros and dump functions for the 32-bit "fec" driver registers */

#define REG(_reg, _name, _val) \
	printf("0x%.03x: %-44.44s 0x%.8x\n", _reg, _name, _val)

#define FIELD(_name, _fmt, ...) \
	printf("    %-47.47s " _fmt "\n", _name, ##__VA_ARGS__)

int fec_dump_regs(struct ethtool_drvinfo *info, struct ethtool_regs *regs)
{
	const u32 *data = (u32 *)regs->data;
	u32 val;
	u16 off;

	if (regs->len < 0x1ff)
		return 1;

	printf("FEC Control/Status Registers\n");
	printf("----------------------------\n");

	for (off = 0; off < 0x1ff; off += 4) {
    		val = data[off / 4];

    		switch (off) {
		case 0x084:
    			REG(off, "RCR (Receive Control Register)", val);
    			FIELD("MAX_FL (Maximum frame length)", "%u", (val & 0x07ff0000) >> 16);
    			FIELD("FCE (Flow control enable)", "%u", !!(val & 0x00000020));
    			FIELD("BC_REJ (Broadcast frame reject)", "%u", !!(val & 0x00000010));
    			FIELD("PROM (Promiscuous mode)", "%u", !!(val & 0x00000008));
    			FIELD("DRT (Disable receive on transmit)", "%u", !!(val & 0x00000002));
    			FIELD("LOOP (Internal loopback)", "%u", !!(val & 0x00000001));
    			break;
		case 0x0c4:
    			REG(off, "TCR (Transmit Control Register)", val);
    			FIELD("RFC_PAUSE (Receive frame control pause)", "%u", !!(val & 0x00000010));
    			FIELD("TFC_PAUSE (Transmit frame control pause)", "%u", !!(val & 0x00000008));
    			FIELD("FDEN (Full duplex enable)", "%u", !!(val & 0x00000004));
    			FIELD("HBC (Heartbeat control)", "%u", !!(val & 0x00000002));
    			FIELD("GTS (Graceful transmit stop)", "%u", !!(val & 0x00000001));
    			break;
		case 0x118:
    			REG(off, "IAUR (Individual Address Upper Register)", val);
    			FIELD("IADDR1", "0x%.16llx", (u64)((u64)val) << 32);
    			break;
		case 0x11c:
    			REG(off, "IALR (Individual Address Lower Register)", val);
    			FIELD("IADDR2", "0x%.16x", val);
    			break;
		case 0x120:
    			REG(off, "GAUR (Group Address Upper Register)", val);
    			FIELD("GADDR1", "0x%.16llx", (u64)((u64)val) << 32);
    			break;
		case 0x124:
    			REG(off, "GALR (Group Address Lower Register)", val);
    			FIELD("GADDR2", "0x%.16x", val);
    			break;
    		case 0x144:
        		REG(off, "TFWR (Transmit FIFO Watermark Register)", val);
    			FIELD("X_WMRK", "%s",
        		      (val & 0x00000003) == 0x00000000 ? "64 bytes" :
        		      (val & 0x00000003) == 0x00000002 ? "128 bytes" :
        		      (val & 0x00000003) == 0x00000003 ? "192 bytes" : "?");
    			break;
    		case 0x14c:
        		REG(off, "FRBR (FIFO Receive Bound Register)", val);
    			FIELD("R_BOUND (Highest valid FIFO RAM address)", "0x%.2x", (val & 0x000003fc) >> 2);
    			break;
    		case 0x188:
        		REG(off, "EMRBR (Maximum Receive Buffer Size)", val);
    			FIELD("R_BUF_SIZE (Receive buffer size)", "%u", (val & 0x000007f0) >> 4);
    			break;
		default:
			REG(off, "", val);
			break;
    		}
	}

	return 0;
}

#undef FIELD
#undef REG
