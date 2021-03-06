/*
 * (C) Copyright 2007-2010  Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
 *
 * This file is released under the GPLv2.  See the COPYING file for more
 * details.
 */

#include <disassm.h>
#include <vsprintf.h>

static struct disassm_instruction l2_01[256] = {	/* 01xx */
	DA_INST		(0x01, E, PR),
	DA_INST		(0x02, E, UPT),
	DA_INST		(0x04, E, PTFF),
	DA_INST		(0x07, E, SCKPF),
	DA_INST		(0x0A, E, PFPO),
	DA_INST		(0x0B, E, TAM),
	DA_INST		(0x0C, E, SAM24),
	DA_INST		(0x0D, E, SAM31),
	DA_INST		(0x0E, E, SAM64),
	DA_INST		(0xFF, E, TRAP2),
};

static struct disassm_instruction l2_a5[16] = {		/* A5x */
	DA_INST		(0x0,  RI1, IIHH),
	DA_INST		(0x1,  RI1, IIHL),
	DA_INST		(0x2,  RI1, IILH),
	DA_INST		(0x3,  RI1, IILL),
	DA_INST		(0x4,  RI1, NIHH),
	DA_INST		(0x5,  RI1, NIHL),
	DA_INST		(0x6,  RI1, NILH),
	DA_INST		(0x7,  RI1, NILL),
	DA_INST		(0x8,  RI1, OIHH),
	DA_INST		(0x9,  RI1, OIHL),
	DA_INST		(0xA,  RI1, OILH),
	DA_INST		(0xB,  RI1, OILL),
	DA_INST		(0xC,  RI1, LLIHH),
	DA_INST		(0xD,  RI1, LLIHL),
	DA_INST		(0xE,  RI1, LLILH),
	DA_INST		(0xF,  RI1, LLILL),
};

static struct disassm_instruction l2_a7[16] = {		/* A7x */
	DA_INST		(0x0,  RI1, TMLH),
	DA_INST		(0x1,  RI1, TMLL),
	DA_INST		(0x2,  RI1, TMHH),
	DA_INST		(0x3,  RI1, TMHL),
	DA_INST		(0x4,  RI2, BRC),
	DA_INST		(0x5,  RI1, BRAS),
	DA_INST		(0x6,  RI1, BRCT),
	DA_INST		(0x7,  RI1, BRCTG),
	DA_INST		(0x8,  RI1, LHI),
	DA_INST		(0x9,  RI1, LGHI),
	DA_INST		(0xA,  RI1, AHI),
	DA_INST		(0xB,  RI1, AGHI),
	DA_INST		(0xC,  RI1, MHI),
	DA_INST		(0xD,  RI1, MGHI),
	DA_INST		(0xE,  RI1, CHI),
	DA_INST		(0xF,  RI1, CGHI),
};

static struct disassm_instruction l2_b2[256] = {	/* B2xx */
	DA_INST		(0x02, S, STIDP),
	DA_INST		(0x04, S, SCK),
	DA_INST		(0x05, S, STCK),
	DA_INST		(0x06, S, SCKC),
	DA_INST		(0x07, S, STCKC),
	DA_INST		(0x08, S, SPT),
	DA_INST		(0x09, S, STPT),
	DA_INST		(0x0A, S, SPKA),
	DA_INST		(0x0B, S, IPK),
	DA_INST		(0x0D, S, PTLB),
	DA_INST		(0x10, S, SPX),
	DA_INST		(0x11, S, STPX),
	DA_INST		(0x12, S, STAP),
	DA_INST		(0x14, S, SIE),
	DA_INST		(0x18, S, PC),
	DA_INST		(0x19, S, SAC),
	DA_INST		(0x1A, S, CFC),
	DA_INST		(0x21, RRE, IPTE),
	DA_INST		(0x22, RRE, IPM),
	DA_INST		(0x23, RRE, IVSK),
	DA_INST		(0x24, RRE, IAC),
	DA_INST		(0x25, RRE, SSAR),
	DA_INST		(0x26, RRE, EPAR),
	DA_INST		(0x27, RRE, ESAR),
	DA_INST		(0x28, RRE, PT),
	DA_INST		(0x29, RRE, ISKE),
	DA_INST		(0x2A, RRE, RRBE),
	DA_INST		(0x2B, RRF2, SSKE),
	DA_INST		(0x2C, RRE, TB),
	DA_INST		(0x2D, RRE, DXR),
	DA_INST		(0x2E, RRE, PGIN),
	DA_INST		(0x2F, RRE, PGOUT),
	DA_INST		(0x30, S, CSCH),
	DA_INST		(0x31, S, HSCH),
	DA_INST		(0x32, S, MSCH),
	DA_INST		(0x33, S, SSCH),
	DA_INST		(0x34, S, STSCH),
	DA_INST		(0x35, S, TSCH),
	DA_INST		(0x36, S, TPI),
	DA_INST		(0x37, S, SAL),
	DA_INST		(0x38, S, RSCH),
	DA_INST		(0x39, S, STCRW),
	DA_INST		(0x3A, S, STCPS),
	DA_INST		(0x3B, S, RCHP),
	DA_INST		(0x3D, S, SCHM),
	DA_INST		(0x40, RRE, BAKR),
	DA_INST		(0x41, RRE, CKSM),
	DA_INST		(0x44, RRE, SQDR),
	DA_INST		(0x45, RRE, SQER),
	DA_INST		(0x46, RRE, STURA),
	DA_INST		(0x47, RRE, MSTA),
	DA_INST		(0x48, RRE, PALB),
	DA_INST		(0x49, RRE, EREG),
	DA_INST		(0x4A, RRE, ESTA),
	DA_INST		(0x4B, RRE, LURA),
	DA_INST		(0x4C, RRE, TAR),
	DA_INST		(0x4D, RRE, CPYA),
	DA_INST		(0x4E, RRE, SAR),
	DA_INST		(0x4F, RRE, EAR),
	DA_INST		(0x50, RRE, CSP),
	DA_INST		(0x52, RRE, MSR),
	DA_INST		(0x54, RRE, MVPG),
	DA_INST		(0x55, RRE, MVST),
	DA_INST		(0x57, RRE, CUSE),
	DA_INST		(0x58, RRE, BSG),
	DA_INST		(0x5A, RRE, BSA),
	DA_INST		(0x5D, RRE, CLST),
	DA_INST		(0x5E, RRE, SRST),
	DA_INST		(0x63, RRE, CMPSC),
	DA_INST		(0x76, S, XSCH),
	DA_INST		(0x77, S, RP),
	DA_INST		(0x78, S, STCKE),
	DA_INST		(0x79, S, SACF),
	DA_INST		(0x7C, S, STCKF),
	DA_INST		(0x7D, S, STSI),
	DA_INST		(0x99, S, SRNM),
	DA_INST		(0x9C, S, STFPC),
	DA_INST		(0x9D, S, LFPC),
	DA_INST		(0xA5, RRE, TRE),
	DA_INST		(0xA6, RRF2, CUUTF),
	DA_INST		(0xA7, RRF2, CUTFU),
	DA_INST		(0xB0, S, STFLE),
	DA_INST		(0xB1, S, STFL),
	DA_INST		(0xB2, S, LPSWE),
	DA_INST		(0xB9, S, SRNMT),
	DA_INST		(0xBD, S, LFAS),
	DA_INST		(0xFF, S, TRAP4),
};

