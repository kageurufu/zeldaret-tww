//
// Generated by dtk
// Translation Unit: JKRDvdAramRipper.cpp
//

#include "JSystem/JKernel/JKRDvdAramRipper.h"
#include "JSystem/JKernel/JKRAram.h"
#include "JSystem/JKernel/JKRAramPiece.h"
#include "JSystem/JKernel/JKRAramStream.h"
#include "JSystem/JKernel/JKRDecomp.h"
#include "JSystem/JKernel/JKRDvdFile.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JSupport/JSUFileStream.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "string.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/vi/vi.h"
#include "global.h"

static int JKRDecompressFromDVDToAram(JKRDvdFile*, u32, u32, u32, u32, u32);
static int decompSZS_subroutine(u8*, u32);
static u8* firstSrcData();
static u8* nextSrcData(u8*);
static u32 dmaBufferFlush(u32);

/* 802BDA14-802BDAB0       .text loadToAram__16JKRDvdAramRipperFlUl15JKRExpandSwitchUlUl */
JKRAramBlock* JKRDvdAramRipper::loadToAram(s32 entryNumber, u32 address, JKRExpandSwitch expandSwitch, u32 param_3, u32 param_4) {
    JKRDvdFile dvdFile;
    if (!dvdFile.open(entryNumber)) {
        return NULL;
    } else {
        return loadToAram(&dvdFile, address, expandSwitch, param_3, param_4);
    }
}

/* 802BDAB0-802BDB50       .text loadToAram__16JKRDvdAramRipperFP10JKRDvdFileUl15JKRExpandSwitchUlUl */
JKRAramBlock* JKRDvdAramRipper::loadToAram(JKRDvdFile* dvdFile, u32 address, JKRExpandSwitch expandSwitch, u32 param_3, u32 param_4) {
    JKRADCommand* command = loadToAram_Async(dvdFile, address, expandSwitch, NULL, param_3, param_4);
    syncAram(command, 0);

    if (command->field_0x44 < 0) {
        delete command;
        return NULL;
    }

    if (address) {
        delete command;
        return (JKRAramBlock*)-1;
    }

    JKRAramBlock* result = command->mBlock;
    delete command;
    return result;
}

bool JKRDvdAramRipper::errorRetry = true;

/* 802BDB50-802BDBFC       .text loadToAram_Async__16JKRDvdAramRipperFP10JKRDvdFileUl15JKRExpandSwitchPFUl_vUlUl */
JKRADCommand* JKRDvdAramRipper::loadToAram_Async(JKRDvdFile* dvdFile, u32 address, JKRExpandSwitch expandSwitch, void (*callback)(u32), u32 param_4, u32 param_5) {
    JKRADCommand* command = new (JKRGetSystemHeap(), -4) JKRADCommand();
    command->mDvdFile = dvdFile;
    command->mAddress = address;
    command->mBlock = NULL;
    command->mExpandSwitch = expandSwitch;
    command->mCallback = callback;
    command->mOffset = param_4;
    command->field_0x18 = param_5;

    if (!callCommand_Async(command)) {
        delete command;
        return NULL;
    }

    return command;
}

JSUList<JKRADCommand> JKRDvdAramRipper::sDvdAramAsyncList;

