#include "chip8.h"

std::string Register::toString() const
{
    std::stringstream stream;
    stream << "0x" << std::hex << static_cast<uint16_t>(reg);
    return stream.str();
}

std::string Address::toString() const
{
    std::stringstream stream;
    stream << "0x" << std::hex << address;
    return stream.str();
}

const Address Chip8::top_stack() const
{
    return stack[SP];
}

void Chip8::call(const uint16_t nnn)
{
    ++SP;
    stack[SP] = PC;
    PC = Address(nnn);
}

void Chip8::se(const uint16_t xkk)
{
    uint8_t x = (xkk & 0xf00) >> 8u; // second 4 bits
    uint8_t kk = static_cast<uint8_t>(xkk & 0xff); // last 8 bits
    if (registers[x] == kk)
    {
        PC += 2;
    }
}

void Chip8::sne(const uint16_t xkk)
{
    uint8_t x = (xkk & 0xf00) >> 8u; // second 4 bits
    uint8_t kk = static_cast<uint8_t>(xkk & 0xff); // last 8 bits
    if (registers[x] != kk)
    {
        PC += 2;
    }
}

void Chip8::se(const uint8_t xy)
{
    uint8_t x = (xy & 0xf0) >> 4u;
    uint8_t y = xy & 0xf;
    if (registers[x] == registers[y])
    {
        PC += 2;
    }
}

void Chip8::ld(const uint16_t xkk)
{
    uint8_t x = (xkk & 0xf00) >> 8u;
    uint8_t kk = static_cast<uint8_t>(xkk & 0xff);
    registers[x].update(kk);
}

void Chip8::add(const uint16_t xkk)
{
    uint8_t x = (xkk & 0xf00) >> 8u;
    uint16_t kk = static_cast<uint16_t>(xkk & 0xff);
    registers[x] += kk;
}

void Chip8::ld(const uint8_t xy)
{
    uint8_t x = (xy & 0xf0) >> 4u;
    uint8_t y = xy & 0xf;
    registers[x].update(registers[y].reg);
}

void Chip8::bit_or(const uint8_t xy)
{
    uint8_t x = (xy & 0xf0) >> 4u;
    uint8_t y = xy & 0xf;
    registers[x].update(registers[x].reg | registers[y].reg);
}

void Chip8::bit_and(const uint8_t xy)
{
    uint8_t x = (xy & 0xf0) >> 4u;
    uint8_t y = xy & 0xf;
    registers[x].update(registers[x].reg & registers[y].reg);
}

void Chip8::bit_xor(const uint8_t xy)
{
    uint8_t x = (xy & 0xf0) >> 4u;
    uint8_t y = xy & 0xf;
    registers[x].update(registers[x].reg ^ registers[y].reg);
}

void Chip8::add(const uint8_t xy)
{
    uint8_t x = (xy & 0xf0) >> 4u;
    uint8_t y = xy & 0xf;
    uint8_t z = static_cast<uint8_t>(registers[x].reg + registers[y].reg);
    registers[x].update(z);
    if (z < registers[y].reg)
    {
        registers[0xf].update(1);
    }
    else
    {
        registers[0xf].update(0);
    }
}

void Chip8::sub(const uint8_t xy)
{
    uint8_t x = (xy & 0xf0) >> 4u;
    uint8_t y = xy & 0xf;
    uint8_t val_x = registers[x].reg;
    uint8_t val_y = registers[y].reg;
    if (val_x > val_y)
    {
        registers[0xf].update(1);
    }
    else
    {
        registers[0xf].update(0);
    }
    registers[x].update(static_cast<uint8_t>(val_x - val_y));
}

void Chip8::shr(const uint8_t xy)
{
    uint8_t x = (xy & 0xf0) >> 4u;
    uint8_t val_x = registers[x].reg;
    if ((val_x & 1) == 1)
    {
        registers[0xf].update(1);
    }
    else
    {
        registers[0xf].update(0);
    }
    registers[x].reg /= 2;
}

void Chip8::subn(const uint8_t xy)
{
    uint8_t x = (xy & 0xf0) >> 4u;
    uint8_t y = xy & 0xf;
    uint8_t val_x = registers[x].reg;
    uint8_t val_y = registers[y].reg;
    if (val_y > val_x)
    {
        registers[0xf].update(1);
    }
    else
    {
        registers[0xf].update(0);
    }
    registers[x].update(static_cast<uint8_t>(val_y - val_x));
}

