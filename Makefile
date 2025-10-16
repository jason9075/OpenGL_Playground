# Makefile

# 預設目標
all: build run

dev:
	fd . | entr -r sh -c 'make build && make run'

# 建立目標
build:
	@echo "執行 build.sh..."
	bash build.sh

# 執行目標
run:
	@echo "執行 playground.app..."
	./playground.app

# 清理目標（如果需要）
clean:
	@echo "清理建置檔案..."
	# 在這裡添加清理指令，例如 rm -rf build/

.PHONY: all build run clean
