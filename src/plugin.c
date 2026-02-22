/**
 * src/plugin.c — はじむ オーディオエンジン プラグインエントリー
 *
 * Copyright (c) 2026 Reo Shiozawa — MIT License
 */
#include "hajimu_plugin.h"
#include "eng_audio.h"

static ENG_Audio* g_a = NULL;

/* ── ヘルパーマクロ ─────────────────────────────────────*/
#define ARG_NUM(i) ((i) < argc && args[(i)].type == VALUE_NUMBER ? args[(i)].number : 0.0)
#define ARG_STR(i) ((i) < argc && args[(i)].type == VALUE_STRING  ? args[(i)].string.data : "")
#define ARG_INT(i) ((int)ARG_NUM(i))
#define ARG_F(i)   ((float)ARG_NUM(i))
#define ARG_B(i)   ((i) < argc && args[(i)].type == VALUE_BOOL ? args[(i)].boolean : false)
#define NUM(v)     hajimu_number((double)(v))
#define BVAL(v)    hajimu_bool((bool)(v))
#define NUL        hajimu_null()

/* ── ライフサイクル ─────────────────────────────────────*/
static Value fn_音声初期化(int argc, Value* args) {
    (void)argc; (void)args;
    if (g_a) { eng_audio_destroy(g_a); g_a = NULL; }
    g_a = eng_audio_create();
    return NUM(g_a ? 0 : -1);
}
static Value fn_音声終了(int argc, Value* args) {
    (void)argc; (void)args;
    if (g_a) { eng_audio_destroy(g_a); g_a = NULL; }
    return NUL;
}

/* ── BGM ────────────────────────────────────────────────*/
static Value fn_音楽読込(int argc, Value* args)        { return NUM(eng_bgm_load(g_a, ARG_STR(0))); }
static Value fn_音楽再生(int argc, Value* args)        { eng_bgm_play(g_a, ARG_INT(0)); return NUL; }
static Value fn_音楽停止(int argc, Value* args)        { eng_bgm_stop(g_a, ARG_INT(0)); return NUL; }
static Value fn_音楽一時停止(int argc, Value* args)    { eng_bgm_pause(g_a, ARG_INT(0)); return NUL; }
static Value fn_音楽再開(int argc, Value* args)        { eng_bgm_resume(g_a, ARG_INT(0)); return NUL; }
static Value fn_音楽ループ設定(int argc, Value* args)  { eng_bgm_set_loop(g_a, ARG_INT(0), ARG_B(1)); return NUL; }
static Value fn_音楽音量設定(int argc, Value* args)    { eng_bgm_set_volume(g_a, ARG_INT(0), ARG_F(1)); return NUL; }
static Value fn_音楽位置設定(int argc, Value* args)    { eng_bgm_seek(g_a, ARG_INT(0), ARG_F(1)); return NUL; }
static Value fn_音楽位置取得(int argc, Value* args)    { return NUM(eng_bgm_position(g_a, ARG_INT(0))); }
static Value fn_音楽再生中(int argc, Value* args)      { return BVAL(eng_bgm_is_playing(g_a, ARG_INT(0))); }
static Value fn_音楽削除(int argc, Value* args)        { eng_bgm_free(g_a, ARG_INT(0)); return NUL; }
static Value fn_音楽ピッチ設定(int argc, Value* args)  { eng_bgm_set_pitch(g_a, ARG_INT(0), ARG_F(1)); return NUL; }

/* ── SE ─────────────────────────────────────────────────*/
static Value fn_SE読込(int argc, Value* args)          { return NUM(eng_se_load(g_a, ARG_STR(0))); }
static Value fn_SE再生(int argc, Value* args)          { eng_se_play(g_a, ARG_INT(0)); return NUL; }
static Value fn_SE再生音量(int argc, Value* args)      { eng_se_play_vol(g_a, ARG_INT(0), ARG_F(1)); return NUL; }
static Value fn_SE停止(int argc, Value* args)          { eng_se_stop(g_a, ARG_INT(0)); return NUL; }
static Value fn_SE音量設定(int argc, Value* args)      { eng_se_set_volume(g_a, ARG_INT(0), ARG_F(1)); return NUL; }
static Value fn_SEピッチ設定(int argc, Value* args)    { eng_se_set_pitch(g_a, ARG_INT(0), ARG_F(1)); return NUL; }
static Value fn_SEループ設定(int argc, Value* args)    { eng_se_set_loop(g_a, ARG_INT(0), ARG_B(1)); return NUL; }
static Value fn_SE再生確認(int argc, Value* args)      { return BVAL(eng_se_is_playing(g_a, ARG_INT(0))); }
static Value fn_SE削除(int argc, Value* args)          { eng_se_free(g_a, ARG_INT(0)); return NUL; }

