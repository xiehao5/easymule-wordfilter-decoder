#include <string>
#include <vector>
#include <cstdio>
#include <windows.h>

#define  UPDATE_URL _T("http://download.verycd.com/wf/wordfilter.txt")
#define FLITER_FILE "wordfilter.txt"

using namespace std;

namespace InternalBase64
{
unsigned char decode(char c)
{
    if(c >= 'A' && c <= 'Z')
    {
        return c - 'A';
    }

    if(c >= 'a' && c <= 'z')
    {
        return c - 'a' + 26;
    }

    if(c >= '0' && c <= '9')
    {
        return c - '0' + 52;
    }

    if(c == '+')
    {
        return 62;
    }

    return 63;
}

bool isBase64(char c)
{
    if(c >= 'A' && c <= 'Z')
    {
        return true;
    }

    if(c >= 'a' && c <= 'z')
    {
        return true;
    }

    if(c >= '0' && c <= '9')
    {
        return true;
    }

    if(c == '+')
    {
        return true;
    }

    if(c == '/')
    {
        return true;
    }

    if(c == '=')
    {
        return true;
    }

    return false;
}

bool isBase64( const std::string & str )
{
    for( size_t i = 0 ; i < str.size() ; i++ )
    {
        if( !isBase64( str[i] ) )
        {
            return false;
        }
    }

    return true;
}

vector<unsigned char> decode(const string& str)
{
    string newStr;

    newStr.reserve(str.length());

    for(unsigned long j = 0; j < str.length(); j++)
    {
        if(isBase64(str[j]))
        {
            newStr += str[j];
        }
    }

    vector<unsigned char> retval;

    if(newStr.length() == 0)
    {
        return retval;
    }

    // Note: This is how we were previously computing the size of the return
    //       sequence.  The method below is more efficient (and correct).
    // unsigned long lines = str.size() / 78;
    // unsigned long totalBytes = (lines * 76) + (((str.size() - (lines * 78)) * 3) / 4);

    // Figure out how long the final sequence is going to be.
    unsigned long totalBytes = (newStr.size() * 3 / 4) + 1;

    retval.reserve(totalBytes);

    unsigned char by1 = 0;
    unsigned char by2 = 0;
    unsigned char by3 = 0;
    unsigned char by4 = 0;

    char c1, c2, c3, c4;

    for(unsigned long i = 0; i < newStr.length(); i += 4)
    {
        c1 = 'A';
        c2 = 'A';
        c3 = 'A';
        c4 = 'A';

        c1 = newStr[i];

        if((i + 1) < newStr.length())
        {
            c2 = newStr[i + 1];
        }

        if((i + 2) < newStr.length())
        {
            c3 = newStr[i + 2];
        }

        if((i + 3) < newStr.length())
        {
            c4 = newStr[i + 3];
        }

        by1 = decode(c1);
        by2 = decode(c2);
        by3 = decode(c3);
        by4 = decode(c4);

        retval.push_back((by1 << 2) | (by2 >> 4));

        if(c3 != '=')
        {
            retval.push_back(((by2 & 0xf) << 4) | (by3 >> 2));
        }

        if(c4 != '=')
        {
            retval.push_back(((by3 & 0x3) << 6) | by4);
        }
    }

    return retval;
}
};

void Init()
{
    HANDLE	hFile;
    DWORD	dwRead;
    BOOL	bResult;

    hFile = CreateFile(FLITER_FILE, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if(hFile == INVALID_HANDLE_VALUE)
    {
        // 读取错误，所有的都过滤 added by kernel1983 2006.08.08
        printf("File Open Error\n\nPlease download the newest wordfilter file from:\nhttp://download.verycd.com/wf/wordfilter.txt\n");
        system("pause");
        return;
    }

    DWORD dwSize = GetFileSize(hFile, NULL);                        //获取文件大小
    TCHAR * pszData = new TCHAR[(dwSize / sizeof(TCHAR)) + 1];			// 申请空间
    bResult = ReadFile(hFile, pszData, dwSize, &dwRead, NULL);		// 读入文件1
    CloseHandle(hFile);                                             //关闭文件
    pszData[(dwSize / sizeof(TCHAR))] = 0;                          //字符串以0结尾
    
    if(*(unsigned char *)pszData != 0x15)							//数据格式检查
    {
    	printf("File Format Error\n\nPlease download the newest wordfilter file from:\nhttp://download.verycd.com/wf/wordfilter.txt\n");
    	delete[] pszData;
        system("pause");
        return;
    }

    std::string tempstr( (char*)pszData + 1 , ((int)dwSize - 1) > 0 ? dwSize -1 : 0 );

//解码开始
    vector<unsigned char> vec = InternalBase64::decode( tempstr );
    char * pszt = (char*) pszData;
    for( size_t i = 0; i < vec.size() ; i++ )
    {
        pszt[i] = vec[i];
    }
    dwSize = vec.size();
    pszData[(dwSize / sizeof(TCHAR))] = 0;
//解码结束

    FILE* fp = fopen( FLITER_FILE,"wb" );      //新建文件，保存解码后的值
    if(fp == NULL)
    {
    	printf("File save Error\n");
    	delete[] pszData;
        system("pause");
        return;
    }
    fwrite(pszData,((int)dwSize - 1),1,fp);
    fclose(fp);
    delete[] pszData;

	printf("Decode Successful\n");
	system("pause");
	return;
}

int main()
{
    Init();
    return 0;
}
