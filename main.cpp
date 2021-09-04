#include <cstdint>
#include "ioport.h"

/*Assume that microcontroller has defined 8-bit registers DDRA, PORTA and PINA for
data direction, writing and reading. 3 higher bits are unused (reserved, used for other purpoces)
since maximum precision for adressing is 1 byte*/

int main()
{
	std::uint8_t DDRA = 0, PORTA = 0, PINA = 16; //dummies instead of real registers
	using RegisterType = decltype(PORTA);
	using IOPort5bits = IOPort <RegisterType, 5>;

	IOPort5bits portA(&DDRA, &PORTA, &PINA);

	return 0;
}