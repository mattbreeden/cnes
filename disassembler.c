#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#define HEADER_SIZE 16
#define ROM_OFFSET 0x8000
uint8_t *buf;


void print_instruction(size_t bytes_read, char *instruction, char *operand, ...)
{
    printf("| ");
    if (bytes_read == 1) printf("%02x      ", *(buf - 1));
    if (bytes_read == 2) printf("%02x %02x   ", *(buf - 2), *(buf - 1));
    if (bytes_read == 3) printf("%02x %02x %02x", *(buf - 3), *(buf - 2), *(buf - 1));
    printf(" |   ");

    /* TODO: Figure out actual max */
    static char operand_buf[32];
    va_list args;
    va_start(args, operand);
    if (operand) {
        vsprintf(operand_buf, operand, args);
    } else {
        operand_buf[0] = '\0';
    }
    va_end(args);
    printf("%-4s %s\n", instruction, operand_buf);
}


int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <file_to_disassemble>\n", argv[0]);
        exit(1);
    }

    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        perror("Error opening file");
        exit(errno);
    }

    if (fseek(fp, 0l, SEEK_END) != 0) {
        printf("Error finding end of file");
        fclose(fp);
        exit(ferror(fp));
    }
    size_t file_size = ftell(fp) - HEADER_SIZE;
    if (file_size > 0xFFFF) {
        printf("Invalid file: too large\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    /* TODO: parse headers */
    fseek(fp, HEADER_SIZE, SEEK_SET);

    buf = malloc(file_size);
    size_t bytes_read = fread(buf, sizeof(*buf), file_size, fp);
    if (bytes_read != file_size) {
        printf("Error reading in file contents");
        fclose(fp);
        exit(ferror(fp));
    }

    uint8_t *buf_start = buf;
    uint8_t *buf_end = &buf[file_size];

    for(;;) {
        /* FIXME: this behavior is prone to errors
           Instructions can be up to 3 bytes long which might flow past
           the end of the buffer */
        if (buf >= buf_end) {
            break;
        }

        printf("0x%04x ", (uint16_t)(buf - buf_start));

        switch (*buf++) {
        case 0x00: print_instruction(1, "BRK", NULL); break;
        case 0x01: print_instruction(2, "ORA", "($%02x,X)", *buf++); break;
        case 0x02: print_instruction(1, "KIL", NULL); break;
        case 0x03: print_instruction(2, "SLO", "($%02x,X)", *buf++); break;
        case 0x04: print_instruction(2, "NOP", "$%02x", *buf++); break;
        case 0x05: print_instruction(2, "ORA", "$%02x", *buf++); break;
        case 0x06: print_instruction(2, "ASL", "$%02x", *buf++); break;
        case 0x07: print_instruction(2, "SLO", "$%02x", *buf++); break;
        case 0x08: print_instruction(1, "PHP", NULL); break;
        case 0x09: print_instruction(2, "ORA", "#%02x", *buf++); break;
        case 0x0a: print_instruction(1, "ASL", NULL); break;
        case 0x0b: print_instruction(2, "ANC", "#%02x", *buf++); break;
        case 0x0c: print_instruction(3, "NOP", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x0d: print_instruction(3, "ORA", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x0e: print_instruction(3, "ASL", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x0f: print_instruction(3, "SLO", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x10: print_instruction(2, "BPL", "PC,$%02x", *buf++); break;
        case 0x11: print_instruction(2, "ORA", "($%02x),Y", *buf++); break;
        case 0x12: print_instruction(1, "KIL", NULL); break;
        case 0x13: print_instruction(2, "SLO", "($%02x),Y", *buf++); break;
        case 0x14: print_instruction(2, "NOP", "$%02x,X", *buf++); break;
        case 0x15: print_instruction(2, "ORA", "$%02x,X", *buf++); break;
        case 0x16: print_instruction(2, "ASL", "$%02x,X", *buf++); break;
        case 0x17: print_instruction(2, "SLO", "$%02x,X", *buf++); break;
        case 0x18: print_instruction(1, "CLC", NULL); break;
        case 0x19: print_instruction(3, "ORA", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0x1a: print_instruction(1, "NOP", NULL); break;
        case 0x1b: print_instruction(3, "SLO", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0x1c: print_instruction(3, "NOP", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x1d: print_instruction(3, "ORA", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x1e: print_instruction(3, "ASL", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x1f: print_instruction(3, "SLO", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x20: print_instruction(3, "JSR", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x21: print_instruction(2, "AND", "($%02x,X)", *buf++); break;
        case 0x22: print_instruction(1, "KIL", NULL); break;
        case 0x23: print_instruction(2, "RLA", "($%02x,X)", *buf++); break;
        case 0x24: print_instruction(2, "BIT", "$%02x", *buf++); break;
        case 0x25: print_instruction(2, "AND", "$%02x", *buf++); break;
        case 0x26: print_instruction(2, "ROL", "$%02x", *buf++); break;
        case 0x27: print_instruction(2, "RLA", "$%02x", *buf++); break;
        case 0x28: print_instruction(1, "PLP", NULL); break;
        case 0x29: print_instruction(2, "AND", "#%02x", *buf++); break;
        case 0x2a: print_instruction(1, "ROL", NULL); break;
        case 0x2b: print_instruction(2, "ANC", "#%02x", *buf++); break;
        case 0x2c: print_instruction(3, "BIT", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x2d: print_instruction(3, "AND", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x2e: print_instruction(3, "ROL", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x2f: print_instruction(3, "RLA", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x30: print_instruction(2, "BMI", "PC,$%02x", *buf++); break;
        case 0x31: print_instruction(2, "AND", "($%02x),Y", *buf++); break;
        case 0x32: print_instruction(1, "KIL", NULL); break;
        case 0x33: print_instruction(2, "RLA", "($%02x),Y", *buf++); break;
        case 0x34: print_instruction(2, "NOP", "$%02x,X", *buf++); break;
        case 0x35: print_instruction(2, "AND", "$%02x,X", *buf++); break;
        case 0x36: print_instruction(2, "ROL", "$%02x,X", *buf++); break;
        case 0x37: print_instruction(2, "RLA", "$%02x,X", *buf++); break;
        case 0x38: print_instruction(1, "SEC", NULL); break;
        case 0x39: print_instruction(3, "AND", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0x3a: print_instruction(1, "NOP", NULL); break;
        case 0x3b: print_instruction(3, "RLA", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0x3c: print_instruction(3, "NOP", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x3d: print_instruction(3, "AND", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x3e: print_instruction(3, "ROL", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x3f: print_instruction(3, "RLA", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x40: print_instruction(1, "RTI", NULL); break;
        case 0x41: print_instruction(2, "EOR", "($%02x,X)", *buf++); break;
        case 0x42: print_instruction(1, "KIL", NULL); break;
        case 0x43: print_instruction(2, "SRE", "($%02x,X)", *buf++); break;
        case 0x44: print_instruction(2, "NOP", "$%02x", *buf++); break;
        case 0x45: print_instruction(2, "EOR", "$%02x", *buf++); break;
        case 0x46: print_instruction(2, "LSR", "$%02x", *buf++); break;
        case 0x47: print_instruction(2, "SRE", "$%02x", *buf++); break;
        case 0x48: print_instruction(1, "PHA", NULL); break;
        case 0x49: print_instruction(2, "EOR", "#%02x", *buf++); break;
        case 0x4a: print_instruction(1, "LSR", NULL); break;
        case 0x4b: print_instruction(2, "ALR", "#%02x", *buf++); break;
        case 0x4c: print_instruction(3, "JMP", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x4d: print_instruction(3, "EOR", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x4e: print_instruction(3, "LSR", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x4f: print_instruction(3, "SRE", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x50: print_instruction(2, "BVC", "PC,$%02x", *buf++); break;
        case 0x51: print_instruction(2, "EOR", "($%02x),Y", *buf++); break;
        case 0x52: print_instruction(1, "KIL", NULL); break;
        case 0x53: print_instruction(2, "SRE", "($%02x),Y", *buf++); break;
        case 0x54: print_instruction(2, "NOP", "$%02x,X", *buf++); break;
        case 0x55: print_instruction(2, "EOR", "$%02x,X", *buf++); break;
        case 0x56: print_instruction(2, "LSR", "$%02x,X", *buf++); break;
        case 0x57: print_instruction(2, "SRE", "$%02x,X", *buf++); break;
        case 0x58: print_instruction(1, "CLI", NULL); break;
        case 0x59: print_instruction(3, "EOR", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0x5a: print_instruction(1, "NOP", NULL); break;
        case 0x5b: print_instruction(3, "SRE", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0x5c: print_instruction(3, "NOP", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x5d: print_instruction(3, "EOR", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x5e: print_instruction(3, "LSR", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x5f: print_instruction(3, "SRE", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x60: print_instruction(1, "RTS", NULL); break;
        case 0x61: print_instruction(2, "ADC", "($%02x,X)", *buf++); break;
        case 0x62: print_instruction(1, "KIL", NULL); break;
        case 0x63: print_instruction(2, "RRA", "($%02x,X)", *buf++); break;
        case 0x64: print_instruction(2, "NOP", "$%02x", *buf++); break;
        case 0x65: print_instruction(2, "ADC", "$%02x", *buf++); break;
        case 0x66: print_instruction(2, "ROR", "$%02x", *buf++); break;
        case 0x67: print_instruction(2, "RRA", "$%02x", *buf++); break;
        case 0x68: print_instruction(1, "PLA", NULL); break;
        case 0x69: print_instruction(2, "ADC", "#%02x", *buf++); break;
        case 0x6a: print_instruction(1, "ROR", NULL); break;
        case 0x6b: print_instruction(2, "ARR", "#%02x", *buf++); break;
        case 0x6c: print_instruction(3, "JMP", "($%02x%02x)", buf[1], buf[0]); buf += 2; break;
        case 0x6d: print_instruction(3, "ADC", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x6e: print_instruction(3, "ROR", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x6f: print_instruction(3, "RRA", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x70: print_instruction(2, "BVS", "PC,$%02x", *buf++); break;
        case 0x71: print_instruction(2, "ADC", "($%02x),Y", *buf++); break;
        case 0x72: print_instruction(1, "KIL", NULL); break;
        case 0x73: print_instruction(2, "RRA", "($%02x),Y", *buf++); break;
        case 0x74: print_instruction(2, "NOP", "$%02x,X", *buf++); break;
        case 0x75: print_instruction(2, "ADC", "$%02x,X", *buf++); break;
        case 0x76: print_instruction(2, "ROR", "$%02x,X", *buf++); break;
        case 0x77: print_instruction(2, "RRA", "$%02x,X", *buf++); break;
        case 0x78: print_instruction(1, "SEI", NULL); break;
        case 0x79: print_instruction(3, "ADC", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0x7a: print_instruction(1, "NOP", NULL); break;
        case 0x7b: print_instruction(3, "RRA", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0x7c: print_instruction(3, "NOP", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x7d: print_instruction(3, "ADC", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x7e: print_instruction(3, "ROR", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x7f: print_instruction(3, "RRA", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x80: print_instruction(2, "NOP", "#%02x", *buf++); break;
        case 0x81: print_instruction(2, "STA", "($%02x,X)", *buf++); break;
        case 0x82: print_instruction(2, "NOP", "#%02x", *buf++); break;
        case 0x83: print_instruction(2, "SAX", "($%02x,X)", *buf++); break;
        case 0x84: print_instruction(2, "STY", "$%02x", *buf++); break;
        case 0x85: print_instruction(2, "STA", "$%02x", *buf++); break;
        case 0x86: print_instruction(2, "STX", "$%02x", *buf++); break;
        case 0x87: print_instruction(2, "SAX", "$%02x", *buf++); break;
        case 0x88: print_instruction(1, "DEY", NULL); break;
        case 0x89: print_instruction(2, "NOP", "#%02x", *buf++); break;
        case 0x8a: print_instruction(1, "TXA", NULL); break;
        case 0x8b: print_instruction(2, "XAA", "#%02x", *buf++); break;
        case 0x8c: print_instruction(3, "STY", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x8d: print_instruction(3, "STA", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x8e: print_instruction(3, "STX", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x8f: print_instruction(3, "SAX", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0x90: print_instruction(2, "BCC", "PC,$%02x", *buf++); break;
        case 0x91: print_instruction(2, "STA", "($%02x),Y", *buf++); break;
        case 0x92: print_instruction(1, "KIL", NULL); break;
        case 0x93: print_instruction(2, "AHX", "($%02x),Y", *buf++); break;
        case 0x94: print_instruction(2, "STY", "$%02x,X", *buf++); break;
        case 0x95: print_instruction(2, "STA", "$%02x,X", *buf++); break;
        case 0x96: print_instruction(2, "STX", "$%02x,Y", *buf++); break;
        case 0x97: print_instruction(2, "SAX", "$%02x,Y", *buf++); break;
        case 0x98: print_instruction(1, "TYA", NULL); break;
        case 0x99: print_instruction(3, "STA", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0x9a: print_instruction(1, "TXS", NULL); break;
        case 0x9b: print_instruction(3, "TAS", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0x9c: print_instruction(3, "SHY", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x9d: print_instruction(3, "STA", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0x9e: print_instruction(3, "SHX", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0x9f: print_instruction(3, "AHX", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0xa0: print_instruction(2, "LDY", "#%02x", *buf++); break;
        case 0xa1: print_instruction(2, "LDA", "($%02x,X)", *buf++); break;
        case 0xa2: print_instruction(2, "LDX", "#%02x", *buf++); break;
        case 0xa3: print_instruction(2, "LAX", "($%02x,X)", *buf++); break;
        case 0xa4: print_instruction(2, "LDY", "$%02x", *buf++); break;
        case 0xa5: print_instruction(2, "LDA", "$%02x", *buf++); break;
        case 0xa6: print_instruction(2, "LDX", "$%02x", *buf++); break;
        case 0xa7: print_instruction(2, "LAX", "$%02x", *buf++); break;
        case 0xa8: print_instruction(1, "TAY", NULL); break;
        case 0xa9: print_instruction(2, "LDA", "#%02x", *buf++); break;
        case 0xaa: print_instruction(1, "TAX", NULL); break;
        case 0xab: print_instruction(2, "LAX", "#%02x", *buf++); break;
        case 0xac: print_instruction(3, "LDY", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0xad: print_instruction(3, "LDA", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0xae: print_instruction(3, "LDX", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0xaf: print_instruction(3, "LAX", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0xb0: print_instruction(2, "BCS", "PC,$%02x", *buf++); break;
        case 0xb1: print_instruction(2, "LDA", "($%02x),Y", *buf++); break;
        case 0xb2: print_instruction(1, "KIL", NULL); break;
        case 0xb3: print_instruction(2, "LAX", "($%02x),Y", *buf++); break;
        case 0xb4: print_instruction(2, "LDY", "$%02x,X", *buf++); break;
        case 0xb5: print_instruction(2, "LDA", "$%02x,X", *buf++); break;
        case 0xb6: print_instruction(2, "LDX", "$%02x,Y", *buf++); break;
        case 0xb7: print_instruction(2, "LAX", "$%02x,Y", *buf++); break;
        case 0xb8: print_instruction(1, "CLV", NULL); break;
        case 0xb9: print_instruction(3, "LDA", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0xba: print_instruction(1, "TSX", NULL); break;
        case 0xbb: print_instruction(3, "LAS", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0xbc: print_instruction(3, "LDY", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0xbd: print_instruction(3, "LDA", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0xbe: print_instruction(3, "LDX", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0xbf: print_instruction(3, "LAX", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0xc0: print_instruction(2, "CPY", "#%02x", *buf++); break;
        case 0xc1: print_instruction(2, "CMP", "($%02x,X)", *buf++); break;
        case 0xc2: print_instruction(2, "NOP", "#%02x", *buf++); break;
        case 0xc3: print_instruction(2, "DCP", "($%02x,X)", *buf++); break;
        case 0xc4: print_instruction(2, "CPY", "$%02x", *buf++); break;
        case 0xc5: print_instruction(2, "CMP", "$%02x", *buf++); break;
        case 0xc6: print_instruction(2, "DEC", "$%02x", *buf++); break;
        case 0xc7: print_instruction(2, "DCP", "$%02x", *buf++); break;
        case 0xc8: print_instruction(1, "INY", NULL); break;
        case 0xc9: print_instruction(2, "CMP", "#%02x", *buf++); break;
        case 0xca: print_instruction(1, "DEX", NULL); break;
        case 0xcb: print_instruction(2, "AXS", "#%02x", *buf++); break;
        case 0xcc: print_instruction(3, "CPY", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0xcd: print_instruction(3, "CMP", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0xce: print_instruction(3, "DEC", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0xcf: print_instruction(3, "DCP", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0xd0: print_instruction(2, "BNE", "PC,$%02x", *buf++); break;
        case 0xd1: print_instruction(2, "CMP", "($%02x),Y", *buf++); break;
        case 0xd2: print_instruction(1, "KIL", NULL); break;
        case 0xd3: print_instruction(2, "DCP", "($%02x),Y", *buf++); break;
        case 0xd4: print_instruction(2, "NOP", "$%02x,X", *buf++); break;
        case 0xd5: print_instruction(2, "CMP", "$%02x,X", *buf++); break;
        case 0xd6: print_instruction(2, "DEC", "$%02x,X", *buf++); break;
        case 0xd7: print_instruction(2, "DCP", "$%02x,X", *buf++); break;
        case 0xd8: print_instruction(1, "CLD", NULL); break;
        case 0xd9: print_instruction(3, "CMP", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0xda: print_instruction(1, "NOP", NULL); break;
        case 0xdb: print_instruction(3, "DCP", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0xdc: print_instruction(3, "NOP", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0xdd: print_instruction(3, "CMP", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0xde: print_instruction(3, "DEC", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0xdf: print_instruction(3, "DCP", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0xe0: print_instruction(2, "CPX", "#%02x", *buf++); break;
        case 0xe1: print_instruction(2, "SBC", "($%02x,X)", *buf++); break;
        case 0xe2: print_instruction(2, "NOP", "#%02x", *buf++); break;
        case 0xe3: print_instruction(2, "ISC", "($%02x,X)", *buf++); break;
        case 0xe4: print_instruction(2, "CPX", "$%02x", *buf++); break;
        case 0xe5: print_instruction(2, "SBC", "$%02x", *buf++); break;
        case 0xe6: print_instruction(2, "INC", "$%02x", *buf++); break;
        case 0xe7: print_instruction(2, "ISC", "$%02x", *buf++); break;
        case 0xe8: print_instruction(1, "INX", NULL); break;
        case 0xe9: print_instruction(2, "SBC", "#%02x", *buf++); break;
        case 0xea: print_instruction(1, "NOP", NULL); break;
        case 0xeb: print_instruction(2, "SBC", "#%02x", *buf++); break;
        case 0xec: print_instruction(3, "CPX", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0xed: print_instruction(3, "SBC", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0xee: print_instruction(3, "INC", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0xef: print_instruction(3, "ISC", "$%02x%02x", buf[1], buf[0]); buf += 2; break;
        case 0xf0: print_instruction(2, "BEQ", "PC,$%02x", *buf++); break;
        case 0xf1: print_instruction(2, "SBC", "($%02x),Y", *buf++); break;
        case 0xf2: print_instruction(1, "KIL", NULL); break;
        case 0xf3: print_instruction(2, "ISC", "($%02x),Y", *buf++); break;
        case 0xf4: print_instruction(2, "NOP", "$%02x,X", *buf++); break;
        case 0xf5: print_instruction(2, "SBC", "$%02x,X", *buf++); break;
        case 0xf6: print_instruction(2, "INC", "$%02x,X", *buf++); break;
        case 0xf7: print_instruction(2, "ISC", "$%02x,X", *buf++); break;
        case 0xf8: print_instruction(1, "SED", NULL); break;
        case 0xf9: print_instruction(3, "SBC", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0xfa: print_instruction(1, "NOP", NULL); break;
        case 0xfb: print_instruction(3, "ISC", "$%02x%02x,Y", buf[1], buf[0]); buf += 2; break;
        case 0xfc: print_instruction(3, "NOP", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0xfd: print_instruction(3, "SBC", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0xfe: print_instruction(3, "INC", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        case 0xff: print_instruction(3, "ISC", "$%02x%02x,X", buf[1], buf[0]); buf += 2; break;
        }

    }

    return 0;
}
