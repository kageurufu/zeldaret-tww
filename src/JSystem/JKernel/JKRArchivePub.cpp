//
// Generated by dtk
// Translation Unit: JKRArchivePub.cpp
//

#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JKernel/JKRAramArchive.h"
#include "JSystem/JKernel/JKRCompArchive.h"
#include "JSystem/JKernel/JKRDvdArchive.h"
#include "JSystem/JKernel/JKRFileFinder.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "JSystem/JKernel/JKRMemArchive.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "dolphin/dvd/dvd.h"

/* 802B7FB4-802B8008       .text check_mount_already__10JKRArchiveFl */
JKRArchive* JKRArchive::check_mount_already(s32 entryNum) {
    JSUList<JKRFileLoader>& volumeList = getVolumeList();
    JSUListIterator<JKRFileLoader> iterator;
    for (iterator = volumeList.getFirst(); iterator != volumeList.getEnd(); ++iterator) {
        if (iterator->getVolumeType() == 'RARC') {
            JKRArchive* archive = (JKRArchive*)iterator.getObject();
            if (archive->mEntryNum == entryNum) {
                archive->mMountCount++;
                return archive;
            }
        }
    }

    return NULL;
}

/* 802B8008-802B8064       .text mount__10JKRArchiveFPCcQ210JKRArchive10EMountModeP7JKRHeapQ210JKRArchive15EMountDirection */
JKRArchive* JKRArchive::mount(const char* path, JKRArchive::EMountMode mountMode, JKRHeap* heap, JKRArchive::EMountDirection mountDirection) {
    s32 entryNum = DVDConvertPathToEntrynum(path);
    if (entryNum < 0)
        return NULL;

    return mount(entryNum, mountMode, heap, mountDirection);
}

/* 802B8064-802B81D0       .text mount__10JKRArchiveFlQ210JKRArchive10EMountModeP7JKRHeapQ210JKRArchive15EMountDirection */
JKRArchive* JKRArchive::mount(s32 entryNum, JKRArchive::EMountMode mountMode, JKRHeap* heap, JKRArchive::EMountDirection mountDirection) {
    JKRArchive* archive = check_mount_already(entryNum);
    if (archive != NULL) {
        return archive;
    } else {
        int alignment;
        if (mountDirection == JKRArchive::MOUNT_DIRECTION_HEAD) {
            alignment = 4;
        } else {
            alignment = -4;
        }

        JKRArchive* archive;
        switch (mountMode) {
        case JKRArchive::MOUNT_MEM:
            archive = new (heap, alignment) JKRMemArchive(entryNum, mountDirection);
            break;
        case JKRArchive::MOUNT_ARAM:
            archive = new (heap, alignment) JKRAramArchive(entryNum, mountDirection);
            break;
        case JKRArchive::MOUNT_DVD:
            archive = new (heap, alignment) JKRDvdArchive(entryNum, mountDirection);
            break;
        case JKRArchive::MOUNT_COMP:
            archive = new (heap, alignment) JKRCompArchive(entryNum, mountDirection);
            break;
        }

        if (archive && archive->getMountMode() == JKRArchive::UNKNOWN_MOUNT_MODE) {
            delete archive;
            archive = NULL;
        }

        return archive;
    }
}

/* 802B81D0-802B8258       .text becomeCurrent__10JKRArchiveFPCc */
bool JKRArchive::becomeCurrent(const char* path) {
    SDIDirEntry* dirEntry;
    if (*path == '/') {
        path++;

        if (*path == '\0')
            path = NULL;
        dirEntry = findDirectory(path, 0);
    } else {
        dirEntry = findDirectory(path, getCurrentDirID());
    }

    bool found = dirEntry != NULL;
    if (found) {
        setCurrentVolume(this);
        setCurrentDirID(dirEntry - mNodes);
    }

    return found;
}

/* 802B8258-802B82D0       .text getDirEntry__10JKRArchiveCFPQ210JKRArchive9SDirEntryUl */
bool JKRArchive::getDirEntry(SDirEntry* dirEntry, u32 index) const {
    SDIFileEntry* fileEntry = findIdxResource(index);
    if (!fileEntry) {
        return false;
    }

    dirEntry->flags = fileEntry->getFlags();
    dirEntry->id = fileEntry->getFileID();
    dirEntry->name = mStringTable + fileEntry->getNameOffset();
    return true;
}

