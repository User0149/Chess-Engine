BUILD_DIR=docs
ENGINE_DIR=src/engine

.PHONY: all engine clean

all:
	emcc $(ENGINE_DIR)/*.cpp -lembind -o $(ENGINE_DIR)/engine.mjs -s MODULARIZE=1 -s EXPORT_ES6=1 && pnpm build

engine:
	emcc $(ENGINE_DIR)/*.cpp -lembind -o $(ENGINE_DIR)/engine.mjs -s MODULARIZE=1 -s EXPORT_ES6=1

clean:
	rm $(ENGINE_DIR)/engine.mjs $(ENGINE_DIR)/engine.wasm && rm -rf $(BUILD_DIR)
