/*
 * ws2812.c
 *
 *  Created on: Oct 26, 2024
 *      Author: Pawel
 */
#include "main.h"
#include "ws2812.h"




uint16_t current_led;

uint8_t buffer[48];

ws2812b_color ws2811_array[WS2811_LEDS];


uint8_t k;

void ws2811_refresh(void)
{

	uint16_t j = 0;

	for(current_led=0; current_led<2; current_led++)
	{

		//GREEN
		for(int8_t k=7; k>=0; k--)
		{
			if((ws2811_array[current_led].red & (1<<k)) == 0)
				buffer[j] = zero;
		  	else
		  	  	buffer[j] = one;
		  	j++;
		}

		//RED
		for(int8_t k=7; k>=0; k--)
		{
			if((ws2811_array[current_led].green & (1<<k)) == 0)
				buffer[j] = zero;
			else
				buffer[j] = one;
			j++;
		}

		//BLUE
		for(int8_t k=7; k>=0; k--)
		{
		  	if((ws2811_array[current_led].blue & (1<<k)) == 0)
		  	  	buffer[j] = zero;
		  	else
		  	  	buffer[j] = one;
		  	j++;
		}
	}


	if(! LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_1))
	{
		LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_1, (uint32_t)buffer, LL_USART_DMA_GetRegAddr(USART1, LL_USART_DMA_REG_DATA_TRANSMIT), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, 48);
		LL_USART_EnableDMAReq_TX(USART1);
		LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
	}
}


void ws2811_refresh_IRQ(void)
{
	if(LL_DMA_IsActiveFlag_HT1(DMA1))
	{
		LL_DMA_ClearFlag_HT1(DMA1);

		uint16_t j = 0;
		current_led++;
		if(current_led > WS2811_LEDS)
			{
				LL_DMA_ClearFlag_HT1(DMA1);
				LL_DMA_ClearFlag_TC1(DMA1);
				LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
			}

		for(int8_t k=7; k>=0; k--)
		{
			if((ws2811_array[current_led].red & (1<<k)) == 0)
				buffer[j] = zero;
			else
				buffer[j] = one;
			j++;
		}

					  	  		  //RED
		for(int8_t k=7; k>=0; k--)
		{
			if((ws2811_array[current_led].green & (1<<k)) == 0)
				buffer[j] = zero;
			else
				buffer[j] = one;
			j++;
			}

					  	  		  //BLUE
		for(int8_t k=7; k>=0; k--)
		{
			if((ws2811_array[current_led].blue & (1<<k)) == 0)
				buffer[j] = zero;
			else
				buffer[j] = one;
			j++;
		}
				//LL_DMA_ClearFlag_HT1(DMA1);
	}

	if(LL_DMA_IsActiveFlag_TC1(DMA1))
	{
		LL_DMA_ClearFlag_TC1(DMA1);
		LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
		uint16_t j = 24;
		current_led++;
		if(current_led > WS2811_LEDS)
		{
			LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
		}

		//RED
		for(int8_t k=7; k>=0; k--)
		{
			if((ws2811_array[current_led].red & (1<<k)) == 0)
				buffer[j] = zero;
			else
				buffer[j] = one;
			j++;
		}

		//GREEN
		for(int8_t k=7; k>=0; k--)
		{
			if((ws2811_array[current_led].green & (1<<k)) == 0)
				buffer[j] = zero;
			else
				buffer[j] = one;
			j++;
		}

		//BLUE
		for(int8_t k=7; k>=0; k--)
		{
			if((ws2811_array[current_led].blue & (1<<k)) == 0)
				buffer[j] = zero;
			else
				buffer[j] = one;
			j++;
		}

		LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_1, (uint32_t)buffer, LL_USART_DMA_GetRegAddr(USART1, LL_USART_DMA_REG_DATA_TRANSMIT), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, 48);
		LL_USART_EnableDMAReq_TX(USART1);
		LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

		NVIC_EnableIRQ(DMA1_Channel1_IRQn);
		LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
		LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_1);

	}
}

