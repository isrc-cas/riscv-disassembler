#include <stdio.h>

#include "riscv-disas.h"

/**
 * XXX: CATUION:
 * This is a debug tool for v8 risc-v backend development.
 * just ignore the magic numbers, security holes, wrong coding practices
 * in this file.
 * No doc. Just read the toy code.
 *
 * Usage:
 *     
 *     cat your-v8-log-file.txt | ./build/bin/v8-dis > log-with-disasm.txt
 *     
 * */

//const int is_debug = 1;
const int is_debug = 0;
const size_t buf_capa = 10000;

int is_codedump(const char *line, size_t n) {
        if (!line || n <= 3)
                return 0;
        return line[0] == '0' && line[1] == 'x';
}

// FIXME: singleton, no reenter
const char* riscv_disasm(const int pc, const int inst) {
        static char dis_buf[1024];
	disasm_inst(dis_buf, sizeof(dis_buf), rv64, pc, inst);
        //snprintf(dis_buf, 1024 - 1, "0x%8x", opcode);
	//printf("0> %s, 17> %s, 18> %s, 19> %s\n", dis_buf, dis_buf+17, dis_buf+18, dis_buf+19);
        return dis_buf;
	// if you don't want the raw hex twice, add offset 18
        //return dis_buf + 18;
}

void disasm_and_append(char *buf, size_t n, size_t max) {
        int pc;
        char unused[32];
        char code[32] = { '0', 'x', };
        int opcode;
        int ret = sscanf(buf, "%x%s%s", &pc, (char *)unused, (char *)(code+2));
        int ret2 = sscanf(code, "%x", &opcode);
	if (is_debug) {
		printf("DEBUG: decode_and_append: ret = %2d / %2d, pc = %12x, "
		     "unused = >>%s<<, code >>%s<<, opcode = %12x\n",
		     ret, ret2, pc, unused, code, opcode);
		printf("DEBUG: buf, before\n\t%s\n", buf);
	}
        snprintf(buf + n - 1, max - n, "\t%s", riscv_disasm(pc, opcode));
	if (is_debug) {
		printf("DEBUG: buf, after\n\t%s\n", buf);
	}
}


int main() {
        char *buf = (char*)malloc(buf_capa + 1);
        if (!buf) return 1;
        while (1) {
                ssize_t n = getline(&buf, &buf_capa, stdin);
                if (n < 1) break;
                *(buf + n - 1) = '\0';

                if (is_codedump(buf, n))
                        disasm_and_append(buf, n, buf_capa);
		if (is_debug)
			printf("n = %4zu len(buf) = %4zu >>>%s<<<\n", n, strlen(buf), buf);
		printf("%s\n", buf);
        }
        return 0;
}