/* ── グローバル ─────────────────────────────────────────*/
static Value fn_主音量設定(int argc, Value* args) {
    eng_audio_set_master_volume(g_a, ARG_F(0)); return NUL;
}

/* ── フェード ────────────────────────────────────────────*/
static Value fn_音楽フェードイン(int argc, Value* args)  { eng_bgm_fade_in(g_a, ARG_INT(0), ARG_F(1)); return NUL; }
static Value fn_音楽フェードアウト(int argc, Value* args){ eng_bgm_fade_out(g_a, ARG_INT(0), ARG_F(1)); return NUL; }
static Value fn_音楽クロスフェード(int argc, Value* args){ eng_bgm_crossfade(g_a, ARG_INT(0), ARG_INT(1), ARG_F(2)); return NUL; }

/* ── パン ────────────────────────────────────────────────*/
static Value fn_音楽パン設定(int argc, Value* args) { eng_bgm_set_pan(g_a, ARG_INT(0), ARG_F(1)); return NUL; }
static Value fn_SEパン設定(int argc, Value* args)   { eng_se_set_pan(g_a, ARG_INT(0), ARG_F(1)); return NUL; }

/* ── 長さ ────────────────────────────────────────────────*/
static Value fn_音楽長さ取得(int argc, Value* args) { return NUM(eng_bgm_duration(g_a, ARG_INT(0))); }

/* ── プラグイン登録 ─────────────────────────────────────*/
#define FN(name, mn, mx) { #name, fn_##name, mn, mx }

static HajimuPluginFunc funcs[] = {
    /* ライフサイクル */
    FN(音声初期化, 0, 0),
    FN(音声終了,   0, 0),
    /* BGM */
    FN(音楽読込,     1, 1),
    FN(音楽再生,     1, 1),
    FN(音楽停止,     1, 1),
    FN(音楽一時停止, 1, 1),
    FN(音楽再開,     1, 1),
    FN(音楽ループ設定, 2, 2),
    FN(音楽音量設定, 2, 2),
    FN(音楽位置設定, 2, 2),
    FN(音楽位置取得, 1, 1),
    FN(音楽再生中,   1, 1),
    FN(音楽削除,     1, 1),
    FN(音楽ピッチ設定, 2, 2),
    /* SE */
    FN(SE読込,    1, 1),
    FN(SE再生,    1, 1),
    FN(SE再生音量, 2, 2),
    FN(SE停止,    1, 1),
    FN(SE音量設定, 2, 2),
    FN(SEピッチ設定, 2, 2),
    FN(SEループ設定, 2, 2),
    FN(SE再生確認, 1, 1),
    FN(SE削除,    1, 1),
    /* グローバル */
    FN(主音量設定, 1, 1),
    /* フェード */
    FN(音楽フェードイン,  2, 2),
    FN(音楽フェードアウト, 2, 2),
    FN(音楽クロスフェード, 3, 3),
    /* パン */
    FN(音楽パン設定, 2, 2),
    FN(SEパン設定,   2, 2),
    /* 長さ */
    FN(音楽長さ取得, 1, 1),
};

HAJIMU_PLUGIN_EXPORT HajimuPluginInfo* hajimu_plugin_init(void) {
    static HajimuPluginInfo info = {
        .name           = "engine_audio",
        .version        = "1.2.0",
        .author         = "Reo Shiozawa",
        .description    = "はじむ用オーディオエンジン (miniaudio BGM/SE/フェード/クロスフェード/パン/ピッチ/ループ)",
        .functions      = funcs,
        .function_count = sizeof(funcs) / sizeof(funcs[0]),
    };
    return &info;
}
