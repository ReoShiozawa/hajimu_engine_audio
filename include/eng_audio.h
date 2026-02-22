/**
 * include/eng_audio.h — はじむ オーディオエンジン 公開 API
 *
 * BGM (ストリーミング再生) と SE (インメモリ再生) を提供する。
 * miniaudio をバックエンドに使用。
 *
 * Copyright (c) 2026 Reo Shiozawa — MIT License
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── 型定義 ─────────────────────────────────────────────*/
typedef struct ENG_Audio ENG_Audio;
typedef uint32_t ENG_SoundID; /* 0 = 無効 */

/* ── ライフサイクル ─────────────────────────────────────*/

/** 音声エンジン初期化。成功時はポインタ、失敗時は NULL を返す。 */
ENG_Audio* eng_audio_create(void);

/** 音声エンジンを破棄し全リソースを解放する。 */
void       eng_audio_destroy(ENG_Audio* a);

/* ── BGM (ストリーミング) ────────────────────────────────*/

/** ファイルをストリーミング読込。戻り値: SoundID (0=失敗) */
ENG_SoundID eng_bgm_load(ENG_Audio* a, const char* path);

/** BGM 再生開始。 */
void eng_bgm_play(ENG_Audio* a, ENG_SoundID id);

/** BGM 停止 (位置リセット)。 */
void eng_bgm_stop(ENG_Audio* a, ENG_SoundID id);

/** BGM 一時停止。 */
void eng_bgm_pause(ENG_Audio* a, ENG_SoundID id);

/** BGM 再開。 */
void eng_bgm_resume(ENG_Audio* a, ENG_SoundID id);

/** ループ設定。 */
void eng_bgm_set_loop(ENG_Audio* a, ENG_SoundID id, bool loop);

/** 音量設定 (0.0〜1.0)。 */
void eng_bgm_set_volume(ENG_Audio* a, ENG_SoundID id, float vol);

/** 再生位置設定 (秒)。 */
void eng_bgm_seek(ENG_Audio* a, ENG_SoundID id, float seconds);

/** 再生位置取得 (秒)。 */
float eng_bgm_position(ENG_Audio* a, ENG_SoundID id);

/** 再生中かどうか。 */
bool eng_bgm_is_playing(ENG_Audio* a, ENG_SoundID id);

/** BGM 解放。 */
void eng_bgm_free(ENG_Audio* a, ENG_SoundID id);

/* ── SE (インメモリ) ────────────────────────────────────*/

/** SE ファイルをメモリに読込。戻り値: SoundID (0=失敗) */
ENG_SoundID eng_se_load(ENG_Audio* a, const char* path);

/** SE 再生 (同一 ID を重ねて鳴らすことも可)。 */
void eng_se_play(ENG_Audio* a, ENG_SoundID id);

/** SE 再生 (音量指定)。 */
void eng_se_play_vol(ENG_Audio* a, ENG_SoundID id, float vol);

/** SE 停止 (全インスタンス)。 */
void eng_se_stop(ENG_Audio* a, ENG_SoundID id);

/** SE 音量設定。 */
void eng_se_set_volume(ENG_Audio* a, ENG_SoundID id, float vol);

/** SE ピッチ設定 (1.0=等倍)。 */
void eng_se_set_pitch(ENG_Audio* a, ENG_SoundID id, float pitch);

/** SE ループ設定。 */
void eng_se_set_loop(ENG_Audio* a, ENG_SoundID id, bool loop);

/** SE が再生中かどうか。 */
bool eng_se_is_playing(ENG_Audio* a, ENG_SoundID id);

/** SE 解放。 */
void eng_se_free(ENG_Audio* a, ENG_SoundID id);

/* ── グローバル ─────────────────────────────────────────*/

/** マスター音量設定 (0.0〜1.0)。 */
void eng_audio_set_master_volume(ENG_Audio* a, float vol);

/** マスター音量取得。 */
float eng_audio_get_master_volume(ENG_Audio* a);

/** SE の長さを秒単位で返す */
float eng_se_duration(ENG_Audio* a, ENG_SoundID id);

/* ── BGM ピッチ ─────────────────────────────────────────*/

/** BGM 再生速度 (ピッチ) 設定: 1.0=等倍, 0.5=半速, 2.0=倍速。 */
void eng_bgm_set_pitch(ENG_Audio* a, ENG_SoundID id, float pitch);

/* ── フェード ───────────────────────────────────────────*/

/**
 * BGM フェードイン: 音量 0→1 で再生開始。
 * @param duration  フェード秒数。
 */
void eng_bgm_fade_in(ENG_Audio* a, ENG_SoundID id, float duration);

/**
 * BGM フェードアウト: 現在音量→0。
 * @param duration  フェード秒数。
 */
void eng_bgm_fade_out(ENG_Audio* a, ENG_SoundID id, float duration);

/**
 * BGM クロスフェード: from をフェードアウトしつつ to をフェードイン。
 * to は先頭から再生開始される。
 */
void eng_bgm_crossfade(ENG_Audio* a, ENG_SoundID from_id, ENG_SoundID to_id, float duration);

/* ── パン (左右定位) ────────────────────────────────────*/

/** BGM パン設定: -1=左, 0=中央, 1=右。 */
void eng_bgm_set_pan(ENG_Audio* a, ENG_SoundID id, float pan);

/** SE パン設定: -1=左, 0=中央, 1=右。 */
void eng_se_set_pan(ENG_Audio* a, ENG_SoundID id, float pan);

/* ── BGM 長さ ───────────────────────────────────────────*/

/** BGM の全長 (秒) を返す。取得失敗時は 0。 */
float eng_bgm_duration(ENG_Audio* a, ENG_SoundID id);

#ifdef __cplusplus
}
#endif
