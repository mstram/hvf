/*
 * (C) Copyright 2007-2010  Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
 *
 * This file is released under the GPLv2.  See the COPYING file for more
 * details.
 */

#
# At this point, the machine is running in ESA390 mode.
#

.long 0x00080000			# IPL PSW
.long START

.org 88					# EXT NEW PSW
.long 0x000A0000
.long 0x00000001

.org 104				# PROG NEW PSW
.long 0x000A0000
.long 0x00000002

.org 120				# IO NEW PSW
.long 0x00080000
.long IOSVC

.org 184
SUBSYSID:				# SUBSYSTEM ID
.long 0x00000000

.org 512
.globl START
START:					# CODE BEGINS
	# LET'S FIND A CONSOLE TO WORK WITH

	LA	%R1,0x1(%R0)		# LOAD R1 WITH 0XFFFF
	LA	%R2,0x1(%R0)
	SLA	%R1,16
	SR	%R1,%R2
FINDNEXT:
	LA	%R1,1(%R1)		# NEXT SUBCHANNEL

	C	%R1,MAXSCH
	BC	2,FAIL_NONE		# WENT THOUGH ALL SCHS BUT NO CONSOLE

	STSCH	SCHIB
	BC	7,FINDNEXT

	NI	SCHIB+5,0x01
	BC	11,FINDNEXT

	CLC	SCHIB+6(2),DEVNUM
	BC	7,FINDNEXT		# THESE ARE NOT THE DROIDS YOU ARE
					# LOOKING FOR

	OI	SCHIB+5,0x80		# ENABLE

	MSCH	SCHIB
	BC	7,FAIL_MSCH		# COULD NOT MODIFY THE SUBCH

	B	FOUND			# GOT THE DEVICE

FAIL_NONE:
	ST	%R1,DISWAIT+4
	LPSW	DISWAIT			# LOAD DISABLED WAIT WITH MAGIC

FAIL_ERR:
	OI	DISWAIT+7,0xF0
	LPSW	DISWAIT			# LOAD DISABLED WAIT WITH MAGIC

FAIL_IO:
	OI	DISWAIT+7,0xF1
	LPSW	DISWAIT			# LOAD DISABLED WAIT WITH MAGIC

FAIL_CLOCK:
	OI	DISWAIT+7,0xF2
	LPSW	DISWAIT			# LOAD DISABLED WAIT WITH MAGIC

FAIL_MSCH:
	OI	DISWAIT+7,0xF3
	LPSW	DISWAIT			# LOAD DISABLED WAIT WITH MAGIC

MAXSCH:
.long 0x0001ffff			# HIGHEST VALID SUBCH

DEVNUM:
.short 0x0009				# THE DEVICE WE ARE LOOKING FOR

.align 8
ENWAIT:
.long 0x020A0000
.long 0x00000707			# ENABLED WAIT PSW

DISWAIT:
.long 0x000A0000
.long 0x00000000			# DISABLED WAIT PSW

.align 8
SCHIB:
.skip 13*4				# SOME SPACE FOR THE SCHIB

	# FOUND THE CONSOLE; LET'S PREPARE THE STRING CONSTANTS
FOUND:
	STCTL	%R6,%R6,CR6
	OI	CR6,0xff
	LCTL	%R6,%R6,CR6		# ENABLE ALL IO SUBCLASSES

	L	%R2,READCCW
	LA	%R3,READBUF
	OR	%R2,%R3
	ST	%R2,READCCW		# SET THE READ BUFFER ADDR
					# IN THE READ CCW

	L	%R2,GREETCCW
	LA	%R3,GREETMSG
	OR	%R2,%R3
	ST	%R2,GREETCCW		# SET THE GREETING MESSAGE
					# ADDR IN THE GREETING CCW

	L	%R2,GREETCCW+4
	LA	%R3,_GREETLEN
	OR	%R2,%R3
	ST	%R2,GREETCCW+4		# SET THE GREETING MSG LEN
					# IN THE CCW

	TR	GREETMSG(_GREETLEN),A2E	# TRANSLATE THE GREETING TO
					# EBCDIC

	LA	%R7,_ANSLEN
	LA	%R8,ANS1
	LA	%R9,ANSEND		# GET THE ANSWER LENGTH, AS
					# WELL AS A PTR TO THE BEGINNING OF
					# THE FIRST AND THE END OF THE LAST

TRLOOP:					# TRANSLATE EACH MESSAGE TO EBCDIC
	TR	0(_ANSLEN,%R8),A2E
	AR	%R8,%R7
	CR	%R8,%R9
	BC	4,TRLOOP

	OI	ORB+6,0xff		# ENABLE ALL PATHS
	LA	%R2,GREETCCW		# GET THE GREETING CCW ADDR

	B	GO			# NOW TIME FOR THE SHOW!


IOSVC:					# THIS IS THE IO INT HANDLER
	L	%R2,SUBSYSID
	CR	%R1,%R2
	BC	7,WAIT			# INT WASN'T FOR THE CONSOLE

	TSCH	IRB
	BC	1,FAIL_IO		# COULDN'T TEST

	CLC	SUBSYSID+4(4),ORB
	BC	7,DO_READ		# ARE WE SUPPOSED TO READ?

	XR	%R3,%R3
	IC	%R3,IRB+8
	N	%R3,DE_MASK
	BC	11,WAIT			# CHECK FOR DEVICE END