void ws2811_set_diode_HSV(int16_t diode_id, uint16_t hue, uint8_t saturation, uint8_t brightness)
{
	if(diode_id >= WS2811_LEDS || diode_id < 0)
	{
		return;
	}

	uint16_t sector, fracts, p, q, t;

	if(saturation == 0)
	{
		ws2811_array[diode_id].red = brightness;
		ws2811_array[diode_id].green = brightness;
		ws2811_array[diode_id].blue = brightness;
	}
	else
	{
		if(hue >= 360) hue = 359;

		sector = hue / 60;		// sector 0 to 5
		fracts = hue % 60;
		p = (brightness * (255 - saturation)) / 256;
		q = (brightness * (255 - (saturation * fracts) / 60)) / 256;
	t = (brightness * (255 - (saturation * (59 - fracts)) / 60)) / 256;

		switch (sector)
		{
			case 0:
				ws2811_array[diode_id].red = brightness;
				ws2811_array[diode_id].green = (uint8_t)t;
				ws2811_array[diode_id].blue = (uint8_t)p;
				break;
			case 1:
				ws2811_array[diode_id].red = (uint8_t)q;
				ws2811_array[diode_id].green = brightness;
				ws2811_array[diode_id].blue = (uint8_t)p;
				break;
			case 2:
				ws2811_array[diode_id].red = (uint8_t)p;
				ws2811_array[diode_id].green = brightness;
				ws2811_array[diode_id].blue = (uint8_t)t;
				break;
			case 3:
				ws2811_array[diode_id].red = (uint8_t)p;
				ws2811_array[diode_id].green = (uint8_t)q;
				ws2811_array[diode_id].blue = brightness;
				break;
			case 4:
				ws2811_array[diode_id].red = (uint8_t)t;
				ws2811_array[diode_id].green = (uint8_t)p;
				ws2811_array[diode_id].blue = brightness;
				break;
			default:	// case5
				ws2811_array[diode_id].red = brightness;
				ws2811_array[diode_id].green = (uint8_t)p;
				ws2811_array[diode_id].blue = (uint8_t)q;
				break;
		}
	}
}

void WS2811_rainbow(uint16_t first_led, uint16_t last_led)
{
	static uint16_t color = 0;
	uint16_t id_led_tmp;

	color++;
	if(color > 360) color = 0;

	for(id_led_tmp = first_led; id_led_tmp <= last_led; id_led_tmp++)
	{

		ws2811_set_diode_HSV(id_led_tmp, color, 0xff, 0xff);

	}
}

void WS2811_rainbow_puls( uint16_t first_led, uint16_t last_led)
{
	static uint16_t color = 0;
	uint16_t id_led_tmp;
	static uint8_t value, jasnosc;



	 for(id_led_tmp = first_led; id_led_tmp <= last_led; id_led_tmp++ )
		  {
		  if(value == 0)
		  {
			  jasnosc++;
			  for(id_led_tmp = first_led; id_led_tmp <= last_led; id_led_tmp++ )
			  {
				  ws2811_set_diode_HSV(id_led_tmp, color, 0xff, jasnosc);
			  }
			  if(jasnosc == 254)
			  {
				  value  = 1;
			  }
		  }
		  else
		  {
			  jasnosc--;
			  for(id_led_tmp = first_led; id_led_tmp <= last_led; id_led_tmp++ )
			  {
				  ws2811_set_diode_HSV(id_led_tmp, color, 0xff, jasnosc);
			  }
			  if(jasnosc == 0)
			  {
			  	value  = 0;
			  	color +=20;
			  	if(color > 360) color = 0;
			  }
		  }
     }
	 LL_mDelay(5);
}

void WS2811_set_color_HSV(uint16_t color, uint8_t saturation, uint8_t brightness, uint16_t first_led, uint16_t last_led)
	{
	for(uint16_t id_led_tmp = first_led; id_led_tmp <= last_led; id_led_tmp++ )
	{
			ws2811_set_diode_HSV(id_led_tmp, color, saturation, brightness);
	}
}

