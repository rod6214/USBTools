#if __PRINTFT__
#include "printft.h"
#include "lcd.h"


//void putchar(char c);

static char* bf;
static char buf[12];
static int num;
static char uc;
static char zs;

static void out(char c) {
    *bf++ = c;
    }

static void outDgt(char dgt) {
	out(dgt+(dgt<10 ? '0' : (uc ? 'A' : 'a')-10));
	zs=1;
    }

static void divOut(unsigned int div) {
    unsigned char dgt=0;
	num &= 0xffff; // just for testing the code  with 32 bit ints
	while (num>=div) {
		num -= div;
		dgt++;
		}
	if (zs || dgt>0)
		outDgt(dgt);
    }

void lcd_printft(char *fmt, ...)
	{
	va_list va;
	char ch;
	char* p;

	va_start(va,fmt);

	while ((ch=*(fmt++))) {
		if (ch!='%') {
//			putchar(ch);
            lcd_putchar(ch);
			}
		else {
			char lz=0;
			char w=0;
			ch=*(fmt++);
			if (ch=='0') {
				ch=*(fmt++);
				lz=1;
				}
			if (ch>='0' && ch<='9') {
				w=0;
				while (ch>='0' && ch<='9') {
					w=((char)(((w<<2)+w)<<1) + (char)ch - '0');
					ch=*fmt++;
					}
				}
			bf=buf;
			p=bf;
			zs=0;
			switch (ch) {
				case 0:
					goto abort;
				case 'u':
				case 'd' :
					num=(int)va_arg(va, int);
					if (ch=='d' && (int)num<0) {
						num = -(int)num;
						out('-');
						}
					divOut(10000);
					divOut(1000);
					divOut(100);
					divOut(10);
					outDgt((char)num);
					break;
				case 'x':
				case 'X' :
				    uc= ch=='X';
					num=va_arg(va, int);
					divOut(0x1000);
					divOut(0x100);
					divOut(0x10);
					outDgt((char)num);
					break;
				case 'c' :
					out((char)(va_arg(va, int)));
					break;
				case 's' :
					p=va_arg(va, char*);
					break;
				case '%' :
					out('%');
				default:
					break;
				}
			*bf=0;
			bf=p;
			while (*bf++ && w > 0)
				w--;
			while (w-- > 0)
//				putchar(lz ? '0' : ' ');
                lcd_putchar(lz ? '0' : ' ');
			while ((ch= *p++))
//				putchar(ch);
                lcd_putchar(ch);
			}
		}
	abort:;
	va_end(va);
	}
#endif