static struct disassm_instruction l2_b3[256] = {	/* B3xx */
	DA_INST		(0x00, RRE, LPEBR),
	DA_INST		(0x01, RRE, LNEBR),
	DA_INST		(0x02, RRE, LTEBR),
	DA_INST		(0x03, RRE, LCEBR),
	DA_INST		(0x04, RRE, LDEBR),
	DA_INST		(0x05, RRE, LXDBR),
	DA_INST		(0x06, RRE, LXEBR),
	DA_INST		(0x07, RRE, MXDBR),
	DA_INST		(0x08, RRE, KEBR),
	DA_INST		(0x09, RRE, CEBR),
	DA_INST		(0x0A, RRE, AEBR),
	DA_INST		(0x0B, RRE, SEBR),
	DA_INST		(0x0C, RRE, MDEBR),
	DA_INST		(0x0D, RRE, DEBR),
	DA_INST		(0x0E, RRF1, MAEBR),
	DA_INST		(0x0F, RRF1, MSEBR),
	DA_INST		(0x10, RRE, LPDBR),
	DA_INST		(0x11, RRE, LNDBR),
	DA_INST		(0x12, RRE, LTDBR),
	DA_INST		(0x13, RRE, LCDBR),
	DA_INST		(0x14, RRE, SQEBR),
	DA_INST		(0x15, RRE, SQDBR),
	DA_INST		(0x16, RRE, SQXBR),
	DA_INST		(0x17, RRE, MEEBR),
	DA_INST		(0x18, RRE, KDBR),
	DA_INST		(0x19, RRE, CDBR),
	DA_INST		(0x1A, RRE, ADBR),
	DA_INST		(0x1B, RRE, SDBR),
	DA_INST		(0x1C, RRE, MDBR),
	DA_INST		(0x1D, RRE, DDBR),
	DA_INST		(0x1E, RRF1, MADBR),
	DA_INST		(0x1F, RRF1, MSDBR),
	DA_INST		(0x24, RRE, LDER),
	DA_INST		(0x25, RRE, LXDR),
	DA_INST		(0x26, RRE, LXER),
	DA_INST		(0x2E, RRF1, MAER),
	DA_INST		(0x2F, RRF1, MSER),
	DA_INST		(0x36, RRE, SQXR),
	DA_INST		(0x37, RRE, MEER),
	DA_INST		(0x38, RRF1, MAYLR),
	DA_INST		(0x39, RRF1, MYLR),
	DA_INST		(0x3A, RRF1, MAYR),
	DA_INST		(0x3B, RRF1, MYR),
	DA_INST		(0x3C, RRF1, MAYHR),
	DA_INST		(0x3D, RRF1, MYHR),
	DA_INST		(0x3E, RRF1, MADR),
	DA_INST		(0x3F, RRF1, MSDR),
	DA_INST		(0x40, RRE, LPXBR),
	DA_INST		(0x41, RRE, LNXBR),
	DA_INST		(0x42, RRE, LTXBR),
	DA_INST		(0x43, RRE, LCXBR),
	DA_INST		(0x44, RRE, LEDBR),
	DA_INST		(0x45, RRE, LDXBR),
	DA_INST		(0x46, RRE, LEXBR),
	DA_INST		(0x47, RRF2, FIXBR),
	DA_INST		(0x48, RRE, KXBR),
	DA_INST		(0x49, RRE, CXBR),
	DA_INST		(0x4A, RRE, AXBR),
	DA_INST		(0x4B, RRE, SXBR),
	DA_INST		(0x4C, RRE, MXBR),
	DA_INST		(0x4D, RRE, DXBR),
	DA_INST		(0x50, RRF2, TBEDR),
	DA_INST		(0x51, RRF2, TBDR),
	DA_INST		(0x53, RRF3, DIEBR),
	DA_INST		(0x57, RRF2, FIEBR),
	DA_INST		(0x58, RRE, THDER),
	DA_INST		(0x59, RRE, THDR),
	DA_INST		(0x5B, RRF3, DIDBR),
	DA_INST		(0x5F, RRF2, FIDBR),
	DA_INST		(0x60, RRE, LPXR),
	DA_INST		(0x61, RRE, LNXR),
	DA_INST		(0x62, RRE, LTXR),
	DA_INST		(0x63, RRE, LCXR),
	DA_INST		(0x65, RRE, LXR),
	DA_INST		(0x66, RRE, LEXR),
	DA_INST		(0x67, RRE, FIXR),
	DA_INST		(0x69, RRE, CXR),
	DA_INST		(0x70, RRE, LPDFR),
	DA_INST		(0x71, RRE, LNDFR),
	DA_INST		(0x72, RRF1, CPSDR),
	DA_INST		(0x73, RRE, LCDFR),
	DA_INST		(0x74, RRE, LZER),
	DA_INST		(0x75, RRE, LZDR),
	DA_INST		(0x76, RRE, LZXR),
	DA_INST		(0x77, RRE, FIER),
	DA_INST		(0x7F, RRE, FIDR),
	DA_INST		(0x84, RRE, SFPC),
	DA_INST		(0x85, RRE, SFASR),
	DA_INST		(0x8C, RRE, EFPC),
	DA_INST		(0x94, RRE, CEFBR),
	DA_INST		(0x95, RRE, CDFBR),
	DA_INST		(0x96, RRE, CXFBR),
	DA_INST		(0x98, RRF2, CFEBR),
	DA_INST		(0x99, RRF2, CFDBR),
	DA_INST		(0x9A, RRF2, CFXBR),
	DA_INST		(0xA4, RRE, CEGBR),
	DA_INST		(0xA5, RRE, CDGBR),
	DA_INST		(0xA6, RRE, CXGBR),
	DA_INST		(0xA8, RRF2, CGEBR),
	DA_INST		(0xA9, RRF2, CGDBR),
	DA_INST		(0xAA, RRF2, CGXBR),
	DA_INST		(0xB4, RRE, CEFR),
	DA_INST		(0xB5, RRE, CDFR),
	DA_INST		(0xB6, RRE, CXFR),
	DA_INST		(0xB8, RRF2, CFER),
	DA_INST		(0xB9, RRF2, CFDR),
	DA_INST		(0xBA, RRF2, CFXR),
	DA_INST		(0xC1, RRE, LDGR),
	DA_INST		(0xC4, RRE, CEGR),
	DA_INST		(0xC5, RRE, CDGR),
	DA_INST		(0xC6, RRE, CXGR),
	DA_INST		(0xC8, RRF2, CGER),
	DA_INST		(0xC9, RRF2, CGDR),
	DA_INST		(0xCA, RRF2, CGXR),
	DA_INST		(0xCD, RRE, LGDR),
	DA_INST		(0xD0, RRR, MDTR),
	DA_INST		(0xD1, RRR, DDTR),
	DA_INST		(0xD2, RRR, ADTR),
	DA_INST		(0xD3, RRR, SDTR),
	DA_INST		(0xD4, RRF3, LDETR),
	DA_INST		(0xD5, RRF3, LEDTR),
	DA_INST		(0xD6, RRE, LTDTR),
	DA_INST		(0xD7, RRF3, FIDTR),
	DA_INST		(0xD8, RRR, MXTR),
	DA_INST		(0xD9, RRR, DXTR),
	DA_INST		(0xDA, RRR, AXTR),
	DA_INST		(0xDB, RRR, SXTR),
	DA_INST		(0xDC, RRF3, LXDTR),
	DA_INST		(0xDD, RRF3, LDXTR),
	DA_INST		(0xDE, RRE, LTXTR),
	DA_INST		(0xDF, RRF3, FIXTR),
	DA_INST		(0xE0, RRE, KDTR),
	DA_INST		(0xE1, RRF2, CGDTR),
	DA_INST		(0xE2, RRE, CUDTR),
	DA_INST		(0xE3, RRF3, CSDTR),
	DA_INST		(0xE4, RRE, CDTR),
	DA_INST		(0xE5, RRE, EEDTR),
	DA_INST		(0xE7, RRE, ESDTR),
	DA_INST		(0xE8, RRE, KXTR),
	DA_INST		(0xE9, RRF2, CGXTR),
	DA_INST		(0xEA, RRE, CUXTR),
	DA_INST		(0xEB, RRF3, CSXTR),
	DA_INST		(0xEC, RRE, CXTR),
	DA_INST		(0xED, RRE, EEXTR),
	DA_INST		(0xEF, RRE, ESXTR),
	DA_INST		(0xF1, RRE, CDGTR),
	DA_INST		(0xF2, RRE, CDUTR),
	DA_INST		(0xF3, RRE, CDSTR),
	DA_INST		(0xF4, RRE, CEDTR),
	DA_INST		(0xF5, RRF3, QADTR),
	DA_INST		(0xF6, RRF3, IEDTR),
	DA_INST		(0xF7, RRF3, RRDTR),
	DA_INST		(0xF9, RRE, CXGTR),
	DA_INST		(0xFA, RRE, CXUTR),
	DA_INST		(0xFB, RRE, CXSTR),
	DA_INST		(0xFC, RRE, CEXTR),
	DA_INST		(0xFD, RRF3, QAXTR),
	DA_INST		(0xFE, RRF3, IEXTR),
	DA_INST		(0xFF, RRF3, RRXTR),
};

