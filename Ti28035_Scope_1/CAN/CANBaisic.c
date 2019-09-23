#include "CANBasic.h"

void CanBasic_init(void) {
    struct ECAN_REGS ECanaShadow;

    /* step1: 先配置CAN 相关的GPIO引脚 */
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO30 = 0;     // Enable pull-up for GPIO30 (CANRXA)
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;     // Enable pull-up for GPIO31 (CANTXA)

    GpioCtrlRegs.GPAQSEL2.bit.GPIO30=3;     //CANA, Asynchronous
    GpioCtrlRegs.GPAQSEL2.bit.GPIO31=3;     //CANA, Asynchronous

    GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 1;    // Configure GPIO30 for CANRXA operation
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 1;    // Configure GPIO31 for CANTXA operation
    EDIS;

    EALLOW;
    /* step2: 配置Rx和Tx分别为eCAN的接收和发送引脚 */
    ECanaRegs.CANTIOC.all=0x0008;           // ECanaRegs.CANTIOC.bit.TXFUNC = 1;
    ECanaRegs.CANRIOC.all=0x0008;           //ECanaRegs.CANRIOC.bit.RXFUNC = 1;

    /* step3: 清楚所有的接收、发送标志 */
    ECanaRegs.CANTA.all=0xFFFFFFFF;
    ECanaRegs.CANRMP.all=0xFFFFFFFF;

    /* step4: 配置主控制寄存器 */
    ECanaShadow.CANMC.all=ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.CCR=1;
    ECanaShadow.CANMC.bit.DBO = 1;          //设置数据的发送/接收的方式(LSB or MSB)
    ECanaRegs.CANMC.all=ECanaShadow.CANMC.all;

    EDIS;
    while(ECanaShadow.CANES.bit.CCE==0)
    {
        ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    }

    EALLOW;

    /* step5: 配置波特率  baudrate = SYSCLKOUT/((BRPREG+1)*(TSEG1REG+TSEG2REG+3) */
    ECanaShadow.CANBTC.all=ECanaRegs.CANBTC.all;

    ECanaShadow.CANBTC.bit.BRPREG = 5;
    ECanaShadow.CANBTC.bit.TSEG2REG = 1;
    ECanaShadow.CANBTC.bit.TSEG1REG = 6;
    ECanaShadow.CANBTC.bit.SAM = 1;
    ECanaRegs.CANBTC.all=ECanaShadow.CANBTC.all;

    /* step6: 主控制器MC寄存器的配置  */
    ECanaShadow.CANMC.all=ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.CCR=0;
    ECanaShadow.CANMC.bit.ABO=1;
    ECanaShadow.CANMC.bit.PDR=0;
    ECanaShadow.CANMC.bit.WUBA=0;
    ECanaShadow.CANMC.bit.CDR=0;
    ECanaShadow.CANMC.bit.DBO=0;
    ECanaShadow.CANMC.bit.STM=0;
    ECanaShadow.CANMC.bit.SRES=0;
    ECanaShadow.CANMC.bit.MBNR=0;
    ECanaRegs.CANMC.all=ECanaShadow.CANMC.all;
    EDIS;

    while(ECanaShadow.CANES.bit.CCE==1)
    {
        ECanaShadow.CANES.all=ECanaRegs.CANES.all;
    }


    EALLOW;
    ECanaRegs.CANME.all = 0;                //屏蔽所有邮箱， 在写MSGID之前需要完成此操作
    EDIS;

    /* step7: 发送邮箱寄存器配置 */
    ECanaMboxes.MBOX0.MSGID.bit.AME = 0;
    ECanaMboxes.MBOX0.MSGID.bit.IDE = 0;
    ECanaMboxes.MBOX0.MSGID.bit.STDMSGID = 0x300;

    ECanaMboxes.MBOX1.MSGID.bit.AME = 0;
    ECanaMboxes.MBOX1.MSGID.bit.IDE = 0;
    ECanaMboxes.MBOX1.MSGID.bit.STDMSGID = 0x301;

    ECanaMboxes.MBOX2.MSGID.bit.AME = 0;
    ECanaMboxes.MBOX2.MSGID.bit.IDE = 0;
    ECanaMboxes.MBOX2.MSGID.bit.STDMSGID = 0x302;


    ECanaMboxes.MBOX3.MSGID.bit.AME = 0;
    ECanaMboxes.MBOX3.MSGID.bit.IDE = 0;
    ECanaMboxes.MBOX3.MSGID.bit.STDMSGID = 0x303;

    ECanaMboxes.MBOX4.MSGID.bit.AME = 0;
    ECanaMboxes.MBOX4.MSGID.bit.IDE = 0;
    ECanaMboxes.MBOX4.MSGID.bit.STDMSGID = 0x304;

    ECanaMboxes.MBOX5.MSGID.bit.AME = 0;
    ECanaMboxes.MBOX5.MSGID.bit.IDE = 0;
    ECanaMboxes.MBOX5.MSGID.bit.STDMSGID = 0x305;

    ECanaMboxes.MBOX6.MSGID.bit.AME = 0;
    ECanaMboxes.MBOX6.MSGID.bit.IDE = 0;
    ECanaMboxes.MBOX6.MSGID.bit.STDMSGID = 0x306;


    ECanaMboxes.MBOX0.MSGCTRL.bit.DLC=0x0008;
    ECanaMboxes.MBOX1.MSGCTRL.bit.DLC=0x0008;
    ECanaMboxes.MBOX2.MSGCTRL.bit.DLC=0x0008;
    ECanaMboxes.MBOX3.MSGCTRL.bit.DLC=0x0008;
    ECanaMboxes.MBOX4.MSGCTRL.bit.DLC=0x0008;
    ECanaMboxes.MBOX5.MSGCTRL.bit.DLC=0x0008;
    ECanaMboxes.MBOX6.MSGCTRL.bit.DLC=0x0008;

    ECanaMboxes.MBOX0.MSGCTRL.bit.RTR=0;
    ECanaMboxes.MBOX1.MSGCTRL.bit.RTR=0;
    ECanaMboxes.MBOX2.MSGCTRL.bit.RTR=0;
    ECanaMboxes.MBOX3.MSGCTRL.bit.RTR=0;
    ECanaMboxes.MBOX4.MSGCTRL.bit.RTR=0;
    ECanaMboxes.MBOX5.MSGCTRL.bit.RTR=0;
    ECanaMboxes.MBOX6.MSGCTRL.bit.RTR=0;

    /* step8: 接收邮箱寄存器配置 */
    ECanaMboxes.MBOX8.MSGID.bit.AME = 0;
    ECanaMboxes.MBOX8.MSGID.bit.IDE = 0;
    ECanaMboxes.MBOX8.MSGID.bit.STDMSGID = 0x2F0;

    ECanaMboxes.MBOX9.MSGID.bit.AME = 0;
    ECanaMboxes.MBOX9.MSGID.bit.IDE = 0;
    ECanaMboxes.MBOX9.MSGID.bit.STDMSGID = 0x2F1;

    ECanaMboxes.MBOX10.MSGID.bit.AME = 0;
    ECanaMboxes.MBOX10.MSGID.bit.IDE = 0;
    ECanaMboxes.MBOX10.MSGID.bit.STDMSGID = 0x2F2;

    ECanaMboxes.MBOX11.MSGID.bit.AME = 0;
    ECanaMboxes.MBOX11.MSGID.bit.IDE = 0;
    ECanaMboxes.MBOX11.MSGID.bit.STDMSGID = 0x2F3;

    ECanaMboxes.MBOX12.MSGID.bit.AME = 0;
    ECanaMboxes.MBOX12.MSGID.bit.IDE = 0;
    ECanaMboxes.MBOX12.MSGID.bit.STDMSGID = 0x2F4;

    ECanaMboxes.MBOX13.MSGID.bit.AME = 0;
    ECanaMboxes.MBOX13.MSGID.bit.IDE = 0;
    ECanaMboxes.MBOX13.MSGID.bit.STDMSGID = 0x2F5;

    ECanaMboxes.MBOX8.MSGCTRL.bit.DLC=0x0008;
    ECanaMboxes.MBOX9.MSGCTRL.bit.DLC=0x0008;
    ECanaMboxes.MBOX10.MSGCTRL.bit.DLC=0x0008;
    ECanaMboxes.MBOX11.MSGCTRL.bit.DLC=0x0008;
    ECanaMboxes.MBOX12.MSGCTRL.bit.DLC=0x0008;
    ECanaMboxes.MBOX13.MSGCTRL.bit.DLC=0x0008;

    ECanaMboxes.MBOX8.MSGCTRL.bit.RTR=0;
    ECanaMboxes.MBOX9.MSGCTRL.bit.RTR=0;
    ECanaMboxes.MBOX10.MSGCTRL.bit.RTR=0;
    ECanaMboxes.MBOX11.MSGCTRL.bit.RTR=0;
    ECanaMboxes.MBOX12.MSGCTRL.bit.RTR=0;
    ECanaMboxes.MBOX13.MSGCTRL.bit.RTR=0;



    ECanaShadow.CANMD.all=ECanaRegs.CANMD.all;
    ECanaShadow.CANMD.bit.MD0=0;
    ECanaShadow.CANMD.bit.MD1=0;
    ECanaShadow.CANMD.bit.MD2=0;
    ECanaShadow.CANMD.bit.MD3=0;
    ECanaShadow.CANMD.bit.MD4=0;
    ECanaShadow.CANMD.bit.MD5=0;
    ECanaShadow.CANMD.bit.MD6=0;

    ECanaShadow.CANMD.bit.MD8=1;
    ECanaShadow.CANMD.bit.MD9=1;
    ECanaShadow.CANMD.bit.MD10=1;
    ECanaShadow.CANMD.bit.MD11=1;
    ECanaShadow.CANMD.bit.MD12=1;
    ECanaShadow.CANMD.bit.MD13=1;
    ECanaRegs.CANMD.all=ECanaShadow.CANMD.all;

    ECanaShadow.CANME.all=ECanaRegs.CANME.all;
    ECanaShadow.CANME.bit.ME0=1;
    ECanaShadow.CANME.bit.ME1=1;
    ECanaShadow.CANME.bit.ME2=1;
    ECanaShadow.CANME.bit.ME3=1;
    ECanaShadow.CANME.bit.ME4=1;
    ECanaShadow.CANME.bit.ME5=1;
    ECanaShadow.CANME.bit.ME6=1;

    ECanaShadow.CANME.bit.ME8=1;
    ECanaShadow.CANME.bit.ME9=1;
    ECanaShadow.CANME.bit.ME10=1;
    ECanaShadow.CANME.bit.ME11=1;
    ECanaShadow.CANME.bit.ME12=1;
    ECanaShadow.CANME.bit.ME13=1;
    ECanaRegs.CANME.all=ECanaShadow.CANME.all;
}


