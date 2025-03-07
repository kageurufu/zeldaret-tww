//
// Generated by dtk
// Translation Unit: d_ovlp_fade2.cpp
//

#include "d/d_ovlp_fade2.h"
#include "d/d_com_inf_game.h"
#include "d/d_procname.h"
#include "f_op/f_op_overlap.h"
#include "new.h"

/* 802237F4-80223D20       .text draw__15dOvlpFd2_dlst_cFv */
void dOvlpFd2_dlst_c::draw() {
    /* Nonmatching */
}

/* 80223D20-80223D84       .text __ct__10dOvlpFd2_cFv */
dOvlpFd2_c::dOvlpFd2_c() {
    field_0xcc = &dOvlpFd2_c::execFirstSnap;
    dComIfGp_2dShowOff();
    field_0x11d = 2;
}

/* 80223D84-80223E18       .text execFirstSnap__10dOvlpFd2_cFv */
void dOvlpFd2_c::execFirstSnap() {
    /* Nonmatching */
}

/* 80223E18-80223F8C       .text execFadeOut__10dOvlpFd2_cFv */
void dOvlpFd2_c::execFadeOut() {
    /* Nonmatching */
}

/* 80223F8C-80224034       .text execNextSnap__10dOvlpFd2_cFv */
void dOvlpFd2_c::execNextSnap() {
    /* Nonmatching */
}

/* 80224034-802240F4       .text execFadeIn__10dOvlpFd2_cFv */
void dOvlpFd2_c::execFadeIn() {
    /* Nonmatching */
}

/* 802240F4-80224200       .text dOvlpFd2_Draw__FP10dOvlpFd2_c */
void dOvlpFd2_Draw(dOvlpFd2_c*) {
    /* Nonmatching */
}

/* 80224200-8022422C       .text dOvlpFd2_Execute__FP10dOvlpFd2_c */
s32 dOvlpFd2_Execute(dOvlpFd2_c* i_this) {
    (i_this->*i_this->field_0xcc)();
    return 1;
}

/* 8022422C-80224234       .text dOvlpFd2_IsDelete__FP10dOvlpFd2_c */
s32 dOvlpFd2_IsDelete(dOvlpFd2_c*) {
    return 1;
}

/* 80224234-8022423C       .text dOvlpFd2_Delete__FP10dOvlpFd2_c */
s32 dOvlpFd2_Delete(dOvlpFd2_c*) {
    return 1;
}

/* 8022423C-80224268       .text dOvlpFd2_Create__FPv */
s32 dOvlpFd2_Create(void* i_this) {
    new (i_this) dOvlpFd2_c();
    return 4;
}

overlap_method_class l_dOvlpFd2_Method = {
    (process_method_func)dOvlpFd2_Create,
    (process_method_func)dOvlpFd2_Delete,
    (process_method_func)dOvlpFd2_Execute,
    (process_method_func)dOvlpFd2_IsDelete,
    (process_method_func)dOvlpFd2_Draw,
};

overlap_process_profile_definition g_profile_OVERLAP2 = {
    fpcLy_ROOT_e,
    2,
    fpcPi_CURRENT_e,
    PROC_OVERLAP2,
    &g_fpcLf_Method.mBase,
    sizeof(dOvlpFd2_c),
    0,
    0,
    &g_fopOvlp_Method,
    0x1E3,
    &l_dOvlpFd2_Method,
};

overlap_process_profile_definition g_profile_OVERLAP3 = {
    fpcLy_ROOT_e,
    2,
    fpcPi_CURRENT_e,
    PROC_OVERLAP3,
    &g_fpcLf_Method.mBase,
    sizeof(dOvlpFd2_c),
    0,
    0,
    &g_fopOvlp_Method,
    0x1E4,
    &l_dOvlpFd2_Method,
};