static struct disassm_instruction l2_b9[256] = {	/* B9xx */
	DA_INST		(0x00, RRE, LPGR),
	DA_INST		(0x01, RRE, LNGR),
	DA_INST		(0x02, RRE, LTGR),
	DA_INST		(0x03, RRE, LCGR),
	DA_INST		(0x04, RRE, LGR),
	DA_INST		(0x05, RRE, LURAG),
	DA_INST		(0x06, RRE, LGBR),
	DA_INST		(0x07, RRE, LGHR),
	DA_INST		(0x08, RRE, AGR),
	DA_INST		(0x09, RRE, SGR),
	DA_INST		(0x0A, RRE, ALGR),
	DA_INST		(0x0B, RRE, SLGR),
	DA_INST		(0x0C, RRE, MSGR),
	DA_INST		(0x0D, RRE, DSGR),
	DA_INST		(0x0E, RRE, EREGG),
	DA_INST		(0x0F, RRE, LRVGR),
	DA_INST		(0x10, RRE, LPGFR),
	DA_INST		(0x11, RRE, LNGFR),
	DA_INST		(0x12, RRE, LTGFR),
	DA_INST		(0x13, RRE, LCGFR),
	DA_INST		(0x14, RRE, LGFR),
	DA_INST		(0x16, RRE, LLGFR),
	DA_INST		(0x17, RRE, LLGTR),
	DA_INST		(0x18, RRE, AGFR),
	DA_INST		(0x19, RRE, SGFR),
	DA_INST		(0x1A, RRE, ALGFR),
	DA_INST		(0x1B, RRE, SLGFR),
	DA_INST		(0x1C, RRE, MSGFR),
	DA_INST		(0x1D, RRE, DSGFR),
	DA_INST		(0x1E, RRE, KMAC),
	DA_INST		(0x1F, RRE, LRVR),
	DA_INST		(0x20, RRE, CGR),
	DA_INST		(0x21, RRE, CLGR),
	DA_INST		(0x25, RRE, STURG),
	DA_INST		(0x26, RRE, LBR),
	DA_INST		(0x27, RRE, LHR),
	DA_INST		(0x2E, RRE, KM),
	DA_INST		(0x2F, RRE, KMC),
	DA_INST		(0x30, RRE, CGFR),
	DA_INST		(0x31, RRE, CLGFR),
	DA_INST		(0x3E, RRE, KIMD),
	DA_INST		(0x3F, RRE, KLMD),
	DA_INST		(0x46, RRE, BCTGR),
	DA_INST		(0x60, RRF2, CGRT),
	DA_INST		(0x61, RRF2, CLGRT),
	DA_INST		(0x72, RRF2, CRT),
	DA_INST		(0x73, RRF2, CLRT),
	DA_INST		(0x80, RRE, NGR),
	DA_INST		(0x81, RRE, OGR),
	DA_INST		(0x82, RRE, XGR),
	DA_INST		(0x83, RRE, FLOGR),
	DA_INST		(0x84, RRE, LLGCR),
	DA_INST		(0x85, RRE, LLGHR),
	DA_INST		(0x86, RRE, MLGR),
	DA_INST		(0x87, RRE, DLGR),
	DA_INST		(0x88, RRE, ALCGR),
	DA_INST		(0x89, RRE, SLBGR),
	DA_INST		(0x8A, RRE, CSPG),
	DA_INST		(0x8D, RRE, EPSW),
	DA_INST		(0x8E, RRF3, IDTE),
	DA_INST		(0x90, RRF2, TRTT),
	DA_INST		(0x91, RRF2, TRTO),
	DA_INST		(0x92, RRF2, TROT),
	DA_INST		(0x93, RRF2, TROO),
	DA_INST		(0x94, RRE, LLCR),
	DA_INST		(0x95, RRE, LLHR),
	DA_INST		(0x96, RRE, MLR),
	DA_INST		(0x97, RRE, DLR),
	DA_INST		(0x98, RRE, ALCR),
	DA_INST		(0x99, RRE, SLBR),
	DA_INST		(0x9A, RRE, EPAIR),
	DA_INST		(0x9B, RRE, ESAIR),
	DA_INST		(0x9D, RRE, ESEA),
	DA_INST		(0x9E, RRE, PTI),
	DA_INST		(0x9F, RRE, SSAIR),
	DA_INST		(0xA2, RRE, PTF),
	DA_INST		(0xAA, RRF3, LPTEA),
	DA_INST		(0xAF, RRE, PFMF),
	DA_INST		(0xB0, RRF2, CU14),
	DA_INST		(0xB1, RRF2, CU24),
	DA_INST		(0xB2, RRE, CU41),
	DA_INST		(0xB3, RRE, CU42),
	DA_INST		(0xBD, RRF2, TRTRE),
	DA_INST		(0xBE, RRE, SRSTU),
	DA_INST		(0xBF, RRF2, TRTE),
};

