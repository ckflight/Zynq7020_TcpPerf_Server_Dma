# 2025-12-01T15:25:05.120769600
import vitis

client = vitis.create_client()
client.set_workspace(path="Zynq7020_Vitis")

platform = client.get_component(name="zynq_perf")
status = platform.build()

comp = client.get_component(name="dma_ddr_interrupt_benchmark")
comp.build()

status = platform.build()

status = comp.clean()

status = comp.clean()

status = platform.build()

comp.build()

comp = client.get_component(name="lwip_tcp_perf_server")
comp.build()

comp.build()

comp.build()

status = comp.clean()

comp.build()

