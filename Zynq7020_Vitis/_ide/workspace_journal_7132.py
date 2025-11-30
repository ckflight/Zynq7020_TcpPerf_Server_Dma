# 2025-11-25T08:52:56.884752300
import vitis

client = vitis.create_client()
client.set_workspace(path="ZC702_Zynq_")

platform = client.get_component(name="zynq_arm_perf")
status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../../../FPGA_Workspace/VIVADO_PROJECTS/ZC702_EVK_Test/top_module.xsa")

status = platform.build()

status = platform.build()

comp = client.get_component(name="tcp_perf_echo_server")
comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../../../FPGA_Workspace/VIVADO_PROJECTS/ZC702_EVK_Test/top_module.xsa")

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../../../FPGA_Workspace/VIVADO_PROJECTS/ZC702_EVK_Test/top_module.xsa")

status = platform.build()

comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../../../FPGA_Workspace/VIVADO_PROJECTS/ZC702_EVK_Test/top_module.xsa")

status = platform.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../../../FPGA_Workspace/VIVADO_PROJECTS/ZC702_EVK_Test/top_module.xsa")

status = platform.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

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

