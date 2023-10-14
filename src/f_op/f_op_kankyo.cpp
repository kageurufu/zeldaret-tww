//
// Generated by dtk
// Translation Unit: f_op_kankyo.cpp
//

#include "f_op/f_op_kankyo.h"
#include "f_op/f_op_draw_tag.h"
#include "f_op/f_op_kankyo_mng.h"
#include "f_pc/f_pc_manager.h"
#include "d/d_procname.h"
#include "d/d_s_play.h"

extern bool dMenu_flag(void);

/* 8002A454-8002A4A4       .text fopKy_Draw__FPv */
static int fopKy_Draw(void* i_ky) {
    int ret;
    kankyo_class* i_this = (kankyo_class*)i_ky;

    if (!dMenu_flag()) {
        ret = fpcLf_DrawMethod((leafdraw_method_class*)i_this->mSubMtd, i_this);
    }

    return ret;
}

/* 8002A4A4-8002A514       .text fopKy_Execute__FPv */
static int fopKy_Execute(void* i_ky) {
    int ret;
    kankyo_class* i_this = (kankyo_class*)i_ky;

    if (!dScnPly_ply_c::isPause() && (!dMenu_flag() || fpcM_GetName(i_ky) == PROC_ENVSE || fpcM_GetName(i_ky) == PROC_LEVEL_SE)) {
        ret = fpcMtd_Execute((process_method_class*)i_this->mSubMtd, i_ky);
    }

    return ret;
}

/* 8002A514-8002A568       .text fopKy_IsDelete__FPv */
static int fopKy_IsDelete(void* i_ky) {
    int ret;
    kankyo_class* i_this = (kankyo_class*)i_ky;

    ret = fpcMtd_IsDelete((process_method_class*)i_this->mSubMtd, i_this);
    if (ret == 1) {
        fopDwTg_DrawQTo(&i_this->mDwTg);
    }

    return ret;
}

/* 8002A568-8002A5B4       .text fopKy_Delete__FPv */
static int fopKy_Delete(void* i_ky) {
    kankyo_class* i_this = (kankyo_class*)i_ky;

    int ret = fpcMtd_Delete((process_method_class*)i_this->mSubMtd, i_this);
    fopDwTg_DrawQTo(&i_this->mDwTg);

    return ret;
}

static int fopKy_KANKYO_TYPE;

/* 8002A5B4-8002A688       .text fopKy_Create__FPv */
static int fopKy_Create(void* i_ky) {
    kankyo_class* i_this = (kankyo_class*)i_ky;

    if (fpcM_IsFirstCreating(i_ky)) {
        kankyo_process_profile_definition* profile = (kankyo_process_profile_definition*)fpcM_GetProfile(i_ky);

        i_this->mBsType = fpcBs_MakeOfType(&fopKy_KANKYO_TYPE);
        i_this->mSubMtd = profile->mSubMtd;

        fopDwTg_Init(&i_this->mDwTg, i_this);
        fopKyM_prm_class* append = (fopKyM_prm_class*)fopKyM_GetAppend(i_this);

        if (append != NULL) {
            i_this->mPos = append->mPos;
            i_this->mScale = append->mScale;
            i_this->mParam = append->mParam;
        }
    }

    int ret = fpcMtd_Create((process_method_class*)i_this->mSubMtd, i_this);
    if (ret == cPhs_COMPLEATE_e) {
        s32 priority = fpcLf_GetPriority(i_this);
        fopDwTg_ToDrawQ(&i_this->mDwTg, priority);
    }

    return ret;
}

leafdraw_method_class g_fopKy_Method = {
    (process_method_func)fopKy_Create,
    (process_method_func)fopKy_Delete,
    (process_method_func)fopKy_Execute,
    (process_method_func)fopKy_IsDelete,
    (process_method_func)fopKy_Draw,
};
