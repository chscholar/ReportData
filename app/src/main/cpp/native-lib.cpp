#include <jni.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/stat.h>
#include <pthread.h>
#include "utils.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/if_packet.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <pthread.h>

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct _iphdr //定义IP首部
{
    unsigned char h_verlen; //4位首部长度+4位IP版本号
    unsigned char tos; //8位服务类型TOS
    unsigned short total_len; //16位总长度（字节）
    unsigned short ident; //16位标识
    unsigned short frag_and_flags; //3位标志位
    unsigned char ttl; //8位生存时间 TTL
    unsigned char proto; //8位协议 (TCP, UDP 或其他)
    unsigned short checksum; //16位IP首部校验和
    unsigned int sourceIP; //32位源IP地址
    unsigned int destIP; //32位目的IP地址
}IP_HEADER;

typedef struct _udphdr //定义UDP首部
{
    unsigned short uh_sport;    //16位源端口
    unsigned short uh_dport;    //16位目的端口
    unsigned int uh_len;//16位UDP包长度
    unsigned int uh_sum;//16位校验和
}UDP_HEADER;

typedef struct IpPacket{
    unsigned int sourceIP; //32位源IP地址
    unsigned int destIP; //32位目的IP地址
};

typedef  struct UdpPacket{
    unsigned short uh_sport;    //16位源端口
    unsigned short uh_dport;    //16位目的端口
    unsigned int uh_len;//16位UDP包长度
};

typedef struct  Packet{
    IpPacket ippacket;
    UdpPacket udppacket;
    std::string tipMessage;
};

IpPacket analyseIP(IP_HEADER *ip);
UdpPacket analyseUDP(UDP_HEADER *udp);


UdpPacket analyseUDP(UDP_HEADER *udp)
{
    printf("UDP -----\n");
    printf("Source port: %u\n", ntohs(udp->uh_sport));
    printf("Dest port: %u\n", ntohs(udp->uh_dport));
    printf("packet Length : %d",udp->uh_len);

    UdpPacket udppacket;
    int srcPort = ntohs(udp->uh_sport);
    int destPort = ntohs(udp->uh_dport);
    int udpLength = udp->uh_len;

    udppacket.uh_sport = srcPort;
    udppacket.uh_dport = destPort;
    udppacket.uh_len = udpLength;

   return udppacket;
}

IpPacket analyseIP(IP_HEADER *ip)
{
    unsigned char* p = (unsigned char*)&ip->sourceIP;
    printf("Source IP\t: %u.%u.%u.%u\n",p[0],p[1],p[2],p[3]);
    p = (unsigned char*)&ip->destIP;
    printf("Destination IP\t: %u.%u.%u.%u\n",p[0],p[1],p[2],p[3]);

    IpPacket ippacket;
    ippacket.sourceIP = ip->sourceIP;
    ippacket.destIP = ip->destIP;
    return  ippacket;
}

typedef struct SwitchData{
    unsigned  char id;   //ID号
    unsigned  char fun;  //功能码
    unsigned  char len;  //数据长度
    unsigned  char data; //数据
    unsigned  short crc; //crc
};

typedef struct ImitateData{
    unsigned  char dtuid; //DTU ID
    unsigned  char fun;     //功能码
    unsigned  char len;          //数据长度
    unsigned  short oneImitateData;  //第一套模拟量部份
    unsigned  short twoImitateData;  //第二套模拟量部分
    unsigned  short threeImitateData;  //第三套模拟量部份
    unsigned  short strikeCount;  //雷击计数
    unsigned  short strikePeak;  //雷击峰值
};

extern "C" JNIEXPORT jstring JNICALL
Java_com_sinlincom_reportdata_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    return env->NewStringUTF(hello.c_str());;;
}

void setTextView(JNIEnv * env, jobject obj){

    jclass mainActivity = env->FindClass("com/sinlincom/reportdata/MainActivity");
    if(mainActivity == 0){
        printf("find class error");
        return;
    }
    jmethodID setTextViewMethod = env->GetMethodID(mainActivity,"setTextView","()V");
    if(setTextViewMethod == 0){
        printf("find method1 error");
        return;
    }
    env->CallVoidMethod(obj, setTextViewMethod);
}

