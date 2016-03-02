.PHONY : all stage2

TARGET = stage2

all : stage2

stage2:
	@$(MAKE) -C payload
	@[ -d data ] || mkdir data
	@cp payload/payload.bin data/stage2.bin

clean:
	@echo clean...
	@$(MAKE) -C payload clean
