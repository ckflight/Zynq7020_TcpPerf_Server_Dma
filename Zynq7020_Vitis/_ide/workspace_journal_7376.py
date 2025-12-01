# 2025-12-01T11:31:36.913792800
import vitis

client = vitis.create_client()
client.set_workspace(path="Zynq7020_Vitis")

platform = client.create_platform_component(name = "zynq_perf",hw_design = "$COMPONENT_LOCATION/../../top_module.xsa",os = "standalone",cpu = "ps7_cortexa9_0",domain_name = "standalone_ps7_cortexa9_0",compiler = "gcc")

platform = client.get_component(name="zynq_perf")
status = platform.build()

domain = platform.get_domain(name="standalone_ps7_cortexa9_0")

status = domain.set_config(option = "lib", param = "XILTIMER_en_interval_timer", value = "true", lib_name="xiltimer")

status = domain.set_config(option = "lib", param = "XILTIMER_tick_timer", value = "ps7_ttc_0", lib_name="xiltimer")

status = platform.build()

status = domain.set_lib(lib_name="lwip220", path="C:\Xilinx\2025.2\Vitis\data\embeddedsw\ThirdParty\sw_services\lwip220_v1_3")

status = platform.build()

status = platform.build()

