//
// Created by Ace on 2017/8/5.
//
//#include "OBOJni.h"
#include "cJSON.h"
#include <curl/curl.h>
#include<string.h>

#define RESPONSE_DATA_LEN 4096

//用来接收服务器一个buffer
typedef struct login_response_data
{
    login_response_data() {
        memset(data, 0, RESPONSE_DATA_LEN);
        data_len = 0;
    }

    char data[RESPONSE_DATA_LEN];
    int data_len;

}response_data_t;


//处理从服务器返回的数据，将数据拷贝到arg中
size_t deal_response(void *ptr, size_t n, size_t m, void *arg)
{
    int count = m*n;

    response_data_t *response_data = (response_data_t*)arg;

    memcpy(response_data->data, ptr, count);

    response_data->data_len = count;

    return response_data->data_len;
}

//#define POSTDATA "{\"username\":\"gailun\",\"password\":\"123123\"}"
#define POSTDATA "{\"username\":\"gailun\",\"password\":\"123123\",\"driver\":\"yes\"}"

int main()
{
    //const char *username = env->GetStringUTFChars(j_username, 0);
    //const char *passwd = env->GetStringUTFChars(j_passwd, 0);
    //const char *isDriver = j_isDriver == 0?"yes":"no";

    char *post_str = NULL;
    /*char *post_str = "123123";

    CURL* curl = NULL;
    CURLcode res;
    response_data_t responseData;//专门用来存放从服务器返回的数据
*/
	CURL* curl = NULL;
	CURLcode res;
    response_data_t responseData;//专门用来存放从服务器返回的数据
    //初始化curl句柄
    curl = curl_easy_init();
    if(curl == NULL) {
       // __android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login: curl init error \n");
        return 1;
    }



    //__android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login: username = %s, passwd = %s, isDriver = %s",username, passwd, isDriver);


    //封装一个数据协议
    /*

       ====给服务端的协议====
     http://ip:port/login [json_data]
    {
        username: "gailun",
        password: "123123",
        driver:   "yes"
    }
     *
     *
     * */
    //（1）封装一个json字符串
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "username", "ldw");
    cJSON_AddStringToObject(root, "password", "123123");
    cJSON_AddStringToObject(root, "driver", "yes");

    post_str = cJSON_Print(root);
    cJSON_Delete(root);
    root = NULL;


   // __android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login: post_str = [%s]\n", post_str);



    //(2) 向web服务器 发送http请求 其中post数据 json字符串
    //1 设置curl url
    curl_easy_setopt(curl, CURLOPT_URL, "http://172.16.1.96:7777/login");

    //客户端忽略CA证书认证
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

    //2 开启post请求开关
    curl_easy_setopt(curl, CURLOPT_POST, true);
    //3 添加post数据
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_str);

    //4 设定一个处理服务器响应的回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, deal_response);

    //5 给回调函数传递一个形参
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    //6 向服务器发送请求,等待服务器的响应
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        //__android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login:perform ERROR, rescode= [%d]\n",
                            //res);
        return 1;

    }
    curl_easy_cleanup(curl);
    //（3）  处理服务器响应的数据 此刻的responseData就是从服务器获取的数据


    /*

      //成功
    {
        result: "ok",
    }
    //失败
    {
        result: "error",
        reason: "why...."
    }

     *
     * */
    //(4) 解析服务器返回的json字符串
    //cJSON *root;
    root = cJSON_Parse(responseData.data);

    cJSON *result = cJSON_GetObjectItem(root, "result");
    if(result && strcmp(result->valuestring, "ok") == 0) {
	    printf("data:%s\n",responseData.data);
        //登陆成功
        //__android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login:login succ！！！");
        return 0;

    }
    else {
        //登陆失败
        cJSON* reason = cJSON_GetObjectItem(root, "reason");
        if (reason) {
            //已知错误
            //__android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login:login error, reason = %s！！！", reason->valuestring);

        }
        else {
            //未知的错误
           // __android_log_print(ANDROID_LOG_ERROR,TAG,"JNI-login:login error, reason = Unknow！！！");

        }

        return 1;
    }



    return 0;
}