IO_DONE:				# SHAKE SHAKE SHAKE
	LA	%R2,GREETCCW
	C	%R2,ORB
	BC	8,WAIT_FOR_USER		# DID WE JUST GREET?

	LA	%R2,RESPCCW
	C	%R2,ORB
	BC	8,WAIT_FOR_USER		# DID WE JUST RESPOND + GREET?

	LA	%R3,ANS1		# PTR TO THE FIRST MSG
	LA	%R4,_ANSLEN		# LEN OF EACH MSG
	LA	%R5,NUMANS		# NUMBER OF ANSWERS
	STCK	CLOCK
	BC	3,FAIL_CLOCK		# CAN'T GET CLOCK
	LM	%R8,%R9,CLOCK		# LOAD TOD INTO R8 & R9
	SRDL	%R8,28			# SHIFT TO MAKE QUOTIENT FIT 32 BITS

	# FOR WHATEVER REASON, GNU AS DOESN'T LIKE DLR IN 31BIT MODE
	#DLR	%R8,%R5			# DIVIDE BY NUMBER OF ANSWERS
	.insn rre,0xB9970000,%R8,%R5

	MSR	%R8,%R4			# MULTIPLY BY ANSWER LENGTH
	AR	%R3,%R8			# ADD TO BASE PTR

	L	%R4,RESPCCW		# GET RESPONSE CCW'S FIRST WORD
					# NOTE: IT COMMAND CHAINS TO THE
					# GREETING CCW
	N	%R4,ADDR_MASK		# GET RID OF THE CURRENT ADDRESS
	AR	%R4,%R3
	ST	%R4,RESPCCW		# SAVE UPDATED FIRST CCW WORD

	B	GO			# DISPLAY THE RESPONSE + GREETING

DO_READ:
	LA	%R2,READCCW

GO:
	ST	%R2,ORB			# STORE AN INTERRUPTION PARAM
	ST	%R2,ORB+8		# STORE THE CCW ADDR

	SSCH	ORB			# MAKE IT SO, NUMBER ONE
	BC	7,FAIL_IO		# FAILED? -> TOO BAD
WAIT:
	LPSW	ENWAIT			# WAIT FOR IO INTERRUPT

WAIT_FOR_USER:
	LA	%R2,0
	ST	%R2,ORB
	LPSW	ENWAIT			# WAIT FOR IO INTERRUPT

DE_MASK:
.long 0x00000004			# DEVICE END MASK
ADDR_MASK:
.long 0xff000000			# FIRST CCW WORD MASK (REMOVE ADDRESS)

.align 8
CLOCK:					# TOD
.long 0x00000000
.long 0x00000000

.align 8
RESPCCW:				# RESPONSE CCW; COMMAND CHAINS TO THE
.long 0x01000000			# NEXT CCW
.long 0x40000000 | _ANSLEN
GREETCCW:				# GREETING CCW
.long 0x01000000
.long 0x00000000

GREETMSG:				# GREETING MESSAGE
.ascii "I AM THE MAGIC 8 BALL...ASK A QUESTION\n"
.equ _GREETLEN, .-GREETMSG

ANS1:  .ascii "AS I SEE IT, YES         \n\n"
ANS2:  .ascii "IT IS CERTAIN            \n\n"
ANS3:  .ascii "IT IS DECIDEDLY SO       \n\n"
ANS4:  .ascii "MOST LIKELY              \n\n"
ANS5:  .ascii "OUTLOOK GOOD             \n\n"
ANS6:  .ascii "SIGNS POINT TO YES       \n\n"
ANS7:  .ascii "WITHOUT A DOUBT          \n\n"
ANS8:  .ascii "YES                      \n\n"
ANS9:  .ascii "YES - DEFINITELY         \n\n"
ANS10: .ascii "YOU MAY RELY ON IT       \n\n"
ANS11: .ascii "REPLY HAZY, TRY AGAIN    \n\n"
ANS12: .ascii "ASK AGAIN LATER          \n\n"
ANS13: .ascii "BETTER NOT TELL YOU NOW  \n\n"
ANS14: .ascii "CANNOT PREDICT NOW       \n\n"
ANS15: .ascii "CONCENTRATE AND ASK AGAIN\n\n"
ANS16: .ascii "DON'T COUNT ON IT        \n\n"
ANS17: .ascii "MY REPLY IS NO           \n\n"
ANS18: .ascii "MY SOURCES SAY NO        \n\n"
ANS19: .ascii "OUTLOOK NOT SO GOOD      \n\n"
ANS20: .ascii "VERY DOUBTFUL            \n\n"
.equ _ANSLEN, .-ANS20
.equ NUMANS, 20
ANSEND:

.align 8
READCCW:				# READ USER INPUT CCW
.long 0x04000000
.long 0x00000001

READBUF:				# BUFFER; WE DON'T CARE ABOUT THE DATA
.byte 0x00

.align 8
ORB:					# ORB
.skip 8*4

.align 8
IRB:					# IRB
.skip 24*4

CR6:					# TEMP FOR CR6
.long 0x00000000

A2E:					# ASCII -> EBCDIC TABLE
.byte 0x00, 0x01, 0x02, 0x03, 0x37, 0x2D, 0x2E, 0x2F
.byte 0x16, 0x05, 0x15, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
.byte 0x10, 0x11, 0x12, 0x13, 0x3C, 0x3D, 0x32, 0x26
.byte 0x18, 0x19, 0x3F, 0x27, 0x22, 0x1D, 0x1E, 0x1F
.byte 0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D
.byte 0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61
.byte 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7
.byte 0xF8, 0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F
.byte 0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7
.byte 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6
.byte 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6
.byte 0xE7, 0xE8, 0xE9, 0xBA, 0xE0, 0xBB, 0xB0, 0x6D
.byte 0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87
.byte 0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96
.byte 0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6
.byte 0xA7, 0xA8, 0xA9, 0xC0, 0x4F, 0xD0, 0xA1, 0x07
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x59, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F
.byte 0x90, 0x3F, 0x3F, 0x3F, 0x3F, 0xEA, 0x3F, 0xFF