static struct disassm_instruction l2_c0[16] = {		/* C0x */
	DA_INST		(0x0,  RIL1, LARL),
	DA_INST		(0x1,  RIL1, LGFI),
	DA_INST		(0x4,  RIL2, BRCL),
	DA_INST		(0x5,  RIL1, BRASL),
	DA_INST		(0x6,  RIL1, XIHF),
	DA_INST		(0x7,  RIL1, XILF),
	DA_INST		(0x8,  RIL1, IIHF),
	DA_INST		(0x9,  RIL1, IILF),
	DA_INST		(0xA,  RIL1, NIHF),
	DA_INST		(0xB,  RIL1, NILF),
	DA_INST		(0xC,  RIL1, OIHF),
	DA_INST		(0xD,  RIL1, OILF),
	DA_INST		(0xE,  RIL1, LLIHF),
	DA_INST		(0xF,  RIL1, LLILF),
};

static struct disassm_instruction l2_c2[16] = {		/* C2x */
	DA_INST		(0x0,  RIL1, MSGFI),
	DA_INST		(0x1,  RIL1, MSFI),
	DA_INST		(0x4,  RIL1, SLGFI),
	DA_INST		(0x5,  RIL1, SLFI),
	DA_INST		(0x8,  RIL1, AGFI),
	DA_INST		(0x9,  RIL1, AFI),
	DA_INST		(0xA,  RIL1, ALGFI),
	DA_INST		(0xB,  RIL1, ALFI),
	DA_INST		(0xC,  RIL1, CGFI),
	DA_INST		(0xD,  RIL1, CFI),
	DA_INST		(0xE,  RIL1, CLGFI),
	DA_INST		(0xF,  RIL1, CLFI),
};

static struct disassm_instruction l2_c4[16] = {		/* C4x */
	DA_INST		(0x2,  RIL1, LLHRL),
	DA_INST		(0x4,  RIL1, LGHRL),
	DA_INST		(0x5,  RIL1, LHRL),
	DA_INST		(0x6,  RIL1, LLGHRL),
	DA_INST		(0x7,  RIL1, STHRL),
	DA_INST		(0x8,  RIL1, LGRL),
	DA_INST		(0xB,  RIL1, STGRL),
	DA_INST		(0xC,  RIL1, LGFRL),
	DA_INST		(0xD,  RIL1, LRL),
	DA_INST		(0xE,  RIL1, LLGFRL),
	DA_INST		(0xF,  RIL1, STRL),
};

static struct disassm_instruction l2_c6[16] = {		/* C6x */
	DA_INST		(0x0,  RIL1, EXRL),
	DA_INST		(0x2,  RIL2, PFDRL),
	DA_INST		(0x4,  RIL1, CGHRL),
	DA_INST		(0x5,  RIL1, CHRL),
	DA_INST		(0x6,  RIL1, CLGHRL),
	DA_INST		(0x7,  RIL1, CLHRL),
	DA_INST		(0x8,  RIL1, CGRL),
	DA_INST		(0xA,  RIL1, CLGRL),
	DA_INST		(0xC,  RIL1, CGFRL),
	DA_INST		(0xD,  RIL1, CRL),
	DA_INST		(0xE,  RIL1, CLGFRL),
	DA_INST		(0xF,  RIL1, CLRL),
};

static struct disassm_instruction l2_c8[16] = {		/* C8x */
	DA_INST		(0x0,  SSF, MVCOS),
	DA_INST		(0x1,  SSF, ECTG),
	DA_INST		(0x2,  SSF, CSST),
};

static struct disassm_instruction l2_e3[256] = {	/* E3xx */
	DA_INST		(0x02, RXY, LTG),
	DA_INST		(0x03, RXY, LRAG),
	DA_INST		(0x04, RXY, LG),
	DA_INST		(0x06, RXY, CVBY),
	DA_INST		(0x08, RXY, AG),
	DA_INST		(0x09, RXY, SG),
	DA_INST		(0x0A, RXY, ALG),
	DA_INST		(0x0B, RXY, SLG),
	DA_INST		(0x0C, RXY, MSG),
	DA_INST		(0x0D, RXY, DSG),
	DA_INST		(0x0E, RXY, CVBG),
	DA_INST		(0x0F, RXY, LRVG),
	DA_INST		(0x12, RXY, LT),
	DA_INST		(0x13, RXY, LRAY),
	DA_INST		(0x14, RXY, LGF),
	DA_INST		(0x15, RXY, LGH),
	DA_INST		(0x16, RXY, LLGF),
	DA_INST		(0x17, RXY, LLGT),
	DA_INST		(0x18, RXY, AGF),
	DA_INST		(0x19, RXY, SGF),
	DA_INST		(0x1A, RXY, ALGF),
	DA_INST		(0x1B, RXY, SLGF),
	DA_INST		(0x1C, RXY, MSGF),
	DA_INST		(0x1D, RXY, DSGF),
	DA_INST		(0x1E, RXY, LRV),
	DA_INST		(0x1F, RXY, LRVH),
	DA_INST		(0x20, RXY, CG),
	DA_INST		(0x21, RXY, CLG),
	DA_INST		(0x24, RXY, STG),
	DA_INST		(0x26, RXY, CVDY),
	DA_INST		(0x2E, RXY, CVDG),
	DA_INST		(0x2F, RXY, STRVG),
	DA_INST		(0x30, RXY, CGF),
	DA_INST		(0x31, RXY, CLGF),
	DA_INST		(0x32, RXY, LTGF),
	DA_INST		(0x34, RXY, CGH),
	DA_INST		(0x36, RXY, PFD),
	DA_INST		(0x3E, RXY, STRV),
	DA_INST		(0x3F, RXY, STRVH),
	DA_INST		(0x46, RXY, BCTG),
	DA_INST		(0x50, RXY, STY),
	DA_INST		(0x51, RXY, MSY),
	DA_INST		(0x54, RXY, NY),
	DA_INST		(0x55, RXY, CLY),
	DA_INST		(0x56, RXY, OY),
	DA_INST		(0x57, RXY, XY),
	DA_INST		(0x58, RXY, LY),
	DA_INST		(0x59, RXY, CY),
	DA_INST		(0x5A, RXY, AY),
	DA_INST		(0x5B, RXY, SY),
	DA_INST		(0x5C, RXY, MFY),
	DA_INST		(0x5E, RXY, ALY),
	DA_INST		(0x5F, RXY, SLY),
	DA_INST		(0x70, RXY, STHY),
	DA_INST		(0x71, RXY, LAY),
	DA_INST		(0x72, RXY, STCY),
	DA_INST		(0x73, RXY, ICY),
	DA_INST		(0x75, RXY, LAEY),
	DA_INST		(0x76, RXY, LB),
	DA_INST		(0x77, RXY, LGB),
	DA_INST		(0x78, RXY, LHY),
	DA_INST		(0x79, RXY, CHY),
	DA_INST		(0x7A, RXY, AHY),
	DA_INST		(0x7B, RXY, SHY),
	DA_INST		(0x7C, RXY, MHY),
	DA_INST		(0x80, RXY, NG),
	DA_INST		(0x81, RXY, OG),
	DA_INST		(0x82, RXY, XG),
	DA_INST		(0x86, RXY, MLG),
	DA_INST		(0x87, RXY, DLG),
	DA_INST		(0x88, RXY, ALCG),
	DA_INST		(0x89, RXY, SLBG),
	DA_INST		(0x8E, RXY, STPQ),
	DA_INST		(0x8F, RXY, LPQ),
	DA_INST		(0x90, RXY, LLGC),
	DA_INST		(0x91, RXY, LLGH),
	DA_INST		(0x94, RXY, LLC),
	DA_INST		(0x95, RXY, LLH),
	DA_INST		(0x96, RXY, ML),
	DA_INST		(0x97, RXY, DL),
	DA_INST		(0x98, RXY, ALC),
	DA_INST		(0x99, RXY, SLB),
};