/* 802B82D0-802B8380       .text getGlbResource__10JKRArchiveFUlPCcP10JKRArchive */
void* JKRArchive::getGlbResource(u32 param_0, const char* path, JKRArchive* archive) {
    void* resource = NULL;
    if (archive) {
        return archive->getResource(param_0, path);
    }

    JSUList<JKRFileLoader>& volumeList = getVolumeList();
    JSUListIterator<JKRFileLoader> iterator;
    for (iterator = volumeList.getFirst(); iterator != volumeList.getEnd(); ++iterator) {
        if (iterator->getVolumeType() == 'RARC') {
            resource = iterator->getResource(param_0, path);
            if (resource)
                break;
        }
    }

    return resource;
}

/* 802B8380-802B8450       .text getResource__10JKRArchiveFPCc */
void* JKRArchive::getResource(const char* path) {
    JUT_ASSERT(VERSION_SELECT(337, 285, 285), isMounted());

    SDIFileEntry* fileEntry;
    if (*path == '/') {
        fileEntry = findFsResource(path + 1, 0);
    } else {
        fileEntry = findFsResource(path, getCurrentDirID());
    }

    if (fileEntry) {
        return fetchResource(fileEntry, NULL);
    }

    return NULL;
}

/* 802B8450-802B8528       .text getResource__10JKRArchiveFUlPCc */
void* JKRArchive::getResource(u32 type, const char* path) {
    JUT_ASSERT(VERSION_SELECT(384, 332, 332), isMounted());

    SDIFileEntry* fileEntry;
    if (type == 0 || type == '????') {
        fileEntry = findNameResource(path);
    } else {
        fileEntry = findTypeResource(type, path);
    }

    if (fileEntry) {
        return fetchResource(fileEntry, NULL);
    }

    return NULL;
}

/* 802B8528-802B85F0       .text readTypeResource__10JKRArchiveFPvUlUlPCcP10JKRArchive */
// missing instructions
u32 JKRArchive::readTypeResource(void* buffer, u32 bufferSize, u32 type, const char* path, JKRArchive* archive) {
    u32 ret = 0;

    if (archive) {
        ret = archive->readResource(buffer, bufferSize, type, path);
    } else {
        for (JSUListIterator<JKRFileLoader> it = sVolumeList.getFirst(); it != sVolumeList.getEnd(); it++) {
            if (it->getVolumeType() == 'RARC') {
                ret = it->readResource(buffer, bufferSize, type, path);
                if (ret != 0)
                    break;
            }
        }
    }

    return ret;
}

/* 802B85F0-802B86DC       .text readResource__10JKRArchiveFPvUlUlPCc */
u32 JKRArchive::readResource(void* buffer, u32 bufferSize, u32 type, const char* path) {
    JUT_ASSERT(VERSION_SELECT(543, 491, 491), isMounted());

    SDIFileEntry* fileEntry;
    if (type == 0 || type == '????') {
        fileEntry = findNameResource(path);
    } else {
        fileEntry = findTypeResource(type, path);
    }

    if (fileEntry) {
        u32 resourceSize;
        fetchResource(buffer, bufferSize, fileEntry, &resourceSize);
        return resourceSize;
    }

    return 0;
}

/* 802B86DC-802B87C0       .text readResource__10JKRArchiveFPvUlPCc */
u32 JKRArchive::readResource(void* buffer, u32 bufferSize, const char* path) {
    JUT_ASSERT(VERSION_SELECT(589, 537, 537), isMounted());

    SDIFileEntry* fileEntry;
    if (*path == '/') {
        fileEntry = findFsResource(path + 1, 0);
    } else {
        fileEntry = findFsResource(path, getCurrentDirID());
    }

    if (fileEntry) {
        u32 resourceSize;
        fetchResource(buffer, bufferSize, fileEntry, &resourceSize);
        return resourceSize;
    }

    return 0;
}

/* 802B87C0-802B8878       .text readIdxResource__10JKRArchiveFPvUlUl */
u32 JKRArchive::readIdxResource(void* buffer, u32 bufferSize, u32 index) {
    JUT_ASSERT(VERSION_SELECT(645, 593, 593), isMounted());

    SDIFileEntry* fileEntry = findIdxResource(index);
    if (fileEntry) {
        u32 resourceSize;
        fetchResource(buffer, bufferSize, fileEntry, &resourceSize);
        return resourceSize;
    }

    return 0;
}

/* 802B8878-802B8930       .text readResource__10JKRArchiveFPvUlUs */
u32 JKRArchive::readResource(void* buffer, u32 bufferSize, u16 id) {
    JUT_ASSERT(VERSION_SELECT(679, 627, 627), isMounted());

    SDIFileEntry* fileEntry = findIdResource(id);
    if (fileEntry) {
        u32 resourceSize;
        fetchResource(buffer, bufferSize, fileEntry, &resourceSize);
        return resourceSize;
    }

    return 0;
}

