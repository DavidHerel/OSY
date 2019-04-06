
int isnum(char ch)
{
        return ch >= '0' && ch <= '9';
}
 
int isspc(char ch)
{
        return ch == ' ' || ch == '\n';
}
 
 void conv(unsigned num, char *buff){
    int j = 0;
    unsigned hist = num;
    buff[0]= '0';
    buff[1] = 'x';
    char hexa[20];
    while (num != 0){
        unsigned rest = 0;
        
        rest = num % 16;
        if (rest < 10){
            j+=1;
            hexa[j] = 48 + rest;
        } else {
            j+=1;
            hexa[j] = 87 + rest;
        }
        num = num /16;
        
    }
    int ind = 2;
    for (int i = j; i >=0; i--){
        buff[ind] = hexa[i];
        ind+=1;
    }
    if (hist != 0){
        buff[ind] = '\0';
    }else if (hist == 0){
        buff[2] = '0';
        buff[3] = '\0';
    }
    
}
 unsigned int exi(unsigned int sys, unsigned int p1){
	unsigned int ret;
	asm volatile ( "int $0x80"
			: "=a" (ret)
			:"a" (sys), "b" (p1) 
			:"memory");
	return ret;
}

unsigned int write(unsigned int sys, unsigned int p1,  unsigned int p2,  unsigned int p3){
	unsigned int ret;
	asm volatile ( "int $0x80"
			: "=a" (ret)
			:"a" (sys), "b" (p1) , "c"(p2), "d"(p3)
			:"memory");
	return ret;
}
 int str_len(char *buff){
    int ind = 0;
    while (buff[ind] != '\0'){
        ind+=1;
    }
    return ind;
}
 
static void print(unsigned num)
{
        char buff[23];

        /* TODO: Get rid of sprintf() and strlen() */
        conv(num, buff);
        int ret = write(4, 1, (unsigned int) buff, str_len(buff));
        if (ret == -1){
            exi(1, 0); // TODO your new exit
        }
        char line[1];
        line[0] = '\n';
        ret = write(4, 1, (unsigned int) line, 1);
        if (ret == -1){
            exi(1, 0); // TODO your new exit
        }
}
 

 
/* TODO: main() is called by libc. Real entry point is _start(). */
void _start()
{
	    
        char buf[20];
        unsigned num = 0;
        int i;
        int num_digits = 0;
        unsigned chars_in_buffer = 0;
 
        for (/* no init */; /* no end condition */; i++, chars_in_buffer--) {
                if (chars_in_buffer == 0) {
                        int ret = write(3, 0, (unsigned int) buf, sizeof(buf));
                        if (ret < 0)
                        exi(1, 0); // TODO replace by exit
                        i = 0;
                        chars_in_buffer = ret;
                }
                if (num_digits > 0 && (chars_in_buffer == 0 /* EOF */ || !isnum(buf[i]))) {
                        print(num);
                        num_digits = 0;
                        num = 0;
                }
                if (chars_in_buffer == 0 /* EOF */ || (!isspc(buf[i]) && !isnum(buf[i])))
                        exi(1, 0); // TODO: replace by exit
 
                if (isnum(buf[i])) {
                        num = num * 10 + buf[i] - '0';
                        num_digits++;
                }
        }
}
