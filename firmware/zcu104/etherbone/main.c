#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pl_mmap.h"
#include "udp_server.h"
#include "etherbone.h"
#include "cmdline.h"
#include "debug.h"

void pl_mem_write(void *_pl_mem, uint32_t addr, uint32_t value) {
    struct pl_mmap *pl_mem = _pl_mem;
    uint32_t *mmap_addr = pl_mem->mem + addr;
    dbg_printf("0x%08x <= 0x%08x\n", addr, value);
    *mmap_addr = value;
}

uint32_t pl_mem_read(void *_pl_mem, uint32_t addr) {
    struct pl_mmap *pl_mem = _pl_mem;
    uint32_t *mmap_addr = pl_mem->mem + addr;
    uint32_t value = *mmap_addr;
    dbg_printf("0x%08x => 0x%08x\n", addr, value);
    return value;
}

int run_server(struct pl_mmap *pl_mem) {
    struct etherbone_memory_handlers mem = {
        .arg = pl_mem,
        .write = &pl_mem_write,
        .read = &pl_mem_read,
    };
    return udp_server_run(&mem, (udp_server_callback) &etherbone_callback,
            cmdline_args.udp_port, cmdline_args.server_buf_size);
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);

    struct pl_mmap pl_mem;
    if (pl_mmap_open(&pl_mem, cmdline_args.pl_mem_base, cmdline_args.pl_mem_size) < 0) {
        return 1;
    }

    int ret = run_server(&pl_mem);

    pl_mmap_close(&pl_mem);

    return ret;
}
