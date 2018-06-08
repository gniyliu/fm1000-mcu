#define EEPROM_ADDR_BASE		0x4000

#define ReadEepromByte(Address)	FLASH_ReadByte( (Address+EEPROM_ADDR_BASE) )

#define WriteEepromByte(Address, Data)	FLASH_ProgramByte((Address+EEPROM_ADDR_BASE), Data)

#define InitEeprom()		FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD)

#define UnlockEeprom()	FLASH_Unlock(FLASH_MEMTYPE_DATA)

#define LockEeprom()		FLASH_Lock(FLASH_MEMTYPE_DATA)

void EepromTest(void)
{
static uint8_t tmp;
uint8_t i;

	UnlockEeprom();

	for(i=0; i<8; i++)
	{
		WriteEepromByte(i, i+tmp*0x8);
		printf("(%4x) = %2x", EEPROM_ADDR_BASE+i, ReadEepromByte(i));
	}
	printf("\n\r\n\r");

	LockEeprom();
	tmp++;
}


enum{
ADDR_RMT_PAIR_DATA = 	0x00,

ADDR_MOTOR_SPEED	= ADDR_RMT_PAIR_DATA+PAIR_DATA_COUNT,

};


void E_read_pair_data(void)
{
uint8_t i;

	UnlockEeprom();
	printf("\n\r Pair data is \n\r");
	for(i=0; i<PAIR_DATA_COUNT; i++)
	{
		rmt_pair_data[i] = ReadEepromByte(i + ADDR_RMT_PAIR_DATA);
		printf("%2x ", rmt_pair_data[i]);
	}
	printf("\n\r");
	LockEeprom();

}

void E_save_pair_data(void)
{
uint8_t i;

	UnlockEeprom();
	for(i=0; i<PAIR_DATA_COUNT; i++)
	{
		WriteEepromByte(i + ADDR_RMT_PAIR_DATA, rmt_pair_data[i]);
	}
	LockEeprom();

}

void E_read_motor_speed(void)
{
	UnlockEeprom();
	motorSpeed = ReadEepromByte(ADDR_MOTOR_SPEED);
	LockEeprom();
	printf("\n\r motorSpeed = %x \n\r", motorSpeed);
}

void E_save_motor_speed(void)
{
	UnlockEeprom();
	WriteEepromByte(ADDR_MOTOR_SPEED, motorSpeed);
	LockEeprom();
}

