#!/bin/bash

# if no files in /assets_local are .png, this script need not be run. convert is imagemagick
for f in *.png; do
	if [ "$f" == '*.png' ]; then
		exit 0
	fi
	rm -rf ${f%%.png}.tga
	convert $f -compress none ${f%%.png}.tga
done

rm *.png

# combine some textures into sprite sheets
# platforms
convert platform_normal.tga platform_sliding.tga platform_disappearing.tga platform_space_normal.tga platform_space_sliding.tga platform_space_disappearing.tga +append platform.tga

# player
convert player_normal.tga player_midblink.tga player_blink.tga +append player.tga

# electro
convert electro_1.tga electro_2.tga electro_3.tga electro_4.tga electro_5.tga +append electro.tga
# space electro
convert electro_space_1.tga electro_space_2.tga electro_space_3.tga electro_space_4.tga electro_space_5.tga +append electro_space.tga

# smasher
convert smasher_normal.tga smasher_space.tga +append smasher.tga

# saw
convert obstacle_normal.tga obstacle_space.tga +append obstacle.tga