Uint8 CanBasic_sendMbox(Uint8 id, Uint16 *pdata) {
    Uint8 res = 0;
    struct ECAN_REGS s_ECanaShadow;
    s_ECanaShadow.CANTA.all = ECanaRegs.CANTA.all;
    /* step1: 检查邮箱状态， 并清空发送成功标志位*/
    if(s_ECanaShadow.CANTA.all &( (Uint32)1<<id)) {
        s_ECanaShadow.CANTA.all = (Uint32)1<<id;
        ECanaRegs.CANTA.all = s_ECanaShadow.CANTA.all;
    }

    /* step2: 检查邮箱是否有发送请求 */
    s_ECanaShadow.CANTRS.all = ECanaRegs.CANTRS.all;
    s_ECanaShadow.CANTA.all = ECanaRegs.CANTA.all;

    if  (((s_ECanaShadow.CANTRS.all&((Uint32)1<<id)) == 0)&&
            ((s_ECanaShadow.CANTA.all &((Uint32)1<<id)) == 0)) {
        /* step3: 发送buffer数据*/
        Uint8 *p =
                ((Uint8 *)&ECanaMboxes) + id * sizeof(struct MBOX);
        ((volatile struct MBOX *)p)->MDL.word.LOW_WORD = pdata[1];
        ((volatile struct MBOX *)p)->MDL.word.HI_WORD = pdata[0];
        ((volatile struct MBOX *)p)->MDH.word.LOW_WORD = pdata[3];
        ((volatile struct MBOX *)p)->MDH.word.HI_WORD = pdata[2];

        s_ECanaShadow.CANTRS.all = (Uint32)1<<id;
        ECanaRegs.CANTRS.all = s_ECanaShadow.CANTRS.all;
        res = 1;
    }

    return res;
}

Uint8 CanBasic_recieveMBox(Uint16 *pbuff) {
    Uint8 res = 0;
    Uint8 i = 0;
    Uint8 pos = 0;
    Uint32 num = 0x00000001;
    volatile struct MBOX *Mailbox;
    struct ECAN_REGS s_ECanaShadow;

    for(i = 8; i < 14; i++) {
        s_ECanaShadow.CANRMP.all = ECanaRegs.CANRMP.all;
        if(s_ECanaShadow.CANRMP.all & (num << i)) {
            Mailbox = &ECanaMboxes.MBOX0 + i;
            pos = (i - 8) * 4;
            pbuff[pos] = Mailbox->MDL.word.HI_WORD;
            pbuff[pos+1] = Mailbox->MDL.word.LOW_WORD;
            pbuff[pos+2] = Mailbox->MDH.word.HI_WORD;
            pbuff[pos+3] = Mailbox->MDH.word.LOW_WORD;
            res += (1<<(i - 8));
            s_ECanaShadow.CANRMP.all = (num << i);
            ECanaRegs.CANRMP.all = s_ECanaShadow.CANRMP.all;
        }
    }
    return res;
}


















