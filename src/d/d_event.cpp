//
// Generated by dtk
// Translation Unit: d_event.cpp
//

#include "d/d_event.h"
#include "d/d_com_inf_game.h"
#include "d/d_procname.h"
#include "d/actor/d_a_itembase.h"
#include "f_op/f_op_actor.h" // dEvt_info_c
#include "f_op/f_op_actor_mng.h"
#include "f_pc/f_pc_manager.h"

/* 8006FE04-8006FE54       .text __ct__14dEvt_control_cFv */
dEvt_control_c::dEvt_control_c() {
    remove();
}

/* 8006FE54-8006FE58       .text __ct__12dEvt_order_cFv */
dEvt_order_c::dEvt_order_c() {}

/* 8006FE58-8006FEE8       .text orderOld__14dEvt_control_cFUsUsUsUsPvPvPCv */
s32 dEvt_control_c::orderOld(u16 eventType, u16 priority, u16 flag, u16 hindFlag, void* ac1, void* ac2, const void* eventName) {
    s32 eventIdx = dComIfGp_evmng_getEventIdx((const char*)eventName, 0xFF);
    return order(eventType, priority, flag, hindFlag, ac1, ac2, eventIdx, 0xFF);
}

/* 8006FEE8-8007002C       .text order__14dEvt_control_cFUsUsUsUsPvPvsUc */
s32 dEvt_control_c::order(u16 eventType, u16 priority, u16 flag, u16 hindFlag, void* ac1, void* ac2, s16 eventIdx, u8 eventInfoIdx) {
    if (mOrderCount >= 8)
        return FALSE;

    dEvt_order_c* pNewOrder = &mOrder[mOrderCount];
    pNewOrder->mEventType = eventType;
    pNewOrder->mPriority = priority;
    pNewOrder->mFlag = flag;
    pNewOrder->mActor1 = (fopAc_ac_c*)ac1;
    pNewOrder->mActor2 = (fopAc_ac_c*)ac2;
    pNewOrder->mEventId = eventIdx;
    pNewOrder->mHindFlag = hindFlag;
    pNewOrder->mEventInfoIdx = eventInfoIdx;

    if (pNewOrder->mPriority == 0)
        JUT_ASSERT(0x93, 0);

    if (mOrderCount == 0) {
        mFirstOrderIdx = 0;
        pNewOrder->mNextOrderIdx = -1;
    } else {
        s32 queueIdx = mFirstOrderIdx;
        dEvt_order_c* pQueue = &mOrder[queueIdx];
        if (pNewOrder->mPriority < pQueue->mPriority) {
            mFirstOrderIdx = mOrderCount;
            pNewOrder->mNextOrderIdx = queueIdx;
        } else {
            s8 nextQueueIdx;
            while ((nextQueueIdx = pQueue->mNextOrderIdx) >= 0) {
                if (pNewOrder->mPriority < mOrder[pQueue->mNextOrderIdx].mPriority)
                    break;
                pQueue = &mOrder[nextQueueIdx];
            }

            pNewOrder->mNextOrderIdx = nextQueueIdx;
            pQueue->mNextOrderIdx = mOrderCount;
        }
    }
    mOrderCount++;
    return TRUE;
}

/* 8007002C-8007015C       .text setParam__14dEvt_control_cFP12dEvt_order_c */
void dEvt_control_c::setParam(dEvt_order_c* order) {
    dStage_EventInfo_c* stageEventInfo = dComIfGp_getStageEventInfo();
    mPt1 = getPId(order->mActor1);
    mPt2 = getPId(order->mActor2);
    mEventId = order->mEventId;
    mHindFlag = order->mHindFlag;
    if (dComIfGp_getPlayer(0) != order->mActor1) {
        mPtTalk = getPId(order->mActor1);
        mPtItem = getPId(order->mActor1);
    } else {
        mPtTalk = getPId(order->mActor2);
        mPtItem = getPId(order->mActor2);
    }
    
    mEventInfoIdx = order->mEventInfoIdx;

    if (order->mEventInfoIdx == 0xFF || stageEventInfo == NULL || order->mEventInfoIdx >= stageEventInfo->num)
        field_0xde = 0xFF;
    else
        field_0xde = stageEventInfo->events[order->mEventInfoIdx].field_0x10;

    mCullFarClipRatio = 1.0f;
    mEventFlag = 0;
}