void WS2811_flash(uint16_t color, uint8_t brightness, uint16_t first_led, uint16_t last_led)
{
	uint16_t random_id_led;
	WS2811_set_color_HSV(color, 0xff, brightness, first_led, last_led);
	random_id_led = rand()%(last_led - first_led) + first_led;
	ws2811_set_diode_HSV(random_id_led, 0xff, 0xff, 0xff);
}

void WS2811_flash_color(uint16_t first_led, uint16_t last_led, uint32_t time)
{

	static uint32_t speed;

	if((GetTick() - speed) > time)
	{
	speed = GetTick();
	uint16_t id_led, color;
	uint8_t brightnes;
	id_led = rand()%WS2811_LEDS;
	color = rand()%360;
	brightnes = rand()%255;
	ws2811_set_diode_HSV(id_led, color, 0xff, brightnes);
	}
}

void WS2811_snake(void)
{
	static uint8_t direction;
	static int16_t counter1;
	//static int16_t counter2;
	//if(counter1 > WS2811_LEDS) counter1 = WS2811_LEDS;
	//if(counter2 > WS2811_LEDS) counter2 = WS2811_LEDS;

	if(direction == 0)
	{
		counter1++;


		ws2811_set_diode_HSV(counter1 , 300, 0xff, 0xff);
		for(uint8_t i = 0; i < 32; i++)
		{
			ws2811_set_diode_HSV(counter1 - i, 300, 0xff, 0xff - i * 8);
		}

		ws2811_set_diode_HSV(WS2811_LEDS - counter1, 20, 0xff, 0xff);
		//ws2811_set_diode_HSV(WS2811_LEDS - counter1 + 1, 20, 0xff, 0xf);
		for(uint8_t i = 0; i < 32; i++)
		{
			ws2811_set_diode_HSV(WS2811_LEDS - counter1 + i, 20, 0xff, 0xff - i * 8);
		}

		if(counter1  > WS2811_LEDS + 32)
		{
			direction = 1;
			//counter1 =0;
		}
	}
	else
	{

		counter1--;
		ws2811_set_diode_HSV(WS2811_LEDS - counter1, 20, 0xff, 0xff);
	//	ws2811_set_diode_HSV(WS2811_LEDS - counter1 - 1, 20, 0xff, 0xf);
		for(uint8_t i = 0; i < 32; i++)
		{
			ws2811_set_diode_HSV(WS2811_LEDS - counter1 - i, 20, 0xff, 0xff - i * 8);
		}

		ws2811_set_diode_HSV(counter1, 300, 0xff, 0xff);
		for(uint8_t i = 0; i < 32; i++)
		{
			ws2811_set_diode_HSV(counter1 + i, 300, 0xff, 0xff - i * 8);
		}

		if(counter1 + 32 < 0)
		{
			direction = 0;
		}
	}
	LL_mDelay(50);
}

void WS2811_rainbow_flow(void)
{
	static uint16_t counter1;
	uint16_t color2;

	for(uint16_t color1 = 0; color1 < 361; color1++)
	{
		if(color1 <= WS2811_LEDS)
		{
			color2 = color1;
		}
		else
		{
			color2 = WS2811_LEDS;
		}

	ws2811_set_diode_HSV(color2, (color1 + counter1)%360 , 0xff, 0xff);
	}
	counter1++;
	counter1 = counter1 % 360;
	LL_mDelay(2);
}

void color_change (uint16_t time2)
{
	static uint32_t new_msTick2;
	static uint16_t step2;
	static uint16_t counter1;

	  if((GetTick() - new_msTick2) > time2)
	 	  {
	 		 step2++;

	 		 new_msTick2 = GetTick();
	 		  if(step2 > WS2811_LEDS)
	 			  {
	 			  step2 = 0;
	 			  counter1 += 60;
	 			  if(counter1 > 360) counter1 = 0;
	 			  }
	 		 ws2811_set_diode_HSV(step2, counter1 , 0xff, 0xff);
	 	  }
}

