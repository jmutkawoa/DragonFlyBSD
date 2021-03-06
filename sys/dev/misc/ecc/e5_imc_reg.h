#ifndef _E5_IMC_REG_H_
#define _E5_IMC_REG_H_

#ifndef _SYS_BITOPS_H_
#include <sys/bitops.h>
#endif

/*
 * E5 v2/v3 supports 4 channels, each channels could have 3 DIMMs.
 * However each channel could only support 8 ranks, e.g. 3 quad-
 * rank DIMMs can _not_ be installed.
 *
 * E5 v2 only has IMC0, which has 4 channels (channel 0~3).
 *
 * E5 v3 has two configuration:
 * - IMC0, which has 4 channels (channel 0~3).
 * - IMC0, which has 2 channels (channel 0~1) and IMC1, which has
 *   2 channels (channel 2~3).
 */ 

#define PCI_E5_IMC_VID_ID			0x8086
#define PCI_E5_IMC_CHN_MAX			4	/* max chans/sock */
#define PCI_E5_IMC_CHN_DIMM_MAX			3	/* max dimms/chan */
#define PCI_E5_IMC_ERROR_RANK_MAX		8

/*
 * UBOX0
 */
/* E5 v2 */
#define PCISLOT_E5V2_UBOX0			11
#define PCIFUNC_E5V2_UBOX0			0
#define PCI_E5V2_UBOX0_DID_ID			0x0e1e
/* E5 v3 */
#define PCISLOT_E5V3_UBOX0			16
#define PCIFUNC_E5V3_UBOX0			5
#define PCI_E5V3_UBOX0_DID_ID			0x2f1e
/* UBOX0 regs */
#define PCI_E5_UBOX0_CPUNODEID			0x40
#define PCI_E5_UBOX0_CPUNODEID_LCLNODEID	__BITS(0, 2) /* local socket */

/*
 * IMC main (aka CPGC)
 */
/* E5 v2 */
#define PCISLOT_E5V2_IMC0_CPGC			15
#define PCIFUNC_E5V2_IMC0_CPGC			0
#define PCI_E5V2_IMC0_CPGC_DID_ID		0x0ea8
/* E5 v3 */
#define PCISLOT_E5V3_IMC0_CPGC			19
#define PCIFUNC_E5V3_IMC0_CPGC			0
#define PCI_E5V3_IMC0_CPGC_DID_ID		0x2fa8
#define PCISLOT_E5V3_IMC1_CPGC			22
#define PCIFUNC_E5V3_IMC1_CPGC			0
#define PCI_E5V3_IMC1_CPGC_DID_ID		0x2f68
/* CPGC regs */
#define PCI_E5_IMC_CPGC_MCMTR			0x7c
#define PCI_E5V2_IMC_CPGC_MCMTR_CHN_DISABLE(c)	__BIT(16 + (c))
#define PCI_E5V3_IMC_CPGC_MCMTR_CHN_DISABLE(c)	__BIT(18 + (c))
#define PCI_E5V3_IMC_CPGC_MCMTR_DDR4		__BIT(14)
#define PCI_E5_IMC_CPGC_MCMTR_IMC_MODE		__BITS(12, 13)
#define PCI_E5_IMC_CPGC_MCMTR_IMC_MODE_DDR3	0	/* v3 native DDR */
#define PCI_E5_IMC_CPGC_MCMTR_ECC_EN		__BIT(2)

/*
 * Channel Target Address Decoder, per-channel
 */
/* E5 v2 */
#define PCISLOT_E5V2_IMC0_CTAD			15
#define PCIFUNC_E5V2_IMC0_CTAD(c)		(2 + (c))
#define PCI_E5V2_IMC0_CTAD_DID_ID(c)		(0x0eaa + (c))
/* E5 v3 */
#define PCISLOT_E5V3_IMC0_CTAD			19
#define PCIFUNC_E5V3_IMC0_CTAD(c)		(2 + (c))
#define PCI_E5V3_IMC0_CTAD_DID_ID(c)		(0x2faa + (c))
#define PCISLOT_E5V3_IMC1_CTAD			22
#define PCIFUNC_E5V3_IMC1_CTAD(c)		(2 + (c))
#define PCI_E5V3_IMC1_CTAD_DID_ID(c)		(0x2f6a + (c))
/* CTAD regs */
#define PCI_E5_IMC_CTAD_DIMMMTR(dimm)		(0x80 + ((dimm) * 4))
#define PCI_E5V3_IMC_CTAD_DIMMMTR_DDR4		__BIT(20)
#define PCI_E5_IMC_CTAD_DIMMMTR_RANK_DISABLE(r)	__BIT(16 + (r))
#define PCI_E5_IMC_CTAD_DIMMMTR_RANK_DISABLE_ALL __BITS(16, 19)
#define PCI_E5_IMC_CTAD_DIMMMTR_DIMM_POP	__BIT(14)
#define PCI_E5_IMC_CTAD_DIMMMTR_RANK_CNT	__BITS(12, 13)
#define PCI_E5_IMC_CTAD_DIMMMTR_RANK_CNT_SR	0
#define PCI_E5_IMC_CTAD_DIMMMTR_RANK_CNT_DR	1
#define PCI_E5_IMC_CTAD_DIMMMTR_RANK_CNT_QR	2
#define PCI_E5V3_IMC_CTAD_DIMMMTR_RANK_CNT_8R	3
#define PCI_E5_IMC_CTAD_DIMMMTR_DDR3_WIDTH	__BITS(7, 8)
#define PCI_E5_IMC_CTAD_DIMMMTR_DDR3_WIDTH_4	0
#define PCI_E5_IMC_CTAD_DIMMMTR_DDR3_WIDTH_8	1
#define PCI_E5_IMC_CTAD_DIMMMTR_DDR3_WIDTH_16	2
#define PCI_E5_IMC_CTAD_DIMMMTR_DDR3_WIDTH_RSVD	3
#define PCI_E5_IMC_CTAD_DIMMMTR_DDR3_DNSTY	__BITS(5, 6)
#define PCI_E5_IMC_CTAD_DIMMMTR_DDR3_DNSTY_1G	0	/* v3 reserved */
#define PCI_E5_IMC_CTAD_DIMMMTR_DDR3_DNSTY_2G	1
#define PCI_E5_IMC_CTAD_DIMMMTR_DDR3_DNSTY_4G	2
#define PCI_E5_IMC_CTAD_DIMMMTR_DDR3_DNSTY_8G	3

