set(COMPONENT_SRCS
	"app_mesh.c"
	"../../../sdk_src/utils/ble_qiot_utils_hmac256.c"
	"../../../sdk_src/utils/ble_qiot_utils_sha256.c"
	"../../../sdk_src/utils/ble_qiot_utils_base64.c"
	"../../../sdk_src/utils/ble_qiot_utils_log.c"
	"../../../sdk_src/core/ble_qiot_utils_mesh.c"
	"../../../hal/ble_qiot_export.c"
	"./hal/ble_qiot_import.c"
	"../../../data_template/ble_qiot_template.c")
set(COMPONENT_ADD_INCLUDEDIRS "." "../../../include" "../../../cfg" "../../../data_template" "../../../sdk_src/include")

register_component()
