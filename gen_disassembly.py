from __future__ import print_function


instruction_byte_count = {
    None: 1,
    'imm': 2,
    'zp': 2,
    'zpx': 2,
    'zpy': 2,
    'izx': 2,
    'izy': 2,
    'abs': 3,
    'abx': 3,
    'aby': 3,
    'ind': 3,
    'rel': 2,
}


instruction_operand = {
    None: 'NULL',
    'imm': '"#%02x"',
    'zp': '"$%02x"',
    'zpx': '"$%02x,X"',
    'zpy': '"$%02x,Y"',
    'izx': '"($%02x,X)"',
    'izy': '"($%02x),Y"',
    'abs': '"$%02x%02x"',
    'abx': '"$%02x%02x,X"',
    'aby': '"$%02x%02x,Y"',
    'ind': '"($%02x%02x)"',
    'rel': '"PC,$%02x"',
}


def print_instruction(instruction, addressing_mode=None):
    instruction_size = instruction_byte_count[addressing_mode]

    if instruction_size == 1:
        print('print_instruction({}, "{}", {});'.format(
            instruction_size,
            instruction,
            instruction_operand[addressing_mode],
        ), end='')
    elif instruction_size == 2:
        print('print_instruction({}, "{}", {}, *rom++);'.format(
            instruction_size,
            instruction,
            instruction_operand[addressing_mode],
        ), end='')
    else:
        print('print_instruction({}, "{}", {}, rom[1], rom[0]); rom += 2;'.format(
            instruction_size,
            instruction,
            instruction_operand[addressing_mode],
        ), end='')


if __name__ == '__main__':
    instruction_table = []

    with open('opcode_table.txt', 'r') as infile:
        for line in infile:
            instruction_table.append(line.split())

    for index, instruction_info in enumerate(instruction_table):
        print('case 0x{}: '.format(format(index, '02x')), end='')
        print_instruction(*instruction_info)
        print(' break;')
