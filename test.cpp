// test.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <string>
 
using namespace std;
 
 
//用来存储eax,ebx,ecx,edx四个寄存器的信息
DWORD deax;
DWORD debx;
DWORD decx;
DWORD dedx;
 
void ExeCPUID(DWORD veax)  //初始化CPU
{
__asm
{
    mov eax,veax
    cpuid
    mov deax,eax
    mov debx,ebx
    mov decx,ecx
    mov dedx,edx
}
}
 
/*	在Intel Pentium以上级别的CPU中，有一个称为“时间戳（Time Stamp）”的部件，
	它以64位无符号整型数的格式，记录了自CPU上电以来所经过的时钟周期数。
	由于目前的CPU主频都非常高，因此这个部件可以达到纳秒级的计时精度。
	这个精确性是上述两种方法所无法比拟的。
	在Pentium以上的CPU中，提供了一条机器指令RDTSC（Read Time Stamp Counter）
	来读取这个时间戳的数字，并将其保存在EDX:EAX寄存器对中
*/
long GetCPUFreq()       //获取CPU频率,单位: MHZ
{
	int start,over;
	_asm 
	{
		RDTSC
		mov start,eax
	}
	Sleep(50);
	_asm 
	{
		RDTSC
		mov over,eax
	}
	return (over-start)/50000;
}
 
 
 
/*   把eax = 0作为输入参数，可以得到CPU的制造商信息。
     cpuid指令执行以后，会返回一个12字符的制造商信息，
     前四个字符的ASC码按低位到高位放在ebx，中间四个放在edx，最后四个字符放在ecx。
*/
string GetManID()   //获取制造商信息
{
	char ID[25];        
	memset(ID,0,sizeof(ID));
	
	ExeCPUID(0);          //初始化
	memcpy(ID+0,&debx,4); //制造商信息复制到数组
	memcpy(ID+4,&dedx,4);
	memcpy(ID+8,&decx,4);
	
	return string(ID);
}
 
 
/*  在我的电脑上点击右键，选择属性，可以在窗口的下面看到一条CPU的信息，
	这就是CPU的商标字符串。CPU的商标字符串也是通过cpuid得到的。
	由于商标的字符串很长(48个字符)，所以不能在一次cpuid指令执行时全部得到，
	所以intel把它分成了3个操作，eax的输入参数分别是0x80000002,0x80000003,0x80000004，
	每次返回的16个字符，按照从低位到高位的顺序依次放在eax, ebx, ecx, edx。
	因此，可以用循环的方式，每次执行完以后保存结果，然后执行下一次cpuid。
*/
string GetCPUType()
{
	const DWORD id = 0x80000002; //从0x80000002开始,到0x80000004结束
	char CPUType[49];//用来存储CPU型号信息
	memset(CPUType,0,sizeof(CPUType));//初始化数组
	
	for(DWORD t = 0 ; t < 3 ; t++ )
	{
		ExeCPUID(id+t);
		//每次循环结束,保存信息到数组
		memcpy(CPUType+16*t+ 0,&deax,4);
		memcpy(CPUType+16*t+ 4,&debx,4);
		memcpy(CPUType+16*t+ 8,&decx,4);
		memcpy(CPUType+16*t+12,&dedx,4);
	}
	
	return string(CPUType);
}
 
void main() 
{ 
	cout<<"本机CPU信息如下:"<<endl;
	cout<<"CPU 主 频: "<<GetCPUFreq()<<" MHZ"<<endl;
	cout<<"CPU 制造商: "<<GetManID()<<endl;
	cout<<"CPU 型 号: "<<GetCPUType()<<endl;
	cin.get();
}

