
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   parsing_tokens.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////        DirectX binding tokenisation.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "parsing_tokens.h"
#include "platforms/dx11/rendering/system/public/state_descs.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin pipeline namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace pipeline
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin tokens namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tokens
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Token string mapping structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct TOKEN_MAP
{
    const char* const*              tokens;
    int                             value;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ECmpFunc token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ECmpFuncNever_Tokens[] = { "0", "false", "never", NULL };
const char* ECmpFuncAlways_Tokens[] = { "1", "true", "always", NULL };
const char* ECmpFuncEqual_Tokens[] = { "=", "==", "equal", "equals", "equalto", "equal_to", "equal to", NULL };
const char* ECmpFuncNotEqual_Tokens[] = { "!=", "!equal", "notequal", "notequalto", "not_equal", "not equal", "not_equal_to", "not equal_to", NULL };
const char* ECmpFuncLess_Tokens[] = { "<", "less", "lessthan", "less_than", "less than", NULL };
const char* ECmpFuncLessEqual_Tokens[] = { "<=", "lessequal", "lessorequal", "lessthanorequal", "lessorequalto", "lessthanorequalto" "less_equal", "less_or_equal", "less_than_or_equal", "less_or_equal_to", "less_than_or_equal_to", "less equal", "less or equal", "less than or equal", "less or equal to", "less than or equal to", NULL };
const char* ECmpFuncGreater_Tokens[] = { ">", "greater", "greaterthan", "greater_than", "greater than", NULL };
const char* ECmpFuncGreaterEqual_Tokens[] = { ">=", "greaterequal", "greaterorequal", "greaterthanorequal", "greaterorequalto", "greaterthanorequalto" "greater_equal", "greater_or_equal", "greater_than_or_equal", "greater_or_equal_to", "greater_than_or_equal_to", "greater equal", "greater or equal", "greater than or equal", "greater or equal to", "greater than or equal to", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ECmpFunc token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP ECmpFunc_Tokens[] =
{
    { ECmpFuncNever_Tokens,         rendering::ECmpFuncNever        },
    { ECmpFuncAlways_Tokens,        rendering::ECmpFuncAlways       },
    { ECmpFuncEqual_Tokens,         rendering::ECmpFuncEqual        },
    { ECmpFuncNotEqual_Tokens,      rendering::ECmpFuncNotEqual     },
    { ECmpFuncLess_Tokens,          rendering::ECmpFuncLess         },
    { ECmpFuncLessEqual_Tokens,     rendering::ECmpFuncLessEqual    },
    { ECmpFuncGreater_Tokens,       rendering::ECmpFuncGreater      },
    { ECmpFuncGreaterEqual_Tokens,  rendering::ECmpFuncGreaterEqual },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ESamplingMode token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ESamplingFilter_Tokens[] = { "filter", NULL };
const char* ESamplingMinimum_Tokens[] = { "min", "minimum", NULL };
const char* ESamplingMaximum_Tokens[] = { "max", "maximum", NULL };
const char* ESamplingCompare_Tokens[] = { "cmp", "compare", "comparison", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ESamplingMode token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP ESamplingMode_Tokens[] =
{
    { ESamplingFilter_Tokens,   rendering::ESamplingFilter  },
    { ESamplingMinimum_Tokens,  rendering::ESamplingMinimum },
    { ESamplingMaximum_Tokens,  rendering::ESamplingMaximum },
    { ESamplingCompare_Tokens,  rendering::ESamplingCompare },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    EWrapMode token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EWrapModeClamp_Tokens[] = { "clamp", NULL };
const char* EWrapModeRepeat_Tokens[] = { "repeat", "wrap", NULL };
const char* EWrapModeMirror_Tokens[] = { "mirror", NULL };
const char* EWrapModeMirrorOnce_Tokens[] = { "mirroronce", "mirror_once", "mirror once", NULL };
const char* EWrapModeBorder_Tokens[] = { "border", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    EWrapMode token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EWrapMode_Tokens[] =
{
    { EWrapModeClamp_Tokens,        rendering::EWrapModeClamp       },
    { EWrapModeRepeat_Tokens,       rendering::EWrapModeRepeat      },
    { EWrapModeMirror_Tokens,       rendering::EWrapModeMirror      },
    { EWrapModeMirrorOnce_Tokens,   rendering::EWrapModeMirrorOnce  },
    { EWrapModeBorder_Tokens,       rendering::EWrapModeBorder      },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    EStencilOp token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EStencilOpKeep_Tokens[] = { "keep", NULL };
const char* EStencilOpZero_Tokens[] = { "0", "zero", NULL };
const char* EStencilOpInv_Tokens[] = { "inv", "invert", NULL };
const char* EStencilOpInc_Tokens[] = { "inc", "incr", "increment", NULL };
const char* EStencilOpDec_Tokens[] = { "dec", "decr", "decrement", NULL };
const char* EStencilOpIncSat_Tokens[] = { "incsat", "incrsat", "incrementandsaturate", "inc_sat", "incr_sat", "increment_and_saturate", "inc sat", "incr sat", "increment and saturate", NULL };
const char* EStencilOpDecSat_Tokens[] = { "decsat", "decrsat", "decrementandsaturate", "dec_sat", "decr_sat", "decrement_and_saturate", "dec sat", "decr sat", "decrement and saturate", NULL };
const char* EStencilOpReplace_Tokens[] = { "=", "replace", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    EStencilOp token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EStencilOp_Tokens[] =
{
    { EStencilOpKeep_Tokens,    rendering::EStencilOpKeep       },
    { EStencilOpZero_Tokens,    rendering::EStencilOpZero       },
    { EStencilOpInv_Tokens,     rendering::EStencilOpInv        },
    { EStencilOpInc_Tokens,     rendering::EStencilOpInc        },
    { EStencilOpDec_Tokens,     rendering::EStencilOpDec        },
    { EStencilOpIncSat_Tokens,  rendering::EStencilOpIncSat     },
    { EStencilOpDecSat_Tokens,  rendering::EStencilOpDecSat     },
    { EStencilOpReplace_Tokens, rendering::EStencilOpReplace    },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ECullMode token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ECullModeNone_Tokens[] = { "none", "off", "disable", "disabled", NULL };
const char* ECullModeBack_Tokens[] = { "back", "backface", "back_face", "back-face", "back face", NULL };
const char* ECullModeFront_Tokens[] = { "face", "front", "frontface", "front_face", "front-face", "front face", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ECullMode token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP ECullMode_Tokens[] =
{
    { ECullModeNone_Tokens,     rendering::ECullModeNone    },
    { ECullModeBack_Tokens,     rendering::ECullModeBack    },
    { ECullModeFront_Tokens,    rendering::ECullModeFront   },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ESamplesUAV token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ESamplesUnforced_Tokens[] = { "0", "unforced", NULL };
const char* ESamplesForce1_Tokens[] = { "1", "set1", "force1", "set_1", "force_1", "set 1", "force 1", NULL };
const char* ESamplesForce2_Tokens[] = { "2", "set2", "force2", "set_2", "force_2", "set 2", "force 2", NULL };
const char* ESamplesForce4_Tokens[] = { "4", "set4", "force4", "set_4", "force_4", "set 4", "force 4", NULL };
const char* ESamplesForce8_Tokens[] = { "8", "set8", "force8", "set_8", "force_8", "set 8", "force 8", NULL };
const char* ESamplesForce16_Tokens[] = { "16", "set16", "force16", "set_16", "force_16", "set 16", "force 16", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ESamplesUAV token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP ESamplesUAV_Tokens[] =
{
    { ESamplesUnforced_Tokens,  rendering::ESamplesUnforced },
    { ESamplesForce1_Tokens,    rendering::ESamplesForce1   },
    { ESamplesForce2_Tokens,    rendering::ESamplesForce2   },
    { ESamplesForce4_Tokens,    rendering::ESamplesForce4   },
    { ESamplesForce8_Tokens,    rendering::ESamplesForce8   },
    { ESamplesForce16_Tokens,   rendering::ESamplesForce16  },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    EBlendMul token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EBlendMulZero_Tokens[] = { "0", "zero", NULL };
const char* EBlendMulOne_Tokens[] = { "1", "one", NULL };
const char* EBlendMulFactor_Tokens[] = { "factor", NULL };
const char* EBlendMulInvFactor_Tokens[] = { "invfactor", "inv_factor", "inv factor", "1-factor", NULL };
const char* EBlendMulSrcA_Tokens[] = { "a", "srca", "src_a", "src a", "alpha", "srcalpha", "src_alpha", "src alpha", NULL };
const char* EBlendMulInvSrcA_Tokens[] = { "inva", "invsrca", "inv_a", "inv_src_a", "inv a", "inv src a", "1-a", "1-srca", "1-src_a", "1-src a", "invalpha", "invsrcalpha", "inv_alpha", "inv_src_alpha", "inv alpha", "inv src alpha", "1-alpha", "1-srcalpha", "1-src_alpha", "1-src alpha", NULL };
const char* EBlendMulTrgA_Tokens[] = { "trga", "dsta", "desta", "trg_a", "dst_a", "dest_a", "trg a", "dst a", "dest a", "trgalpha", "dstalpha", "destalpha", "trg_alpha", "dst_alpha", "dest_alpha", "trg alpha", "dst alpha", "dest alpha", NULL };
const char* EBlendMulInvTrgA_Tokens[] = { "invtrga", "invdsta", "invdesta", "inv_trg_a", "inv_dst_a", "inv_dest_a", "inv trg a", "inv dst a", "inv dest a", "1-trga", "1-dsta", "1-desta", "1-trg_a", "1-dst_a", "1-dest_a", "1-trg a", "1-dst a", "1-dest a", "invtrgalpha", "invdstalpha", "invdestalpha", "inv_trg_alpha", "inv_dst_alpha", "inv_dest_alpha", "inv trg alpha", "inv dst alpha", "inv dest alpha", "1-trgalpha", "1-dstalpha", "1-destalpha", "1-trg_alpha", "1-dst_alpha", "1-dest_alpha", "1-trg alpha", "1-dst alpha", "1-dest alpha", NULL };
const char* EBlendMulAltA_Tokens[] = { "alta", "src1a", "alt_a", "src1_a", "alt a", "src1 a", "altalpha", "src1alpha", "alt_alpha", "src1_alpha", "alt alpha", "src1 alpha", NULL };
const char* EBlendMulInvAltA_Tokens[] = { "invalta", "invsrc1a", "inv_alt_a", "inv_src1_a", "inv alt a", "inv src1 a", "1-alta", "1-src1a", "1-alt_a", "1-src1_a", "1-alt a", "1-src1 a", "invaltalpha", "invsrc1alpha", "inv_alt_alpha", "inv_src1_alpha", "inv alt alpha", "inv src1 alpha", "1-altalpha", "1-src1alpha", "1-alt_alpha", "1-src1_alpha", "1-alt alpha", "1-src1 alpha", NULL };
const char* EBlendMulSrcSatA_Tokens[] = { "asat", "srcasat", "a_sat", "src_a_sat", "a sat", "src a sat", "alphasat", "srcalphasat", "alpha_sat", "src_alpha_sat", "alpha sat", "src alpha sat", NULL };
const char* EBlendMulSrcRGB_Tokens[] = { "rgb", "srcrgb", "src_rgb", "src rgb", "color", "srccolor", "src_color", "src color", "colour", "srccolour", "src_colour", "src colour", NULL };
const char* EBlendMulInvSrcRGB_Tokens[] = { "invrgb", "invsrcrgb", "inv_rgb", "inv_src_rgb", "inv rgb", "inv src rgb", "1-rgb", "1-srcrgb", "1-src_rgb", "1-src rgb", "invcolor", "invsrccolor", "inv_color", "inv_src_color", "inv color", "inv src color", "1-color", "1-srccolor", "1-src_color", "1-src color", "invcolour", "invsrccolour", "inv_colour", "inv_src_colour", "inv colour", "inv src colour", "1-colour", "1-srccolour", "1-src_colour", "1-src colour", NULL };
const char* EBlendMulTrgRGB_Tokens[] = { "trgrgb", "dstrgb", "destrgb", "trgrgb", "dstrgb", "destrgb", "trg_rgb", "dst_rgb", "dest_rgb", "trg_rgb", "dst_rgb", "dest_rgb", "trg rgb", "dst rgb", "dest rgb", "trg rgb", "dst rgb", "dest rgb", "trgcolor", "dstcolor", "destcolor", "trgcolor", "dstcolor", "destcolor", "trg_color", "dst_color", "dest_color", "trg_color", "dst_color", "dest_color", "trg color", "dst color", "dest color", "trg color", "dst color", "dest color", "trgcolour", "dstcolour", "destcolour", "trgcolour", "dstcolour", "destcolour", "trg_colour", "dst_colour", "dest_colour", "trg_colour", "dst_colour", "dest_colour", "trg colour", "dst colour", "dest colour", "trg colour", "dst colour", "dest colour", NULL };
const char* EBlendMulInvTrgRGB_Tokens[] = { "invtrgrgb", "invdstrgb", "invdestrgb", "inv_trg_rgb", "inv_dst_rgb", "inv_dest_rgb", "inv trg rgb", "inv dst rgb", "inv dest rgb", "1-trgrgb", "1-dstrgb", "1-destrgb", "1-trg_rgb", "1-dst_rgb", "1-dest_rgb", "1-trg rgb", "1-dst rgb", "1-dest rgb", "invtrgcolor", "invdstcolor", "invdestcolor", "inv_trg_color", "inv_dst_color", "inv_dest_color", "inv trg color", "inv dst color", "inv dest color", "1-trgcolor", "1-dstcolor", "1-destcolor", "1-trg_color", "1-dst_color", "1-dest_color", "1-trg color", "1-dst color", "1-dest color", "invtrgcolour", "invdstcolour", "invdestcolour", "inv_trg_colour", "inv_dst_colour", "inv_dest_colour", "inv trg colour", "inv dst colour", "inv dest colour", "1-trgcolour", "1-dstcolour", "1-destcolour", "1-trg_colour", "1-dst_colour", "1-dest_colour", "1-trg colour", "1-dst colour", "1-dest colour", NULL };
const char* EBlendMulAltRGB_Tokens[] = { "altrgb", "src1rgb", "altrgb", "src1rgb", "alt_rgb", "src1_rgb", "alt_rgb", "src1_rgb", "alt rgb", "src1 rgb", "alt rgb", "src1 rgb", "altcolor", "src1color", "altcolor", "src1color", "alt_color", "src1_color", "alt_color", "src1_color", "alt color", "src1 color", "alt color", "src1 color", "altcolour", "src1colour", "altcolour", "src1colour", "alt_colour", "src1_colour", "alt_colour", "src1_colour", "alt colour", "src1 colour", "alt colour", "src1 colour", NULL };
const char* EBlendMulInvAltRGB_Tokens[] = { "invaltrgb", "invsrc1rgb", "inv_alt_rgb", "inv_src1_rgb", "inv alt rgb", "inv src1 rgb", "1-altrgb", "1-src1rgb", "1-alt_rgb", "1-src1_rgb", "1-alt rgb", "1-src1 rgb", "invaltcolor", "invsrc1color", "inv_alt_color", "inv_src1_color", "inv alt color", "inv src1 color", "1-altcolor", "1-src1color", "1-alt_color", "1-src1_color", "1-alt color", "1-src1 color", "invaltcolour", "invsrc1colour", "inv_alt_colour", "inv_src1_colour", "inv alt colour", "inv src1 colour", "1-altcolour", "1-src1colour","1-alt_colour", "1-src1_colour", "1-alt colour", "1-src1 colour", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    EBlendMul token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EBlendMul_Tokens[] =
{
    { EBlendMulZero_Tokens,         rendering::EBlendMulZero        },
    { EBlendMulOne_Tokens,          rendering::EBlendMulOne         },
    { EBlendMulFactor_Tokens,       rendering::EBlendMulFactor      },
    { EBlendMulInvFactor_Tokens,    rendering::EBlendMulInvFactor   },
    { EBlendMulSrcA_Tokens,         rendering::EBlendMulSrcA        },
    { EBlendMulInvSrcA_Tokens,      rendering::EBlendMulInvSrcA     },
    { EBlendMulTrgA_Tokens,         rendering::EBlendMulTrgA        },
    { EBlendMulInvTrgA_Tokens,      rendering::EBlendMulInvTrgA     },
    { EBlendMulAltA_Tokens,         rendering::EBlendMulAltA        },
    { EBlendMulInvAltA_Tokens,      rendering::EBlendMulInvAltA     },
    { EBlendMulSrcSatA_Tokens,      rendering::EBlendMulSrcSatA     },
    { EBlendMulSrcRGB_Tokens,       rendering::EBlendMulSrcRGB      },
    { EBlendMulInvSrcRGB_Tokens,    rendering::EBlendMulInvSrcRGB   },
    { EBlendMulTrgRGB_Tokens,       rendering::EBlendMulTrgRGB      },
    { EBlendMulInvTrgRGB_Tokens,    rendering::EBlendMulInvTrgRGB   },
    { EBlendMulAltRGB_Tokens,       rendering::EBlendMulAltRGB      },
    { EBlendMulInvAltRGB_Tokens,    rendering::EBlendMulInvAltRGB   },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ECombiner token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ECombinerAdd_Tokens[] = { "add", NULL };
const char* ECombinerSub_Tokens[] = { "sub", "subtract", NULL };
const char* ECombinerRevSub_Tokens[] = { "revsub", "subneg", "reversesubtract", "subtractnegate", "subtractandnegate", "rev_sub", "sub_neg", "reverse_subtract", "subtract_negate", "subtract_and_negate", "rev sub", "sub neg", "reverse subtract", "subtract negate", "subtract and negate", NULL };
const char* ECombinerMin_Tokens[] = { "min", "minimum", NULL };
const char* ECombinerMax_Tokens[] = { "max", "maximum", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ECombiner token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP ECombiner_Tokens[] =
{
    { ECombinerAdd_Tokens,      rendering::ECombinerAdd     },
    { ECombinerSub_Tokens,      rendering::ECombinerSub     },
    { ECombinerRevSub_Tokens,   rendering::ECombinerRevSub  },
    { ECombinerMin_Tokens,      rendering::ECombinerMin     },
    { ECombinerMax_Tokens,      rendering::ECombinerMax     },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ELogicOp token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ELogicOpNop_Tokens[] = { "nop", "noop", NULL };
const char* ELogicOpZero_Tokens[] = { "0", "zero", "clear", NULL };
const char* ELogicOpSet_Tokens[] = { "=", "set", NULL };
const char* ELogicOpCopy_Tokens[] = { "copy", NULL };
const char* ELogicOpCopyInv_Tokens[] = { "invcopy", "inv_copy", NULL };
const char* ELogicOpInv_Tokens[] = { "inv", "invert", NULL };
const char* ELogicOpAnd_Tokens[] = { "&", "and", NULL };
const char* ELogicOpNand_Tokens[] = { "nand", NULL };
const char* ELogicOpOr_Tokens[] = { "|", "or", NULL };
const char* ELogicOpNor_Tokens[] = { "nor", NULL };
const char* ELogicOpXor_Tokens[] = { "^", "xor", "eor", NULL };
const char* ELogicOpEquiv_Tokens[] = { "==", "equiv", NULL };
const char* ELogicOpAndRev_Tokens[] = { "andrev", "andreverse", "and_rev", "and_reverse", "and rev", "and reverse", NULL };
const char* ELogicOpAndInv_Tokens[] = { "andinv", "andinverted", "and_inv", "and_inverted", "and inv", "and inverted", NULL };
const char* ELogicOpOrRev_Tokens[] = { "orrev", "orreverse", "or_rev", "or_reverse", "or rev", "or reverse", NULL };
const char* ELogicOpOrInv_Tokens[] = { "orinv", "orinverted", "or_inv", "or_inverted", "or inv", "or inverted", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ELogicOp token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP ELogicOp_Tokens[] =
{
    { ELogicOpNop_Tokens,       rendering::ELogicOpNop      },
    { ELogicOpZero_Tokens,      rendering::ELogicOpZero     },
    { ELogicOpSet_Tokens,       rendering::ELogicOpSet      },
    { ELogicOpCopy_Tokens,      rendering::ELogicOpCopy     },
    { ELogicOpCopyInv_Tokens,   rendering::ELogicOpCopyInv  },
    { ELogicOpInv_Tokens,       rendering::ELogicOpInv      },
    { ELogicOpAnd_Tokens,       rendering::ELogicOpAnd      },
    { ELogicOpNand_Tokens,      rendering::ELogicOpNand     },
    { ELogicOpOr_Tokens,        rendering::ELogicOpOr       },
    { ELogicOpNor_Tokens,       rendering::ELogicOpNor      },
    { ELogicOpXor_Tokens,       rendering::ELogicOpXor      },
    { ELogicOpEquiv_Tokens,     rendering::ELogicOpEquiv    },
    { ELogicOpAndRev_Tokens,    rendering::ELogicOpAndRev   },
    { ELogicOpAndInv_Tokens,    rendering::ELogicOpAndInv   },
    { ELogicOpOrRev_Tokens,     rendering::ELogicOpOrRev    },
    { ELogicOpOrInv_Tokens,     rendering::ELogicOpOrInv    },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Sampler configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ESamplerConfigName_Tokens[] = { "name", "sampler", "samplername", "sampler_name", "sampler name", NULL };
const char* ESamplerConfigSamplingMode_Tokens[] = { "mode", "sampling", "samplingmode", "sampling_mode", "sampling mode", NULL };
const char* ESamplerConfigCmpFunc_Tokens[] = { "cmp", "test", "func", "function", "comparison", "cmpfunc", "cmp_func", "depth_func", "cmp func", NULL };
const char* ESamplerConfigLinearMin_Tokens[] = { "linearmin", "linear_min", "linear min", "minlinear", "min_linear", "min linear", NULL };
const char* ESamplerConfigLinearMag_Tokens[] = { "linearmag", "linear_mag", "linear mag", "maglinear", "mag_linear", "mag linear", NULL };
const char* ESamplerConfigLinearMip_Tokens[] = { "linearmip", "linear_mip", "linear mip", "miplinear", "mip_linear", "mip linear", NULL };
const char* ESamplerConfigWrapU_Tokens[] = { "wrapu", "wrap_u", "wrap u", "uwrap", "u_wrap", "u wrap", "wrapx", "wrap_x", "wrap x", "xwrap", "x_wrap", "x wrap", NULL };
const char* ESamplerConfigWrapV_Tokens[] = { "wrapv", "wrap_v", "wrap v", "vwrap", "v_wrap", "v wrap", "wrapy", "wrap_y", "wrap y", "ywrap", "y_wrap", "y wrap", NULL };
const char* ESamplerConfigWrapW_Tokens[] = { "wrapw", "wrap_w", "wrap w", "wwrap", "w_wrap", "w wrap", "wrapz", "wrap_z", "wrap z", "zwrap", "z_wrap", "z wrap", NULL };
const char* ESamplerConfigAnisotropicMax_Tokens[] = { "anisotropic", "anisotropicmax", "anisotropic_max", "anisotropic max", "maxanisotropic", "anisotropic_max", "max_anisotropic", "max anisotropic", "anisotropy", "anisotropymax", "anisotropy_max", "anisotropy max", "maxanisotropy", "anisotropy_max", "max_anisotropy", "max anisotropy", "aniso", "anisomax", "aniso_max", "aniso max", "maxaniso", "aniso_max", "max_aniso", "max aniso", NULL };
const char* ESamplerConfigBorder_Tokens[] = { "border", NULL };
const char* ESamplerConfigMinLOD_Tokens[] = { "minlod", "min_lod", "min lod", "lodmin", "lod_min", "lod min", "minimumlod", "minimum_lod", "minimum lod", "lodminimum", "lod_minimum", "lod minimum", NULL };
const char* ESamplerConfigMaxLOD_Tokens[] = { "maxlod", "max_lod", "max lod", "lodmax", "lod_max", "lod max", "maximumlod", "maximum_lod", "maximum lod", "lodmaximum", "lod_maximum", "lod maximum", NULL };
const char* ESamplerConfigBiasLOD_Tokens[] = { "biaslod", "bias_lod", "bias lod", "lodbias", "lod_bias", "lod bias", "biaslod", "bias_lod", "bias lod", "lodbias", "lod_bias", "lod bias", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Sampler configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP ESamplerConfig_Tokens[] =
{
    { ESamplerConfigSamplingMode_Tokens,    ESamplerConfigSamplingMode      },
    { ESamplerConfigCmpFunc_Tokens,         ESamplerConfigCmpFunc           },
    { ESamplerConfigLinearMin_Tokens,       ESamplerConfigLinearMin         },
    { ESamplerConfigLinearMag_Tokens,       ESamplerConfigLinearMag         },
    { ESamplerConfigLinearMip_Tokens,       ESamplerConfigLinearMip         },
    { ESamplerConfigWrapU_Tokens,           ESamplerConfigWrapU             },
    { ESamplerConfigWrapV_Tokens,           ESamplerConfigWrapV             },
    { ESamplerConfigWrapW_Tokens,           ESamplerConfigWrapW             },
    { ESamplerConfigAnisotropicMax_Tokens,  ESamplerConfigAnisotropicMax    },
    { ESamplerConfigBorder_Tokens,          ESamplerConfigBorder            },
    { ESamplerConfigMinLOD_Tokens,          ESamplerConfigMinLOD            },
    { ESamplerConfigMaxLOD_Tokens,          ESamplerConfigMaxLOD            },
    { ESamplerConfigBiasLOD_Tokens,         ESamplerConfigBiasLOD           },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Depth configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EDepthConfigReadEnable_Tokens[] = { "enable", "read", "readenable", "read_enable", "read enable", "enableread", "enable_read", "enable read", "reads", "readsenable", "reads_enable", "reads enable", "enablereads", "enable_reads", "enable reads", NULL };
const char* EDepthConfigWriteEnable_Tokens[] = { "write", "writeenable", "write_enable", "write enable", "enablewrite", "enable_write", "enable write", "writes", "writesenable", "writes_enable", "writes enable", "enablewrites", "enable_writes", "enable writes", NULL };
const char* EDepthConfigCmpFunc_Tokens[] = { "cmp", "test", "func", "function", "comparison", "cmpfunc", "cmp_func", "depth_func", "cmp func", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Depth configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EDepthConfig_Tokens[] =
{
    { EDepthConfigReadEnable_Tokens,    EDepthConfigReadEnable  },
    { EDepthConfigWriteEnable_Tokens,   EDepthConfigWriteEnable },
    { EDepthConfigCmpFunc_Tokens,       EDepthConfigCmpFunc     },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Stencil configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EStencilConfigEnable_Tokens[] = { "enable", NULL };
const char* EStencilConfigReadMask_Tokens[] = { "readmask", "read_mask", "read-mask", "read mask", NULL };
const char* EStencilConfigWriteMask_Tokens[] = { "writemask", "write_mask", "write-mask", "write mask", NULL };
const char* EStencilConfigFrontFace_Tokens[] = { "front", "frontface", "front_face", "front-face", "front face", NULL };
const char* EStencilConfigBackFace_Tokens[] = { "back", "backface", "back_face", "back-face", "back face", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Stencil configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EStencilConfig_Tokens[] =
{
    { EStencilConfigEnable_Tokens,      EStencilConfigEnable       },
    { EStencilConfigReadMask_Tokens,    EStencilConfigReadMask     },
    { EStencilConfigWriteMask_Tokens,   EStencilConfigWriteMask    },
    { EStencilConfigFrontFace_Tokens,   EStencilConfigFrontFace    },
    { EStencilConfigBackFace_Tokens,    EStencilConfigBackFace     },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Stencil face configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EStencilFaceConfigCmpFunc_Tokens[] = { "cmp", "test", "func", "function", "comparison", "cmpfunc", "cmp_func", "depth_func", "cmp func", NULL };
const char* EStencilFaceConfigPassBoth_Tokens[] = { "pass", "passboth", "pass_both", "pass both", NULL };
const char* EStencilFaceConfigPassStencil_Tokens[] = { "passstencil", "pass_stencil", "pass stencil", NULL };
const char* EStencilFaceConfigFailStencil_Tokens[] = { "fail", "failstencil", "fail_stencil", "fail stencil", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Stencil face configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EStencilFaceConfig_Tokens[] =
{
    { EStencilFaceConfigCmpFunc_Tokens,     EStencilFaceConfigCmpFunc       },
    { EStencilFaceConfigPassBoth_Tokens,    EStencilFaceConfigPassBoth      },
    { EStencilFaceConfigPassStencil_Tokens, EStencilFaceConfigPassStencil   },
    { EStencilFaceConfigFailStencil_Tokens, EStencilFaceConfigFailStencil   },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Depth-stencil configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EDepthStencilConfigDepth_Tokens[] = { "depth", NULL };
const char* EDepthStencilConfigStencil_Tokens[] = { "stencil", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Depth-stencil state configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EDepthStencilConfig_Tokens[] =
{
    { EDepthStencilConfigDepth_Tokens,      EDepthStencilConfigDepth    },
    { EDepthStencilConfigStencil_Tokens,    EDepthStencilConfigStencil  },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Raster configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ERasterConfigCulling_Tokens[] = { "cull", "culling", NULL };
const char* ERasterConfigSamplesUAV_Tokens[] = { "samples", "sampling", "samplesuav", "uavsamples", "uavsampling", "samples_uav", "uav_samples", "uav_sampling", "samples uav", "uav samples", "uav sampling", NULL };
const char* ERasterConfigClockwise_Tokens[] = { "cw", "clockwise", "frontcw", "frontclockwise", "front_cw", "front_clockwise", "front cw", "front clockwise", NULL };
const char* ERasterConfigAntiClockwise_Tokens[] = { "ccw", "frontccw", "front_ccw", "front ccw", "anticlockwise", "anti_clockwise", "anti-clockwise", "frontanticlockwise", "front_anticlockwise", "front_anti_clockwise", "front_anti-clockwise", "front anticlockwise", "front anti_clockwise", "front anti-clockwise", NULL };
const char* ERasterConfigEnableScissor_Tokens[] = { "scissor", "enablescissor", "enable_scissor", "enable scissor", "scissorenable", "scissor_enable", "scissor enable", "scissoring", "enablescissoring", "enable_scissoring", "enable scissoring", "scissoringenable", "scissoring_enable", "scissoring enable", NULL };
const char* ERasterConfigEnableWireframe_Tokens[] = { "wireframe", "wire_frame", "wire-frame", "enable_wireframe", "enable_wire_frame", "enable_wire-frame", "enable wireframe", "enable wire_frame", "enable wire-frame", "wireframe_enable", "wire_frame_enable", "wire-frame_enable","wireframe enable", "wire_frame enable", "wire-frame enable", NULL };
const char* ERasterConfigEnableDepthClip_Tokens[] = { "depthclip", "depth_clip", "depth-clip", "enable_depthclip", "enable_depth_clip", "enable_depth-clip", "enable depthclip", "enable depth_clip", "enable depth-clip", "depthclip_enable", "depth_clip_enable", "depth-clip_enable","depthclip enable", "depth_clip enable", "depth-clip enable", NULL };
const char* ERasterConfigEnableMultiSample_Tokens[] = { "multisample", "multi_sample", "multi-sample", "enable_multisample", "enable_multi_sample", "enable_multi-sample", "enable multisample", "enable multi_sample", "enable multi-sample", "multisample_enable", "multi_sample_enable", "multi-sample_enable","multisample enable", "multi_sample enable", "multi-sample enable", "multisampling", "multi_sampling", "multi-sampling", "enable_multisampling", "enable_multi_sampling", "enable_multi-sampling", "enable multisampling", "enable multi_sampling", "enable multi-sampling", "multisampling_enable", "multi_sampling_enable", "multi-sampling_enable","multisampling enable", "multi_sampling enable", "multi-sampling enable", NULL };
const char* ERasterConfigEnableAntiAliasedLine_Tokens[] = { "antialiasline", "antialiaslines", "antialiasedline", "antialiasedlines", "antialias_line", "antialias_lines", "antialiased_line", "antialiased_lines", "anti_alias_line", "anti_alias_lines", "anti_aliased_line", "anti_aliased_lines", "anti-alias_line", "anti-alias_lines", "anti-aliased_line", "anti-aliased_lines", "anti-alias line", "anti-alias lines", "anti-aliased line", "anti-aliased lines", NULL };
const char* ERasterConfigDepthBias_Tokens[] = { "depthbias", "depth_bias", "depth bias", "biasdepth", "bias_depth", "bias depth", NULL };
const char* ERasterConfigSlopeBias_Tokens[] = { "slopebias", "slopescaledbias", "slopedepthbias", "slopescaleddepthbias", "slope_bias", "slope_scaled_bias", "slope_depth_bias", "slope_scaled_depth_bias", "slope bias", "slope scaled bias", "slope depth bias", "slope scaled depth bias", NULL };
const char* ERasterConfigClampBias_Tokens[] = { "clampbias", "biasclamp", "limitbias", "biaslimit", "clamp_bias", "bias_clamp", "limit_bias", "bias_limit", "clamp bias", "bias clamp", "limit bias", "bias limit", "biasclamping", "bias_clamping", "bias clamping", "clampdepthbias", "depthbiasclamp", "limitdepthbias", "depthbiaslimit", "clamp_depth_bias", "depth_bias_clamp", "limit_depth_bias", "depth_bias_limit", "clamp depth bias", "depth biasc lamp", "limit depth bias", "depth bias limit", "depthbiasclamping", "depth_bias_clamping", "depth bias clamping", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Raster configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP ERasterConfig_Tokens[] =
{
    { ERasterConfigCulling_Tokens,                  ERasterConfigCulling                },
    { ERasterConfigSamplesUAV_Tokens,               ERasterConfigSamplesUAV             },
    { ERasterConfigClockwise_Tokens,                ERasterConfigClockwise              },
    { ERasterConfigAntiClockwise_Tokens,            ERasterConfigAntiClockwise          },
    { ERasterConfigEnableScissor_Tokens,            ERasterConfigEnableScissor          },
    { ERasterConfigEnableWireframe_Tokens,          ERasterConfigEnableWireframe        },
    { ERasterConfigEnableDepthClip_Tokens,          ERasterConfigEnableDepthClip        },
    { ERasterConfigEnableMultiSample_Tokens,        ERasterConfigEnableMultiSample      },
    { ERasterConfigEnableAntiAliasedLine_Tokens,    ERasterConfigEnableAntiAliasedLine  },
    { ERasterConfigDepthBias_Tokens,                ERasterConfigDepthBias              },
    { ERasterConfigSlopeBias_Tokens,                ERasterConfigSlopeBias              },
    { ERasterConfigClampBias_Tokens,                ERasterConfigClampBias              },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Blend target configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EBlendTargetConfigRGBA_Tokens[] = { "rgba", NULL };
const char* EBlendTargetConfigColour_Tokens[] = { "rgb", "color", "colour", NULL };
const char* EBlendTargetConfigAlpha_Tokens[] = { "a", "alpha", NULL };
const char* EBlendTargetConfigLogic_Tokens[] = { "logic", NULL };
const char* EBlendTargetConfigMask_Tokens[] = { "mask", "channelmask", "channel_mask", "channel mask", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Blend target configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EBlendTargetConfig_Tokens[] =
{
    { EBlendTargetConfigRGBA_Tokens,    EBlendTargetConfigRGBA      },
    { EBlendTargetConfigColour_Tokens,  EBlendTargetConfigColour    },
    { EBlendTargetConfigAlpha_Tokens,   EBlendTargetConfigAlpha     },
    { EBlendTargetConfigLogic_Tokens,   EBlendTargetConfigLogic     },
    { EBlendTargetConfigMask_Tokens,    EBlendTargetConfigMask      },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Blend effect (colour or alpha) configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EBlendEffectConfigEnable_Tokens[] = { "enable", NULL };
const char* EBlendEffectConfigSourceMul_Tokens[] = { "src", "source", "srcmul", "sourcemul", "src_mul", "source_mul", "src mul", "source mul", NULL };
const char* EBlendEffectConfigTargetMul_Tokens[] = { "trg", "dst", "dest", "target", "trgmul", "dstmul", "destmul", "targetmul", "trg_mul", "dst_mul", "dest_mul", "target_mul", "trg mul", "dst mul", "dest mul", "target mul", NULL };
const char* EBlendEffectConfigCombiner_Tokens[] = { "combiner", "blendop", "blendfunc", "blendfunction", "blend_op", "blend_func", "blend_function", "blend op", "blend func", "blend function", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Blend effect (colour or alpha) configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EBlendEffectConfig_Tokens[] =
{
    { EBlendEffectConfigEnable_Tokens,    EBlendEffectConfigEnable      },
    { EBlendEffectConfigSourceMul_Tokens, EBlendEffectConfigSourceMul   },
    { EBlendEffectConfigTargetMul_Tokens, EBlendEffectConfigTargetMul   },
    { EBlendEffectConfigCombiner_Tokens,  EBlendEffectConfigCombiner    },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Blend logic configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EBlendLogicConfigEnable_Tokens[] = { "enable", NULL };
const char* EBlendLogicConfigOp_Tokens[] = { "op", "func", "function", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Blend logic configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EBlendLogicConfig_Tokens[] =
{
    { EBlendLogicConfigEnable_Tokens,   EBlendLogicConfigEnable },
    { EBlendLogicConfigOp_Tokens,       EBlendLogicConfigOp     },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Blend configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EBlendConfigEnableA2C_Tokens[] = { "a2c", "alphatocoverage", "enablealphatocoverage", "alphatocoverageenable", "alpha_to_coverage", "enable_alpha_to_coverage", "alpha_to_coverage_enable", "alpha-to-coverage", "enable alpha-to-coverage", "alpha-to-coverage enable", "alpha to coverage", "enable alpha to coverage", "alpha to coverage enable", NULL };
const char* EBlendConfigTargets_Tokens[] = { "targets", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Blend configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EBlendConfig_Tokens[] =
{
    { EBlendTargetConfigRGBA_Tokens,    EBlendConfigRGBA        },
    { EBlendTargetConfigColour_Tokens,  EBlendConfigColour      },
    { EBlendTargetConfigAlpha_Tokens,   EBlendConfigAlpha       },
    { EBlendTargetConfigLogic_Tokens,   EBlendConfigLogic       },
    { EBlendTargetConfigMask_Tokens,    EBlendConfigMask        },
    { EBlendConfigEnableA2C_Tokens,     EBlendConfigEnableA2C   },
    { EBlendConfigTargets_Tokens,       EBlendConfigTargets     },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    rendering::EElementFormat token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EStreamData_UNKNOWN_Tokens[] = {        "unknown",              "unk",          NULL };
const char* EStreamData_8x4_SINT_Tokens[] = {       "r8g8b8a8_int",         "int8x4",       "char4",    NULL };
const char* EStreamData_8x4_UINT_Tokens[] = {       "r8g8b8a8_uint",        "uint8x4",      "uchar4",   NULL };
const char* EStreamData_8x4_SNORM_Tokens[] = {      "r8g8b8a8_snorm",       "snorm8x4",     "snorm",    NULL };
const char* EStreamData_8x4_UNORM_Tokens[] = {      "r8g8b8a8_unorm",       "unorm8x4",     "unorm",    "color",    "colour",   "r8g8b8a8", NULL };
const char* EStreamData_8x4_SRGB_Tokens[] = {       "r8g8b8a8_srgb",        "srgb8x4",      "srgb",     NULL };
const char* EStreamData_16x2_SINT_Tokens[] = {      "r16g16_int",           "int16x2",      "short2",   NULL };
const char* EStreamData_16x2_UINT_Tokens[] = {      "r16g16_uint",          "uint16x2",     "ushort2",  NULL };
const char* EStreamData_16x2_SNORM_Tokens[] = {     "r16g16_snorm",         "snorm16x2",    NULL };
const char* EStreamData_16x2_UNORM_Tokens[] = {     "r16g16_unorm",         "unorm16x2",    "r16g16",   NULL };
const char* EStreamData_16x2_FLOAT_Tokens[] = {     "r16g16_float",         "float16x2",    "half2",    NULL };
const char* EStreamData_16x4_SINT_Tokens[] = {      "r16g16b16a16_int",     "int16x4",      "short4",   NULL };
const char* EStreamData_16x4_UINT_Tokens[] = {      "r16g16b16a16_uint",    "uint16x4",     "ushort4",  NULL };
const char* EStreamData_16x4_SNORM_Tokens[] = {     "r16g16b16a16_snorm",   "snorm16x4",    NULL };
const char* EStreamData_16x4_UNORM_Tokens[] = {     "r16g16b16a16_unorm",   "unorm16x4",    NULL };
const char* EStreamData_16x4_FLOAT_Tokens[] = {     "r16g16b16a16_float",   "float16x4",    "half4",    NULL };
const char* EStreamData_32x1_TYPELESS_Tokens[] = {  "r32",                  "typeless1",    "typeless", NULL };
const char* EStreamData_32x1_SINT_Tokens[] = {      "r32_int",              "int32x1",      "int1",     "int32",    "int",      NULL };
const char* EStreamData_32x1_UINT_Tokens[] = {      "r32_uint",             "uint32x1",     "uint1",    "uint32",   "uint",     NULL };
const char* EStreamData_32x1_FLOAT_Tokens[] = {     "r32_float",            "float32x1",    "float1",   "float32",  "float",    NULL };
const char* EStreamData_32x2_TYPELESS_Tokens[] = {  "r32g32",               "typeless2",    NULL };
const char* EStreamData_32x2_SINT_Tokens[] = {      "r32g32_int",           "int32x2",      "int2",     NULL };
const char* EStreamData_32x2_UINT_Tokens[] = {      "r32g32_uint",          "uint32x2",     "uint2",    NULL };
const char* EStreamData_32x2_FLOAT_Tokens[] = {     "r32g32_float",         "float32x2",    "float2",   NULL };
const char* EStreamData_32x3_TYPELESS_Tokens[] = {  "r32g32b32",            "typeless3",    NULL };
const char* EStreamData_32x3_SINT_Tokens[] = {      "r32g32b32_int",        "int32x3",      "int3",     NULL };
const char* EStreamData_32x3_UINT_Tokens[] = {      "r32g32b32_uint",       "uint32x3",     "uint3",    NULL };
const char* EStreamData_32x3_FLOAT_Tokens[] = {     "r32g32b32_float",      "float32x3",    "float3",   NULL };
const char* EStreamData_32x4_TYPELESS_Tokens[] = {  "r32g32b32a32",         "typeless4",    NULL };
const char* EStreamData_32x4_SINT_Tokens[] = {      "r32g32b32a32_int",     "int32x4",      "int4",     NULL };
const char* EStreamData_32x4_UINT_Tokens[] = {      "r32g32b32a32_uint",    "uint32x4",     "uint4",    NULL };
const char* EStreamData_32x4_FLOAT_Tokens[] = {     "r32g32b32a32_float",   "float32x4",    "float4",   NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    rendering::EElementFormat token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EStreamDataType_Tokens[] =
{
    { EStreamData_UNKNOWN_Tokens,       rendering::EElement_UNKNOWN         },
    { EStreamData_8x4_SINT_Tokens,      rendering::EElement_8x4_SINT        },
    { EStreamData_8x4_UINT_Tokens,      rendering::EElement_8x4_UINT        },
    { EStreamData_8x4_SNORM_Tokens,     rendering::EElement_8x4_SNORM       },
    { EStreamData_8x4_UNORM_Tokens,     rendering::EElement_8x4_UNORM       },
    { EStreamData_8x4_SRGB_Tokens,      rendering::EElement_8x4_SRGB        },
    { EStreamData_16x2_SINT_Tokens,     rendering::EElement_16x2_SINT       },
    { EStreamData_16x2_UINT_Tokens,     rendering::EElement_16x2_UINT       },
    { EStreamData_16x2_SNORM_Tokens,    rendering::EElement_16x2_SNORM      },
    { EStreamData_16x2_UNORM_Tokens,    rendering::EElement_16x2_UNORM      },
    { EStreamData_16x2_FLOAT_Tokens,    rendering::EElement_16x2_FLOAT      },
    { EStreamData_16x4_SINT_Tokens,     rendering::EElement_16x4_SINT       },
    { EStreamData_16x4_UINT_Tokens,     rendering::EElement_16x4_UINT       },
    { EStreamData_16x4_SNORM_Tokens,    rendering::EElement_16x4_SNORM      },
    { EStreamData_16x4_UNORM_Tokens,    rendering::EElement_16x4_UNORM      },
    { EStreamData_16x4_FLOAT_Tokens,    rendering::EElement_16x4_FLOAT      },
    { EStreamData_32x1_TYPELESS_Tokens, rendering::EElement_32x1_TYPELESS   },
    { EStreamData_32x1_SINT_Tokens,     rendering::EElement_32x1_SINT       },
    { EStreamData_32x1_UINT_Tokens,     rendering::EElement_32x1_UINT       },
    { EStreamData_32x1_FLOAT_Tokens,    rendering::EElement_32x1_FLOAT      },
    { EStreamData_32x2_TYPELESS_Tokens, rendering::EElement_32x2_TYPELESS   },
    { EStreamData_32x2_SINT_Tokens,     rendering::EElement_32x2_SINT       },
    { EStreamData_32x2_UINT_Tokens,     rendering::EElement_32x2_UINT       },
    { EStreamData_32x2_FLOAT_Tokens,    rendering::EElement_32x2_FLOAT      },
    { EStreamData_32x3_TYPELESS_Tokens, rendering::EElement_32x3_TYPELESS   },
    { EStreamData_32x3_SINT_Tokens,     rendering::EElement_32x3_SINT       },
    { EStreamData_32x3_UINT_Tokens,     rendering::EElement_32x3_UINT       },
    { EStreamData_32x3_FLOAT_Tokens,    rendering::EElement_32x3_FLOAT      },
    { EStreamData_32x4_TYPELESS_Tokens, rendering::EElement_32x4_TYPELESS   },
    { EStreamData_32x4_SINT_Tokens,     rendering::EElement_32x4_SINT       },
    { EStreamData_32x4_UINT_Tokens,     rendering::EElement_32x4_UINT       },
    { EStreamData_32x4_FLOAT_Tokens,    rendering::EElement_32x4_FLOAT      },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Stream configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EStreamConfigFormat_Tokens[] = { "format", NULL };
const char* EStreamConfigOffset_Tokens[] = { "offset", NULL };
const char* EStreamConfigSource_Tokens[] = { "source", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Stream configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EStreamConfig_Tokens[] =
{
    { EStreamConfigFormat_Tokens,   EStreamConfigFormat },
    { EStreamConfigOffset_Tokens,   EStreamConfigOffset },
    { EStreamConfigSource_Tokens,   EStreamConfigSource },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Linkage configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* ELinkageConfigSemantic_Tokens[] = { "semantic", NULL };
const char* ELinkageConfigSemanticIndex_Tokens[] = { "index", "semanticindex", "semantic_index", "semantic index", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Linkage configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP ELinkageConfig_Tokens[] =
{
    { ELinkageConfigSemantic_Tokens,        ELinkageConfigSemantic      },
    { ELinkageConfigSemanticIndex_Tokens,   ELinkageConfigSemanticIndex },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Assembly configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EAssemblyConfigFrequency_Tokens[] = { "rate", "stepping", "frequency", "streamrate", "streamstepping", "streamfrequency", "instancerate", "instancestepping", "instancefrequency", "stream_rate", "stream_stepping", "stream_frequency", "instance_rate", "instance_stepping", "instance_frequency", "stream rate", "stream stepping", "stream frequency", "instance rate", "instance stepping", "instance frequency", NULL };
const char* EAssemblyConfigStream_Tokens[] = { "stream", NULL };
const char* EAssemblyConfigLinkage_Tokens[] = { "linkage", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Assembly configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EAssemblyConfig_Tokens[] =
{
    { EAssemblyConfigFrequency_Tokens,  EAssemblyConfigFrequency    },
    { EAssemblyConfigStream_Tokens,     EAssemblyConfigStream       },
    { EAssemblyConfigLinkage_Tokens,    EAssemblyConfigLinkage      },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Shader configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EStagesConfigCompute_Tokens[] = { "cs", "compute", NULL };
const char* EStagesConfigPixel_Tokens[] = { "ps", "fs", "pixel", "fragment", NULL };
const char* EStagesConfigVertex_Tokens[] = { "vs", "vertex", NULL };
const char* EStagesConfigHull_Tokens[] = { "hs", "hull", NULL };
const char* EStagesConfigDomain_Tokens[] = { "ds", "domain", NULL };
const char* EStagesConfigGeometry_Tokens[] = { "gs", "geometry", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Stages configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EStagesConfig_Tokens[] =
{
    { EStagesConfigCompute_Tokens,  EStagesConfigCompute    },
    { EStagesConfigPixel_Tokens,    EStagesConfigPixel      },
    { EStagesConfigVertex_Tokens,   EStagesConfigVertex     },
    { EStagesConfigHull_Tokens,     EStagesConfigHull       },
    { EStagesConfigDomain_Tokens,   EStagesConfigDomain     },
    { EStagesConfigGeometry_Tokens, EStagesConfigGeometry   },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Shader configuration token strings
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* EShaderConfigStages_Tokens[] = { "stages", NULL };
const char* EShaderConfigAssembly_Tokens[] = { "ia", "input", "assembly", "vertex", "vertices", NULL };
const char* EShaderConfigStreamOut_Tokens[] = { "so", "out", "output", "streamout", "stream_out", "stream-out", "stream out", NULL };
const char* EShaderConfigRenderTargets_Tokens[] = { "targets", "rendertargets", "render_targets", "render-targets", "render targets", NULL };
const char* EShaderConfigRenderTarget_Tokens[] = { "target", "rendertarget", "render_target", "render-target", "render target", NULL };
const char* EShaderConfigDepthStencil_Tokens[] = { "dbuffer", "depthbuffer", "depthstencil", "depth_buffer", "depth_stencil", "depth-buffer", "depth-stencil", "depth buffer", "depth stencil", NULL };
const char* EShaderConfigDepthState_Tokens[] = { "ds", "depth", "depthstate", "depth_state", "depth-state", "depth state", NULL };
const char* EShaderConfigRasterState_Tokens[] = { "rs", "raster", "rasterstate", "raster_state", "raster-state", "raster state", NULL };
const char* EShaderConfigBlendState_Tokens[] = { "bs", "blend", "blendstate", "blend_state", "blend-state", "blend state", NULL };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Shader configuration token mapping
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TOKEN_MAP EShaderConfig_Tokens[] =
{
    { EShaderConfigStages_Tokens,           EShaderConfigStages        },
    { EShaderConfigAssembly_Tokens,         EShaderConfigAssembly      },
    { EShaderConfigStreamOut_Tokens,        EShaderConfigStreamOut     },
    { EShaderConfigRenderTargets_Tokens,    EShaderConfigRenderTargets },
    { EShaderConfigRenderTarget_Tokens,     EShaderConfigRenderTarget  },
    { EShaderConfigDepthStencil_Tokens,     EShaderConfigDepthStencil  },
    { EShaderConfigDepthState_Tokens,       EShaderConfigDepthState    },
    { EShaderConfigRasterState_Tokens,      EShaderConfigRasterState   },
    { EShaderConfigBlendState_Tokens,       EShaderConfigBlendState    },
    { NULL, -1 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end tokens namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace tokens

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Token finder functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool checkTokens( const char* const token, const char* const* const tokens )
{
    if( ( token != NULL ) && ( tokens != NULL ) )
    {
        for( int index = 0; tokens[ index ] != NULL; ++index )
        {
            if( _stricmp( token, tokens[ index ] ) == 0 )
            {
                return( true );
            }
        }
    }
    return( false );
}

int findToken( const char* const token, const tokens::TOKEN_MAP* token_map )
{
    if( ( token != NULL ) && ( token_map != NULL ) )
    {
        for( int index = 0; token_map[ index ].tokens != NULL; ++index )
        {
            if( checkTokens( token, token_map[ index ].tokens ) )
            {
                return( token_map[ index ].value );
            }
        }
    }
    return( -1 );
}

int findCmpFuncToken( const char* const token )             { return( findToken( token, tokens::ECmpFunc_Tokens ) ); };
int findSamplingModeToken( const char* const token )        { return( findToken( token, tokens::ESamplingMode_Tokens ) ); };
int findWrapModeToken( const char* const token )            { return( findToken( token, tokens::EWrapMode_Tokens ) ); };
int findStencilOpToken( const char* const token )           { return( findToken( token, tokens::EStencilOp_Tokens ) ); };
int findCullModeToken( const char* const token )            { return( findToken( token, tokens::ECullMode_Tokens ) ); };
int findSamplesUAVToken( const char* const token )          { return( findToken( token, tokens::ESamplesUAV_Tokens ) ); };
int findBlendMulToken( const char* const token )            { return( findToken( token, tokens::EBlendMul_Tokens ) ); };
int findCombinerToken( const char* const token )            { return( findToken( token, tokens::ECombiner_Tokens ) ); };
int findLogicOpToken( const char* const token )             { return( findToken( token, tokens::ELogicOp_Tokens ) ); };
int findSamplerConfigToken( const char* const token )       { return( findToken( token, tokens::ESamplerConfig_Tokens ) ); };
int findDepthConfigToken( const char* const token )         { return( findToken( token, tokens::EDepthConfig_Tokens ) ); };
int findStencilConfigToken( const char* const token )       { return( findToken( token, tokens::EStencilConfig_Tokens ) ); };
int findStencilFaceConfigToken( const char* const token )   { return( findToken( token, tokens::EStencilFaceConfig_Tokens ) ); };
int findDepthStencilConfigToken( const char* const token )  { return( findToken( token, tokens::EDepthStencilConfig_Tokens ) ); };
int findRasterConfigToken( const char* const token )        { return( findToken( token, tokens::ERasterConfig_Tokens ) ); };
int findBlendTargetToken( const char* const token )         { return( findToken( token, tokens::EBlendTargetConfig_Tokens ) ); };
int findBlendEffectToken( const char* const token )         { return( findToken( token, tokens::EBlendEffectConfig_Tokens ) ); };
int findBlendLogicToken( const char* const token )          { return( findToken( token, tokens::EBlendLogicConfig_Tokens ) ); };
int findBlendConfigToken( const char* const token )         { return( findToken( token, tokens::EBlendConfig_Tokens ) ); };
int findStreamItemTypeToken( const char* const token )      { return( findToken( token, tokens::EStreamDataType_Tokens ) ); };
int findStreamConfigToken( const char* const token )        { return( findToken( token, tokens::EStreamConfig_Tokens ) ); };
int findLinkageConfigToken( const char* const token )       { return( findToken( token, tokens::ELinkageConfig_Tokens ) ); };
int findAssemblyConfigToken( const char* const token )      { return( findToken( token, tokens::EAssemblyConfig_Tokens ) ); };
int findStagesConfigToken( const char* const token )        { return( findToken( token, tokens::EStagesConfig_Tokens ) ); };
int findShaderConfigToken( const char* const token )        { return( findToken( token, tokens::EShaderConfig_Tokens ) ); };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end pipeline namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace pipeline

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