/* 802BDBFC-802BDF34       .text callCommand_Async__16JKRDvdAramRipperFP12JKRADCommand */
JKRADCommand* JKRDvdAramRipper::callCommand_Async(JKRADCommand* command) {
    s32 compression;
    s32 uncompressedSize;
    bool bVar1 = true;
    JKRDvdFile* dvdFile = command->mDvdFile;
    compression = 0;
    OSLockMutex(&dvdFile->mMutex2);

    if (dvdFile->field_0x50) {
        bVar1 = false;
    } else {
        dvdFile->field_0x50 = OSGetCurrentThread();
        JSUFileInputStream* stream = new (JKRGetSystemHeap(), -4) JSUFileInputStream(dvdFile);
        dvdFile->mFileStream = stream;
        u32 fileSize = dvdFile->getFileSize();
        if (command->field_0x18 && fileSize > command->field_0x18) {
            fileSize = command->field_0x18;
        }
        fileSize = ALIGN_NEXT(fileSize, 0x20);
        if (command->mExpandSwitch == 1) {
            u8 buffer[0x40];
            u8* bufPtr = (u8*)ALIGN_NEXT((u32)&buffer, 0x20);
            while (true) {
                if (DVDReadPrio(dvdFile->getFileInfo(), bufPtr, 0x20, 0, 2) >= 0) {
                    break;
                }

                if (errorRetry == 0) {
                    delete stream;
                    return NULL;
                }

                VIWaitForRetrace();
            }
            DCInvalidateRange(bufPtr, 0x20);

            compression = JKRCheckCompressed(bufPtr);
            u32 expandSize = JKRDecompExpandSize(bufPtr);
            uncompressedSize = expandSize;
            if (command->field_0x18 && uncompressedSize > command->field_0x18) {
                uncompressedSize = command->field_0x18;
            }
        }

        if (compression == 0) {
            command->mExpandSwitch = EXPAND_SWITCH_UNKNOWN0;
        }

        if (command->mExpandSwitch == EXPAND_SWITCH_UNKNOWN1) {
            if (command->mAddress == 0 && command->mBlock == NULL) {
                command->mBlock =
                    JKRAllocFromAram(uncompressedSize, JKRAramHeap::HEAD);
                if (command->mBlock) {
                    command->mAddress = command->mBlock->getAddress();
                }
                dvdFile->mBlock = command->mBlock;
            }

            if (command->mBlock) {
                command->mAddress = command->mBlock->getAddress();
            }

            if (command->mAddress == 0) {
                dvdFile->field_0x50 = NULL;
                return NULL;
            }
        } else {
            if (command->mAddress == 0 && !command->mBlock) {
                command->mBlock = JKRAram::getAramHeap()->alloc(fileSize, JKRAramHeap::HEAD);
            }

            if (command->mBlock) {
                command->mAddress = command->mBlock->getAddress();
            }

            if (command->mAddress == 0) {
                dvdFile->field_0x50 = NULL;
                return NULL;
            }
        }

        if (compression == 0) {
            command->mStreamCommand = JKRStreamToAram_Async(stream, command->mAddress, fileSize - command->mOffset, command->mOffset, NULL);
        } else if (compression == 1) {
            command->mStreamCommand = JKRStreamToAram_Async(stream, command->mAddress, fileSize - command->mOffset, command->mOffset, NULL);
        } else if (compression == 2) {
            command->mStreamCommand = NULL;
            JKRDecompressFromDVDToAram(command->mDvdFile, command->mAddress, fileSize, uncompressedSize, command->mOffset, 0);
        }

        if (!command->mCallback) {
            (*((JSUList<JKRADCommand>*)&sDvdAramAsyncList)).append(&command->mLink);
        } else {
            command->mCallback((u32)command);
        }
    }

    OSUnlockMutex(&dvdFile->mMutex2);
    return bVar1 == true ? command : NULL;
}

/* 802BDFA4-802BE078       .text syncAram__16JKRDvdAramRipperFP12JKRADCommandi */
bool JKRDvdAramRipper::syncAram(JKRADCommand* command, int param_1) {
    JKRDvdFile* dvdFile = command->mDvdFile;
    OSLockMutex(&dvdFile->mMutex2);

    if (command->mStreamCommand) {
        JKRAramStreamCommand* var1 = JKRAramStream::sync(command->mStreamCommand, param_1);
        command->field_0x44 = -(var1 == NULL);

        if (param_1 != 0 && var1 == NULL) {
            OSUnlockMutex(&dvdFile->mMutex2);
            return false;
        }
    }

    (*((JSUList<JKRADCommand>*)&sDvdAramAsyncList)).remove(&command->mLink);
    if (command->mStreamCommand) {
        delete command->mStreamCommand;
    }

    delete dvdFile->mFileStream;
    dvdFile->field_0x50 = NULL;
    OSUnlockMutex(&dvdFile->mMutex2);
    return true;
}

