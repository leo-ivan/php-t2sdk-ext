/** @file
* 演示T2_SDK进行异步发包、收包
* 并且实现了T2_SDK断开重连的一个方法（运行平台Win32）
* @author  T2小组
* @author  恒生电子股份有限公司
* @version 1.0
* @date    20090325
*/

#include <Include/t2sdk_interface.h>
#include <dlfcn.h> 
#include "source/connection.h"

// 全局连接对象
CConnectionInterface *g_Connection = NULL;

// 全局事件

void ShowPacket(IF2UnPacker *lpUnPacker)
{
    int i = 0, t = 0, j = 0, k = 0;

    for (i = 0; i < lpUnPacker->GetDatasetCount(); ++i)
    {
        // 设置当前结果集
        lpUnPacker->SetCurrentDatasetByIndex(i);

        // 打印字段
        for (t = 0; t < lpUnPacker->GetColCount(); ++t)
        {
            printf("%20s", lpUnPacker->GetColName(t));
        }

        putchar('\n');

        // 打印所有记录
        for (j = 0; j < (int)lpUnPacker->GetRowCount(); ++j)
        {
            // 打印每条记录
            for (k = 0; k < lpUnPacker->GetColCount(); ++k)
            {
                switch (lpUnPacker->GetColType(k))
                {
                case 'I':
                    printf("%20d", lpUnPacker->GetIntByIndex(k));
                    break;

                case 'C':
                    printf("%20c", lpUnPacker->GetCharByIndex(k));
                    break;

                case 'S':
                    printf("%20s", lpUnPacker->GetStrByIndex(k));
                    break;

                case 'F':
                    printf("%20f", lpUnPacker->GetDoubleByIndex(k));
                    break;

                case 'R':
                    {
                        int nLength = 0;
                        void *lpData = lpUnPacker->GetRawByIndex(k, &nLength);

                        // 对2进制数据进行处理
                        break;
                    }

                default:
                    // 未知数据类型
                    printf("未知数据类型。\n");
                    break;
                }
            }

            putchar('\n');

            lpUnPacker->Next();
        }

        putchar('\n');
    }
}

// 自定义类CCallback，通过继承（实现）CCallbackInterface，来自定义各种事件（包括连接成功、
// 连接断开、发送完成、收到数据等）发生时的回调方法
class CCallback : public CCallbackInterface
{
public:
    // 因为CCallbackInterface的最终纯虚基类是IKnown，所以需要实现一下这3个方法
    unsigned long  FUNCTION_CALL_MODE QueryInterface(const char *iid, IKnown **ppv);
    unsigned long  FUNCTION_CALL_MODE AddRef();
    unsigned long  FUNCTION_CALL_MODE Release();

    // 各种事件发生时的回调方法，实际使用时可以根据需要来选择实现，对于不需要的事件回调方法，可直接return
    // Reserved?为保留方法，为以后扩展做准备，实现时可直接return或return 0。
    void FUNCTION_CALL_MODE OnConnect(CConnectionInterface *lpConnection);
    void FUNCTION_CALL_MODE OnSafeConnect(CConnectionInterface *lpConnection);
    void FUNCTION_CALL_MODE OnRegister(CConnectionInterface *lpConnection);
    void FUNCTION_CALL_MODE OnClose(CConnectionInterface *lpConnection);
    void FUNCTION_CALL_MODE OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData);
    void FUNCTION_CALL_MODE Reserved1(void *a, void *b, void *c, void *d);
    void FUNCTION_CALL_MODE Reserved2(void *a, void *b, void *c, void *d);
    int  FUNCTION_CALL_MODE Reserved3();
    void FUNCTION_CALL_MODE Reserved4();
    void FUNCTION_CALL_MODE Reserved5();
    void FUNCTION_CALL_MODE Reserved6();
    void FUNCTION_CALL_MODE Reserved7();
    void FUNCTION_CALL_MODE OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult);
		void FUNCTION_CALL_MODE OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult);
    void FUNCTION_CALL_MODE OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg);

};

