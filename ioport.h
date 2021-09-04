#include <cstdint>
#include <stdexcept>

/*! 
	\brief Main template for I/O ports interface. Can be used for non-standart ports with the number of lines that are\
	not multiples of eight

	Interface allows user to read and write information from/to port, changing lines direction and mode (Hi-Z, PullUp) 

	\tparam RegisterType Type of pointer that will point to the registers (DDRx, PORTx, PINx)
	\tparam BitsCount Number of bits (lines) in I/O port. Can't be more than size of RegisterType in bits
*/
template <typename RegisterType, std::size_t BitsCount>
class IOPort
{
public:
	/*!
	* \brief The only constructor
	* Takes three pointers to port registers and stores them for further interactions 
	* \param DDR Pointer to Data Direction register
	* \param PORT Pointer to PORT register for writing
	* \param PIN Const pointer to PIN register for reading
	*/
	IOPort(RegisterType* DDR, RegisterType* PORT, const RegisterType* PIN) :
		DDR(DDR), PORT(PORT), PIN(PIN)
	{
	}
	/*!
	* \brief Default destructor
	*/
	~IOPort() = default;

	/*!
	* \brief Reading function
	* Reads value stored in PIN. Bits higher than BitsCount are ignored
	* \return RegisterType value not higher than 2^BitsCount - 1
	*/
	RegisterType read() const noexcept
	{
		return (*PIN) & bitsMask;
	}
	/*!
	* \brief Reading function for single bit
	* Reads value stored in selected bit of PIN, if possible.
	* \param bitNumber Number of bit to read, starting with 0. Should be less than size of RegisterType in bits 
	* \return bool value of bit
	* \exception std::invalid_argument Is thrown if bitNumber is incorrect
	*/
	bool read(const std::size_t bitNumber) const
	{
		checkBitNumber(bitNumber);

		return ((*PIN) >> bitNumber) % 2;
	}
	/*!
	* \brief Writing function for RegisterType value
	* Writes value in PORT, if DDR is set in output mode. Bits higher than BitsCount are saved from previous PORT value
	* \param value RegisterType value to write in register. Value higher than 2^BitsCount - 1 can't be written correctly
	* \exception std::runtime_error Is thrown if DDR is not set in output mode
	*/
	void write(const RegisterType& value)
	{
		if (((*DDR) & bitsMask) != bitsMask)
			throw std::runtime_error("Trying to write value with wrong data direction");

		auto higherBits = (*PORT) & (~bitsMask); //save higher bits
		(*PORT) = (value & bitsMask) | higherBits;
	}
	/*!
	* \brief Writing function for single bit
	* Writes value in selected bit of PORT, if DDR's same bit is set in output mode and bitNumber value is correct
	* \param bitNumber Number of bit to write, starting with 0. Should be less than size of RegisterType in bits
	* \param value Value of bit to write 
	* \exception std::runtime_error Is thrown if DDR's bit is not set in output mode
	* \exception std::invalid_argument Is thrown if bitNumber is incorrect
	*/
	void write(const std::size_t bitNumber, const bool value)
	{
		if ((*DDR) >> bitNumber % 2 == 0)
			throw std::runtime_error("Trying to write value with wrong data direction");

		setBit(PORT, bitNumber, value);
	}
	/*!
	* \brief Function for setting DDR in output mode
	* Writes value '1' to all bits in DDR not higher than BitsCount
	*/
	void out() noexcept
	{
		(*DDR) |= bitsMask;
	}
	/*!
	* \brief Function for setting DDR's single bit in output mode
	* Writes value '1' to bit in DDR not higher than BitsCount
	* \param bitNumber Number of bit to set in output mode, starting with 0. Should be less than size of RegisterType in bits
	* \exception std::invalid_argument Is thrown if bitNumber is incorrect
	*/
	void out(const std::size_t bitNumber)
	{
		setBit(DDR, bitNumber, 1);
	}
	/*!
	* \brief Function for setting DDR in input mode
	* Writes value '0' to all bits in DDR not higher than BitsCount
	*/
	void in() noexcept
	{
		(*DDR) &= ~(bitsMask);
	}
	/*!
	* \brief Function for setting DDR's single bit in input mode
	* Writes value '0' to bit in DDR not higher than BitsCount
	* \param bitNumber Number of bit to set in input mode, starting with 0. Should be less than size of RegisterType in bits
	* \exception std::invalid_argument Is thrown if bitNumber is incorrect
	*/
	void in(const std::size_t bitNumber)
	{
		setBit(DDR, bitNumber, 0);
	}
	/*!
	* \brief Alternative function for port line. Switches line's mode to HI-Z
	* Sets DDR's bit in input mode, writes value '0' to PORT's bit
	* \param bitNumber Number of line to set in HI-Z mode, starting with 0. Should be less than size of RegisterType in bits
	* \exception std::invalid_argument Is thrown if bitNumber is incorrect
	*/
	void hiZ(const std::size_t bitNumber)
	{
		setBit(DDR, bitNumber, 0);
		setBit(PORT, bitNumber, 0);
	}
	/*!
	* \brief Alternative function for port line. Switches line's mode to PullUp
	* Sets DDR's bit in input mode, writes value '1' to PORT's bit
	* \param bitNumber Number of line to set in PullUp mode, starting with 0. Should be less than size of RegisterType in bits
	* \exception std::invalid_argument Is thrown if bitNumber is incorrect
	*/
	void pullUp(const std::size_t bitNumber)
	{
		setBit(DDR, bitNumber, 0);
		setBit(PORT, bitNumber, 1);
	}
private:
	/*!
	* \brief Pointer to I/O port's Data Direction Register, is set in the constructor
	*/
	RegisterType* DDR;
	/*!
	* \brief Pointer to I/O port's PORT Register, is set in the constructor
	*/
	RegisterType* PORT;
	/*!
	* \brief Pointer to I/O port's PIN Register, is set in the constructor
	*/
	const RegisterType* PIN;
	/*!
	* \brief Mask is used for ignoring bits that are higher than the amount of lines in I/O ports
	*/
	const RegisterType bitsMask = (1 << BitsCount) - 1;

	/*!
	* \brief Private helper function for checking bit correctness
	* \param bitNumber Number of bit to change, starting with 0. Should be less than size of RegisterType in bits
	* \exception std::invalid_argument Is thrown if bitNumber is incorrect
	*/
	void checkBitNumber(const std::size_t bitNumber)
	{
		if (bitNumber >= BitsCount)
			throw std::invalid_argument("Too large bit number");
	}

	/*!
	* \brief Private helper function for changing one bit
	* \param reg Pointer to register for changing
	* \param bitNumber Number of bit to change, starting with 0. Should be less than size of RegisterType in bits 
	* \param value Value to write down
	* \exception std::invalid_argument Is thrown if bitNumber is incorrect
	*/
	void setBit(RegisterType* reg, const std::size_t bitNumber, const bool value)
	{
		checkBitNumber(bitNumber);

		if (value == true)
			(*reg) |= (1 << bitNumber);
		else
			(*reg) &= ~(1 << bitNumber);
	}
};
