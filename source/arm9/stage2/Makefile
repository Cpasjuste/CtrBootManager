.PHONY : all screen_init stage2

TARGET = stage2

all : screen_init stage2

screen_init:
	@[ -d payload/data ] || mkdir -p payload/data
	$(MAKE) -C screen_init
	@cp screen_init/screen_init.bin payload/data

stage2: screen_init
	@cp screen_init/screen_init.bin payload/data
	@$(MAKE) -C payload
	@[ -d data ] || mkdir data
	@cp payload/payload.bin data/stage2.bin

clean:
	@echo clean...
	@$(MAKE) -C screen_init clean
	@$(MAKE) -C payload clean
