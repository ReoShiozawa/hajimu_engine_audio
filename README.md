# hajimu_engine_audio

> **はじむ言語**向け BGM + SE オーディオプラグイン

[miniaudio](https://miniaud.io/) をバックエンドに使用したシングルヘッダーオーディオエンジンです。  
BGM ストリーミングと SE インメモリ再生を両立します。

---

## 機能

| カテゴリ | 内容 |
|---|---|
| BGM | MP3 / OGG / WAV / FLAC ストリーミング再生、ループ、音量、シーク |
| SE | WAV / MP3 インメモリ再生、音量・ピッチ指定 |
| グローバル | マスター音量設定 |

## 依存ライブラリ

| ライブラリ | 用途 | 入手方法 |
|---|---|---|
| miniaudio | オーディオバックエンド | `make vendor` |

## インストール

### 推奨: はじむパッケージマネージャー

```bash
hajimu パッケージ 追加 ReoShiozawa/hajimu_engine_audio
```

### ソースからビルド

```bash
git clone https://github.com/ReoShiozawa/hajimu_engine_audio.git
cd hajimu_engine_audio

make vendor   # miniaudio.h をダウンロード
make          # → build/engine_audio.hjp
make install  # → ~/.hajimu/plugins/engine_audio/
```

## クイックスタート

```jp
プラグイン読込("engine_audio")

音声初期化()

BGM = 音楽読込("BGM.ogg")
音楽音量設定(BGM, 0.8)
音楽ループ設定(BGM, 真)
音楽再生(BGM)

効果音 = SE読込("jump.wav")
SE再生(効果音)

# ... ゲームループ ...

音声終了()
```

## API リファレンス

### ライフサイクル

| 関数 | 説明 |
|---|---|
| `音声初期化()` | オーディオエンジン起動 |
| `音声終了()` | 全サウンド解放・シャットダウン |

### BGM（ストリーミング再生）

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `音楽読込(パス)` | str | int | 0=失敗 |
| `音楽再生(id)` | int | null | 再生開始 |
| `音楽停止(id)` | int | null | 停止+先頭へ |
| `音楽一時停止(id)` | int | null | 一時停止 |
| `音楽再開(id)` | int | null | 再開 |
| `音楽ループ設定(id, bool)` | int, bool | null | ループ設定 |
| `音楽音量設定(id, vol)` | int, float | null | 0.0〜1.0 |
| `音楽位置設定(id, 秒)` | int, float | null | シーク |
| `音楽位置取得(id)` | int | float | 現在位置(秒) |
| `音楽再生中(id)` | int | bool | 再生中かどうか |
| `音楽削除(id)` | int | null | 解放 |

### SE（インメモリ再生）

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `SE読込(パス)` | str | int | 0=失敗 |
| `SE再生(id)` | int | null | 先頭から再生 |
| `SE再生音量(id, vol)` | int, float | null | 音量付き再生 |
| `SE停止(id)` | int | null | 停止 |
| `SE音量設定(id, vol)` | int, float | null | 音量設定 |
| `SEピッチ設定(id, pitch)` | int, float | null | 1.0=等倍 |
| `SE削除(id)` | int | null | 解放 |

### グローバル

| 関数 | 引数 | 説明 |
|---|---|---|
| `主音量設定(vol)` | float | マスター音量 (0.0〜1.0) |

## 制限

| 項目 | 上限 |
|---|---|
| BGM スロット | 16 |
| SE スロット | 64 |

## サンプル

- [examples/](examples/) — BGM ループ再生、SE 多重再生デモ

---

## ライセンス

MIT License — Copyright (c) 2026 Reo Shiozawa  
miniaudio: MIT/Public Domain — Copyright (c) David Reid