/*
 * ERROR, per-channel
 */
/* E5 v2 */
#define PCISLOT_E5V2_IMC0_ERROR_CHN0		16
#define PCIFUNC_E5V2_IMC0_ERROR_CHN0		2
#define PCI_E5V2_IMC0_ERROR_CHN0_DID_ID		0x0eb2
#define PCISLOT_E5V2_IMC0_ERROR_CHN1		16
#define PCIFUNC_E5V2_IMC0_ERROR_CHN1		3
#define PCI_E5V2_IMC0_ERROR_CHN1_DID_ID		0x0eb3
#define PCISLOT_E5V2_IMC0_ERROR_CHN2		16
#define PCIFUNC_E5V2_IMC0_ERROR_CHN2		6
#define PCI_E5V2_IMC0_ERROR_CHN2_DID_ID		0x0eb6
#define PCISLOT_E5V2_IMC0_ERROR_CHN3		16
#define PCIFUNC_E5V2_IMC0_ERROR_CHN3		7
#define PCI_E5V2_IMC0_ERROR_CHN3_DID_ID		0x0eb7
/* E5 v3 */
#define PCISLOT_E5V3_IMC0_ERROR_CHN0		20
#define PCIFUNC_E5V3_IMC0_ERROR_CHN0		2
#define PCI_E5V3_IMC0_ERROR_CHN0_DID_ID		0x2fb2
#define PCISLOT_E5V3_IMC0_ERROR_CHN1		20
#define PCIFUNC_E5V3_IMC0_ERROR_CHN1		3
#define PCI_E5V3_IMC0_ERROR_CHN1_DID_ID		0x2fb3
#define PCISLOT_E5V3_IMC0_ERROR_CHN2		21
#define PCIFUNC_E5V3_IMC0_ERROR_CHN2		2
#define PCI_E5V3_IMC0_ERROR_CHN2_DID_ID		0x2fb6
#define PCISLOT_E5V3_IMC0_ERROR_CHN3		21
#define PCIFUNC_E5V3_IMC0_ERROR_CHN3		3
#define PCI_E5V3_IMC0_ERROR_CHN3_DID_ID		0x2fb7
#define PCISLOT_E5V3_IMC1_ERROR_CHN0		23
#define PCIFUNC_E5V3_IMC1_ERROR_CHN0		2
#define PCI_E5V3_IMC1_ERROR_CHN0_DID_ID		0x2fd6
#define PCISLOT_E5V3_IMC1_ERROR_CHN1		23
#define PCIFUNC_E5V3_IMC1_ERROR_CHN1		3
#define PCI_E5V3_IMC1_ERROR_CHN1_DID_ID		0x2fd7
/* ERROR regs */
#define PCI_E5_IMC_ERROR_COR_ERR_CNT(i)		(0x104 + ((i) * 4))
#define PCI_E5_IMC_ERROR_COR_ERR_CNT_HI_OVFL	__BIT(31)
#define PCI_E5_IMC_ERROR_COR_ERR_CNT_HI		__BITS(16, 30)
#define PCI_E5_IMC_ERROR_COR_ERR_CNT_LO_OVFL	__BIT(15)
#define PCI_E5_IMC_ERROR_COR_ERR_CNT_LO		__BITS(0, 14)
#define PCI_E5_IMC_ERROR_COR_ERR_TH(i)		(0x11c + ((i) * 4))
#define PCI_E5_IMC_ERROR_COR_ERR_TH_HI		__BITS(16, 30)
#define PCI_E5_IMC_ERROR_COR_ERR_TH_LO		__BITS(0, 14)
#define PCI_E5_IMC_ERROR_COR_ERR_STAT		0x134
#define PCI_E5_IMC_ERROR_COR_ERR_STAT_RANKS	__BITS(0, 7)

