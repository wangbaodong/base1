/**
 ****************************************************************************************************
 * @file        atk_md0130.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK_MD0130ģ����������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ̽���� F407������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */
 
#ifndef __ATK_MD0130_H
#define __ATK_MD0130_H

#include "main.h"
#include "mb.h"
/* ״̬�������ֵ */
/**
 ����IP �˿� �汾��
 ���10������
 ���10������
 ���10������
*/
#define MAX_LCDSTAT   5  
/* ����ATK-MD0130ģ��LCD�ߴ� */
#define ATK_MD0130_LCD_WIDTH                240
#define ATK_MD0130_LCD_HEIGHT               240

/* ����ATK-MD0130ģ�����õ����� */
#define ATK_MD0130_FONT_12                  1
#define ATK_MD0130_FONT_16                  1
#define ATK_MD0130_FONT_24                  1
#define ATK_MD0130_FONT_32                  1

/* Ĭ������12������ */
#if ((ATK_MD0130_FONT_12 == 0) && (ATK_MD0130_FONT_16 == 0) && (ATK_MD0130_FONT_24 == 0) && (ATK_MD0130_FONT_32 == 0))
#undef ATK_MD0130_FONT_12
#defien ATK_MD0130_FONT_12 1
#endif

/* ���Ŷ��� */
#define ATK_MD0130_PWR_GPIO_PORT            GPIOG
#define ATK_MD0130_PWR_GPIO_PIN             GPIO_PIN_6
#define ATK_MD0130_PWR_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)
#define ATK_MD0130_CS_GPIO_PORT             GPIOG
#define ATK_MD0130_CS_GPIO_PIN              GPIO_PIN_7
#define ATK_MD0130_CS_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)
#define ATK_MD0130_WR_GPIO_PORT             GPIOB
#define ATK_MD0130_WR_GPIO_PIN              GPIO_PIN_4
#define ATK_MD0130_WR_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)
#define ATK_MD0130_RST_GPIO_PORT            GPIOG
#define ATK_MD0130_RST_GPIO_PIN             GPIO_PIN_8
#define ATK_MD0130_RST_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)

/* IO���� */
#define ATK_MD0130_PWR(x)                   do{ x ?                                                                                     \
                                                HAL_GPIO_WritePin(LCD_PWR_PORT, LCD_PWR_PIN, GPIO_PIN_SET) :    \
                                                HAL_GPIO_WritePin(LCD_PWR_PORT, LCD_PWR_PIN, GPIO_PIN_RESET);   \
                                            }while(0)
#define ATK_MD0130_CS(x)                    do{ x ?                                                                                     \
                                                HAL_GPIO_WritePin(ATK_MD0130_CS_GPIO_PORT, ATK_MD0130_CS_GPIO_PIN, GPIO_PIN_SET) :      \
                                                HAL_GPIO_WritePin(ATK_MD0130_CS_GPIO_PORT, ATK_MD0130_CS_GPIO_PIN, GPIO_PIN_RESET);     \
                                            }while(0)
#define ATK_MD0130_WR(x)                    do{ x ?                                                                                     \
                                                HAL_GPIO_WritePin(LCD_WR_PORT, LCD_WR_PIN, GPIO_PIN_SET) :      \
                                                HAL_GPIO_WritePin(LCD_WR_PORT, LCD_WR_PIN, GPIO_PIN_RESET);     \
                                            }while(0)
#define ATK_MD0130_RST(x)                   do{ x ?                                                                                     \
                                                HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_SET) :    \
                                                HAL_GPIO_WritePin(LCD_RST_PORT, LCD_RST_PIN, GPIO_PIN_RESET);   \
                                            }while(0)

/* ATK-MD0130ģ��LCD��ʾ����ö�� */
typedef enum
{
#if (ATK_MD0130_FONT_12 != 0)
    ATK_MD0130_LCD_FONT_12,             /* 12������ */
#endif
#if (ATK_MD0130_FONT_16 != 0)
    ATK_MD0130_LCD_FONT_16,             /* 16������ */
#endif
#if (ATK_MD0130_FONT_24 != 0)
    ATK_MD0130_LCD_FONT_24,             /* 24������ */
#endif
#if (ATK_MD0130_FONT_32 != 0)
    ATK_MD0130_LCD_FONT_32,             /* 32������ */
#endif
} atk_md0130_lcd_font_t;

/* ATK-MD0130ģ��LCD��ʾ����ģʽö�� */
typedef enum
{
    ATK_MD0130_NUM_SHOW_NOZERO = 0x00,  /* ���ָ�λ0����ʾ */
    ATK_MD0130_NUM_SHOW_ZERO,           /* ���ָ�λ0��ʾ */
} atk_md0130_num_mode_t;

/* ������ɫ���壨RGB565�� */
#define ATK_MD0130_WHITE        0xFFFF
#define ATK_MD0130_BLACK        0x0000
#define ATK_MD0130_BLUE         0x001F
#define ATK_MD0130_BRED         0XF81F		//����
#define ATK_MD0130_GRED         0XFFE0		//�Ⱥ�
#define ATK_MD0130_GBLUE        0X07FF		//����
#define ATK_MD0130_RED          0xF800		//
#define ATK_MD0130_MAGENTA      0xF81F  	//���
#define ATK_MD0130_GREEN        0x07E0		//
#define ATK_MD0130_CYAN         0x7FFF		//��ɫ
#define ATK_MD0130_YELLOW       0xFFE0	
#define ATK_MD0130_BROWN        0XBC40
#define ATK_MD0130_BRRED        0XFC07		//����
#define ATK_MD0130_GRAY         0X8430

/* �������� */
void atk_md0130_init(void);                                                                                                                             /* ATK-MD0130ģ���ʼ�� */
void atk_md0130_display_on(void);                                                                                                                       /* ����ATK-MD0130ģ��LCD���� */
void atk_md0130_display_off(void);                                                                                                                      /* �ر�ATK-MD0130ģ��LCD���� */
void atk_md0130_fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);                                                               /* ATK-MD0130ģ��LCD������� */
void atk_md0130_clear(uint16_t color);                                                                                                                  /* ATK-MD0130ģ��LCD���� */
void atk_md0130_draw_point(uint16_t x, uint16_t y, uint16_t color);                                                                                     /* ATK-MD0130ģ��LCD���� */
void atk_md0130_draw_line(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);                                                          /* ATK-MD0130ģ��LCD���߶� */
void atk_md0130_draw_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);                                                          /* ATK-MD0130ģ��LCD�����ο� */
void atk_md0130_draw_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);                                                                        /* ATK-MD0130ģ��LCD��Բ�ο� */
void atk_md0130_show_char(uint16_t x, uint16_t y, char ch, atk_md0130_lcd_font_t font, uint16_t color);                                                 /* ATK-MD0130ģ��LCD��ʾ1���ַ� */
void atk_md0130_show_string(uint16_t x, uint16_t y, char *str, atk_md0130_lcd_font_t font, uint16_t color);                                             /* ATK-MD0130ģ��LCD��ʾ�ַ��� */
void atk_md0130_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, atk_md0130_num_mode_t mode, atk_md0130_lcd_font_t font, uint16_t color);   /* ATK-MD0130ģ��LCD��ʾ���֣��ɿ�����ʾ��λ0 */
void atk_md0130_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, atk_md0130_lcd_font_t font, uint16_t color);                                /* ATK-MD0130ģ��LCD��ʾ���֣�����ʾ��λ0 */
void atk_md0130_show_pic(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *pic);                                                        /* ATK-MD0130ģ��LCDͼƬ */

#endif