/* 8007015C-8007018C       .text beforeFlagProc__14dEvt_control_cFP12dEvt_order_c */
BOOL dEvt_control_c::beforeFlagProc(dEvt_order_c* order) {
    fopAc_ac_c* actor2 = order->mActor2;
    if ((order->mFlag & dEvtFlag_TALK_e) && !actor2->mEvtInfo.chkCondition(dEvtCnd_CANTALK_e))
        return false;
    return true;
}

/* 8007018C-80070218       .text afterFlagProc__14dEvt_control_cFP12dEvt_order_c */
void dEvt_control_c::afterFlagProc(dEvt_order_c* order) {
    if ((order->mFlag & dEvtFlag_NOPARTNER_e))
        mPt2 = getPId(NULL);

    if ((order->mFlag & dEvtFlag_STAFF_ALL_e))
        dComIfGp_getPEvtManager()->issueStaff("ALL");

    if ((order->mFlag & dEvtFlag_UNK8_e))
        onEventFlag(0x20);
}

/* 80070218-80070290       .text commonCheck__14dEvt_control_cFP12dEvt_order_cUsUs */
BOOL dEvt_control_c::commonCheck(dEvt_order_c* order, u16 cond, u16 cmd) {
    fopAc_ac_c* actor1 = order->mActor1;
    fopAc_ac_c* actor2 = order->mActor2;
    if (actor1 != NULL && actor1->mEvtInfo.chkCondition(cond) && actor2 != NULL && actor2->mEvtInfo.chkCondition(cond)) {
        actor1->mEvtInfo.setCommand(cmd);
        actor2->mEvtInfo.setCommand(cmd);
        setParam(order);
        return TRUE;
    } else {
        return FALSE;
    }
}

/* 80070290-80070390       .text talkCheck__14dEvt_control_cFP12dEvt_order_c */
BOOL dEvt_control_c::talkCheck(dEvt_order_c* order) {
    const char* defaultEventName = "DEFAULT_TALK";
    fopAc_ac_c* actor2 = order->mActor2;
    if (commonCheck(order, dEvtCnd_CANTALK_e, dEvtCmd_INTALK_e)) {
        mMode = dEvtMode_TALK_e;
        if (mEventId == -1) {
            if (actor2 != NULL && actor2->mEvtInfo.getEventName() != NULL) {
                mEventId = actor2->mEvtInfo.getEventId();
            } else {
                mEventId = dComIfGp_evmng_getEventIdx(defaultEventName, 0xFF);
            }
        }

        if (!dComIfGp_evmng_order(mEventId))
            JUT_ASSERT(0x145, 0);

        return TRUE;
    } else {
        return FALSE;
    }
}

/* 80070390-8007055C       .text talkXyCheck__14dEvt_control_cFP12dEvt_order_c */
BOOL dEvt_control_c::talkXyCheck(dEvt_order_c* order) {
    const char* defaultEventName = "DEFAULT_TALK_XY";
    
    fopAc_ac_c* actor2 = order->mActor2;
    
    int equippedItemIdx;
    switch (order->mEventType) {
    case dEvtType_SHOWITEM_X_e:
        equippedItemIdx = 0;
        break;
    case dEvtType_SHOWITEM_Y_e:
        equippedItemIdx = 1;
        break;
    default:
        equippedItemIdx = 2;
        break;
    }
    
    if (dComIfGp_getSelectItem(equippedItemIdx) == NO_ITEM) {
        return FALSE;
    }
    
    if (actor2 == NULL || !actor2->mEvtInfo.chkCondition(dEvtCnd_CANTALKITEM_e)) {
        return FALSE;
    }
    
    if (commonCheck(order, dEvtCnd_CANTALK_e, dEvtCmd_INTALK_e)) {
        mMode = dEvtMode_TALK_e;
        mItemNo = dComIfGp_getSelectItem(equippedItemIdx);
        
        switch (order->mEventType) {
        case dEvtType_SHOWITEM_X_e:
            mTalkButton = 1;
            break;
        case dEvtType_SHOWITEM_Y_e:
            mTalkButton = 2;
            break;
        default:
            mTalkButton = 3;
            break;
        }
        
        // Fakematch
        s16 r3;
        if (actor2 != NULL && (r3 = actor2->mEvtInfo.runXyEventCB(actor2, equippedItemIdx), r3 != -1)) {
            mEventId = r3;
        } else {
            mEventId = dComIfGp_evmng_getEventIdx(defaultEventName, 0xFF);
        }
        
        if (!dComIfGp_evmng_order(mEventId)) {
            JUT_ASSERT(401, 0);
        }
        return TRUE;
    }
    
    return FALSE;
}

