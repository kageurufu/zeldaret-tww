//
// Generated by dtk
// Translation Unit: d_a_spotbox.cpp
//

#include "d/actor/d_a_spotbox.h"
#include "f_op/f_op_actor_mng.h"
#include "d/d_com_inf_game.h"
#include "m_Do/m_Do_mtx.h"
#include "dolphin/types.h"
#include "d/d_procname.h"

s32 daSpotbox_c::create() {
    fopAcM_SetupActor(this, daSpotbox_c);
    f32 baseScale = getType() != 0 ? 1000.0f : 100.0f;
    mScale.x *= baseScale;
    mScale.y *= baseScale;
    mScale.z *= (baseScale * 1.2f);
    current.pos.y += mScale.y * 0.5f;
    fopAcM_SetMtx(this, mMtx);
    fopAcM_setCullSizeBox(this, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f);

    return cPhs_COMPLEATE_e;
}

BOOL daSpotbox_c::draw() {
    if (dComIfGd_getSpotModelNum() != 0) {
        dComIfGd_setSpotModel(dDlst_alphaModel_c::TYPE_CUBE, mMtx, 0x20);
    }
    return TRUE;
}

BOOL daSpotbox_c::execute() {
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::YrotM(current.angle.y);
    mDoMtx_stack_c::scaleM(mScale);
    cMtx_copy(mDoMtx_stack_c::get(), mMtx);
    return TRUE;
}

u32 daSpotbox_c::getType() {
    return fopAcM_GetParam(this) & 1;
}

/* 00000078-000000C4       .text daSpotbox_Draw__FP11daSpotbox_c */
static BOOL daSpotbox_Draw(daSpotbox_c* i_this) {
    return ((daSpotbox_c*)i_this)->draw();
}

/* 000000C4-00000138       .text daSpotbox_Execute__FP11daSpotbox_c */
static BOOL daSpotbox_Execute(daSpotbox_c* i_this) {
    return ((daSpotbox_c*)i_this)->execute();
}

/* 00000138-00000140       .text daSpotbox_IsDelete__FP11daSpotbox_c */
static BOOL daSpotbox_IsDelete(daSpotbox_c* i_this) {
    return TRUE;
}

/* 00000140-00000170       .text daSpotbox_Delete__FP11daSpotbox_c */
static BOOL daSpotbox_Delete(daSpotbox_c* i_this) {
    i_this->~daSpotbox_c();
    return true;
}

/* 00000170-00000250       .text daSpotbox_Create__FP10fopAc_ac_c */
static s32 daSpotbox_Create(fopAc_ac_c* i_this) {
    daSpotbox_c* spotbox = (daSpotbox_c*)i_this;
    return spotbox->create();
}

static actor_method_class l_daSpotbox_Method = {
    (process_method_func)daSpotbox_Create,
    (process_method_func)daSpotbox_Delete,
    (process_method_func)daSpotbox_Execute,
    (process_method_func)daSpotbox_IsDelete,
    (process_method_func)daSpotbox_Draw,
};

actor_process_profile_definition g_profile_SPOTBOX = {
    /* LayerID      */ fpcLy_CURRENT_e,
    /* ListID       */ 7,
    /* ListPrio     */ fpcPi_CURRENT_e,
    /* ProcName     */ PROC_SPOTBOX,
    /* Proc SubMtd  */ &g_fpcLf_Method.mBase,
    /* Size         */ sizeof(daSpotbox_c),
    /* SizeOther    */ 0,
    /* Parameters   */ 0,
    /* Leaf SubMtd  */ &g_fopAc_Method.base,
    /* Priority     */ 0x01C9,
    /* Actor SubMtd */ &l_daSpotbox_Method,
    /* Status       */ fopAcStts_CULL_e | fopAcStts_NOCULLEXEC_e | fopAcStts_UNK4000_e | fopAcStts_UNK40000_e,
    /* Group        */ fopAc_ACTOR_e,
    /* CullType     */ fopAc_CULLBOX_CUSTOM_e,
};