static struct disassm_instruction l2_e5[256] = {	/* E5xx */
	DA_INST		(0x00, SSE, LASP),
	DA_INST		(0x01, SSE, TPROT),
	DA_INST		(0x02, SSE, STRAG),
	DA_INST		(0x0E, SSE, MVCSK),
	DA_INST		(0x0F, SSE, MVCDK),
	DA_INST		(0x44, SIL, MVHHI),
	DA_INST		(0x48, SIL, MVGHI),
	DA_INST		(0x4C, SIL, MVHI),
	DA_INST		(0x54, SIL, CHHSI),
	DA_INST		(0x55, SIL, CLHHSI),
	DA_INST		(0x58, SIL, CGHSI),
	DA_INST		(0x59, SIL, CLGHSI),
	DA_INST		(0x5C, SIL, CHSI),
	DA_INST		(0x5D, SIL, CLFHSI),
};

static struct disassm_instruction l2_eb[256] = {	/* EBxx */
	DA_INST		(0x04, RSY1, LMG),
	DA_INST		(0x0A, RSY1, SRAG),
	DA_INST		(0x0B, RSY1, SLAG),
	DA_INST		(0x0C, RSY1, SRLG),
	DA_INST		(0x0D, RSY1, SLLG),
	DA_INST		(0x0F, RSY1, TRACG),
	DA_INST		(0x14, RSY1, CSY),
	DA_INST		(0x1C, RSY1, RLLG),
	DA_INST		(0x1D, RSY1, RLL),
	DA_INST		(0x20, RSY2, CLMH),
	DA_INST		(0x21, RSY2, CLMY),
	DA_INST		(0x24, RSY1, STMG),
	DA_INST		(0x25, RSY1, STCTG),
	DA_INST		(0x26, RSY1, STMH),
	DA_INST		(0x2C, RSY2, STCMH),
	DA_INST		(0x2D, RSY2, STCMY),
	DA_INST		(0x2F, RSY1, LCTLG),
	DA_INST		(0x30, RSY1, CSG),
	DA_INST		(0x31, RSY1, CDSY),
	DA_INST		(0x3E, RSY1, CDSG),
	DA_INST		(0x44, RSY1, BXHG),
	DA_INST		(0x45, RSY1, BXLEG),
	DA_INST		(0x4C, RSY1, ECAG),
	DA_INST		(0x51, SIY, TMY),
	DA_INST		(0x52, SIY, MVIY),
	DA_INST		(0x54, SIY, NIY),
	DA_INST		(0x55, SIY, CLIY),
	DA_INST		(0x56, SIY, OIY),
	DA_INST		(0x57, SIY, XIY),
	DA_INST		(0x6A, SIY, ASI),
	DA_INST		(0x6E, SIY, ALSI),
	DA_INST		(0x80, RSY2, ICMH),
	DA_INST		(0x81, RSY2, ICMY),
	DA_INST		(0x8E, RSY1, MVCLU),
	DA_INST		(0x8F, RSY1, CLCLU),
	DA_INST		(0x90, RSY1, STMY),
	DA_INST		(0x96, RSY1, LMH),
	DA_INST		(0x98, RSY1, LMY),
	DA_INST		(0x9A, RSY1, LAMY),
	DA_INST		(0x9B, RSY1, STAMY),
	DA_INST		(0xC0, RSL, TP),
};

static struct disassm_instruction l2_ec[256] = {	/* ECxx */
	DA_INST		(0x44, RIE, BRXHG),
	DA_INST		(0x45, RIE, BRXLG),
	DA_INST		(0x54, RIE, RNSBG),
	DA_INST		(0x55, RIE, RISBG),
	DA_INST		(0x56, RIE, ROSBG),
	DA_INST		(0x57, RIE, RXSBG),
	DA_INST		(0x64, RIE, CGRJ),
	DA_INST		(0x65, RIE, CLGRJ),
	DA_INST		(0x70, RIE, CGIT),
	DA_INST		(0x71, RIE, CLGIT),
	DA_INST		(0x72, RIE, CIT),
	DA_INST		(0x73, RIE, CLFIT),
	DA_INST		(0x76, RIE, CRJ),
	DA_INST		(0x77, RIE, CLRJ),
	DA_INST		(0x7C, RIE, CGIJ),
	DA_INST		(0x7D, RIE, CLGIJ),
	DA_INST		(0x7E, RIE, CIJ),
	DA_INST		(0x7F, RIE, CLIJ),
	DA_INST		(0xE4, RRS, CGRB),
	DA_INST		(0xE5, RRS, CLGRB),
	DA_INST		(0xF6, RRS, CRB),
	DA_INST		(0xF7, RRS, CLRB),
	DA_INST		(0xFC, RIS, CGIB),
	DA_INST		(0xFD, RIS, CLGIB),
};