/* 8007055C-800706D8       .text photoCheck__14dEvt_control_cFP12dEvt_order_c */
BOOL dEvt_control_c::photoCheck(dEvt_order_c* order) {
    fopAc_ac_c* actor2 = order->mActor2;
    const char* defaultEventName = "DEFAULT_TALK";

    mbInPhoto = 0;

    if (dComIfGp_getPictureStatus() == 1) {
        JUT_ASSERT(0x1a5, actor2);
        if (!commonCheck(order, dEvtCnd_CANTALK_e, dEvtCmd_INTALK_e))
            return FALSE;

        s16 eventId = actor2->mEvtInfo.runPhotoEventCB(actor2, 0);
        if (eventId != -1) {
            mEventId = eventId;
        } else {
            mEventId = dComIfGp_evmng_getEventIdx(defaultEventName, 0xFF);
        }

        if (!dComIfGp_evmng_order(mEventId))
            JUT_ASSERT(0x1b5, 0);

        mbInPhoto = 1;
        mMode = dEvtMode_TALK_e;
        return TRUE;
    } else if (dComIfGp_getPictureStatus() == 0) {
        return FALSE;
    }

    return FALSE;
}

/* 800706D8-800707C0       .text catchCheck__14dEvt_control_cFP12dEvt_order_c */
BOOL dEvt_control_c::catchCheck(dEvt_order_c* order) {
    fopAc_ac_c* actor2 = order->mActor2;
    fopAc_ac_c* actor1 = order->mActor1;

    if (actor1 == NULL || (actor2 != NULL && !actor2->mEvtInfo.chkCondition(dEvtCnd_CANCATCH_e)))
        return FALSE;

    actor1->mEvtInfo.setCommand(dEvtCmd_INCATCH_e);
    if (actor2 != NULL)
        actor2->mEvtInfo.setCommand(dEvtCmd_INCATCH_e);
    setParam(order);
    mItemNo = dComIfGp_att_getCatchChgItem();
    mMode = dEvtMode_DEMO_e;
    if (mEventId != -1 && !dComIfGp_evmng_order(mEventId))
        JUT_ASSERT(0x1e3, 0);
    onEventFlag(0x80);
    return TRUE;
}

/* 800707C0-80070870       .text talkEnd__14dEvt_control_cFv */
BOOL dEvt_control_c::talkEnd() {
    fopAc_ac_c* actor1 = getPt1();
    if (actor1 != NULL)
        actor1->mEvtInfo.setCommand(dEvtCmd_NONE_e);
    fopAc_ac_c* actor2 = getPt2();
    if (actor2 != NULL)
        actor2->mEvtInfo.setCommand(dEvtCmd_NONE_e);
    if (mEventId != -1) {
        dComIfGp_getPEvtManager()->endProc(mEventId, 1);
        mEventId = -1;
    }
    fopAc_ac_c* itemPartner = fopAcM_getItemEventPartner(NULL);
    if (itemPartner != NULL && (fopAcM_GetName(itemPartner) == PROC_ITEM || fopAcM_GetName(itemPartner) == PROC_Demo_Item))
        ((daItemBase_c*)itemPartner)->dead();
    return TRUE;
}

/* 80070870-800709C0       .text demoCheck__14dEvt_control_cFP12dEvt_order_c */
BOOL dEvt_control_c::demoCheck(dEvt_order_c* order) {
    fopAc_ac_c* actor1 = order->mActor1;
    fopAc_ac_c* actor2 = order->mActor2;
    s16 eventId = order->mEventId;

    if (actor2 == NULL) {
        JUT_ASSERT(0x21f, 0);
        return FALSE;
    }

    if (!beforeFlagProc(order))
        return FALSE;

    if (!dComIfGp_evmng_order(eventId))
        return FALSE;

    if (actor1 != NULL)
        actor1->mEvtInfo.setCommand(dEvtCmd_INDEMO_e);
    if (actor2 != NULL)
        actor2->mEvtInfo.setCommand(dEvtCmd_INDEMO_e);

    mMode = dEvtMode_DEMO_e;
    setParam(order);
    afterFlagProc(order);

    u8 eventEndSound = dComIfGp_evmng_getEventEndSound(eventId);
    if (eventEndSound != 0 && mEventEndSound == 0) {
        mEventEndSound = eventEndSound;
        mDoAud_seStart(JA_SE_READ_RIDDLE_1);
    }
    return TRUE;
}

