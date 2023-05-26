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
#ifndef NETBSD_EXPORTER_H
#define NETBSD_EXPORTER_H

const char *program_name = "netbsd_exporter";

int no_http_header = 0;
int use_syslog = 0;

void print_filesystem_metric(const char*, const char*, const char*, long );

void print_disk_io_metric(const char*, long long unsigned int, long long unsigned int );

void print_load_metric(const char*, double );

void print_network_metric(const char*, unsigned long long, unsigned long long, unsigned long long );

void print_memory_metric(const char*, long );

void retrieve_disk_space_metrics();

void retrieve_cpu_load_metrics();

void retrieve_network_card_metrics();

void retrieve_memory_metrics();

void retrieve_disk_io_metrics();

void log_message(int, const char* );

void print_help();
#endif
