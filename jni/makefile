activity=andbeyond

all:../assets/assets ../assets/uiassets ../assets/atlas ../assets/atlas_light
	NDK_DEBUG=1 ndk-build -C ..
	ant debug -f ../build.xml
	adb install -r $(CURDIR)/../bin/$(activity)-debug.apk
	adb shell am start -n joshwinter.$(activity)/android.app.NativeActivity

../assets/assets:../assets_local/build.txt ../assets_local/*
	cd ../assets_local && ./convert.sh
	cd ../assets_local/ && pack --build

../assets/uiassets:../assets_local/uibuild.txt ../assets_local/*
	cd ../assets_local && ./convert.sh
	cd ../assets_local/ && pack --build uibuild.txt

../assets/atlas:../assets_local/atlas.txt ../assets_local/*
	cd ../assets_local && ./convert.sh
	cd ../assets_local && atlas ../assets/atlas `cat atlas.txt`

../assets/atlas_light:../assets_local/atlas_light.txt ../assets_local/*
	cd ../assets_local && ./convert.sh
	cd ../assets_local && atlas ../assets/atlas_light `cat atlas_light.txt`

#../assets/aassets:../assets_local/apack.txt ../assets_local/*.ogg
#	cd ../assets_local/ && apack --build