/* 800709C0-80070A64       .text demoEnd__14dEvt_control_cFv */
BOOL dEvt_control_c::demoEnd() {
    fopAc_ac_c* actor1 = getPt1();
    if (actor1 != NULL)
        actor1->mEvtInfo.setCommand(dEvtCmd_NONE_e);
    fopAc_ac_c* actor2 = getPt2();
    if (actor2 != NULL)
        actor2->mEvtInfo.setCommand(dEvtCmd_NONE_e);
    if (mEventId != -1) {
        dComIfGp_getPEvtManager()->endProc(mEventId, 1);
        mEventId = -1;
    }
    dComIfGp_getPEvtManager()->cancelStaff("ALL");
    return TRUE;
}

/* 80070A64-80070B24       .text potentialCheck__14dEvt_control_cFP12dEvt_order_c */
BOOL dEvt_control_c::potentialCheck(dEvt_order_c* order) {
    fopAc_ac_c* actor1 = order->mActor1;
    fopAc_ac_c* actor2 = order->mActor2;
    if (actor1 == NULL || actor2 == NULL)
        JUT_ASSERT(0x280, 0);

    if (!beforeFlagProc(order))
        return FALSE;

    actor1->mEvtInfo.setCommand(dEvtCmd_INDEMO_e);
    mMode = dEvtMode_DEMO_e;
    setParam(order);
    afterFlagProc(order);
    return TRUE;
}

/* 80070B24-80070C54       .text doorCheck__14dEvt_control_cFP12dEvt_order_c */
BOOL dEvt_control_c::doorCheck(dEvt_order_c* order) {
    if (commonCheck(order, dEvtCnd_CANDOOR_e, dEvtCmd_INDOOR_e)) {
        mMode = dEvtMode_DEMO_e;
        fopAc_ac_c* actor2 = getPt2();
        if (mEventId == -1 && actor2 != NULL && actor2->mEvtInfo.getEventId() != -1)
            mEventId = actor2->mEvtInfo.getEventId();
        if (mEventId != -1 && dComIfGp_getPEvtManager()->getEventData(mEventId) != NULL) {
            if (!dComIfGp_evmng_order(mEventId))
                JUT_ASSERT(0x2c0, 0);
        } else {
            mEventId = -1;
            reset();
        }

        if (order->mEventType == dEvtType_TREASURE_e)
            dComIfGp_event_onEventFlag(4);

        return TRUE;
    } else {
        return FALSE;
    }
}

/* 80070C54-80070D1C       .text itemCheck__14dEvt_control_cFP12dEvt_order_c */
BOOL dEvt_control_c::itemCheck(dEvt_order_c* order) {
    const char* defaultEventName = "DEFAULT_GETITEM";
    if (commonCheck(order, dEvtCnd_CANGETITEM_e, dEvtCmd_INGETITEM_e)) {
        mMode = dEvtMode_DEMO_e;
        mEventId = dComIfGp_evmng_getEventIdx(defaultEventName, 0xFF);
        if (!dComIfGp_evmng_order(mEventId))
            JUT_ASSERT(0x2ea, 0);
        return TRUE;
    } else {
        return FALSE;
    }
}

/* 80070D1C-80070DD4       .text endProc__14dEvt_control_cFv */
BOOL dEvt_control_c::endProc() {
    switch (getMode()) {
    case dEvtMode_TALK_e:
        talkEnd();
        break;
    case dEvtMode_DEMO_e:
        demoEnd();
        break;
    case dEvtMode_COMPULSORY_e:
        break;
    default:
        JUT_ASSERT(0x315, 0);
    }

    mMode = dEvtMode_NONE_e;
    field_0xde = 0xFF;
    mEventInfoIdx = 0xFF;
    mTalkButton = 0;
    mbInPhoto = 0;
    mItemNo = 0xFF;
    mEventFlag = 0;
    return TRUE;
}