jobject Java_com_sinlincom_reportdata_MainActivity_ReturnSwitchData
        (JNIEnv * env, jobject obj,SwitchData* sData){

    jclass  mainActivityClass =  env->GetObjectClass(obj);
    jfieldID funType = env->GetFieldID(mainActivityClass,"funType","I");
    jint number = env->GetIntField(obj,funType);
    env->SetIntField(obj,funType,1);


    jclass switchClass = env->FindClass("com/sinlincom/reportdata/SwitchData");
    jmethodID initMeth = (env)->GetMethodID(switchClass, "<init>", "()V");

    jfieldID id = (env)->GetFieldID(switchClass,"id","C");
    jfieldID fun = (env)->GetFieldID(switchClass,"fun","C");
    jfieldID len = (env)->GetFieldID(switchClass,"len","C");
    jfieldID status = (env)->GetFieldID(switchClass,"data","C");
    jfieldID crc = (env)->GetFieldID(switchClass,"crc","S");

    jobject  jo = env->NewObject(switchClass,initMeth);

    env->SetCharField(jo,id,sData->id);
    env->SetCharField(jo,fun,sData->fun);
    env->SetCharField(jo,id,sData->len);
    env->SetCharField(jo,id,sData->data);
    env->SetShortField(jo,crc,sData->crc);

    return jo;
}

jobject Java_com_sinlincom_reportdata_MainActivity_ReturnImitateData
        (JNIEnv * env, jobject obj,ImitateData* iData){

    jclass  mainActivityClass =  env->GetObjectClass(obj);
    jfieldID funType = env->GetFieldID(mainActivityClass,"funType","I");
    jint number = env->GetIntField(obj,funType);
    env->SetIntField(obj,funType,3);

    jclass imidateClass = env->FindClass("com/sinlincom/reportdata/ImitateData");
    jmethodID initMeth = (env)->GetMethodID(imidateClass, "<init>", "()V");

    jfieldID dtuid = (env)->GetFieldID(imidateClass,"dtuid","C");
    jfieldID fun = (env)->GetFieldID(imidateClass,"fun","C");
    jfieldID len = (env)->GetFieldID(imidateClass,"len","C");
    jfieldID oneData = (env)->GetFieldID(imidateClass,"oneImitateData","S");
    jfieldID twoData = (env)->GetFieldID(imidateClass,"twoImitateData","S");
    jfieldID threeData = (env)->GetFieldID(imidateClass,"threeImitateData","S");
    jfieldID strikeCount =  (env)->GetFieldID(imidateClass,"strikeCount","S");
    jfieldID strikePeak = (env)->GetFieldID(imidateClass,"strikePeak","S");

    jobject  jo = env->NewObject(imidateClass,initMeth);


    env->SetCharField(jo,dtuid,iData->dtuid);
    env->SetCharField(jo,fun,iData->fun);
    env->SetCharField(jo,len,iData->len);
    env->SetShortField(jo,oneData,iData->oneImitateData);
    env->SetShortField(jo,twoData,iData->twoImitateData);
    env->SetShortField(jo,threeData,iData->threeImitateData);
    env->SetShortField(jo,strikeCount,iData->strikeCount);
    env->SetShortField(jo,strikePeak,iData->strikePeak);


    /*
    env->SetCharField(jo,dtuid,1);
    env->SetCharField(jo,fun,2);
    env->SetCharField(jo,len,3);
    env->SetShortField (jo,oneData,4);
    env->SetShortField(jo,twoData,5);
    env->SetShortField(jo,threeData,6);
    env->SetShortField(jo,strikeCount,7);
    env->SetShortField(jo,strikePeak,8);
    */

    return jo;
}