unsigned long CCallback::QueryInterface(const char *iid, IKnown **ppv)
{
    return 0;
}

unsigned long CCallback::AddRef()
{
    return 0;
}

unsigned long CCallback::Release()
{
    return 0;
}

// 以下各回调方法的实现仅仅为演示使用
void CCallback::OnConnect(CConnectionInterface *lpConnection)
{
    puts("CCallback::OnConnect");
}

void CCallback::OnSafeConnect(CConnectionInterface *lpConnection)
{
    puts("CCallback::OnSafeConnect");
}

void CCallback::OnRegister(CConnectionInterface *lpConnection)
{
    puts("CCallback::OnRegister");
}

void CCallback::OnClose(CConnectionInterface *lpConnection)
{
    puts("CCallback::OnClose");
}

void CCallback::OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData)
{
}

void CCallback::Reserved1(void *a, void *b, void *c, void *d)
{
}

void CCallback::Reserved2(void *a, void *b, void *c, void *d)
{
}

void CCallback::OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult)
{
	
}

void CCallback::OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult)
{
    switch (nResult)
    {
    case 0:
        {
            puts("业务操作成功。");
            ShowPacket((IF2UnPacker *)lpUnPackerOrStr);
            break;
        }

    case 1:
        {
            puts("业务操作失败。");
            ShowPacket((IF2UnPacker *)lpUnPackerOrStr);
            break;
        }

    case 2:
        {
            puts((char *)lpUnPackerOrStr);
            break;
        }

    default:
        {
            puts("业务包解包失败。");
            break;
        }
    }
}

int  CCallback::Reserved3()
{
    return 0;
}

void CCallback::Reserved4()
{
}

void CCallback::Reserved5()
{
}

void CCallback::Reserved6()
{
}

void CCallback::Reserved7()
{
}
void CCallback::OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg)
{
	
}

Connection::Connection()
{

}

void Connection::connect()
{
    //CConnectionInterface *g_Connection = NULL;

    void *handle = dlopen("/data/work/php/extension/libt2sdk.so", RTLD_LAZY | RTLD_GLOBAL);
    char *error;
    if (!handle)  
    {  
       error = dlerror();
       puts(error);
    }  

    typedef CConfigInterface* (*config)(); 
    typedef CConnectionInterface* (*connection)(CConfigInterface*); 
    typedef IF2Packer* (*packer)(int); 
    config NewConfig = (config) dlsym(handle, "NewConfig");
    connection NewConnection = (connection) dlsym(handle, "NewConnection");
    packer NewPacker = (packer) dlsym(handle, "NewPacker");
  
    CConfigInterface* lpConfig = NewConfig(); 
    error = dlerror();
    if(error)
    {
        dlclose(handle);
        puts(error);
    }

    lpConfig->AddRef();

    // [t2sdk] servers指定需要连接的IP地址及端口
    lpConfig->SetString("t2sdk", "servers", "192.168.94.30:9999");

    // [t2sdk] license_file指定许可证文件
    lpConfig->SetString("t2sdk", "license_file", "license.dat");

    // [t2sdk] send_queue_size指定T2_SDK的发送队列大小
    lpConfig->SetString("t2sdk", "send_queue_size", "100");
    //在此设置一下就可以支持自动重连
    lpConfig->SetString("t2sdk", "auto_reconnect", "1");

    g_Connection = NewConnection(lpConfig);

    g_Connection->AddRef();

    // 创建自定义类CCallback的对象（在初始化连接对象时需传递此对象，请看下面代码）
    CCallback callback;

    int ret = 0;

    // 初始化连接对象，返回0表示初始化成功，注意此时并没开始连接服务器
    if (0 == (ret = g_Connection->Create(&callback)))
    {
        // 正式开始连接注册，参数1000为超时参数，单位是ms
        if (ret = g_Connection->Connect(1000))
        {
            // 若连接/注册失败，打印失败原因
            puts(g_Connection->GetErrorMsg(ret));
        }
        else
        {
            // 演示如何使用打包器
            IF2Packer *lpPacker = NewPacker(2);

            lpPacker->AddRef();
            lpPacker->BeginPack();
            lpPacker->AddField("Field");
            lpPacker->AddStr("Data");

            lpPacker->EndPack();

            // 异步模式收发包，第三个参数填1
            g_Connection->SendBiz(100, lpPacker, 1);

            // 释放打包器，不释放会引起内存泄露，后果严重
            lpPacker->FreeMem(lpPacker->GetPackBuf());
            lpPacker->Release();
        }
    }
    else
    {
        puts(g_Connection->GetErrorMsg(ret));
    }

    // 通过getchar阻塞线程，等待服务端应答包到达
    // 演示断开重连时，可在此时关闭服务器，然后再恢复
    getchar();

    // 释放资源
    g_Connection->Release();
    lpConfig->Release();
}