/* 80070DD4-80070E1C       .text checkChange__14dEvt_control_cFv */
BOOL dEvt_control_c::checkChange() {
    if (mOrderCount != 0) {
        dEvt_order_c* order;
        s8 nextIdx = mFirstOrderIdx;
        while (true) {
            order = &mOrder[nextIdx];
            nextIdx = order->mNextOrderIdx;
            if ((s32)order->mEventType != dEvtType_CHANGE_e) {
                // Fakematch, assigning a float member to itself tricks the compiler into creating a beq then a b.
                mCullFarClipRatio = mCullFarClipRatio;
            } else {
                return TRUE;
            }

            if (nextIdx < 0)
                break;
        }
    }

    return FALSE;
}

/* 80070E1C-80070EA8       .text changeProc__14dEvt_control_cFv */
BOOL dEvt_control_c::changeProc() {
    if (mOrderCount != 0) {
        mOrderCount = 0;
        s8 nextIdx = mFirstOrderIdx;
        while (true) {
            dEvt_order_c* order = &mOrder[nextIdx];
            nextIdx = order->mNextOrderIdx;
            if ((s32)order->mEventType != dEvtType_CHANGE_e) {
                // Fakematch, assigning a float member to itself tricks the compiler into creating a beq then a b.
                mCullFarClipRatio = mCullFarClipRatio;
            } else if (demoCheck(order)) {
                return TRUE;
            }

            if (nextIdx < 0)
                break;
        }
    }

    return FALSE;
}

/* 80070EA8-80071020       .text checkStart__14dEvt_control_cFv */
BOOL dEvt_control_c::checkStart() {
    if (mOrderCount != 0) {
        s8 idx = mFirstOrderIdx;
        mOrderCount = 0;

        while (true) {
            dEvt_order_c* pOrder = &mOrder[idx];
            idx = pOrder->mNextOrderIdx;

            switch (pOrder->mEventType) {
            case dEvtType_TALK_e:
                if (talkCheck(pOrder))
                    return TRUE;
                break;
            case dEvtType_OTHER_e:
                if (demoCheck(pOrder))
                    return TRUE;
                break;
            case dEvtType_DOOR_e:
            case dEvtType_TREASURE_e:
                if (doorCheck(pOrder))
                    return TRUE;
                break;
            case dEvtType_COMPULSORY_e:
                mMode = dEvtMode_COMPULSORY_e;
                setParam(pOrder);
                return TRUE;
            case dEvtType_POTENTIAL_e:
                return potentialCheck(pOrder);
            case dEvtType_ITEM_e:
                return itemCheck(pOrder);
            case dEvtType_SHOWITEM_X_e:
            case dEvtType_SHOWITEM_Y_e:
            case dEvtType_SHOWITEM_Z_e:
                if (talkXyCheck(pOrder))
                    return TRUE;
                break;
            case dEvtType_PHOTO_e:
                if (photoCheck(pOrder))
                    return TRUE;
                break;
            case dEvtType_CATCH_e:
                if (catchCheck(pOrder))
                    return TRUE;
                break;
            case dEvtType_CHANGE_e:
                break;
            default:
                JUT_ASSERT(0x39c, 0);
                break;
            }

            if (idx < 0)
                break;
        }
    }

    return FALSE;
}

/* 80071020-80071048       .text soundProc__14dEvt_control_cFv */
BOOL dEvt_control_c::soundProc() {
    if (mEventEndSound != 0 && getMode() != dEvtMode_DEMO_e)
        mEventEndSound = 0;
    return TRUE;
}

/* 80071048-80071140       .text check__14dEvt_control_cFv */
BOOL dEvt_control_c::check() {
    field_0xdb = 0;
    field_0xdc = mOrderCount;

    if (chkEventFlag(8)) {
        mbEndProc = 1;
        offEventFlag(8);
    }

    if (runCheck() && mbEndProc == 1) {
        mbEndProc = 0;
        endProc();
    }

    if (checkChange()) {
        if (runCheck()) {
            mbEndProc = 0;
            endProc();
        }

        changeProc();
    }

    if (!runCheck() && checkStart())
        g_dComIfG_gameInfo.play.getVibration().StopQuake(-1);

    soundProc();
    mOrderCount = 0;
    return FALSE;
}