jobject Java_com_sinlincom_reportdata_MainActivity_returnToJava
        (JNIEnv * env, jobject obj, int type,void* Data)
{
    if (type == 1)
    {
        SwitchData *sdata = ( SwitchData *)Data;
        return Java_com_sinlincom_reportdata_MainActivity_ReturnSwitchData(env,obj,sdata);
    } else if (type == 3)
    {
        ImitateData *idata = (ImitateData*)Data;
        return Java_com_sinlincom_reportdata_MainActivity_ReturnImitateData(env,obj,idata);
    }
}



extern "C" JNIEXPORT jobject JNICALL
Java_com_sinlincom_reportdata_MainActivity_getReportData(
        JNIEnv *env,
        jobject obj,
        jboolean type) {
    int sockfd;
    IP_HEADER *ip;
    char buf[10240];
    ssize_t n;
    std::string resutl;
    Packet packet;
    std::string strResult;
    /* capture ip datagram without ethernet header */
   //SOCK_RAW SOCK_DGRAM ETH_P_ALL ETH_P_IP
   if ((sockfd = socket(PF_PACKET,  SOCK_RAW, htons(ETH_P_ALL)))== -1)
   // if ((sockfd = socket(PF_PACKET,  SOCK_DGRAM, htons(ETH_P_IP)))== -1)
    {
        printf("socket error!\n");
        resutl = "socket error";
        packet.tipMessage = resutl;
        return obj;
    }
    std::string udpSrcDestPort;
    void * resultData;
    //int iType;
    while (1)
    {
        n = recv(sockfd, buf, sizeof(buf), 0);
        if (n == -1)
        {
            printf("recv error!\n");
            strResult = "recv error!";
            break;
        }
        else if (n==0)
            continue;
        //接收数据不包括数据链路帧头
        ip = ( IP_HEADER *)(buf);
       packet.ippacket =  analyseIP(ip);
        size_t iplen =  (ip->h_verlen&0x0f)*4;

        if (ip->proto == IPPROTO_TCP)
        {

            int a = 1;
            printf("get tcp data");
        }else  if (ip->proto == IPPROTO_UDP)
        {
            UDP_HEADER *udp = (UDP_HEADER *)(buf + iplen);
            packet.udppacket =  analyseUDP(udp);
            char * udpData = (char*)udp+8;

             char* fun = (char*)udpData + 2; //取得功能码

             /*
             if (fun[0] == 1) {
                 //开关量部分
                 SwitchData *sData = (SwitchData*) udpData;
                 resultData = sData;
                 type = 1;
                 break;
             } else if( fun[0] == 3){
                 //模拟量部分
                ImitateData *idata = (ImitateData*)udpData;
                resultData = idata;
                 type = 3;
                break;
             }
              */

             resultData = (ImitateData*)udpData; type = 3;
             //resultData = (SwitchData*) udpData;
             break;
        }
    }
    close(sockfd);

    return obj;
   // return Java_com_sinlincom_reportdata_MainActivity_returnToJava(env,obj,type,resultData);
}


extern "C" JNIEXPORT void JNICALL
Java_com_sinlincom_reportdata_MainActivity_callMethod1
        (JNIEnv * env, jobject obj){

    jclass mainActivity = env->FindClass("com/sinlincom/reportdata/MainActivity");
    if(mainActivity == 0){
       printf("find class error");
        return;
    }
    jmethodID setTextViewMethod = env->GetMethodID(mainActivity,"setTextView","()V");
    if(setTextViewMethod == 0){
        printf("find method1 error");
        return;
    }
    env->CallVoidMethod(obj, setTextViewMethod);
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_sinlincom_reportdata_MainActivity_testTransStruct
        (JNIEnv * env, jobject obj){

    jclass switchClass = env->FindClass("com/sinlincom/reportdata/SwitchData");
    jmethodID initMeth = (env)->GetMethodID(switchClass, "<init>", "()V");

    jfieldID a = (env)->GetFieldID(switchClass,"a","I");
    jfieldID b = (env)->GetFieldID(switchClass,"b","I");

    jobject  jo = env->NewObject(switchClass,initMeth);

   env->SetIntField(jo,a,1);
   env->SetIntField(jo,b,2);
    return jo;
}