# netbsd_exporter

The netbsd_exporter retrieves system metrics such as disk I/O, network I/O, RAM and filesystem usage, as well as CPU load from the running system and exposes them in the format of Prometheus metrics. It is designed to be integrated into inetd, providing a lightweight, NetBSD-focused alternative to the node_exporter.

# example

```
netbsd_fs_size_bytes{device="dk0",mountpoint="/"} 14573516800
netbsd_fs_used_bytes{device="dk0",mountpoint="/"} 8648296448
netbsd_fs_free_bytes{device="dk0",mountpoint="/"} 5196546048
netbsd_load1 0.000000
netbsd_load5 0.000000
netbsd_load15 0.000000
netbsd_netif_rx_bytes{interface="xennet0"} 2048990748
netbsd_netif_tx_bytes{interface="xennet0"} 98329050
netbsd_netif_errors{interface="xennet0"} 0
netbsd_netif_rx_bytes{interface="lo0"} 12998
netbsd_netif_tx_bytes{interface="lo0"} 12998
netbsd_netif_errors{interface="lo0"} 0
netbsd_mem_size_bytes 249638912
netbsd_mem_free_bytes 23531520
netbsd_mem_active_bytes 79384576
netbsd_mem_inactive_bytes 48852992
netbsd_mem_paging_bytes 0
netbsd_mem_wired_bytes 16723968
netbsd_mem_swap_size_bytes 2147442688
netbsd_mem_swap_used_bytes 6533120
netbsd_dk_read_bytes{device="xbd0"} 307225201664
netbsd_dk_write_bytes{device="xbd0"} 33737978368
netbsd_dk_read_bytes{device="dk0"} 305721165824
netbsd_dk_write_bytes{device="dk0"} 32393216512
netbsd_dk_read_bytes{device="dk1"} 1503978496
netbsd_dk_write_bytes{device="dk1"} 1344761856
```