/* 80071140-80071270       .text photoCheck__14dEvt_control_cFv */
BOOL dEvt_control_c::photoCheck() {
    fopAc_ac_c* actor1;
    fopAc_ac_c* actor2;
    s8 orderIdx = mFirstOrderIdx;
    if (mOrderCount != 0) {
        dEvt_order_c* order = &mOrder[orderIdx];
        int equippedItemIdx = -1;
        switch (order->mEventType) {
        case dEvtType_SHOWITEM_X_e:
            equippedItemIdx = 0;
            break;
        case dEvtType_SHOWITEM_Y_e:
            equippedItemIdx = 1;
            break;
        case dEvtType_SHOWITEM_Z_e:
            equippedItemIdx = 2;
            break;
        }
        
        if (equippedItemIdx != -1 &&
            (dComIfGp_getSelectItem(equippedItemIdx) == CAMERA || dComIfGp_getSelectItem(equippedItemIdx) == CAMERA2) &&
            dComIfGs_getPictureNum() != 0
        ) {
            actor2 = order->mActor2;
            if (actor2 == NULL) {
                return FALSE;
            }
            if (!(actor2->mAttentionInfo.mFlags & fopAc_Attn_UNK1000000_e)) {
                return FALSE;
            }
            if (dComIfGp_getPictureStatus() == 2) {
                return FALSE;
            }
            actor1 = order->mActor1;
            if (actor1 == NULL) {
                return FALSE;
            }
            if (!actor1->mEvtInfo.chkCondition(dEvtCnd_CANTALK_e) ||
                !actor2->mEvtInfo.chkCondition(dEvtCnd_CANTALK_e)
            ) {
                return FALSE;
            }
            order->mEventType = dEvtType_PHOTO_e;
            dComIfGp_setPictureStatusOn();
            return TRUE;
        }
    }
    
    return FALSE;
}

/* 80071270-80071418       .text moveApproval__14dEvt_control_cFPv */
s32 dEvt_control_c::moveApproval(void* actor) {
    fopAc_ac_c* i_ac = (fopAc_ac_c*)actor;
    if (getMode() == dEvtMode_NONE_e)
        return dEvtMove_MOVE_e;

    switch (getMode()) {
    case dEvtMode_TALK_e:
        if (convPId(mPt1) == i_ac || convPId(mPt2) == i_ac)
            return dEvtMove_FORCE_e;
        break;
    case dEvtMode_DEMO_e:
        if (convPId(mPt1) == i_ac || convPId(mPt2) == i_ac)
            return dEvtMove_FORCE_e;
        if (i_ac->mDemoActorId != 0)
            return dEvtMove_FORCE_e;
        break;
    case dEvtMode_COMPULSORY_e:
        if (convPId(mPt1) == i_ac)
            return dEvtMove_FORCE_e;
        break;
    }

    if (fopAcM_checkStatus(i_ac, fopAcStts_FORCEMOVE_e))
        return dEvtMove_FORCE_e;

    if (getMode() == dEvtMode_TALK_e && fopAcM_checkStatus(i_ac, fopAcStts_UNK40_e))
        return dEvtMove_MOVE_e;
    if (dComIfGp_demo_mode() == 1)
        return dEvtMove_MOVE_e;
    if (fopAcM_checkStatus(i_ac, fopAcStts_UNK800_e))
        return dEvtMove_MOVE_e;
    if ((dComIfGp_event_getMode() == dEvtMode_COMPULSORY_e || dComIfGp_event_getMode() == dEvtMode_TALK_e) && fopAcM_checkStatus(i_ac, fopAcStts_BOSS_e))
        return dEvtMove_NOMOVE_e;
    if (chkEventFlag(0x80) && fopAcM_checkStatus(i_ac, fopAcStts_BOSS_e))
        return dEvtMove_NOMOVE_e;
    if (fopAcM_checkStatus(i_ac, fopAcStts_UNK4000_e))
        return dEvtMove_MOVE_e;
    return fopAcM_checkStatus(i_ac, fopAcStts_CARRY_e) ? dEvtMove_MOVE_e : dEvtMove_NOMOVE_e;
}