void Chip8::shl(const uint8_t xy)
{
    uint8_t x = (xy & 0xf0) >> 4u;
    uint8_t val_x = registers[x].reg;
    if ((val_x >> 7u) == 1)
    {
        registers[0xf].update(1);
    }
    else
    {
        registers[0xf].update(0);
    }
    registers[x].update(static_cast<uint8_t>(val_x*2));
}

std::vector<Instruction> Chip8::readFromFile(const std::filesystem::path path) const
    {
        assert(exists(path));

        std::vector<Instruction> output;
        std::ifstream file {path};

        if (file.is_open())
        {
            uint16_t byte1, byte2;
            while (!file.eof())
            {
                byte1 = static_cast<uint16_t>(file.std::istream::get());
                byte1 = static_cast<uint16_t>(byte1 << 8u);
                byte2 = static_cast<uint16_t>(file.std::istream::get());

                uint16_t i =static_cast<uint16_t>(byte1 | byte2);
                output.emplace_back(i); // construct Instruction(i) in-place
            }

            return output;
        }
        else
        {
            std::cout << "Unable to open file.\n";
            return {};
        }
    }

void Chip8::run(const std::vector<Instruction> instructions)
    {
        for (Instruction instruction : instructions)
        {
            execute(instruction);
        }
    }

void Chip8::execute(const Instruction i)
{
    uint16_t inst = i.instruction();
    switch (inst)
    {
    case static_cast<uint16_t>(0x00ee):
        ret();
        break;

    default:
        break;
    }

    uint16_t first4bits = (inst & 0xf000) >> 12u;
    switch (first4bits)
    {
    case 1:
        {
            uint16_t nnn = static_cast<uint16_t>(inst & 0xfff);
            jp(nnn);
            break;
        }

    case 2:
    {
        uint16_t nnn = static_cast<uint16_t>(inst & 0xfff);
        call(nnn);
        break;
    }

    case 3:
    {
        uint16_t xkk = static_cast<uint16_t>(inst & 0xfff);
        se(xkk);
        break;
    }

    case 4:
    {
        uint16_t xkk = static_cast<uint16_t>(inst & 0xfff);
        sne(xkk);
        break;
    }

    case 5:
    {
        uint8_t xy0 = static_cast<uint8_t>((inst & 0xff0) >> 4u);
        se(xy0);
        break;
    }

    case 6:
    {
        uint16_t xkk = static_cast<uint16_t>(inst & 0xfff);
        ld(xkk);
        break;
    }

    case 7:
    {
        uint16_t xkk = static_cast<uint16_t>(inst & 0xfff);
        add(xkk);
        break;
    }

    case 8:
    {
        uint8_t last4bits = inst & 0xf;
        switch (last4bits)
        {
        case 0:
        {
            uint8_t xy = static_cast<uint8_t>((inst & 0xff0) >> 4u);
            ld(xy);
            break;
        }

        case 1:
        {
            uint8_t xy = static_cast<uint8_t>((inst & 0xff0) >> 4u);
            bit_or(xy);
            break;
        }

        case 2:
        {
            uint8_t xy = static_cast<uint8_t>((inst & 0xff0) >> 4u);
            bit_and(xy);
            break;
        }

        case 3:
        {
            uint8_t xy = static_cast<uint8_t>((inst & 0xff0) >> 4u);
            bit_xor(xy);
            break;
        }

        case 4:
        {
            uint8_t xy = static_cast<uint8_t>((inst & 0xff0) >> 4u);
            add(xy);
            break;
        }

        case 5:
        {
            uint8_t xy = static_cast<uint8_t>((inst & 0xff0) >> 4u);
            sub(xy);
            break;
        }

        case 6:
        {
            uint8_t xy = static_cast<uint8_t>((inst & 0xff0) >> 4u);
            shr(xy);
            break;
        }

        case 7:
        {
            uint8_t xy = static_cast<uint8_t>((inst & 0xff0) >> 4u);
            subn(xy);
            break;
        }

        case 0xe:
        {
            uint8_t xy = static_cast<uint8_t>((inst & 0xff0) >> 4u);
            shl(xy);
            break;
        }

        default:
            break;
        }
    }

    default:
        break;
    }
}