static struct disassm_instruction l2_ed[256] = {	/* EDxx */
	DA_INST		(0x04, RXE, LDEB),
	DA_INST		(0x05, RXE, LXDB),
	DA_INST		(0x06, RXE, LXEB),
	DA_INST		(0x07, RXE, MXDB),
	DA_INST		(0x08, RXE, KEB),
	DA_INST		(0x09, RXE, CEB),
	DA_INST		(0x0A, RXE, AEB),
	DA_INST		(0x0B, RXE, SEB),
	DA_INST		(0x0C, RXE, MDEB),
	DA_INST		(0x0D, RXE, DEB),
	DA_INST		(0x0E, RXE, MAEB),
	DA_INST		(0x0F, RXE, MSEB),
	DA_INST		(0x10, RXE, TCEB),
	DA_INST		(0x11, RXE, TCDB),
	DA_INST		(0x12, RXE, TCXB),
	DA_INST		(0x14, RXE, SQEB),
	DA_INST		(0x15, RXE, SQDB),
	DA_INST		(0x17, RXE, MEEB),
	DA_INST		(0x18, RXE, KDB),
	DA_INST		(0x19, RXE, CDB),
	DA_INST		(0x1A, RXE, ADB),
	DA_INST		(0x1B, RXE, SDB),
	DA_INST		(0x1C, RXE, MDB),
	DA_INST		(0x1D, RXE, DDB),
	DA_INST		(0x1E, RXF, MADB),
	DA_INST		(0x1F, RXF, MSDB),
	DA_INST		(0x24, RXE, LDE),
	DA_INST		(0x25, RXE, LXD),
	DA_INST		(0x26, RXE, LXE),
	DA_INST		(0x2E, RXF, MAE),
	DA_INST		(0x2F, RXF, MSE),
	DA_INST		(0x34, RXE, SQE),
	DA_INST		(0x35, RXE, SQD),
	DA_INST		(0x37, RXE, MEE),
	DA_INST		(0x38, RXF, MAYL),
	DA_INST		(0x39, RXF, MYL),
	DA_INST		(0x3A, RXF, MAY),
	DA_INST		(0x3B, RXF, MY),
	DA_INST		(0x3C, RXF, MAYH),
	DA_INST		(0x3D, RXF, MYH),
	DA_INST		(0x3E, RXF, MAD),
	DA_INST		(0x3F, RXF, MSD),
	DA_INST		(0x40, RXF, SLDT),
	DA_INST		(0x41, RXF, SRDT),
	DA_INST		(0x48, RXF, SLXT),
	DA_INST		(0x49, RXF, SRXT),
	DA_INST		(0x50, RXE, TDCET),
	DA_INST		(0x51, RXE, TDGET),
	DA_INST		(0x54, RXE, TDCDT),
	DA_INST		(0x55, RXE, TDGDT),
	DA_INST		(0x58, RXE, TDCXT),
	DA_INST		(0x59, RXE, TDGXT),
	DA_INST		(0x64, RXY, LEY),
	DA_INST		(0x65, RXY, LDY),
	DA_INST		(0x66, RXY, STEY),
	DA_INST		(0x67, RXY, STDY),
};

