cmake_minimum_required(VERSION 3.4.1)

# set(nativeSourceDir src/main/cpp/)
set(meshSourceDir ../TelinkSigMeshLib/src/main/cpp/sig_mesh_src_code)

ADD_DEFINITIONS(-DANDROID_APP_ENABLE=1)
ADD_DEFINITIONS(-DWIN32=1)

add_library( # Sets the name of the library.
             mesh_lib_static

             STATIC

             # Provides a relative path to your source file(s).
             # mesh source
            # ${nativeSourceDir}/mesh_lib.c

        	${meshSourceDir}/ble_lt_mesh/proj_lib/mesh_crypto/aes_att.c
            ${meshSourceDir}/ble_lt_mesh/proj_lib/mesh_crypto/Le_crypto.c
            ${meshSourceDir}/ble_lt_mesh/proj_lib/mesh_crypto/mesh_crypto.c
            ${meshSourceDir}/ble_lt_mesh/proj_lib/mesh_crypto/mesh_md5.c
            ${meshSourceDir}/ble_lt_mesh/proj_lib/mesh_crypto/sha256_telink.c
			${meshSourceDir}/ble_lt_mesh/proj_lib/sig_mesh/app_mesh.c

			${meshSourceDir}/ble_lt_mesh/vendor/common/app_beacon.c
			${meshSourceDir}/ble_lt_mesh/vendor/common/app_provison.c
			${meshSourceDir}/ble_lt_mesh/vendor/common/app_heartbeat.c
			${meshSourceDir}/ble_lt_mesh/vendor/common/app_health.c
			${meshSourceDir}/ble_lt_mesh/vendor/common/app_proxy.c

            ${meshSourceDir}/ble_lt_mesh/vendor/common/cmd_interface.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/lighting_model_HSL.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/lighting_model_LC.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/lighting_model_xyl.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/Lighting_model.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/light.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/vendor_model.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/time_model.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/scheduler.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/scene.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/mesh_node.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/mesh_common.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/mesh_ota.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/system_time.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/factory_reset.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/generic_model.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/sensors_model.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/mesh_property.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/mesh_fn.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/mesh_lpn.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/remote_prov.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/fast_provision_model.c
            ${meshSourceDir}/ble_lt_mesh/vendor/common/user_proc.c
            ${meshSourceDir}/ble_lt_mesh/proj/common/tstring.c
            ${meshSourceDir}/ble_lt_mesh/proj/common/tutility.c
            ${meshSourceDir}/reference/tl_bulk/lib_file/gatt_provision.c
            ${meshSourceDir}/reference/tl_bulk/lib_file/hw_fun.c
            ${meshSourceDir}/reference/tl_bulk/lib_file/host_fifo.c
             )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.
