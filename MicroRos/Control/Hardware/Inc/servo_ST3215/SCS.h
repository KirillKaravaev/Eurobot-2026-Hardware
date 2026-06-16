/*
 * SCS.h
 * ?????????????
 * ??: 2019.12.18
 * ??: 
 */

#ifndef _SCS_H
#define _SCS_H

#include "INST.h"

class SCS{
public:
	SCS();
	SCS(u8 End);
	SCS(u8 End, u8 Level);
	int genWrite(u8 ID, u8 MemAddr, u8 *nDat, u8 nLen);//?????
	int regWrite(u8 ID, u8 MemAddr, u8 *nDat, u8 nLen);//?????
	int RegWriteAction(u8 ID = 0xfe);//???????
	void syncWrite(u8 ID[], u8 IDN, u8 MemAddr, u8 *nDat, u8 nLen);//?????
	int writeByte(u8 ID, u8 MemAddr, u8 bDat);//?1???
	int writeWord(u8 ID, u8 MemAddr, u16 wDat);//?2???
	int Read(u8 ID, u8 MemAddr, u8 *nData, u8 nLen);//???
	int readByte(u8 ID, u8 MemAddr);//?1???
	int readWord(u8 ID, u8 MemAddr);//?2???
	int Ping(u8 ID);//Ping??
	int syncReadPacketTx(u8 ID[], u8 IDN, u8 MemAddr, u8 nLen);//????????
	int syncReadPacketRx(u8 ID, u8 *nDat);//????????,?????????,????0
	int syncReadRxPacketToByte();//??????
	int syncReadRxPacketToWrod(u8 negBit=0);//??????,negBit????,negBit=0?????
public:
	u8 Level;//??????
	u8 End;//????????
	u8 Error;//????
	u8 syncReadRxPacketIndex;
	u8 syncReadRxPacketLen;
	u8 *syncReadRxPacket;
protected:
	virtual int writeSCS(unsigned char *nDat, int nLen) = 0;
	virtual int readSCS(unsigned char *nDat, int nLen) = 0;
	virtual int writeSCS(unsigned char bDat) = 0;
	virtual void rFlushSCS() = 0;
	virtual void wFlushSCS() = 0;
protected:
	void writeBuf(u8 ID, u8 MemAddr, u8 *nDat, u8 nLen, u8 Fun);
	void Host2SCS(u8 *DataL, u8* DataH, u16 Data);//1?16?????2?8??
	u16	SCS2Host(u8 DataL, u8 DataH);//2?8?????1?16??
	int	Ack(u8 ID);//????
	int checkHead();//????
};
#endif