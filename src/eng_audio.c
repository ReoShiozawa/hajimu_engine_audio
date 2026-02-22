/**
 * src/eng_audio.c — miniaudio バックエンド実装
 *
 * Copyright (c) 2026 Reo Shiozawa — MIT License
 */

/* miniaudio のみを実装するファイル。他ファイルは include のみ。 */
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "eng_audio.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ── 定数 ───────────────────────────────────────────────*/
#define ENG_MAX_BGM  16
#define ENG_MAX_SE   64

/* ── サウンドスロット ────────────────────────────────────*/
typedef struct {
    ma_sound sound;
    bool     used;
    bool     streaming; /* BGM=true, SE=false */
} SoundSlot;

/* ── エンジン本体 ────────────────────────────────────────*/
struct ENG_Audio {
    ma_engine  engine;
    SoundSlot  bgm[ENG_MAX_BGM];
    SoundSlot  se[ENG_MAX_SE];
};

/* helpers */
static SoundSlot* bgm_slot(ENG_Audio* a, ENG_SoundID id) {
    if (!a || id == 0 || id > ENG_MAX_BGM) return NULL;
    SoundSlot* s = &a->bgm[id - 1];
    return s->used ? s : NULL;
}
static SoundSlot* se_slot(ENG_Audio* a, ENG_SoundID id) {
    if (!a || id == 0 || id > ENG_MAX_SE) return NULL;
    SoundSlot* s = &a->se[id - 1];
    return s->used ? s : NULL;
}

/* ── ライフサイクル ─────────────────────────────────────*/
ENG_Audio* eng_audio_create(void) {
    ENG_Audio* a = calloc(1, sizeof(ENG_Audio));
    if (!a) return NULL;
    ma_result r = ma_engine_init(NULL, &a->engine);
    if (r != MA_SUCCESS) {
        fprintf(stderr, "[eng_audio] ma_engine_init 失敗: %s\n", ma_result_description(r));
        free(a);
        return NULL;
    }
    return a;
}

void eng_audio_destroy(ENG_Audio* a) {
    if (!a) return;
    for (int i = 0; i < ENG_MAX_BGM; ++i)
        if (a->bgm[i].used) { ma_sound_uninit(&a->bgm[i].sound); a->bgm[i].used = false; }
    for (int i = 0; i < ENG_MAX_SE; ++i)
        if (a->se[i].used) { ma_sound_uninit(&a->se[i].sound); a->se[i].used = false; }
    ma_engine_uninit(&a->engine);
    free(a);
}

/* ── BGM ────────────────────────────────────────────────*/
ENG_SoundID eng_bgm_load(ENG_Audio* a, const char* path) {
    if (!a || !path) return 0;
    for (int i = 0; i < ENG_MAX_BGM; ++i) {
        if (!a->bgm[i].used) {
            ma_result r = ma_sound_init_from_file(
                &a->engine, path,
                MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_ASYNC,
                NULL, NULL, &a->bgm[i].sound);
            if (r != MA_SUCCESS) {
                fprintf(stderr, "[eng_audio] BGM読込失敗 '%s': %s\n", path, ma_result_description(r));
                return 0;
            }
            ma_sound_set_looping(&a->bgm[i].sound, MA_TRUE);
            a->bgm[i].used      = true;
            a->bgm[i].streaming = true;
            return (ENG_SoundID)(i + 1);
        }
    }
    fprintf(stderr, "[eng_audio] BGMスロット満杯 (max=%d)\n", ENG_MAX_BGM);
    return 0;
}