static struct disassm_instruction l1[256] = {		/* xx */
	DA_INST_TBL	(0x01, l2_01, 8, 8),
	DA_INST		(0x04, RR, SPM),
	DA_INST		(0x05, RR, BALR),
	DA_INST		(0x06, RR, BCTR),
	DA_INST		(0x07, RR_MASK, BCR),
	DA_INST		(0x0A, I, SVC),
	DA_INST		(0x0B, RR, BSM),
	DA_INST		(0x0C, RR, BASSM),
	DA_INST		(0x0D, RR, BASR),
	DA_INST		(0x0E, RR, MVCL),
	DA_INST		(0x0F, RR, CLCL),
	DA_INST		(0x10, RR, LPR),
	DA_INST		(0x11, RR, LNR),
	DA_INST		(0x12, RR, LTR),
	DA_INST		(0x13, RR, LCR),
	DA_INST		(0x14, RR, NR),
	DA_INST		(0x15, RR, CLR),
	DA_INST		(0x16, RR, OR),
	DA_INST		(0x17, RR, XR),
	DA_INST		(0x18, RR, LR),
	DA_INST		(0x19, RR, CR),
	DA_INST		(0x1A, RR, AR),
	DA_INST		(0x1B, RR, SR),
	DA_INST		(0x1C, RR, MR),
	DA_INST		(0x1D, RR, DR),
	DA_INST		(0x1E, RR, ALR),
	DA_INST		(0x1F, RR, SLR),
	DA_INST		(0x20, RR, LPDR),
	DA_INST		(0x21, RR, LNDR),
	DA_INST		(0x22, RR, LTDR),
	DA_INST		(0x23, RR, LCDR),
	DA_INST		(0x24, RR, HDR),
	DA_INST		(0x25, RR, LDXR),
	DA_INST		(0x26, RR, MXR),
	DA_INST		(0x27, RR, MXDR),
	DA_INST		(0x28, RR, LDR),
	DA_INST		(0x29, RR, CDR),
	DA_INST		(0x2A, RR, ADR),
	DA_INST		(0x2B, RR, SDR),
	DA_INST		(0x2C, RR, MDR),
	DA_INST		(0x2D, RR, DDR),
	DA_INST		(0x2E, RR, AWR),
	DA_INST		(0x2F, RR, SWR),
	DA_INST		(0x30, RR, LPER),
	DA_INST		(0x31, RR, LNER),
	DA_INST		(0x32, RR, LTER),
	DA_INST		(0x33, RR, LCER),
	DA_INST		(0x34, RR, HER),
	DA_INST		(0x35, RR, LEDR),
	DA_INST		(0x36, RR, AXR),
	DA_INST		(0x37, RR, SXR),
	DA_INST		(0x38, RR, LER),
	DA_INST		(0x39, RR, CER),
	DA_INST		(0x3A, RR, AER),
	DA_INST		(0x3B, RR, SER),
	DA_INST		(0x3C, RR, MDER),
	DA_INST		(0x3D, RR, DER),
	DA_INST		(0x3E, RR, AUR),
	DA_INST		(0x3F, RR, SUR),
	DA_INST		(0x40, RX, STH),
	DA_INST		(0x41, RX, LA),
	DA_INST		(0x42, RX, STC),
	DA_INST		(0x43, RX, IC),
	DA_INST		(0x44, RX, EX),
	DA_INST		(0x45, RX, BAL),
	DA_INST		(0x46, RX, BCT),
	DA_INST		(0x47, RX_MASK, BC),
	DA_INST		(0x48, RX, LH),
	DA_INST		(0x49, RX, CH),
	DA_INST		(0x4A, RX, AH),
	DA_INST		(0x4B, RX, SH),
	DA_INST		(0x4C, RX, MH),
	DA_INST		(0x4D, RX, BAS),
	DA_INST		(0x4E, RX, CVD),
	DA_INST		(0x4F, RX, CVB),
	DA_INST		(0x50, RX, ST),
	DA_INST		(0x51, RX, LAE),
	DA_INST		(0x54, RX, N),
	DA_INST		(0x55, RX, CL),
	DA_INST		(0x56, RX, O),
	DA_INST		(0x57, RX, X),
	DA_INST		(0x58, RX, L),
	DA_INST		(0x59, RX, C),
	DA_INST		(0x5A, RX, A),
	DA_INST		(0x5B, RX, S),
	DA_INST		(0x5C, RX, M),
	DA_INST		(0x5D, RX, D),
	DA_INST		(0x5E, RX, AL),
	DA_INST		(0x5F, RX, SL),
	DA_INST		(0x60, RX, STD),
	DA_INST		(0x67, RX, MXD),
	DA_INST		(0x68, RX, LD),
	DA_INST		(0x69, RX, CD),
	DA_INST		(0x6A, RX, AD),
	DA_INST		(0x6B, RX, SD),
	DA_INST		(0x6C, RX, MD),
	DA_INST		(0x6D, RX, DD),
	DA_INST		(0x6E, RX, AW),
	DA_INST		(0x6F, RX, SW),
	DA_INST		(0x70, RX, STE),
	DA_INST		(0x71, RX, MS),
	DA_INST		(0x78, RX, LE),
	DA_INST		(0x79, RX, CE),
	DA_INST		(0x7A, RX, AE),
	DA_INST		(0x7B, RX, SE),
	DA_INST		(0x7C, RX, MDE),
	DA_INST		(0x7D, RX, DE),
	DA_INST		(0x7E, RX, AU),
	DA_INST		(0x7F, RX, SU),
	DA_INST		(0x80, S, SSM),
	DA_INST		(0x82, S, LPSW),
	DA_INST		(0x83, DIAG, DIAG),
	DA_INST		(0x84, RSI, BRXH),
	DA_INST		(0x85, RSI, BRXLE),
	DA_INST		(0x86, RS1, BXH),
	DA_INST		(0x87, RS1, BXLE),
	DA_INST		(0x88, RS1, SRL),
	DA_INST		(0x89, RS1, SLL),
	DA_INST		(0x8A, RS1, SRA),
	DA_INST		(0x8B, RS1, SLA),
	DA_INST		(0x8C, RS1, SRDL),
	DA_INST		(0x8D, RS1, SLDL),
	DA_INST		(0x8E, RS1, SRDA),
	DA_INST		(0x8F, RS1, SLDA),
	DA_INST		(0x90, RS1, STM),
	DA_INST		(0x91, SI, TM),
	DA_INST		(0x92, SI, MVI),
	DA_INST		(0x93, S, TS),
	DA_INST		(0x94, SI, NI),
	DA_INST		(0x95, SI, CLI),
	DA_INST		(0x96, SI, OI),
	DA_INST		(0x97, SI, XI),
	DA_INST		(0x98, RS1, LM),
	DA_INST		(0x99, RS1, TRACE),
	DA_INST		(0x9A, RS1, LAM),
	DA_INST		(0x9B, RS1, STAM),
	DA_INST_TBL	(0xA5, l2_a5, 4, 12),
	DA_INST_TBL	(0xA7, l2_a7, 4, 12),
	DA_INST		(0xA8, RS1, MVCLE),
	DA_INST		(0xA9, RS1, CLCLE),
	DA_INST		(0xAC, SI, STNSM),
	DA_INST		(0xAD, SI, STOSM),
	DA_INST		(0xAE, RS1, SIGP),
	DA_INST		(0xAF, SI, MC),
	DA_INST		(0xB1, RX, LRA),
	DA_INST_TBL	(0xB2, l2_b2, 8, 8),
	DA_INST_TBL	(0xB3, l2_b3, 8, 8),
	DA_INST		(0xB6, RS1, STCTL),
	DA_INST		(0xB7, RS1, LCTL),
	DA_INST_TBL	(0xB9, l2_b9, 8, 8),
	DA_INST		(0xBA, RS1, CS),
	DA_INST		(0xBB, RS1, CDS),
	DA_INST		(0xBD, RS2, CLM),
	DA_INST		(0xBE, RS2, STCM),
	DA_INST		(0xBF, RS2, ICM),
	DA_INST_TBL	(0xC0, l2_c0, 4, 12),
	DA_INST_TBL	(0xC2, l2_c2, 4, 12),
	DA_INST_TBL	(0xC4, l2_c4, 4, 12),
	DA_INST_TBL	(0xC6, l2_c6, 4, 12),
	DA_INST_TBL	(0xC8, l2_c8, 4, 12),
	DA_INST		(0xD0, SS1, TRTR),
	DA_INST		(0xD1, SS1, MVN),
	DA_INST		(0xD2, SS1, MVC),
	DA_INST		(0xD3, SS1, MVZ),
	DA_INST		(0xD4, SS1, NC),
	DA_INST		(0xD5, SS1, CLC),
	DA_INST		(0xD6, SS1, OC),
	DA_INST		(0xD7, SS1, XC),
	DA_INST		(0xD9, SS4, MVCK),
	DA_INST		(0xDA, SS4, MVCP),
	DA_INST		(0xDB, SS4, MVCS),
	DA_INST		(0xDC, SS1, TR),
	DA_INST		(0xDD, SS1, TRT),
	DA_INST		(0xDE, SS1, ED),
	DA_INST		(0xDF, SS1, EDMK),
	DA_INST		(0xE1, SS1, PKU),
	DA_INST		(0xE2, SS1, UNPKU),
	DA_INST_TBL	(0xE3, l2_e3, 8, 40),
	DA_INST_TBL	(0xE5, l2_e5, 8, 8),
	DA_INST		(0xE8, SS1, MVCIN),
	DA_INST		(0xE9, SS1, PKA),
	DA_INST		(0xEA, SS1, UNPKA),
	DA_INST_TBL	(0xEB, l2_eb, 8, 40),
	DA_INST_TBL	(0xEC, l2_ec, 8, 40),
	DA_INST_TBL	(0xED, l2_ed, 8, 40),
	DA_INST		(0xEE, SS5, PLO),
	DA_INST		(0xEF, SS5, LMD),
	DA_INST		(0xF0, SS3, SRP),
	DA_INST		(0xF1, SS2, MVO),
	DA_INST		(0xF2, SS2, PACK),
	DA_INST		(0xF3, SS2, UNPK),
	DA_INST		(0xF8, SS2, ZAP),
	DA_INST		(0xF9, SS2, CP),
	DA_INST		(0xFA, SS2, AP),
	DA_INST		(0xFB, SS2, SP),
	DA_INST		(0xFC, SS2, MP),
	DA_INST		(0xFD, SS2, DP),
};

