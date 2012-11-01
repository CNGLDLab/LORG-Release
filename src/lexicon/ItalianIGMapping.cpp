#include "ItalianIGMapping.h"

#include <vector>

namespace {
  bool ends_with(const std::string& word, const std::string& suffix)
  {
    //    std::cout << "is " << word << " ending with " << suffix << " ? " ;
    bool result =
      (word.length() >= suffix.length()) &&
      (suffix == word.substr(word.length() - suffix.length(), suffix.length()));

    //    std::cout << (result ? "True" : "False") << std::endl;
    return result;
  }
}

std::string ItalianIGMapping::get_unknown_mapping(const std::string& word, unsigned position) const
{
  std::string word_class("UNK");
  std::string word_lower_case=to_lower_case(word);

  bool hasDigit = word.find_first_of("0123456789") != std::string::npos;
  bool hasDash =  word.find_first_of("-") != std::string::npos;
  bool hasLowerCase = false;
  int upperCaseCount=0;

  //std::cout << "this string contains a digit, see " << word << std::endl;
  //std::cout << "this string contains a dash, see " << word << std::endl;

  for(std::string::const_iterator c = word.begin(); c != word.end(); c++){

    if (is_upper_case_letter(*c)){
      //Berkeley parser puts hasLower = true here - an error?
      ++upperCaseCount;
    }else if  (is_lower_case_letter(*c)){
      hasLowerCase = true;
    }
  }

  //	if (upperCaseCount > 0){
  //	std::cout << "original word is " << word << " lower case version is " << word_lower_case << std::endl;
  //}
  //if first character is upper case and it is the first word in sentences and the remaining characters are lower case

  SymbolTable& sym_tab_word = SymbolTable::instance_word();

  //deal with capitalisation
  if ( is_upper_case_letter(*(word.begin()))){
    if (position==0 && upperCaseCount==1){
      word_class.append("-INITC");
      if (sym_tab_word.token_exists(word_lower_case)){
	word_class.append("-KNOWNLC");
      }
    }else {
      word_class.append("-CAPS");
    }
    //missing an else if here - basically, I'm not dealing with non-letters
  }else if (hasLowerCase){
    word_class.append("-LC");
  }

  if (hasDigit){
    word_class.append("-NUM");
  }
  if (hasDash){
    word_class.append("-DASH");
  }

  unsigned word_length = word_lower_case.size();

  // if ( (word_length >=3) && (ends_with(word_lower_case,"s") && !ends_with(word_lower_case, "ss")) ){
  //   word_class.append("-s");
  // }

  // else

    if (word_length >= 5 && !hasDash && !(hasDigit && upperCaseCount > 0)) {
      // don't do for very short words;

 // 0.25963        47 SUFFIXi_46
 // 0.16248        16 SUFFIXo_15
 // 0.14304        51 SUFFIXre_50
 // 0.12829         4 PREFIXd_3
 // 0.11433         3 SUFFIXllquote_2
 // 0.11415         2 SUFFIXlquote_1
 // 0.11148         6 PREFIXdel_5
 // 0.10971         1 SUFFIXquote_0
 // 0.10951        31 SUFFIXno_30
 // 0.10844         5 PREFIXde_4
 // 0.1002          9 SUFFIXlle_8
 // 0.09949        41 SUFFIXto_40
 // 0.09552         7 SUFFIXe_6
 // 0.08859         8 SUFFIXle_7
 // 0.08704        12 SUFFIXlla_11
 // 0.08649       229 SUFFIXti_228
 // 0.08555       132 SUFFIX√†_131
 // 0.08456        10 SUFFIXa_9
 // 0.08377        89 SUFFIXni_88
 // 0.07892       133 SUFFIXt√†_132
 // 0.0788         15 PREFIXall_14
 // 0.0781        275 SUFFIXone_274
 // 0.07805        14 PREFIXal_13
 // 0.07765       115 PREFIXqu_114
 // 0.07765       114 PREFIXq_113
 // 0.07666        11 SUFFIXla_10
 // 0.07278        32 SUFFIXono_31
 // 0.0695        162 SUFFIXne_161
 // 0.06929       345 SUFFIXare_344
 // 0.05866       198 PREFIXque_197
 // 0.05729        52 SUFFIXere_51
 // 0.05418        64 SUFFIXSB-_63
 // 0.05418        62 SUFFIX-_61
 // 0.05418        63 SUFFIXB-_62
 // 0.05404        65 PREFIX-_64
 // 0.05392       221 SUFFIXit√†_220
 // 0.04969        46 PREFIXdal_45
 // 0.04965        33 PREFIXs_32
 // 0.04582        45 PREFIXda_44
 // 0.04461       287 SUFFIXta_286
 // 0.04413       224 SUFFIXoni_223
 // 0.04391        48 SUFFIXli_47
 // 0.04389       103 SUFFIXato_102
 // 0.04266       861 PREFIX1_860
 // 0.04134       273 SUFFIXsi_272
 // 0.04117        35 PREFIXson_34
 // 0.04077       263 SUFFIXano_262
 // 0.0403         24 PREFIXp_23
 // 0.03967        18 SUFFIXndo_17
 // 0.03965        13 PREFIXa_12
 // 0.03933        17 SUFFIXdo_16
 // 0.03797       128 SUFFIXte_127
 // 0.03768        55 PREFIXess_54
 // 0.03757        72 PREFIXnel_71
 // 0.03744       175 SUFFIXnte_174
 // 0.03703        71 PREFIXne_70
 // 0.03678        22 SUFFIXio_21
 // 0.03659       116 PREFIXqua_115
 // 0.03655       210 SUFFIXri_209
 // 0.03525       230 SUFFIXnti_229
 // 0.03515       297 SUFFIXale_296
 // 0.03427        53 PREFIXe_52
 // 0.03417        49 SUFFIXgli_48
 // 0.03414       208 PREFIXst_207
 // 0.03363       851 SUFFIXrsi_850
 // 0.03339        54 PREFIXes_53
 // 0.03323       212 PREFIXalt_211
 // 0.03277        34 PREFIXso_33
 // 0.03079       209 PREFIXsta_208
 // 0.03075        94 PREFIXc_93
 // 0.03018        68 PREFIX-R_67
 // 0.03018        67 PREFIX-LS_66
 // 0.03018        66 PREFIX-L_65
 // 0.03018        69 PREFIX-RS_68
 // 0.02856       336 SUFFIXati_335
 // 0.02841        30 PREFIXanc_29
 // 0.02768       439 PREFIXr_438
 // 0.02728       299 SUFFIXali_298
 // 0.02707       295 SUFFIXata_294
 // 0.02587        28 SUFFIXche_27
 // 0.02586        27 SUFFIXhe_26
 // 0.02563       358 SUFFIXva_357
 // 0.02553        70 PREFIXn_69
 // 0.02475       172 PREFIXt_171
 // 0.02435       251 SUFFIXnto_250
 // 0.02428        73 SUFFIXro_72
 // 0.0239         25 PREFIXpr_24
 // 0.0236         82 SUFFIXve_81
 // 0.0235        659 SUFFIXire_658
 // 0.02311       184 PREFIXi_183
 // 0.02199        95 PREFIXco_94
 // 0.02169        85 SUFFIXnno_84
 // 0.02154       705 PREFIX2_704
 // 0.02132        84 PREFIXdev_83
 // 0.02128       194 SUFFIXia_193
 // 0.02112        29 PREFIXan_28
 // 0.02038       245 PREFIXtut_244
 // 0.02032       144 PREFIXsul_143
 // 0.01997       152 PREFIXm_151
 // 0.01994        38 PREFIXl_37
 // 0.01981       243 SUFFIXtti_242
 // 0.0197         50 PREFIXdeg_49
 // 0.01949       244 PREFIXtu_243
 // 0.01858       101 PREFIXpo_100
 // 0.01846       158 SUFFIXsto_157
 // 0.01834        42 SUFFIXtto_41
 // 0.01832       110 SUFFIXza_109
 // 0.01817        26 PREFIXpro_25
 // 0.01817       573 PREFIXri_572
 // 0.01788        76 PREFIXlor_75
 // 0.01781       139 SUFFIXlo_138
 // 0.01779      1143 PREFIX10_1142
 // 0.01758        74 SUFFIXoro_73
 // 0.01716       143 PREFIXsu_142
 // 0.01709       199 SUFFIXna_198
 // 0.01675        23 SUFFIXrio_22
 // 0.01622       290 PREFIXdov_289
 // 0.01619        83 SUFFIXeve_82
 // 0.01608        88 PREFIXhan_87
 // 0.01605        87 PREFIXha_86
 // 0.01599        75 PREFIXlo_74
 // 0.01579       386 PREFIXav_385
 // 0.01571        86 PREFIXh_85
 // 0.01564       117 SUFFIXgni_116
 // 0.01561        59 PREFIXA_58
 // 0.01561       157 SUFFIXllo_156
 // 0.01535       111 SUFFIXnza_110
 // 0.01525        19 PREFIXf_18
 // 0.01508       102 PREFIXpos_101
 // 0.01505       387 PREFIXave_386
 // 0.01501       577 SUFFIXaquote_576
 // 0.01492       219 PREFIXdo_218
 // 0.01464       159 PREFIXv_158
 // 0.01462       818 SUFFIXr√†_817
 // 0.01461       185 PREFIXin_184
 // 0.01445       507 SUFFIXate_506
 // 0.01438       238 PREFIXpri_237
 // 0.0143        118 PREFIXo_117
 // 0.01385       567 SUFFIXbe_566
 // 0.01383       270 PREFIXB_269
 // 0.01374       568 SUFFIXbbe_567
 // 0.01363       361 PREFIXste_360
 // 0.01361       112 PREFIXse_111
 // 0.01359       385 SUFFIXeva_384
 // 0.01354       395 SUFFIXde_394
 // 0.01342       211 SUFFIXtri_210
 // 0.01336       281 SUFFIXra_280
 // 0.01335       176 PREFIXP_175
 // 0.01332        77 SUFFIXca_76
 // 0.0133        107 SUFFIXsa_106
 // 0.01316       119 PREFIXog_118
 // 0.01314       512 SUFFIX_DI_511
 // 0.01313       134 SUFFIXet√†_133
 // 0.0131        236 SUFFIXmo_235
 // 0.01309       120 PREFIXogn_119
 // 0.01284       511 SUFFIXDI_510
 // 0.0128        215 SUFFIXdi_214
 // 0.01277       214 PREFIXcon_213
 // 0.01265       256 SUFFIXco_255
 // 0.01248       233 PREFIXg_232
 // 0.01224       257 SUFFIXico_256
 // 0.01217        78 SUFFIXica_77
 // 0.01214       356 PREFIXved_355
 // 0.01204        99 PREFIXca_98
 // 0.01198       237 SUFFIXimo_236
 // 0.01158        43 PREFIXdi_42
 // 0.0114        858 SUFFIX0_857
 // 0.01133       649 SUFFIXsce_648
 // 0.0113        510 SUFFIXI_509
 // 0.0113        213 SUFFIXtro_212
 // 0.01129        97 SUFFIXso_96
 // 0.01129       820 PREFIXsar_819
 // 0.01129       578 SUFFIXtaquote_577
 // 0.01127       371 SUFFIXuto_370
 // 0.01125       298 SUFFIXari_297
 // 0.01116       240 SUFFIXtre_239
 // 0.01109       205 PREFIXT_204
 // 0.01106      1087 PREFIXM_1086
 // 0.01099       355 PREFIXve_354
 // 0.01097        93 SUFFIXome_92
 // 0.01096      1977 SUFFIXava_1976
 // 0.01096       938 SUFFIXn_937
 // 0.01086       127 PREFIXser_126
 // 0.01078        92 SUFFIXme_91
 // 0.01074       288 SUFFIXsta_287
 // 0.01056       322 PREFIXsia_321
 // 0.0105        706 PREFIX20_705
 // 0.01036       411 PREFIXI_410
 // 0.01034       136 PREFIXC_135
 // 0.01032       124 SUFFIX√π_123
 // 0.01027       125 SUFFIXt√π_124
 // 0.01019       104 PREFIXS_103
 // 0.01019       191 SUFFIXore_190
 // 0.01016       560 SUFFIXce_559
 // 0.01015       126 SUFFIXit√π_125
 // 0.01012       140 SUFFIXolo_139
 // 0.00999       182 SUFFIXse_181
 // 0.00998       324 SUFFIXiti_323
 // 0.00993       440 PREFIXre_439
 // 0.00986       155 SUFFIXma_154
 // 0.0098        121 PREFIXu_120
 // 0.00975       599 SUFFIX√©_598
 // 0.00975       601 SUFFIXch√©_600
 // 0.00975       600 SUFFIXh√©_599
 // 0.00974       481 SUFFIXile_480
 // 0.0097         20 PREFIXfo_19
 // 0.00968       428 PREFIXalc_427
 // 0.00963       329 PREFIXpre_328
 // 0.00956       503 SUFFIXito_502
 // 0.00956       660 SUFFIXr_659
 // 0.00953       375 SUFFIXini_374
 // 0.00945       726 SUFFIXuno_725
 // 0.00945       707 PREFIX200_706
 // 0.00945       614 SUFFIXci_613
 // 0.00943       321 PREFIXsi_320
 // 0.00941      1237 SUFFIXrne_1236
 // 0.00934       604 PREFIXsa_603
 // 0.00934       130 PREFIXpa_129
 // 0.00933       266 SUFFIXria_265
 // 0.00927       335 PREFIXtal_334
 // 0.00924        57 SUFFIXt._56
 // 0.00915       289 SUFFIXove_288
 // 0.00913       360 SUFFIXssa_359
 // 0.00903       523 SUFFIXsti_522
 // 0.00892       201 PREFIXV_200
 // 0.00889       824 SUFFIXori_823
 // 0.00885       795 SUFFIX2_794
 // 0.0087         58 SUFFIXrt._57
 // 0.00867       196 PREFIXAl_195
 // 0.00866        56 SUFFIX._55
 // 0.00864        96 PREFIXcom_95
 // 0.00862       539 SUFFIXze_538
 // 0.00859       679 PREFIXIN__678
 // 0.00857       534 SUFFIXero_533
 // 0.00855      1373 PREFIX19_1372
 // 0.00854       346 PREFIXfar_345
 // 0.00854       446 PREFIXQu_445
 // 0.0085        173 PREFIXte_172
 // 0.00847       334 PREFIXta_333
 // 0.00847       445 PREFIXQ_444
 // 0.00847        21 PREFIXfon_20
 // 0.00845       170 SUFFIXpo_169
 // 0.00841       160 PREFIXvi_159
 // 0.00838       292 SUFFIXeri_291
 // 0.00823        36 SUFFIXge_35
 // 0.00823       149 SUFFIXgi_148
 // 0.00821       150 SUFFIXggi_149
 // 0.00818       650 SUFFIXvi_649
 // 0.00818       443 SUFFIXie_442
 // 0.00817       319 PREFIXfa_318
 // 0.00817       569 PREFIXpot_568
 // 0.00816       678 PREFIXIN_677
 // 0.00811        90 SUFFIXnni_89
 // 0.0081         37 SUFFIXgge_36
 // 0.00805       433 SUFFIX√≤_432
 // 0.00805       398 SUFFIXvo_397
 // 0.00803       250 PREFIXsuo_249
 // 0.00792       283 PREFIXsol_282
 // 0.00785       451 PREFIXap_450
 // 0.00783       661 SUFFIXer_660
 // 0.00783       249 SUFFIXuoi_248
 // 0.00775       241 PREFIXma_240
 // 0.00774       248 SUFFIXoi_247
 // 0.00771        79 PREFIXR_78
 // 0.00771       291 SUFFIXora_290
 // 0.00769       768 SUFFIXmi_767
 // 0.00766       702 SUFFIX4_701
 // 0.00765       161 PREFIXvis_160
 // 0.00754      1240 SUFFIXrlo_1239
 // 0.00741        39 PREFIXle_38
 // 0.00734       146 SUFFIXque_145
 // 0.00733       317 PREFIXter_316
 // 0.00731       217 PREFIXsec_216
 // 0.00722       123 PREFIXusu_122
 // 0.00721       452 PREFIXapp_451
 // 0.0072        675 SUFFIXO_674
 // 0.00715       195 SUFFIXnia_194
 // 0.00713       455 PREFIXmi_454
 // 0.00708       615 SUFFIXici_614
 // 0.007         444 SUFFIXrie_443
 // 0.00698        44 PREFIXdir_43
 // 0.00698       231 PREFIXseg_230
 // 0.00698        40 PREFIXleg_39
 // 0.00693       528 SUFFIXhi_527
 // 0.00692       402 SUFFIXssi_401
 // 0.0069        122 PREFIXus_121
 // 0.00685       540 SUFFIXnze_539
 // 0.00685       396 SUFFIXede_395
 // 0.0068        513 PREFIXD_512
 // 0.0068        271 PREFIXBe_270
 // 0.00679       282 SUFFIXtra_281
 // 0.00675       202 PREFIXVa_201
 // 0.00675       261 PREFIXme_260
 // 0.00674       167 PREFIXG_166
 // 0.00674       145 SUFFIXue_144
 // 0.00673       258 PREFIXpu_257
 // 0.00662       366 SUFFIXite_365
 // 0.00662       982 SUFFIX3_981
 // 0.00662       137 PREFIXCo_136
 // 0.00659       338 PREFIXb_337
 // 0.00656       376 PREFIXci_375
 // 0.00652       357 SUFFIXima_356
 // 0.00651       608 PREFIXA__607
 // 0.0065        790 PREFIXul_789
 // 0.0065        791 PREFIXult_790
 // 0.00645       197 PREFIXAlb_196
 // 0.00643        60 PREFIXAr_59
 // 0.00641       179 SUFFIXrme_178
 // 0.0064        474 SUFFIXene_473
 // 0.00637       259 PREFIXpub_258
 // 0.00634        61 PREFIXArt_60
 // 0.0063        234 PREFIXgi_233
 // 0.00624       466 SUFFIXita_465
 // 0.0062        592 SUFFIXm_591
 // 0.00619       876 SUFFIXili_875
 // 0.00617      1395 SUFFIX8_1394
 // 0.00615       731 SUFFIXda_730
 // 0.00614       629 SUFFIXrre_628
 // 0.00614       203 PREFIXVal_202
 // 0.00611       756 SUFFIX5_755
 // 0.0061        849 SUFFIXivo_848
 // 0.00608       204 SUFFIXana_203
 // 0.00607       254 SUFFIXino_253
 // 0.00606      1440 SUFFIXraquote_1439
 // 0.00593       400 PREFIXnu_399
 // 0.00588       223 PREFIXaut_222
 // 0.00584      1389 PREFIXL_1388
 // 0.00581       200 SUFFIXona_199
 // 0.0058        359 SUFFIXiva_358
 // 0.00578       207 PREFIXTir_206
 // 0.00578       268 SUFFIXha_267
 // 0.00573      1215 SUFFIXer√†_1214
 // 0.00564       559 PREFIXera_558
 // 0.00564       206 PREFIXTi_205
 // 0.00564       189 SUFFIXzio_188
 // 0.00563       401 PREFIXnuo_400
 // 0.00545       156 SUFFIXmma_155



      /* IG 0.01 suffixes reordered by length */
      static std::string ig_sufs[] = {

        /* 0.1002          9 */ "lle",
        /* 0.08704        12 */ "lla",
        /* 0.0781        275 */ "one",
        /* 0.07278        32 */ "ono",
        /* 0.06929       345 */ "are",
        /* 0.05729        52 */ "ere",
        /* 0.05392       221 */ "ità",
        /* 0.04413       224 */ "oni",
        /* 0.04389       103 */ "ato",
        /* 0.04077       263 */ "ano",
        // /* 0.03967        18 */ "ndo",
        // /* 0.03744       175 */ "nte",
        // /* 0.03525       230 */ "nti",
        // /* 0.03515       297 */ "ale",
        // /* 0.03417        49 */ "gli",
        // /* 0.03363       851 */ "rsi",
        //        /* 0.02856       336 */ "ati",
        // /* 0.02728       299 */ "ali",
        // /* 0.02707       295 */ "ata",
        // /* 0.02587        28 */ "che",
        // /* 0.02435       251 */ "nto",
        // /* 0.0235        659 */ "ire",
        // /* 0.02169        85 */ "nno",

        /* 0.14304        51 */ "re",
        /* 0.11433         3 */ "ll'",
        /* 0.10951        31 */ "no",
        /* 0.09949        41 */ "to",
        /* 0.08859         8 */ "le",
        /* 0.08649       229 */ "ti",
        /* 0.08377        89 */ "ni",
        /* 0.07892       133 */ "tà",
        /* 0.07666        11 */ "la",
        /* 0.0695        162 */ "ne",
        /* 0.04461       287 */ "ta",
        /* 0.04391        48 */ "li",
        /* 0.04134       273 */ "si",
        // /* 0.03933        17 */ "do",
        // /* 0.03797       128 */ "te",
        // /* 0.03678        22 */ "io",
        // /* 0.03655       210 */ "ri",
        // /* 0.02586        27 */ "he",
        // /* 0.02563       358 */ "va",
        // /* 0.02428        73 */ "ro",
        // /* 0.0236         82 */ "ve",
        // /* 0.02128       194 */ "ia",

        /* 0.25963        47 */ "i",
        /* 0.16248        16 */ "o",
        /* 0.11415         2 */ "l'",
        /* 0.10971         1 */ "'",
        /* 0.09552         7 */ "e",
        /* 0.08555       132 */ "à",
        /* 0.08456        10 */ "a",

        // 0.01 < IG < 0.02
 // /* 0.01981       243 */ "tti",
 // /* 0.01846       158 */ "sto",
 // /* 0.01834        42 */ "tto",
 // /* 0.01832       110 */ "za",
 // /* 0.01781       139 */ "lo",
 // /* 0.01758        74 */ "oro",
 // /* 0.01709       199 */ "na",
 // /* 0.01675        23 */ "rio",
 // /* 0.01619        83 */ "eve",
 // /* 0.01564       117 */ "gni",
 // /* 0.01561       157 */ "llo",
 // /* 0.01535       111 */ "nza",
 // /* 0.01501       577 */ "a'",
 // /* 0.01462       818 */ "rà",
 // /* 0.01445       507 */ "ate",
 // /* 0.01385       567 */ "be",
 // /* 0.01374       568 */ "bbe",
 // /* 0.01359       385 */ "eva",
 // /* 0.01354       395 */ "de",
 // /* 0.01342       211 */ "tri",
 // /* 0.01336       281 */ "ra",
 // /* 0.01332        77 */ "ca",
 // /* 0.0133        107 */ "sa",
 // /* 0.01314       512 */ "_DI",
 // /* 0.01313       134 */ "età",
 // /* 0.0131        236 */ "mo",
 // /* 0.01284       511 */ "DI",
 // /* 0.0128        215 */ "di",
 // /* 0.01265       256 */ "co",
 // /* 0.01224       257 */ "ico",
 // /* 0.01217        78 */ "ica",
 // /* 0.01198       237 */ "imo",
 // /* 0.0114        858 */ "0",
 // /* 0.01133       649 */ "sce",
 // /* 0.0113        510 */ "I",
 // /* 0.0113        213 */ "tro",
 // /* 0.01129        97 */ "so",
 // /* 0.01129       578 */ "ta'",
 // /* 0.01127       371 */ "uto",
 // /* 0.01125       298 */ "ari",
 // /* 0.01116       240 */ "tre",
 // /* 0.01097        93 */ "ome",
 // /* 0.01096      1977 */ "ava",
 // /* 0.01096       938 */ "n",
 // /* 0.01078        92 */ "me",
 // /* 0.01074       288 */ "sta",
 // /* 0.01032       124 */ "ù",
 // /* 0.01027       125 */ "tù",
 // /* 0.01019       191 */ "ore",
 // /* 0.01016       560 */ "ce",
 // /* 0.01015       126 */ "itù",
 // /* 0.01012       140 */ "olo"
      };
      static std::vector<std::string> suffixes(ig_sufs,ig_sufs+30);

      bool found = false;
      for(std::vector<std::string>::const_iterator i(suffixes.begin()); i != suffixes.end(); ++i) {
	//	std::cout << "testing " << word_lower_case << " against " << *i << std::endl;
	if(ends_with(word_lower_case, *i)) {
	  //	  std::cout << "testing " << word_lower_case << " against " << *i << std::endl;
	  word_class.append("-"); word_class.append(*i);
	  found = true;
	  break;
	}
      }

      // if(!found) {

      // 	for(std::vector<std::string>::const_iterator i(V_suffixes.begin()); i != V_suffixes.end(); ++i) {
      // 	  if(ends_with(word_lower_case, *i)) {
      // 	    word_class.append("-V"); word_class.append(*i);
      // 	    found = true;
      // 	    break;
      // 	  }
      // 	}
      // }


    }
  //std::cout << "word class " << word_class << std::endl;

  //    std::cout << word << " " << position << ": " << word_class << std::endl;


  return word_class;

}