/* 802BE078-802BE0B8       .text __ct__12JKRADCommandFv */
JKRADCommand::JKRADCommand() : mLink(this) {
    field_0x44 = 0;
    field_0x48 = false;
}

/* 802BE0B8-802BE144       .text __dt__12JKRADCommandFv */
JKRADCommand::~JKRADCommand() {
    if (field_0x48 == true) {
        delete mDvdFile;
    }
}

static OSMutex decompMutex;
u32 JKRDvdAramRipper::sSzpBufferSize = 0x00000400;
static u8* szpBuf;
static u8* szpEnd;
static u8* refBuf;
static u8* refEnd;
static u8* refCurrent;
u8* dmaBuf;
u8* dmaEnd;
u8* dmaCurrent;
static u32 srcOffset;
static u32 transLeft;
static u8* srcLimit;
static JKRDvdFile* srcFile;
static u32 fileOffset;
static int readCount;
static u32 maxDest;
static bool isInitMutex;

/* 802BE144-802BE34C       .text JKRDecompressFromDVDToAram__FP10JKRDvdFileUlUlUlUlUl */
static int JKRDecompressFromDVDToAram(JKRDvdFile* dvdFile, u32 param_1, u32 fileSize, u32 uncompressedSize, u32 param_4, u32 param_5) {
    BOOL level = OSDisableInterrupts();
    if (!isInitMutex) {
        OSInitMutex(&decompMutex);
        isInitMutex = true;
    }

    OSRestoreInterrupts(level);
    OSLockMutex(&decompMutex);
    u32 bufferSize = JKRDvdAramRipper::getSzpBufferSize();
    szpBuf = (u8*)JKRAllocFromSysHeap(bufferSize, 0x20);
    JUT_ASSERT(VERSION_SELECT(703, 693, 693), szpBuf != 0);
    szpEnd = szpBuf + bufferSize;
    refBuf = (u8*)JKRAllocFromSysHeap(0x1120, 0);
    JUT_ASSERT(VERSION_SELECT(711, 701, 701), refBuf != 0);
    refEnd = refBuf + 0x1120;
    refCurrent = refBuf;
    dmaBuf = (u8*)JKRAllocFromSysHeap(0x100, 0x20);
    JUT_ASSERT(VERSION_SELECT(720, 710, 710), dmaBuf != 0);
    dmaEnd = dmaBuf + 0x100;
    dmaCurrent = dmaBuf;
    srcFile = dvdFile;
    srcOffset = param_5;
    transLeft = fileSize - param_5;
    fileOffset = param_4;
    readCount = 0;
    maxDest = uncompressedSize;
    u8* first = firstSrcData();
    int result = first ? decompSZS_subroutine(first, param_1) : -1;
    JKRHeap::free(szpBuf, 0);
    JKRHeap::free(refBuf, 0);
    JKRHeap::free(dmaBuf, 0);
    OSUnlockMutex(&decompMutex);
    return result;
}

