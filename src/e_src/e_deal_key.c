
void SendKeyInfo(unsigned char key)
{
	if(key == KID_K8)	g_FanStat = FAN_STAT_BLINK;
	else if(key == KID_K9)	g_FanStat = FAN_STAT_ALL_OFF;
	else	g_FanStat = FAN_STAT_ALL_OFF;

	putchar(0xff);
	putchar(key);
}


void KeySrv(void)
{
	switch(KEY_MASK)
	{
	case KEY_STAT_RELEASE:
		break;
	case KEY_STAT_LONG:
		break;
	case KEY_STAT_SHORT:
		break;
	case KEY_STAT_LIANXU:
		break;
	case KEY_STAT_PRESS:
		switch(KEY_CODE)
		{
		case KID_PWR:
			if(onOff)
			{
				onOff = 0;
				printf("\r\nmotor off\r\n");
			}
			else
			{
				onOff = 1;
				printf("\r\nmotor on\r\n");
			}
			break;

		case KID_INC:
			if( motorSpeed < MOTOR_SPEED_MAX )
				++motorSpeed;
			E_save_motor_speed();
			printf("speed = %d",motorSpeed);
			break;

		case KID_DEC:
			if( motorSpeed > MOTOR_SPEED_MIN )
				--motorSpeed;
			E_save_motor_speed();
			printf("speed = %d",motorSpeed);
			break;

		case KID_K1:		SendKeyInfo(KID_K1);		KeyLedAllOff();	KeyLed(0, 1);			g_LedTimer = 0;			break;
		case KID_K2:		SendKeyInfo(KID_K2);		KeyLedAllOff();	KeyLed(1, 1);			g_LedTimer = 0;			break;
		case KID_K3:		SendKeyInfo(KID_K3);		KeyLedAllOff();	KeyLed(2, 1);			g_LedTimer = 0;			break;
		case KID_K4:		SendKeyInfo(KID_K4);		KeyLedAllOff();	KeyLed(3, 1);			g_LedTimer = 0;			break;
		case KID_K5:		SendKeyInfo(KID_K5);		KeyLedAllOff();	KeyLed(4, 1);			g_LedTimer = 0;			break;
		case KID_K6:		SendKeyInfo(KID_K6);		KeyLedAllOff();	KeyLed(5, 1);			g_LedTimer = 0;			break;
		case KID_K7:		SendKeyInfo(KID_K7);		KeyLedAllOff();	KeyLed(6, 1);			g_LedTimer = 0;			break;
		case KID_K8:		SendKeyInfo(KID_K8);		KeyLedAllOff();	KeyLed(7, 1);			g_LedTimer = 0;			break;
		case KID_K9:		SendKeyInfo(KID_K9);		KeyLedAllOff();	KeyLed(8, 1);			g_LedTimer = 0;			break;
		case KID_K10:		SendKeyInfo(KID_K10);		KeyLedAllOff();	KeyLed(9, 1);			g_LedTimer = 0;			break;
		case KID_K11:		SendKeyInfo(KID_K11);		KeyLedAllOff();	KeyLed(10, 1);		g_LedTimer = 0;			break;
		case KID_K12:		SendKeyInfo(KID_K12);		KeyLedAllOff();	KeyLed(11, 1);		g_LedTimer = 0;			break;
		case KID_K13:		SendKeyInfo(KID_K13);		KeyLedAllOff();	KeyLed(12, 1);		g_LedTimer = 0;			break;

		default:
			break;
		}
		break;
	default:
		break;
	}


}


