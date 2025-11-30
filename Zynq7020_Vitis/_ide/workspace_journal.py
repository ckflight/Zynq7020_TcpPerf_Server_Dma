# 2025-11-25T12:50:05.149762300
import vitis

client = vitis.create_client()
client.set_workspace(path="ZC702_Zynq")

platform = client.get_component(name="zynq_arm_perf")
status = platform.build()

status = platform.build()

comp = client.get_component(name="tcp_perf_echo_server")
comp.build()