/* 802BE34C-802BE5C0       .text decompSZS_subroutine__FPUcUl */
static int decompSZS_subroutine(u8* src, u32 dest) {
    u32 endAddr;
    u8* copySource;
    s32 validBitCount;
    u32 currCodeByte;
    s32 numBytes;
    u32 startDest;

    validBitCount = 0;
    currCodeByte = 0;
    startDest = dest;

    if (src[0] != 'Y' || src[1] != 'a' || src[2] != 'z' || src[3] != '0') {
        return -1;
    }

    SYaz0Header* header = (SYaz0Header*)src;
    endAddr = dest + (header->length - fileOffset);
    if (endAddr > dest + maxDest) {
        endAddr = dest + maxDest;
    }

    src += 0x10;
    do {
        if (validBitCount == 0) {
            if ((src > srcLimit) && transLeft) {
                src = nextSrcData(src);
            }
            currCodeByte = *src;
            validBitCount = 8;
            src++;
        }
        if (currCodeByte & 0x80) {
            if (readCount >= fileOffset) {
                *(dmaCurrent++) = *src;
                dest++;
                if (dmaCurrent == dmaEnd) {
                    startDest += dmaBufferFlush(startDest);
                }
                if (dest == endAddr) {
                    break;
                }
            }
            *(refCurrent++) = *src;
            if (refCurrent == refEnd) {
                refCurrent = refBuf;
            }
            src++;
            readCount++;
        } else {
            u32 dist = ((src[0] & 0x0F) << 8) | src[1];
            numBytes = src[0] >> 4;
            src += 2;
            copySource = refCurrent - dist - 1;
            if (copySource < refBuf) {
                copySource += refEnd - refBuf;
            }
            if (numBytes == 0) {
                numBytes = *(src++);
                numBytes += 0x12;
            } else {
                numBytes += 2;
            }
            do {
                if (readCount >= fileOffset) {
                    *(dmaCurrent++) = *copySource;
                    dest++;
                    if (dmaCurrent == dmaEnd) {
                        startDest += dmaBufferFlush(startDest);
                    }
                    if (dest == endAddr) {
                        break;
                    }
                }
                *(refCurrent++) = *copySource;
                if (refCurrent == refEnd) {
                    refCurrent = refBuf;
                }
                copySource++;
                if (copySource == refEnd) {
                    copySource = refBuf;
                }
                readCount++;
                numBytes--;
            } while (numBytes != 0);
        }
        currCodeByte <<= 1;
        validBitCount--;
    } while (dest < endAddr);
    dmaBufferFlush(startDest);
    return 0;
}

/* 802BE5C0-802BE674       .text firstSrcData__Fv */
static u8* firstSrcData() {
    srcLimit = szpEnd - 0x19;
    u8* buffer = szpBuf;
    u32 bufSize = szpEnd - buffer;
    u32 length = transLeft < bufSize ? transLeft : bufSize;
    while (true) {
        int result = DVDReadPrio(&srcFile->mFileInfo, buffer, length, 0, 2);
        if (result >= 0) {
            break;
        }
        if (JKRDvdAramRipper::errorRetry == 0) {
            return NULL;
        }
        VIWaitForRetrace();
    }
    srcOffset += length;
    transLeft -= length;
    return buffer;
}

/* 802BE674-802BE790       .text nextSrcData__FPUc */
static u8* nextSrcData(u8* src) {
    u32 size = szpEnd - src;
    u8* dest = IS_NOT_ALIGNED(size, 0x20) ? szpBuf + 0x20 - (size & (0x20 - 1)) : szpBuf;
    memcpy(dest, src, size);
    u32 transSize = szpEnd - (dest + size);
    if (transSize > transLeft) {
        transSize = transLeft;
    }
    JUT_ASSERT(VERSION_SELECT(979, 966, 966), transSize > 0);
    while (true) {
        s32 result = DVDReadPrio(&srcFile->mFileInfo, dest + size, transSize, srcOffset, 2);
        if (result >= 0) {
            break;
        }
        if (JKRDvdAramRipper::errorRetry == 0) {
            return NULL;
        }
        VIWaitForRetrace();
    }
    srcOffset += transSize;
    transLeft -= transSize;
    if (transLeft == 0) {
        srcLimit = dest + size + transSize;
    }
    return dest;
}

/* 802BE790-802BE7F8       .text dmaBufferFlush__FUl */
static u32 dmaBufferFlush(u32 param_1) {
    if (dmaCurrent == dmaBuf) {
        return 0;
    }
    u32 size = ALIGN_NEXT(dmaCurrent - dmaBuf, 0x20);
    JKRAramPiece::orderSync(0, (u32)dmaBuf, param_1, size, NULL);
    dmaCurrent = dmaBuf;
    return size;
}
