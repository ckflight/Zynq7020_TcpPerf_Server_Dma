# 2025-12-01T14:26:59.625511900
import vitis

client = vitis.create_client()
client.set_workspace(path="Zynq7020_Vitis")

platform = client.get_component(name="zynq_perf")
status = platform.build()

comp = client.get_component(name="dma_ddr_interrupt_benchmark")
comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

status = comp.clean()

comp.build()

vitis.dispose()

