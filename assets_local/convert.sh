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
convert platform_normal.tga platform_sliding.tga platform_disappearing.tga +append platform.tga

convert player_normal.tga player_midblink.tga player_blink.tga +append player.tga

convert electro_1.tga electro_2.tga electro_3.tga electro_4.tga electro_5.tga +append electro.tga
