# 2025-12-01T14:21:20.554542300
import vitis

client = vitis.create_client()
client.set_workspace(path="Zynq7020_Vitis")

platform = client.get_component(name="zynq_perf")
status = platform.build()

status = platform.build()

vitis.dispose()

