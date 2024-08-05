TARGET = funkin
TYPE = ps-exe

SRCS = src/main.c \
	   \
       src/stages/default/default.c \
	   \
       src/scenes/menu/menu.c \
       src/scenes/menu/options.c \
	   \
       src/scenes/stage/characters/speaker.c \
       src/scenes/stage/characters/chardef.c \
       src/scenes/stage/characters/gfdef.c \
       src/scenes/stage/characters/playerdef.c \
	   \
	   src/events/event.c \
	   src/fonts/font.c \
	   src/psx/archive.c \
	   src/psx/audio.c \
	   src/psx/gfx.c \
	   src/psx/io.c \
	   src/psx/loadscr.c \
	   src/psx/movie.c \
	   src/psx/mutil.c \
	   src/psx/pad.c \
	   src/psx/psx.c \
	   src/psx/random.c \
	   src/psx/save.c \
	   src/psx/sound.c \
	   src/psx/timer.c \
	   src/psx/trans.c \
	   src/scenes/stage/animation.c \
	   src/scenes/stage/character.c \
	   src/scenes/stage/debug.c \
	   src/scenes/stage/gameover.c \
	   src/scenes/stage/object.c \
	   src/scenes/stage/object/combo.c \
	   src/scenes/stage/object/splash.c \
	   src/scenes/stage/pause.c \
	   src/scenes/stage/stage.c \
       mips/common/crt0/crt0.s

CPPFLAGS += -Wall -Wextra -pedantic -mno-check-zero-division
LDFLAGS += -Wl,--start-group
# TODO: remove unused libraries
LDFLAGS += -lapi
#LDFLAGS += -lc
LDFLAGS += -lc2
LDFLAGS += -lcard
LDFLAGS += -lcd
#LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
#LDFLAGS += -lgs
#LDFLAGS += -lgte
#LDFLAGS += -lgun
#LDFLAGS += -lhmd
#LDFLAGS += -lmath
LDFLAGS += -lmcrd
#LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
#LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
#LDFLAGS += -ltap
LDFLAGS += -flto -Wl,--end-group

include mips/common.mk
