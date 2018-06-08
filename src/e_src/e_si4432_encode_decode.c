enum{
CMD_ON_OFF,
CMD_MOTOR_SPEED,
CMD_TIMER,
CMD_MOTOR_START_TIME,
CMD_MOTOR_STOP_TIME,
CMD_RMT_PAIR,
};

#if SI4432_TX()
void Si4432SendCommand(uint8_t cmd)
{
uint8_t i;
uint8_t count = 0;
uint8_t chksum = 0;

	SI4432_TxBUFF[count++] = cmd;

	for(i=0; i<CHIP_ID_LENGTH; i++)
		SI4432_TxBUFF[count++] = ChipUniqueID[i];

	switch(cmd)
	{
	case CMD_ON_OFF:
		SI4432_TxBUFF[count++] = onOff;
		SI4432_TxBUFF[count++] = motorSpeed;
		break;

	case CMD_MOTOR_SPEED:
		SI4432_TxBUFF[count++] = motorSpeed;
		break;

	case CMD_TIMER:
		SI4432_TxBUFF[count++] = timer.hour;
		SI4432_TxBUFF[count++] = timer.miniute;
		break;

	case CMD_MOTOR_START_TIME:
		SI4432_TxBUFF[count++] = motor_start_time.hour;
		SI4432_TxBUFF[count++] = motor_start_time.miniute;
		break;

	case CMD_MOTOR_STOP_TIME:
		SI4432_TxBUFF[count++] = motor_stop_time.hour;
		SI4432_TxBUFF[count++] = motor_stop_time.miniute;
		break;

	case CMD_RMT_PAIR:
		break;

	default:
		return;
		break;
	}

	for(i=0; i<count; i++)
	{
		chksum ^= SI4432_TxBUFF[i];
	}
	SI4432_TxBUFF[count++] = chksum;

	SI4432_TxCount = count;
	E_SI4432_Tx();

}

#elif SI4432_RX()
u8 SI4432_RxBUFF[100];
u8 SI4432_RxCount,SI4432_RxLenth;
#define PACKET_BUFFER_START_ADDR		(1+PAIR_DATA_COUNT)
void Si4432DecodeCommand(void)
{
uint8_t i;
//uint8_t count = 0;
uint8_t chksum = 0;
uint8_t rmt_pair_flag;

	for(i=0; i< (SI4432_RxLenth-1); i++)
	{
		chksum ^= SI4432_RxBUFF[i];
	}
	if(chksum == SI4432_RxBUFF[SI4432_RxLenth-1])
	{
		printf("\n\r Transfer successed! \n\r");

		if(rmt_pair_timerout)	//	&& (SI4432_RxBUFF[0] == CMD_RMT_PAIR)
		{
			for(i=0; i<PAIR_DATA_COUNT; i++)
			{
				rmt_pair_data[i] = SI4432_RxBUFF[i+1];
			}
			E_save_pair_data();
			rmt_pair_timerout = 0;

			printf("\n\r Pair successed! \n\r");
		}

		rmt_pair_flag = 1;
		for(i=0; i<PAIR_DATA_COUNT; i++)
		{
			if( rmt_pair_data[i] !=  SI4432_RxBUFF[i+1] )
			{
				rmt_pair_flag = 0;
				break;
			}
		}

		if(rmt_pair_flag)
		{
			switch(SI4432_RxBUFF[0])
			{
			case CMD_ON_OFF:
				onOff = SI4432_RxBUFF[PACKET_BUFFER_START_ADDR];
				motorSpeed = SI4432_RxBUFF[PACKET_BUFFER_START_ADDR+1];
				break;

			case CMD_MOTOR_SPEED:
				motorSpeed = SI4432_RxBUFF[PACKET_BUFFER_START_ADDR];
				E_save_motor_speed();
				break;

			case CMD_TIMER:
				break;

			case CMD_MOTOR_START_TIME:
				break;

			case CMD_MOTOR_STOP_TIME:
				break;

			case CMD_RMT_PAIR:
				break;

			default:
				return;
				break;
			}
		}
		else
		{
			printf("\n\r Remote with control not paired\n\r");
		}

	}
	else
	{
		printf("\n\r Transfer error! \n\r");
	}



}


#endif

