BUILD_DIR=docs
ENGINE_DIR=src/engine

.PHONY: all engine clean

all:
	source ./emsdk/emsdk_env.sh && emcc $(ENGINE_DIR)/*.cpp -lembind -o $(ENGINE_DIR)/engine.mjs -s ALLOW_MEMORY_GROWTH=1 -s MODULARIZE=1 -s EXPORT_ES6=1 && pnpm build

engine:
	source ./emsdk/emsdk_env.sh && emcc $(ENGINE_DIR)/*.cpp -lembind -o $(ENGINE_DIR)/engine.mjs -s ALLOW_MEMORY_GROWTH=1 -s MODULARIZE=1 -s EXPORT_ES6=1

clean:
	rm $(ENGINE_DIR)/engine.mjs $(ENGINE_DIR)/engine.wasm && rm -r $(BUILD_DIR)
