const char HTTPheader[] = "HTTP/1.1 200 OK\nContent-type:";
const char HTTPMimeTypeHTML[] = "text/html\n\n";
const char HTTPMimeTypeScript[] = "text/plain\n\n";

//
//Define the HTML page to be sent to the PC
//

char StrtWebPage[] =
"<html><body>\
<form name=\"input\" method=\"get\">\
 <table align=center width=500 bgcolor= LightGray border=2>\
 <tr><td align=center colspan=2><font size=5 color=RoyalBlue face=\"verdana\">\
 <b>	Web-Based Control and Monitoring </br></font></td>\
 </tr><tr> <td height=\"200\" align=center bgcolor=LightGray><input name=\"TA\"\
 type=\"submit\"value=\"TOGGLE RELAY A\"></td>\
 <td height=\"200\" align=center bgcolor=LightGray><input name=\"TB\"\
 type=\"submit\"value=\"TOGGLE RELAY B\"></td></tr>\
</table></form><body><html>";

//Ethernet NIC interface define

sfr sbit SPI_Ethernet_Rst at RC0_bit;
sfr sbit SPI_Ethernet_CS at RC1_bit;
sfr sbit SPI_Ethernet_Rst_Direction at TRISC0_bit;
sfr sbit SPI_Ethernet_CS_Direction at TRISC1_bit;

//Define Serial Ethernet Board MAC Address and IP address to be used for the communication

unsigned char MACAddr[6]={0x00, 0x14, 0xA5, 0x76, 0x19, 0x3f};
unsigned char IPAddr[4] = {192,168,1,8};
unsigned char getRequest[10];

typedef struct
{
    unsigned canCloseTCP:1;
    unsigned isBroadcast:1;

}TethPktFlags;


//TCP routine.This is where the user request to toggle relay a and relay b are processed

unsigned int SPI_Ethernet_UserTCP(unsigned char *remoteHost,
                                  unsigned int remotePort, unsigned int localPort,
                                  unsigned int regLength, TEthPktFlags *flags)
//Any received packets are checked and the function continues if the packets are of type "GET/"

{
    unsigned int Len;
    for(len=0; len<10; len++)getRequest[len]=SPI_Ethernet_getByte();
    getRequest[len]=0;
    if(memcmp(getRequest,"GET /",5))return(0);
    //then the command tail is checked and the led's are toggled as required
    if(!memcmp(getRequest+6,"TA",2))RD0_bit = ~ RD0_bit;// toggle RD0 if command tail is "TA"
    else if (!memcmp(getRequest+6,"TB",2))RD1_bit = ~ RD1_bit; //toggle RD1 if command tail is "TB"


    //the transmit buffer is loaded with the HTML response and the length of
    //the buffer is returned from the function which then sends the buffer to the PC

    if(localPort != 80)return(0);
    Len = SPI_Ethernet_putConstString(HTTPheader);
    Len +=SPI_Ethernet_putConstString(HTTPMimeTypeHTML);
    Len += SPI_Ethernet_putString(StrtWebPage);
    return Len;

}

//udp routine. must be declared even though it is not used
//as the ethernet library requires both the udp and tcp functions to be present

unsigned int SPI_Ethernet_UserUDP(unsigned char *remoteHost,
                                  unsigned int remotePort, unsigned int destPort,
                                  unsigned int reqLength, TethPktFlags *flags)
{
    return(0);

}


void main() {
    TRISD = 0 ;
    ANSELC = 0;
    ANSELD= 0;
    PORTD= 0;
    SPI1_Init();
    SPI_Ethernet_Init(MACAddr, IPAddr, 0x01);

    while(1) //loop forever
    {
        SPI_Ethernet_doPacket(); //Process next received packet

    }

}