static int da_snprintf(u8 *bytes, char *buf, int buflen, u8 opcode, struct
		       disassm_instruction *table)
{
#define IPFX "%-6s "

	int ilc = opcode >> 6;
	struct disassm_instruction *inst = &table[opcode];

	switch (inst->fmt) {
		case IF_INV:
			snprintf(buf, buflen, "??");
			break;
		case IF_VAR:
			{
				unsigned char subop; /* sub op-code */

				/* get the right byte */
				subop = *(bytes+(inst->loc/8));

				/* shift the needed portion right */
				subop >>= 8 - inst->len - (inst->loc%8);

				/* mask out any unneeded high bits */
				subop &= (1 << inst->len) - 1;

				snprintf(buf, buflen, "??? (many, X'%02X'+  "
					 "X'%02X', table = %p)", opcode, subop,
					 inst->u.ptr);
				if (inst->u.ptr)
					da_snprintf(bytes, buf, buflen, subop, inst->u.ptr);
				break;
			}
		case IF_DIAG:
			snprintf(buf, buflen, IPFX "X'%06X'",
				 inst->u.name,
				 *((u32*)(bytes)) & 0xffffff);		/* I  */
			break;
		case IF_E:
			snprintf(buf, buflen, IPFX,
				 inst->u.name);
			break;
		case IF_I:
			snprintf(buf, buflen, IPFX "X'%02X'",
				 inst->u.name,
				 *(bytes+1));				/* I  */
			break;
		case IF_RI1:
		case IF_RI2:
			snprintf(buf, buflen, IPFX "%s%d,%d",
				 inst->u.name,
				 (inst->fmt == IF_RI1 ? "R" : ""),	/* reg/mask */
				 bytes[1] >> 4,				/* R1/M1 */
				 *((u16*)(bytes+2)));			/* I2 */
			break;
		case IF_RIL1:
		case IF_RIL2:
			snprintf(buf, buflen, IPFX "%s%d,%d",
				 inst->u.name,
				 (inst->fmt == IF_RIL1 ? "R" : ""),	/* reg/mask */
				 bytes[1] >> 4,				/* R1/M1 */
				 *((u32*)(bytes+2)));			/* I2 */
			break;
		case IF_RR:
		case IF_RR_MASK:
			snprintf(buf, buflen, IPFX "%s%d,R%d",
				 inst->u.name,
				 inst->fmt == IF_RR_MASK ? "" : "R",
				 bytes[1] >> 4,				/* R1 */
				 bytes[1] & 0xf);			/* R2 */
			break;
		case IF_RRE:
			snprintf(buf, buflen, IPFX "R%d,R%d",
				 inst->u.name,
				 bytes[3] >> 4,				/* R1 */
				 bytes[3] & 0xf);			/* R2 */
			break;
		case IF_RS1:
		case IF_RS2:
			snprintf(buf, buflen, IPFX "R%d,%s%d,%d(R%d)",
				 inst->u.name,
				 bytes[1] >> 4,				/* R1 */
				 (inst->fmt == IF_RS1 ? "R" : ""),	/* reg/mask */
				 bytes[1] & 0xf,			/* R3/M3 */
				 *((u16*)(bytes+2)) & 0x0fff,		/* D2 */
				 bytes[2] >> 4);			/* B2 */
			break;
		case IF_RSI:
			snprintf(buf, buflen, IPFX "R%d,R%d,%d",
				 inst->u.name,
				 bytes[1] >> 4,				/* R1 */
				 bytes[1] & 0xf,			/* R3 */
				 *((u16*)(bytes+2)));			/* I2 */
			break;
		case IF_RSY1:
		case IF_RSY2:
			snprintf(buf, buflen, IPFX "R%d,%s%d,%d(R%d)",
				 inst->u.name,
				 bytes[1] >> 4,				/* R1 */
				 (inst->fmt == IF_RSY1 ? "R" : ""),	/* reg/mask */
				 bytes[1] & 0xf,			/* R3/M3 */
				 ((u32)*((u16*)(bytes+2)) & 0x0fff) +
				 (((u32)*(bytes+4)) << 12),		/* D2 */
				 bytes[2] >> 4);			/* B2 */
			break;
		case IF_RX:
		case IF_RX_MASK:
			snprintf(buf, buflen, IPFX "%s%d,%d(R%d,R%d)",
				 inst->u.name,
				 inst->fmt == IF_RX_MASK ? "" : "R",
				 bytes[1] >> 4,				/* R1 */
				 *((u16*)(bytes+2)) & 0x0fff,		/* D2 */
				 bytes[1] & 0xf,			/* X2 */
				 bytes[2] >> 4);			/* B2 */
			break;
		case IF_RXY:
			snprintf(buf, buflen, IPFX "R%d,%d(R%d,R%d)",
				 inst->u.name,
				 bytes[1] >> 4,				/* R1 */
				 ((u32)*((u16*)(bytes+2)) & 0x0fff) +
				 (((u32)*(bytes+4)) << 12),		/* D2 */
				 bytes[1] & 0xf,			/* X2 */
				 bytes[2] >> 4);			/* B2 */
			break;
		case IF_S:
			snprintf(buf, buflen, IPFX "%d(R%d)",
				 inst->u.name,
				 *((u16*)(bytes+2)) & 0x0fff,		/* D2 */
				 bytes[2] >> 4);			/* B2 */
			break;
		case IF_SI:
			snprintf(buf, buflen, IPFX "%d(R%d),%d",
				 inst->u.name,
				 *((u16*)(bytes+2)) & 0x0fff,		/* D1 */
				 bytes[2] >> 4,				/* B1 */
				 bytes[1]);				/* I2 */
			break;
		case IF_SS1:
			snprintf(buf, buflen, IPFX "%d(%d,R%d),%d(R%d)",
				 inst->u.name,
				 *((u16*)(bytes+2)) & 0x0fff,		/* D1 */
				 bytes[1]+1,				/* L  */
				 bytes[2] >> 4,				/* B1 */
				 *((u16*)(bytes+4)) & 0x0fff,		/* D2 */
				 bytes[4] >> 4);			/* B2 */
			break;
		case IF_SS2:
			snprintf(buf, buflen, IPFX "%d(%d,R%d),%d(%d,R%d)",
				 inst->u.name,
				 *((u16*)(bytes+2)) & 0x0fff,		/* D1 */
				 (bytes[1] >> 4) + 1,			/* L1 */
				 bytes[2] >> 4,				/* B1 */
				 *((u16*)(bytes+4)) & 0x0fff,		/* D2 */
				 (bytes[1] & 0xf) + 1,			/* L2 */
				 bytes[4] >> 4);			/* B2 */
			break;
		case IF_SS3:
			snprintf(buf, buflen, IPFX "%d(%d,R%d),%d(R%d),%d",
				 inst->u.name,
				 *((u16*)(bytes+2)) & 0x0fff,		/* D1 */
				 (bytes[1] >> 4) + 1,			/* L1 */
				 bytes[2] >> 4,				/* B1 */
				 *((u16*)(bytes+4)) & 0x0fff,		/* D2 */
				 bytes[4] >> 4,				/* B2 */
				 bytes[1] & 0xf);			/* I3 */
			break;
		case IF_SS4:
			snprintf(buf, buflen, IPFX "%d(%d,R%d),%d(R%d),R%d",
				 inst->u.name,
				 *((u16*)(bytes+2)) & 0x0fff,		/* D1 */
				 (bytes[1] >> 4) + 1,			/* L1 */
				 bytes[2] >> 4,				/* B1 */
				 *((u16*)(bytes+4)) & 0x0fff,		/* D2 */
				 bytes[4] >> 4,				/* B2 */
				 bytes[1] & 0xf);			/* R3 */
			break;
		case IF_SS5:
			snprintf(buf, buflen, IPFX "R%d,R%d,%d(R%d),%d(R%d)",
				 inst->u.name,
				 bytes[1] >> 4,				/* R1 */
				 bytes[1] & 0xf,			/* R3 */
				 *((u16*)(bytes+2)) & 0x0fff,		/* D2 */
				 bytes[2] >> 4,				/* B2 */
				 *((u16*)(bytes+4)) & 0x0fff,		/* D4 */
				 bytes[4] >> 4);			/* B4 */
			break;
		default:
			snprintf(buf, buflen, "%s ???", inst->u.name);
			break;
	}

	/* return instruction length in bytes */
	return 2 * (ilc >= 2 ? ilc : ilc + 1);
}

int disassm(u8 *bytes, char *buf, int buflen)
{
	return da_snprintf(bytes, buf, buflen, *bytes, l1);
}
