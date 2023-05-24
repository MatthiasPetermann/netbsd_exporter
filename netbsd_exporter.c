/*-
 * Copyright (c) 2023 Matthias Petermann <mp@petermann-it.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* This program retrieves system metrics such as disk I/O, network I/O,
 * RAM and filesystem usage, as well as CPU load from the running system
 * and exposes them in the format of Prometheus metrics. It is designed
 * to be integrated into inetd, providing a lightweight, NetBSD-focused
 * alternative to the node_exporter.
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/statvfs.h>
#include <sys/sysctl.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <sys/iostat.h>
#include <syslog.h>
#include <unistd.h>
#include <uvm/uvm_extern.h>

/* sysctl hw.drivestats buffer. */
static struct io_sysctl *drives = NULL;


void print_filesystem_metric(const char* metric, const char* device, const char* mountpoint, long value) {
    printf("os_filesystem_%s_bytes{device=\"%s\",mountpoint=\"%s\"} %ld\n", metric, device, mountpoint, value);
}

void print_disk_io_metric(const char* device, long long unsigned int rbytes, long long unsigned int wbytes) {
    printf("os_disk_read_bytes{device=\"%s\"} %llu\n", device, rbytes);
    printf("os_disk_write_bytes{device=\"%s\"} %llu\n", device, wbytes);
}

void print_load_metric(const char* metric, double value) {
    printf("os_load%s %lf\n", metric, value);
}

void print_network_metric(const char* interface, unsigned long long rxbytes, unsigned long long txbytes, unsigned long long errors) {
    printf("os_network_rx_bytes{interface=\"%s\"} %llu\n", interface, rxbytes);
    printf("os_network_tx_bytes{interface=\"%s\"} %llu\n", interface, txbytes);
    printf("os_network_errors{interface=\"%s\"} %llu\n", interface, errors);
}

void print_memory_metric(const char* metric, long value) {
    printf("os_memory_%s_bytes %ld\n", metric, value);
}

int main() {
    openlog("netbsd_exporter", LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Das Programm wurde aufgerufen.");

    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: text/plain\r\n\r\n");

    int mib[3];
    size_t size;
    int i;

    // Disk Space Metrics
    struct statvfs* fsinfo;
    int numfs;

    numfs = getmntinfo(&fsinfo, MNT_WAIT);
    if (numfs == -1) {
        syslog(LOG_ERR, "getmntinfo failed");
        return 1;
    }

    for (i = 0; i < numfs; i++) {
	// Include only FFS filesystems in metrics
	if(strcmp(fsinfo[i].f_fstypename,"ffs") != 0) {
	    continue;
        }
	char *token;
	token = strtok(fsinfo[i].f_mntfromname,"/");
        if(token != NULL) { 
            token = strtok(NULL, "/");
	}

	if(token == NULL) {
	    syslog(LOG_ERR, "parsing dev failed");
	    continue;
        }
        print_filesystem_metric("size", token, fsinfo[i].f_mntonname, fsinfo[i].f_blocks * fsinfo[i].f_frsize);
        print_filesystem_metric("used", token, fsinfo[i].f_mntonname, (fsinfo[i].f_blocks - fsinfo[i].f_bfree) * fsinfo[i].f_frsize);
        print_filesystem_metric("free", token, fsinfo[i].f_mntonname, fsinfo[i].f_bavail * fsinfo[i].f_frsize);
    }

    // CPU Load Metrics
    double loadavg[3];
    if(getloadavg(loadavg,3) != -1) {
        print_load_metric("1", loadavg[0]);
        print_load_metric("5", loadavg[1]);
        print_load_metric("15", loadavg[2]);
    } else {
        syslog(LOG_ERR, "loadavg failed.");
    }

    // Network Card Metrics
    struct ifaddrs* ifap, *ifa;
    if (getifaddrs(&ifap) == 0) {
        for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL || !(ifa->ifa_flags & IFF_UP))
                continue;

            if (ifa->ifa_addr->sa_family == AF_LINK) {
                struct if_data* ifd = (struct if_data*)ifa->ifa_data;
                print_network_metric(ifa->ifa_name, ifd->ifi_ibytes, ifd->ifi_obytes, ifd->ifi_ierrors + ifd->ifi_oerrors);
            }
        }
        freeifaddrs(ifap);
    } else {
        syslog(LOG_ERR, "Fehler beim Abrufen der Netzwerkstatistiken.");
    }

    // Memory Metrics
    int pagesize;
    struct uvmexp_sysctl u;

    // getpagesize() is obsolete, see manpage
    pagesize = sysconf(_SC_PAGESIZE);
    mib[0] = CTL_VM;
    mib[1] = VM_UVMEXP2;
    size = sizeof(struct uvmexp_sysctl);
    if (sysctl(mib, 2, &u, &size, NULL, 0) == -1) {
        syslog(LOG_ERR, "sysctl failed."); 
        return 1;	
    }
    print_memory_metric("size", u.npages * pagesize);
    print_memory_metric("free", u.free * pagesize);
    print_memory_metric("active", u.active * pagesize);
    print_memory_metric("inactive", u.inactive * pagesize);
    print_memory_metric("paging", u.paging * pagesize);
    print_memory_metric("wired", u.wired * pagesize);
    print_memory_metric("swap_size", u.swpages * pagesize);
    print_memory_metric("swap_used", u.swpginuse * pagesize);

    // Disk IO
    int ndrive;
    mib[0] = CTL_HW;
    mib[1] = HW_IOSTATS;
    mib[2] = sizeof(struct io_sysctl);
    if (sysctl(mib,3 , NULL, &size, NULL, 0) == -1) {
        syslog(LOG_ERR, "No drives found"); 
        return 1;	
    }

    ndrive = size / sizeof(struct io_stats);
    if (size == 0) {
        syslog(LOG_ERR, "No drives attached.");
    } else {
        drives = (struct io_sysctl *)malloc(size);
        if (drives == NULL) {
            syslog(LOG_ERR,"Memory allocation failure.");
        }

        /* Read the drive names and set initial selection. */
        mib[0] = CTL_HW;                /* Should be still set from */
        mib[1] = HW_IOSTATS;            /* ... above, but be safe... */
        mib[2] = sizeof(struct io_sysctl);
        if (sysctl(mib, 3, drives, &size, NULL, 0) == -1) {
            syslog(LOG_ERR, "sysctl hw.iostats failed");
        }
        ndrive = size / sizeof(struct io_sysctl);
        for (i = 0; i < ndrive; i++) {
            print_disk_io_metric(drives[i].name, drives[i].rbytes, drives[i].wbytes);
        }
    }

    closelog();
    return 0;
}