/* 802B8930-802B89B8       .text removeResourceAll__10JKRArchiveFv */
void JKRArchive::removeResourceAll() {
    if (mArcInfoBlock && mMountMode != MOUNT_MEM) {
        SDIFileEntry* fileEntry = mFiles;
        for (int i = 0; i < mArcInfoBlock->num_file_entries; fileEntry++, i++) {
            if (fileEntry->data) {
                JKRFreeToHeap(mHeap, fileEntry->data);
                fileEntry->data = NULL;
            }
        }
    }
}

/* 802B89B8-802B8A54       .text removeResource__10JKRArchiveFPv */
bool JKRArchive::removeResource(void* resource) {
    JUT_ASSERT(VERSION_SELECT(755, 687, 687), resource != 0);

    SDIFileEntry* fileEntry = findPtrResource(resource);
    if (fileEntry == NULL)
        return false;

    fileEntry->data = NULL;
    JKRFreeToHeap(mHeap, resource);
    return true;
}

/* 802B8A54-802B8AE4       .text detachResource__10JKRArchiveFPv */
bool JKRArchive::detachResource(void* resource) {
    JUT_ASSERT(VERSION_SELECT(787, 719, 719), resource != 0);

    SDIFileEntry* fileEntry = findPtrResource(resource);
    if (fileEntry == NULL)
        return false;

    fileEntry->data = NULL;
    return true;
}

/* 802B8AE4-802B8B6C       .text getResSize__10JKRArchiveCFPCv */
u32 JKRArchive::getResSize(const void* resource) const {
    JUT_ASSERT(VERSION_SELECT(814, 746, 746), resource != 0);

    SDIFileEntry* fileEntry = findPtrResource(resource);
    if (fileEntry == NULL)
        return -1;

    return fileEntry->data_size;
}

/* 802B8B6C-802B8BF0       .text countResource__10JKRArchiveCFUl */
u32 JKRArchive::countResource(u32 type) const {
    SDIDirEntry* dirEntry = findResType(type);
    if (dirEntry) {
        int count = 0;
        for (int i = dirEntry->first_file_index; i < dirEntry->first_file_index + dirEntry->num_entries; i++) {
            if (mFiles[i].isUnknownFlag1()) {
                count++;
            }
        }
        return count;
    }
    return 0;
}

/* 802B8BF0-802B8C50       .text countFile__10JKRArchiveCFPCc */
u32 JKRArchive::countFile(const char* path) const {
    SDIDirEntry* dirEntry;
    if (*path == '/') {
        path++;

        if (*path == '\0')
            path = NULL;
        dirEntry = findDirectory(path, 0);
    } else {
        dirEntry = findDirectory(path, sCurrentDirID);
    }

    if (dirEntry) {
        return dirEntry->num_entries;
    }

    return 0;
}

/* 802B8C50-802B8CFC       .text getFirstFile__10JKRArchiveCFPCc */
JKRFileFinder* JKRArchive::getFirstFile(const char* path) const {
    SDIDirEntry* dirEntry;
    if (*path == '/') {
        path++;

        if (*path == '\0')
            path = NULL;
        dirEntry = findDirectory(path, 0);
    } else {
        dirEntry = findDirectory(path, sCurrentDirID);
    }

    if (dirEntry) {
        return new (JKRHeap::sSystemHeap, 0) JKRArcFinder((JKRArchive*)this, dirEntry->first_file_index, dirEntry->num_entries);
    }

    return NULL;
}

/* 802B8CFC-802B8DC4       .text getFirstResource__10JKRArchiveCFUl */
JKRArcFinder* JKRArchive::getFirstResource(u32 type) const {
    SDIDirEntry* dirEntry = findResType(type);
    if (dirEntry && (getFileAttribute(dirEntry->first_file_index) & 1)) {
        return new (JKRHeap::sSystemHeap, 0) JKRArcFinder((JKRArchive*)this, dirEntry->first_file_index, countResource(type));
    }
    return new (JKRHeap::sSystemHeap, 0) JKRArcFinder((JKRArchive*)this, 0, 0);
}

/* 802B8DC4-802B8DFC       .text getFileAttribute__10JKRArchiveCFUl */
u32 JKRArchive::getFileAttribute(u32 index) const {
    SDIFileEntry* fileEntry = findIdxResource(index);
    if (fileEntry) {
        return fileEntry->getFlags();
    }

    return 0;
}