/* 80071418-80071468       .text compulsory__14dEvt_control_cFPvPCcUs */
BOOL dEvt_control_c::compulsory(void* actor, const char* eventName, u16 p3) {
    if (getMode() != dEvtMode_NONE_e)
        return FALSE;

    return orderOld(dEvtType_COMPULSORY_e, 1, 0, p3, actor, NULL, eventName);
}

/* 80071468-800714AC       .text remove__14dEvt_control_cFv */
void dEvt_control_c::remove() {
    mMode = dEvtMode_NONE_e;
    mbEndProc = 0;
    mOrderCount = 0;
    field_0xdd = 0;
    mEventEndSound = 0;
    field_0xdb = 0;
    mCullFarClipRatio = 0.0f;
    field_0xde = 0xff;
    mEventInfoIdx = 0xff;
    mTalkButton = 0;
    mbInPhoto = 0;
    mItemNo = 0xff;
    mEventFlag = 0;
}

/* 800714AC-80071534       .text getStageEventDt__14dEvt_control_cFv */
dStage_Event_dt_c* dEvt_control_c::getStageEventDt() {
    dStage_EventInfo_c* stageEventInfo = dComIfGp_getStageEventInfo();
    if (getMode() == dEvtMode_NONE_e)
        return NULL;

    if (stageEventInfo == NULL || mEventInfoIdx == 0xFF || mEventInfoIdx >= stageEventInfo->num)
        return NULL;

    return &stageEventInfo->events[mEventInfoIdx];
}

/* 80071534-800715B8       .text nextStageEventDt__14dEvt_control_cFPv */
dStage_Event_dt_c* dEvt_control_c::nextStageEventDt(void* idxp) {
    dStage_EventInfo_c* stageEventInfo = dComIfGp_getStageEventInfo();
    if (idxp == NULL)
        return NULL;

    if (stageEventInfo == NULL || *(u8*)idxp == 0xFF || *(u8*)idxp >= stageEventInfo->num)
        return NULL;

    return &stageEventInfo->events[*(u8*)idxp];
}

/* 800715B8-800715DC       .text getPId__14dEvt_control_cFPv */
u32 dEvt_control_c::getPId(void* ac) {
    if (ac == NULL)
        return -1;
    return fopAcM_GetID(ac);
}

/* 800715DC-8007160C       .text convPId__14dEvt_control_cFUi */
fopAc_ac_c* dEvt_control_c::convPId(unsigned int pid) {
    return fopAcM_SearchByID(pid);
}

/* 8007160C-80071618       .text getTactFreeMStick__14dEvt_control_cFi */
u8 dEvt_control_c::getTactFreeMStick(int which) {
    return mTactFreeMStick[which];
}

/* 80071618-80071624       .text getTactFreeCStick__14dEvt_control_cFi */
u8 dEvt_control_c::getTactFreeCStick(int which) {
    return mTactFreeCStick[which];
}

/* 80071624-80071658       .text __ct__11dEvt_info_cFv */
dEvt_info_c::dEvt_info_c() {
    mCommand = dEvtCmd_NONE_e;
    mCondition = dEvtCnd_NONE_e;
    mEventId = -1;
    mMapToolId = -1;
    mpEventCB = NULL;
    mpCheckCB = NULL;
}

/* 80071658-800716AC       .text setEventName__11dEvt_info_cFPc */
void dEvt_info_c::setEventName(char* evtName) {
    if (evtName == NULL) {
        mEventId = -1;
    } else {
        mEventId = dComIfGp_evmng_getEventIdx(evtName, -1);
    }
}

/* 800716AC-800716F8       .text getEventName__11dEvt_info_cFv */
char* dEvt_info_c::getEventName() {
    if (mEventId == -1) {
        return NULL;
    } else {
        dEvDtEvent_c* evtData = dComIfGp_getPEvtManager()->getEventData(mEventId);
        if (evtData == NULL)
            return NULL;
        return evtData->getName();
    }
}

/* 800716F8-80071778       .text giveItemCut__14dEvt_control_cFUc */
bool dEvt_control_c::giveItemCut(u8 item) {
    s32 staffIdx = dComIfGp_evmng_getMyStaffId("GIVEMAN");
    if (staffIdx == -1)
        return false;

    dComIfGp_evmng_cutEnd(staffIdx);
    mGetItemNo = item;
    return true;
}
