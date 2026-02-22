BUILD_DIR := build
STB_URL   := https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h
VENDOR    := vendor/miniaudio.h

.PHONY: all vendor clean install

all: vendor
	@mkdir -p $(BUILD_DIR)
	@cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release -Wno-dev > /dev/null
	@echo "▶ ビルド中..."
	@cmake --build $(BUILD_DIR)
	@echo "✅ ビルド完了: $(BUILD_DIR)/engine_audio.hjp"

vendor: $(VENDOR)

$(VENDOR):
	@mkdir -p vendor
	@echo "⬇ miniaudio.h をダウンロード中..."
	@curl -fsSL $(STB_URL) -o $@
	@echo "✅ miniaudio.h"

install: all
	@mkdir -p ~/.hajimu/plugins/engine_audio
	@cp $(BUILD_DIR)/engine_audio.hjp ~/.hajimu/plugins/engine_audio/
	@echo "✅ インストール完了: ~/.hajimu/plugins/engine_audio"

clean:
	@rm -rf $(BUILD_DIR)
	@echo "🗑  クリーン完了"
