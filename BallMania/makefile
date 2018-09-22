DIR = c:/gba_studio
include $(DIR)/system/master.mak



 ####################
# GBA ROM Image Name #
 ####################

PROGNAME="BallMania"

 #########################################
# Set a list of files you want to compile #
 #########################################

OFILES += main.o myLib.o affine.o cosLUT.o sinLUT.o

#Sounds
OFILES += game_music.raw.o bounce_sound.raw.o

#Images
OFILES += spritemap.raw.o
OFILES += level1_background.raw.o level1_collisionmap.raw.o
OFILES += level2_background.raw.o level2_collisionmap.raw.o
OFILES += splashscreen.raw.o victoryscreen.raw.o



 #########################################################
# Compile using Krawall (sound stuff, don't mess with it) #
 #########################################################
 
USE_KRAWALL=no
KRAWALL_IS_REGISTERED=no
KRAWALL_FILES=



 ######################################
# Standard Makefile targets start here #
 ######################################
all : $(PROGNAME).$(EXT) clean



 ##############################################
# Most Makefile targets are predefined for you,#
# such as vba, clean ... in the following file #
 ##############################################

include $(DIR)/system/standard-targets.mak



 #####################################
# Custom  Makefile targets start here #
 #####################################

gfx: makefile