void eng_bgm_play(ENG_Audio* a, ENG_SoundID id) {
    SoundSlot* s = bgm_slot(a, id);
    if (s) ma_sound_start(&s->sound);
}
void eng_bgm_stop(ENG_Audio* a, ENG_SoundID id) {
    SoundSlot* s = bgm_slot(a, id);
    if (s) { ma_sound_stop(&s->sound); ma_sound_seek_to_pcm_frame(&s->sound, 0); }
}
void eng_bgm_pause(ENG_Audio* a, ENG_SoundID id) {
    SoundSlot* s = bgm_slot(a, id);
    if (s) ma_sound_stop(&s->sound);
}
void eng_bgm_resume(ENG_Audio* a, ENG_SoundID id) {
    SoundSlot* s = bgm_slot(a, id);
    if (s) ma_sound_start(&s->sound);
}
void eng_bgm_set_loop(ENG_Audio* a, ENG_SoundID id, bool loop) {
    SoundSlot* s = bgm_slot(a, id);
    if (s) ma_sound_set_looping(&s->sound, loop ? MA_TRUE : MA_FALSE);
}
void eng_bgm_set_volume(ENG_Audio* a, ENG_SoundID id, float vol) {
    SoundSlot* s = bgm_slot(a, id);
    if (s) ma_sound_set_volume(&s->sound, vol);
}
void eng_bgm_seek(ENG_Audio* a, ENG_SoundID id, float seconds) {
    SoundSlot* s = bgm_slot(a, id);
    if (!s) return;
    ma_uint32 sr = ma_engine_get_sample_rate(&a->engine);
    ma_sound_seek_to_pcm_frame(&s->sound, (ma_uint64)(seconds * sr));
}
float eng_bgm_position(ENG_Audio* a, ENG_SoundID id) {
    SoundSlot* s = bgm_slot(a, id);
    if (!s) return 0.0f;
    float pos = 0.0f;
    ma_sound_get_cursor_in_seconds(&s->sound, &pos);
    return pos;
}
bool eng_bgm_is_playing(ENG_Audio* a, ENG_SoundID id) {
    SoundSlot* s = bgm_slot(a, id);
    return s && ma_sound_is_playing(&s->sound);
}
void eng_bgm_free(ENG_Audio* a, ENG_SoundID id) {
    SoundSlot* s = bgm_slot(a, id);
    if (s) { ma_sound_uninit(&s->sound); memset(s, 0, sizeof(*s)); }
}

/* ── SE ─────────────────────────────────────────────────*/
ENG_SoundID eng_se_load(ENG_Audio* a, const char* path) {
    if (!a || !path) return 0;
    for (int i = 0; i < ENG_MAX_SE; ++i) {
        if (!a->se[i].used) {
            ma_result r = ma_sound_init_from_file(
                &a->engine, path, 0, NULL, NULL, &a->se[i].sound);
            if (r != MA_SUCCESS) {
                fprintf(stderr, "[eng_audio] SE読込失敗 '%s': %s\n", path, ma_result_description(r));
                return 0;
            }
            a->se[i].used      = true;
            a->se[i].streaming = false;
            return (ENG_SoundID)(i + 1);
        }
    }
    fprintf(stderr, "[eng_audio] SEスロット満杯 (max=%d)\n", ENG_MAX_SE);
    return 0;
}

void eng_se_play(ENG_Audio* a, ENG_SoundID id) {
    SoundSlot* s = se_slot(a, id);
    if (!s) return;
    ma_sound_seek_to_pcm_frame(&s->sound, 0);
    ma_sound_start(&s->sound);
}
void eng_se_play_vol(ENG_Audio* a, ENG_SoundID id, float vol) {
    SoundSlot* s = se_slot(a, id);
    if (!s) return;
    ma_sound_set_volume(&s->sound, vol);
    ma_sound_seek_to_pcm_frame(&s->sound, 0);
    ma_sound_start(&s->sound);
}
void eng_se_stop(ENG_Audio* a, ENG_SoundID id) {
    SoundSlot* s = se_slot(a, id);
    if (s) { ma_sound_stop(&s->sound); ma_sound_seek_to_pcm_frame(&s->sound, 0); }
}
void eng_se_set_volume(ENG_Audio* a, ENG_SoundID id, float vol) {
    SoundSlot* s = se_slot(a, id);
    if (s) ma_sound_set_volume(&s->sound, vol);
}
void eng_se_set_pitch(ENG_Audio* a, ENG_SoundID id, float pitch) {
    SoundSlot* s = se_slot(a, id);
    if (s) ma_sound_set_pitch(&s->sound, pitch);
}
void eng_se_free(ENG_Audio* a, ENG_SoundID id) {
    SoundSlot* s = se_slot(a, id);
    if (s) { ma_sound_uninit(&s->sound); memset(s, 0, sizeof(*s)); }
}

/* ── グローバル ─────────────────────────────────────────*/
void eng_audio_set_master_volume(ENG_Audio* a, float vol) {
    if (a) ma_engine_set_volume(&a->engine, vol);
}
float eng_audio_get_master_volume(ENG_Audio* a) {
    /* miniaudio に getter がないため内部値を保持しない — 0を返す */
    (void)a;
    return 1.0f;
}
