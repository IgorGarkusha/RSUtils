MAKE_FILE=-f makefile.$(PLATFORM)

all:	s2_set_projection check_reflectance s2_utm_tile_finder tif2con l8_dn_conversion calc_index tcap stat_map_creator

clean: 
	cd ./src/s2_set_projection && $(MAKE) $(MAKE_FILE) clean
	cd ./src/check_reflectance && $(MAKE) $(MAKE_FILE) clean
	cd ./src/s2_utm_tile_finder && $(MAKE) $(MAKE_FILE) clean
	cd ./src/tif2con && $(MAKE) $(MAKE_FILE) clean
	cd ./src/l8_dn_conversion && $(MAKE) $(MAKE_FILE) clean
	cd ./src/calc_index && $(MAKE) $(MAKE_FILE) clean
	cd ./src/tcap && $(MAKE) $(MAKE_FILE) clean
	cd ./src/stat_map_creator && $(MAKE) $(MAKE_FILE) clean

s2_set_projection: 
	cd ./src/s2_set_projection && $(MAKE) $(MAKE_FILE) all

check_reflectance: 
	cd ./src/check_reflectance && $(MAKE) $(MAKE_FILE) all

s2_utm_tile_finder:
	cd ./src/s2_utm_tile_finder && $(MAKE) $(MAKE_FILE) all

tif2con:
	cd ./src/tif2con && $(MAKE) $(MAKE_FILE) all

l8_dn_conversion:
	cd ./src/l8_dn_conversion && $(MAKE) $(MAKE_FILE) all

calc_index:
	cd ./src/calc_index && $(MAKE) $(MAKE_FILE) all

tcap:
	cd ./src/tcap && $(MAKE) $(MAKE_FILE) all

stat_map_creator:
	cd ./src/stat_map_creator && $(MAKE) $(MAKE_FILE) all
