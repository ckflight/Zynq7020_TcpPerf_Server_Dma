# 2025-11-24T14:15:07.915828100
import vitis

client = vitis.create_client()
client.set_workspace(path="ZC702_Zynq_")

platform = client.create_platform_component(name = "zynq_arm_perf",hw_design = "$COMPONENT_LOCATION/../../../../FPGA_Workspace/VIVADO_PROJECTS/ZC702_EVK_Test/top_module.xsa",os = "standalone",cpu = "ps7_cortexa9_0",domain_name = "standalone_ps7_cortexa9_0",compiler = "gcc")

platform = client.get_component(name="zynq_arm_perf")
domain = platform.get_domain(name="standalone_ps7_cortexa9_0")

status = domain.set_lib(lib_name="lwip220", path="C:\Xilinx\2025.2\Vitis\data\embeddedsw\ThirdParty\sw_services\lwip220_v1_3")

status = platform.build()

status = domain.set_config(option = "lib", param = "lwip220_dhcp", value = "true", lib_name="lwip220")

status = domain.set_config(option = "lib", param = "lwip220_lwip_dhcp_does_acd_check", value = "true", lib_name="lwip220")

status = domain.set_config(option = "lib", param = "lwip220_mem_size", value = "524288", lib_name="lwip220")

status = domain.set_config(option = "lib", param = "lwip220_memp_n_pbuf", value = "1024", lib_name="lwip220")

status = domain.set_config(option = "lib", param = "lwip220_memp_n_tcp_seg", value = "1024", lib_name="lwip220")

status = domain.set_config(option = "lib", param = "lwip220_n_rx_descriptors", value = "512", lib_name="lwip220")

status = domain.set_config(option = "lib", param = "lwip220_n_tx_descriptors", value = "512", lib_name="lwip220")

status = domain.set_config(option = "lib", param = "lwip220_pbuf_pool_size", value = "16384", lib_name="lwip220")

status = domain.set_config(option = "lib", param = "lwip220_tcp_snd_buf", value = "65535", lib_name="lwip220")

status = domain.set_config(option = "lib", param = "lwip220_tcp_wnd", value = "65535", lib_name="lwip220")

status = platform.build()

status = domain.set_config(option = "lib", param = "XILTIMER_en_interval_timer", value = "true", lib_name="xiltimer")

status = platform.build()

comp = client.create_app_component(name="lwip_tcp_perf_server",platform = "$COMPONENT_LOCATION/../zynq_arm_perf/export/zynq_arm_perf/zynq_arm_perf.xpfm",domain = "standalone_ps7_cortexa9_0",template = "lwip_tcp_perf_server")

status = platform.build()

comp = client.get_component(name="lwip_tcp_perf_server")
comp.build()

comp = client.create_app_component(name="tcp_server_echo_perf",platform = "$COMPONENT_LOCATION/../zynq_arm_perf/export/zynq_arm_perf/zynq_arm_perf.xpfm",domain = "standalone_ps7_cortexa9_0",template = "hello_world")

status = platform.build()

comp = client.get_component(name="tcp_server_echo_perf")
comp.build()

status = platform.build()

comp.build()

client.delete_component(name="tcp_server_echo_perf")

comp = client.create_app_component(name="tcp_perf_echo_server",platform = "$COMPONENT_LOCATION/../zynq_arm_perf/export/zynq_arm_perf/zynq_arm_perf.xpfm",domain = "standalone_ps7_cortexa9_0",template = "lwip_tcp_perf_server")

status = platform.build()

comp = client.get_component(name="tcp_perf_echo_server")
comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../../../FPGA_Workspace/VIVADO_PROJECTS/ZC702_EVK_Test/top_module.xsa")

status = platform.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

status = platform.build()

comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../../../FPGA_Workspace/VIVADO_PROJECTS/ZC702_EVK_Test/top_module.xsa")

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