/*
 * Thermal, per-channel
 */
/* E5 v2 */
#define PCISLOT_E5V2_IMC0_THERMAL_CHN0		16
#define PCIFUNC_E5V2_IMC0_THERMAL_CHN0		0
#define PCI_E5V2_IMC0_THERMAL_CHN0_DID_ID	0x0eb0
#define PCISLOT_E5V2_IMC0_THERMAL_CHN1		16
#define PCIFUNC_E5V2_IMC0_THERMAL_CHN1		1
#define PCI_E5V2_IMC0_THERMAL_CHN1_DID_ID	0x0eb1
#define PCISLOT_E5V2_IMC0_THERMAL_CHN2		16
#define PCIFUNC_E5V2_IMC0_THERMAL_CHN2		4
#define PCI_E5V2_IMC0_THERMAL_CHN2_DID_ID	0x0eb4
#define PCISLOT_E5V2_IMC0_THERMAL_CHN3		16
#define PCIFUNC_E5V2_IMC0_THERMAL_CHN3		5
#define PCI_E5V2_IMC0_THERMAL_CHN3_DID_ID	0x0eb5
/* E5 v3 */
#define PCISLOT_E5V3_IMC0_THERMAL_CHN0		20
#define PCIFUNC_E5V3_IMC0_THERMAL_CHN0		0
#define PCI_E5V3_IMC0_THERMAL_CHN0_DID_ID	0x2fb0
#define PCISLOT_E5V3_IMC0_THERMAL_CHN1		20
#define PCIFUNC_E5V3_IMC0_THERMAL_CHN1		1
#define PCI_E5V3_IMC0_THERMAL_CHN1_DID_ID	0x2fb1
#define PCISLOT_E5V3_IMC0_THERMAL_CHN2		21
#define PCIFUNC_E5V3_IMC0_THERMAL_CHN2		0
#define PCI_E5V3_IMC0_THERMAL_CHN2_DID_ID	0x2fb4
#define PCISLOT_E5V3_IMC0_THERMAL_CHN3		21
#define PCIFUNC_E5V3_IMC0_THERMAL_CHN3		1
#define PCI_E5V3_IMC0_THERMAL_CHN3_DID_ID	0x2fb5
#define PCISLOT_E5V3_IMC1_THERMAL_CHN0		23
#define PCIFUNC_E5V3_IMC1_THERMAL_CHN0		0
#define PCI_E5V3_IMC1_THERMAL_CHN0_DID_ID	0x2fd0
#define PCISLOT_E5V3_IMC1_THERMAL_CHN1		23
#define PCIFUNC_E5V3_IMC1_THERMAL_CHN1		1
#define PCI_E5V3_IMC1_THERMAL_CHN1_DID_ID	0x2fd1
/* Thermal regs */
#define PCI_E5_IMC_THERMAL_CHN_TEMP_CFG		0x108
#define PCI_E5_IMC_THERMAL_CHN_TEMP_CFG_OLTT_EN	__BIT(31)
#define PCI_E5_IMC_THERMAL_CHN_TEMP_CFG_CLTT	__BIT(29)
#define PCI_E5_IMC_THERMAL_DIMM_TEMP_TH(dimm)	(0x120 + ((dimm) * 4))
#define PCI_E5_IMC_THERMAL_DIMM_TEMP_TH_TEMPHI	__BITS(16, 23)
#define PCI_E5_IMC_THERMAL_DIMM_TEMP_TH_TEMPMID	__BITS(8, 15)
#define PCI_E5_IMC_THERMAL_DIMM_TEMP_TH_TEMPLO	__BITS(0, 7)
#define PCI_E5_IMC_THERMAL_DIMM_TEMP_TH_TEMPMIN	32	/* [MIN, MAX) */
#define PCI_E5_IMC_THERMAL_DIMM_TEMP_TH_TEMPMAX	128
#define PCI_E5_IMC_THERMAL_DIMM_TEMP_TH_DISABLE	255
#define PCI_E5_IMC_THERMAL_DIMMTEMPSTAT(dimm)	(0x150 + ((dimm) * 4))
#define PCI_E5_IMC_THERMAL_DIMMTEMPSTAT_TEMPHI	__BIT(28)
#define PCI_E5_IMC_THERMAL_DIMMTEMPSTAT_TEMPMID	__BIT(27)
#define PCI_E5_IMC_THERMAL_DIMMTEMPSTAT_TEMPLO	__BIT(26)
#define PCI_E5_IMC_THERMAL_DIMMTEMPSTAT_TEMPOEMLO __BIT(25)
#define PCI_E5_IMC_THERMAL_DIMMTEMPSTAT_TEMPOEMHI __BIT(24)
#define PCI_E5_IMC_THERMAL_DIMMTEMPSTAT_TEMP	__BITS(0, 7)
#define PCI_E5_IMC_THERMAL_DIMMTEMPSTAT_TEMPMIN 0	/* [MIN, MAX) */
#define PCI_E5_IMC_THERMAL_DIMMTEMPSTAT_TEMPMAX	127

#endif	/* !_E5_IMC_REG_H_ */
