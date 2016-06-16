#include "t2connection.h"
#include "php_t2sdk.h"

// 全局连接对象
// extern CConnectionInterface *T2SDK_G(g_pConnection);
// extern CConnectionInterface *T2SDK_G(g_pConnection)Hq;

// extern CBusiness g_szBusiness;
// extern CBusiness g_szBusinessHq;
extern config T2NewConfig;
extern connection T2NewConnection;
extern packer T2NewPacker;
extern unpacker T2NewUnPacker;
extern biz_message T2NewBizMessage;

zval * errorToZval(int errorNo, char *errorMsg)
{
    zval *result;
    ALLOC_INIT_ZVAL(result);
    array_init(result);

    add_assoc_long(result, "errorNo", errorNo);
    add_assoc_string(result, "errorMsg", errorMsg, 1);

    return result;
}

zval * packToZval(IF2UnPacker *pUnPacker)
{
    zval *result;
    ALLOC_INIT_ZVAL(result);
    array_init(result);
    
    int i = 0, t = 0, j = 0, k = 0;
    int ivalue;
    char cvalue;
    char *ccvalue = new char[10];
    char *svalue = new char[100];
    const char * csvalue;
    float fvalue;
    char index[8];
    char *col = new char[100];

    for (i = 0; i < pUnPacker->GetDatasetCount(); ++i)
    {
        // 设置当前结果集
        pUnPacker->SetCurrentDatasetByIndex(i);
        
        // 打印所有记录
        for (j = 0; j < (int)pUnPacker->GetRowCount(); ++j)
        {
            zval *arr;
            ALLOC_INIT_ZVAL(arr);
            array_init(arr);
            for (k = 0; k < pUnPacker->GetColCount(); ++k)
            {
                const char *col_name = pUnPacker->GetColName(k);
                // strcpy(col, col_name);
                // puts(col);

                switch (pUnPacker->GetColType(k))
                {
                    case 'I':
                    //printf("%20d", pUnPacker->GetIntByIndex(k););
                    ivalue = pUnPacker->GetIntByIndex(k);
                    add_assoc_long(arr, col_name, ivalue);
                    break;
                    
                    case 'C':
                    //printf("%20c", pUnPacker->GetCharByIndex(k));
                    cvalue = pUnPacker->GetCharByIndex(k);
                    sprintf(ccvalue, "%20c", cvalue);
                    add_assoc_string(arr, col_name, ccvalue, 1);
                    break;
                    
                    case 'S':
                    //printf("%20s", pUnPacker->GetStrByIndex(k));
                    csvalue = pUnPacker->GetStrByIndex(k);
                    strcpy(svalue, csvalue);
                    add_assoc_string(arr, col_name, svalue, 1);
                    break;
                    
                    case 'F':
                    //printf("%20f", pUnPacker->GetDoubleByIndex(k));
                    fvalue = pUnPacker->GetDoubleByIndex(k);
                    add_assoc_double(arr, col_name, fvalue);
                    break;
                    
                    case 'R':
                    {
                        break;
                    }               
                    default:
                    // 未知数据类型
                    printf("未知数据类型。\n");
                    break;
                }
            }   
            sprintf(index, "%d", j);  
            add_assoc_zval(result, index, arr);    
            pUnPacker->Next();
        }
    }
    return result;
}

T2Connection::T2Connection(char *lib_t2sdk_file, char *ini_file, char *fund_account, char *password)
{
    this->lib_t2sdk_file = lib_t2sdk_file;
    this->ini_file = ini_file;
    this->fund_account = fund_account;
    this->password = password;
}


int T2Connection::connect(char * &error)
{
    void *handle = dlopen(this->lib_t2sdk_file, RTLD_LAZY | RTLD_GLOBAL);
    if (!handle)  
    {  
     error = dlerror();
     puts(error);
 }  

 T2NewConfig = (config) dlsym(handle, "NewConfig");

 T2NewConnection = (connection) dlsym(handle, "NewConnection");
 T2NewPacker = (packer) dlsym(handle, "NewPacker");
 T2NewUnPacker = (unpacker) dlsym(handle, "NewUnPacker");
 T2NewBizMessage = (biz_message) dlsym(handle, "NewBizMessage");

 lp_SecuRequestMode = new SecuRequestMode();
    //lp_SecuRequestMode->InitConn("demo", "license.dat", "218.108.19.190:18002");
 int ret = lp_SecuRequestMode->InitConn(this->ini_file, this->fund_account, this->password, error);


 return ret;
}

void T2Connection::disconnect()
{
    delete lp_SecuRequestMode;
}

zval* T2Connection::login()
{
    IF2UnPacker *pUnPacker;
    lp_SecuRequestMode->Login(pUnPacker);

    return packToZval(pUnPacker);
}

zval* T2Connection::req330300()
{
    IBizMessage* lpBizMessage = T2NewBizMessage();
    lpBizMessage->AddRef();


    IBizMessage* lpBizMessageRecv = NULL;

        //功能号
    lpBizMessage->SetFunction(330300);
        //请求类型
    lpBizMessage->SetPacketType(REQUEST_PACKET);
        //设置营业部号
    lpBizMessage->SetBranchNo(1);
        //设置company_id
    lpBizMessage->SetCompanyID(91000);
       //设置SenderCompanyID
    lpBizMessage->SetSenderCompanyID(91000);
        //设置系统号
    lpBizMessage->SetSystemNo(lp_SecuRequestMode->iSystemNo);

        ///获取版本为2类型的pack打包器
    IF2Packer *pPacker = T2NewPacker(2);
    if(!pPacker)
    {
        return errorToZval(-1, "取打包器失败!");
    }
    pPacker->AddRef();

        ///开始打包
    pPacker->BeginPack();
    //加入字段名
    //pPacker->AddField("op_branch_no", 'I', 5);//操作分支机构
    pPacker->AddField("op_branch_no", 'I', 5);//操作分支机构
    pPacker->AddField("op_entrust_way", 'C', 1);//委托方式 
    pPacker->AddField("op_station", 'S', 255);//站点地址
    pPacker->AddField("query_type",'C');
    pPacker->AddField("exchange_type",'S');
    pPacker->AddField("stock_type",'S');
    pPacker->AddField("stcok_code",'S');
    pPacker->AddField("position_str",'S');
    pPacker->AddField("request_num",'I', 5);
    
    
    ///加入对应的字段值
    pPacker->AddInt(lp_SecuRequestMode->m_op_branch_no);
    char entrust_way = lp_SecuRequestMode->GetEntrustWay();                        
    pPacker->AddChar(entrust_way); 
    string opStation = lp_SecuRequestMode->GetOpStation();            
    pPacker->AddStr(opStation.c_str());               
    pPacker->AddChar('0');
    pPacker->AddStr("1");
    pPacker->AddStr("");
    pPacker->AddStr("600570");
    pPacker->AddStr(" ");  
    pPacker->AddInt(1);
    ///加入对应的字段值
    
    char * output;
    sprintf(output, "op_branch_no:%d entrust_way:%20c, op_station:%s", lp_SecuRequestMode->m_op_branch_no, entrust_way, opStation.c_str());
    fputs(output);
    ///结束打包
    pPacker->EndPack();

    lpBizMessage->SetContent(pPacker->GetPackBuf(),pPacker->GetPackLen());

    IF2UnPacker *pUnPacker;

    int send = lp_SecuRequestMode->SendRequest(lpBizMessage, pPacker, iSystemNo, pUnPacker);

    return packToZval(pUnPacker);
}