int main()
{
    // 通过T2SDK的引出函数，来获取一个新的CConfig对象
    // 此对象在创建连接对象时被传递，用于配置所创建的连接对象的各种属性（比如服务器IP地址、安全模式）
    CConfigInterface * lpConfig = NewConfig();

    // 通过T2SDK的引出函数NewXXXX返回的对象，需要调用对象的Release方法释放，而不能直接用delete
    // 因为t2sdk.dll和调用程序可能是由不同的编译器、编译模式生成，delete可能会导致异常
    // 为了适应Delphi等使用（Delphi对接口自动调用AddRef方法），用C/C++开发的代码，需要在NewXXXX之后调用一下AddRef
    // 以保证引用计数正确
    lpConfig->AddRef();

    // [t2sdk] servers指定需要连接的IP地址及端口
    lpConfig->SetString("t2sdk", "servers", "192.168.94.30:9999");

    // [t2sdk] license_file指定许可证文件
    lpConfig->SetString("t2sdk", "license_file", "license.dat");

    // [t2sdk] send_queue_size指定T2_SDK的发送队列大小
    lpConfig->SetString("t2sdk", "send_queue_size", "100");
    //在此设置一下就可以支持自动重连
    lpConfig->SetString("t2sdk", "auto_reconnect", "1");

    // 通过T2SDK的引出函数，来获取一个新的CConnection对象
    g_Connection = NewConnection(lpConfig);

    g_Connection->AddRef();

    // 创建自定义类CCallback的对象（在初始化连接对象时需传递此对象，请看下面代码）
    CCallback callback;

    int ret = 0;

    // 初始化连接对象，返回0表示初始化成功，注意此时并没开始连接服务器
    if (0 == (ret = g_Connection->Create(&callback)))
    {
        // 正式开始连接注册，参数1000为超时参数，单位是ms
        if (ret = g_Connection->Connect(1000))
        {
            // 若连接/注册失败，打印失败原因
            puts(g_Connection->GetErrorMsg(ret));
        }
        else
        {
            // 演示如何使用打包器
            IF2Packer *lpPacker = NewPacker(2);

            lpPacker->AddRef();
            lpPacker->BeginPack();
            lpPacker->AddField("Field");
            lpPacker->AddStr("Data");

            lpPacker->EndPack();

            // 异步模式收发包，第三个参数填1
            g_Connection->SendBiz(100, lpPacker, 1);

            // 释放打包器，不释放会引起内存泄露，后果严重
            lpPacker->FreeMem(lpPacker->GetPackBuf());
            lpPacker->Release();
        }
    }
    else
    {
        puts(g_Connection->GetErrorMsg(ret));
    }

    // 通过getchar阻塞线程，等待服务端应答包到达
    // 演示断开重连时，可在此时关闭服务器，然后再恢复
    getchar();

    // 释放资源
    g_Connection->Release();
    lpConfig->Release();

    return 0;
}
